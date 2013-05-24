#include <fstream>
#include <sstream>
#include <stdexcept>

#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../../common/vision.hpp"
#include "../../common/path.hpp"
#include "../../common/debug.hpp"

#include "RandomForestTracker.hpp"

#include "random-forest-internals-implementation/FeaturePointsExtractor.hpp"
#include "random-forest-internals-implementation/ClassificatorParameters.hpp"
#include "random-forest-internals-implementation/RandomForest.hpp"
#include "random-forest-internals-implementation/RandomForestBuilder.hpp"

// Constants.
const std::string RandomForestTracker::Name = "Random Forest Tracker";

const cv::Scalar RandomForestTracker::DrawingColor = cv::Scalar(0, 255, 0);
const cv::Size RandomForestTracker::GaussianKernelSize = cv::Size(7, 7);

// Internal functions.
std::string toString(int value) {
  std::ostringstream result;
  result << value;

  return result.str();
}

// PIMPL idiom implementation.
struct RandomForestTracker::PIMPL {
  ClassificatorParameters parameters;
  FeaturesCollection* trainingBase;
  RandomForest* randomForest;

  PIMPL() {
    trainingBase = 0;
    randomForest = 0;

    parameters.TrainingBaseFolder = "bin/training-base-directory/";

    parameters.FeaturePointsCount = 100;
    parameters.HalfPatchSize = 50;

    parameters.MaximumTreeHeight = 30;
    parameters.MinimumElementPerNode = 100;

    parameters.RandomTreesCount = 30;
    parameters.MinimumClassificationConfidence = 0.1;

    parameters.ClassifierIntensityThreshold = 10;
    parameters.GeneratedRandomPointsCount = 30;
  }

  ~PIMPL() {
    deleteTrainingBase();

    if (randomForest != 0) {
      delete randomForest;
      randomForest = 0;
    }
  }

  void deleteTrainingBase() {
    if (trainingBase != 0) {
      delete trainingBase;
      trainingBase = 0;
    }
  }

  void setPath(const std::string& movieName) {
    parameters.TrainingBaseFolder += common::path::extractBaseName(common::path::extractFileName(movieName)) + "/";
  }

};

// Constructor.
RandomForestTracker::RandomForestTracker() {
  implementation = new RandomForestTracker::PIMPL();
}

RandomForestTracker::~RandomForestTracker() {
  delete implementation;
}

// Private methods.
void RandomForestTracker::generateTrainingBase() {
  cv::cvtColor(implementation->parameters.InitialImage, implementation->parameters.InitialImage, CV_BGR2GRAY);

  FeaturePointsExtractor featureExtractor(
                            implementation->parameters.FeaturePointsCount,
                            implementation->parameters.HalfPatchSize);

  common::debug::log("Generating feature points\n");
  featureExtractor.generateFeaturePoints(implementation->parameters.InitialImage);

  common::debug::log("Copying training base to internal structure\n");
  implementation->deleteTrainingBase();
  implementation->trainingBase = new FeaturesCollection(featureExtractor.getFeatures());
}

void RandomForestTracker::writeTrainingBaseToFolder() {
  if (!implementation->trainingBase || implementation->trainingBase->empty()) {
    throw std::logic_error("Can't write empty training base!");
  }

  const FeaturesCollection& featureCollection = *implementation->trainingBase;

  common::debug::log("Creating directory for training base\n");
  common::path::makeDir(implementation->parameters.TrainingBaseFolder);

  common::debug::log("Saving training base informations\n");
  std::ofstream trainingBaseHeader((implementation->parameters.TrainingBaseFolder + "training-base.txt").c_str());

  if (!trainingBaseHeader.good()) {
    throw std::logic_error("Can't open training base file for write!");
  }

  trainingBaseHeader << featureCollection.size() << " " << featureCollection[0].second.size();
  trainingBaseHeader.close();

  const std::size_t featuresLength = featureCollection.size();

  for (std::size_t i = 0; i < featuresLength; ++i) {
    const std::size_t patchesLength = featureCollection[i].second.size();
    std::string folderPath = implementation->parameters.TrainingBaseFolder + toString(i) + "/";

    common::debug::log("Creating directory and saving feature %3d from %d (patches: %3d)\r",
                       i + 1,
                       featuresLength,
                       patchesLength);

    common::path::makeDir(folderPath);
    featureCollection[i].first.save(folderPath);

    for (std::size_t k = 0; k < patchesLength; ++k) {
      if (k == 0) {
        common::debug::log("\n");
      }

      std::string imagePath = folderPath + toString(k) + ".bmp";

      common::debug::log("Saving patch %3d from %d (%s)\r", k + 1, patchesLength, imagePath.c_str());

      if (!imwrite(imagePath, featureCollection[i].second[k])) {
        throw std::runtime_error("Couldn't save patch image.");
      }
    }

    if (patchesLength > 0) {
      common::debug::log("\n");
    }
  }
}

void RandomForestTracker::loadTrainingBaseFromFolder() {
  common::debug::log("Allocating new training base");
  implementation->deleteTrainingBase();
  implementation->trainingBase = new FeaturesCollection();

  common::debug::log("Reading training base from directory\n");
  std::ifstream trainingBaseHeader((implementation->parameters.TrainingBaseFolder + "training-base.txt").c_str());

  if (!trainingBaseHeader.good()) {
    throw std::logic_error("Can't open training base file for read!");
  }

  unsigned int featurePointsCount = 0;
  trainingBaseHeader >> featurePointsCount;

  if (featurePointsCount < 1) {
    throw std::logic_error("Invalid feature points amount read from header file!");
  }

  unsigned int patchesPerFeaturePoint = 0;
  trainingBaseHeader >> patchesPerFeaturePoint;

  if (patchesPerFeaturePoint < 1) {
    throw std::logic_error("Invalid patches per feature amount read from header file!");
  }

  trainingBaseHeader.close();

  common::debug::log("Reading patches for feature points\n");

  for (std::size_t i = 0; i < featurePointsCount; ++i) {
    std::string folderPath = implementation->parameters.TrainingBaseFolder + toString(i) + "/";

    if (!common::path::directoryExists(folderPath)) {
      throw std::logic_error("Specified patch folder is not avalable!");
    }

    implementation->trainingBase->push_back(FeatureWithFragments(Feature(folderPath), ImagesList()));

    common::debug::log("Loading feature %3d from %d (patches: %3d)\r",
                       i + 1,
                       featurePointsCount,
                       patchesPerFeaturePoint);

    for (std::size_t k = 0; k < patchesPerFeaturePoint; ++k) {
      if (k == 0) {
        common::debug::log("\n");
      }

      std::string imagePath = folderPath + toString(k) + ".bmp";

      common::debug::log("Loading patch %3d from %d (%s)\r", k + 1, patchesPerFeaturePoint, imagePath.c_str());
      implementation->trainingBase->back().second.push_back(cv::imread(imagePath, CV_LOAD_IMAGE_GRAYSCALE));

      if (implementation->trainingBase->back().second.back().empty()) {
        throw std::logic_error("Couldn't load patch image!");
      }
    }

    if (patchesPerFeaturePoint > 0) {
      common::debug::log("\n");
    }
  }

  common::debug::log("Loaded feature points - amount: %d\n", implementation->trainingBase->size());
}

void RandomForestTracker::trainClassifier() {
  RandomForestBuilder randomForestBuilder(*(implementation->trainingBase), implementation->parameters);

  common::debug::log("Building random forest structure\n");

  randomForestBuilder.build();
  implementation->randomForest = randomForestBuilder.getRandomForest();
}

void RandomForestTracker::createClassificationResultImage(
                              const cv::Mat& initial,
                              const cv::Mat& frame,
                              cv::Mat& output) {
  const cv::Size resultImageSize(initial.cols + frame.cols, std::max(initial.rows, frame.rows));
  cv::Mat resultImage = cv::Mat(resultImageSize, CV_8UC(frame.channels()));

  const cv::Rect wholeInputImageRect(0, 0, frame.cols - 1, frame.rows - 1);
  cv::Mat frameROI = frame(wholeInputImageRect);

  const cv::Rect wholeInputImageRectInResultImage = cvRect(initial.cols, 0, frame.cols - 1, frame.rows - 1);
  cv::Mat resultImageROI = resultImage(wholeInputImageRectInResultImage);

  resultImageROI = frameROI;

  const cv::Rect wholeInitialImageRect(0, 0, initial.cols - 1, initial.rows - 1);
  cv::Mat initialROI = initial(wholeInitialImageRect);

  const cv::Rect wholeInitialImageRectInResultImage(0, 0, initial.cols - 1, initial.rows - 1);
  resultImageROI = resultImage(wholeInitialImageRectInResultImage);

  resultImageROI = initialROI;
}

void RandomForestTracker::classifyPatchesFromCollection(
                              const FeaturesCollection& featuresStore,
                              std::vector<PairsContainer>& results) {
  results.insert(results.begin(), featuresStore.size(), PairsContainer());

  for (std::size_t i = 0; i < featuresStore.size(); ++i) {
    for (std::size_t k = 0; k < featuresStore[i].second.size(); ++k) {
      ClassesAndPropabilities classificationResult;

      classificationResult.first = -1;
      classificationResult.second = 0;

      implementation->randomForest->classify(
                                      featuresStore[i].second[k],
                                      classificationResult.first,
                                      classificationResult.second);

      results[i].push_back(classificationResult);
    }
  }
}

void RandomForestTracker::drawFeaturePointsCorrespondence(
                              std::vector<Feature>& trainingBaseFeaturePoints,
                              const FeaturesCorrespondence& correspondence,
                              const cv::Mat& initial,
                              cv::Mat& frame) const {
  for (std::size_t i = 0; i < correspondence.size(); ++i) {
    const cv::Point initialImageFeaturePoint = correspondence[i].first.getPoint();
    const cv::Point rawInputImageFeaturePoint = correspondence[i].second.getPoint();

    const cv::Point inputImageFeaturePoint(
                        rawInputImageFeaturePoint.x + initial.rows - 1,
                        rawInputImageFeaturePoint.y);

    if (rawInputImageFeaturePoint.x > frame.rows - 1 || rawInputImageFeaturePoint.x < 0 ||
        rawInputImageFeaturePoint.y > frame.cols - 1 || rawInputImageFeaturePoint.y < 0 ||
        initialImageFeaturePoint.x > frame.rows - 1 || initialImageFeaturePoint.x < 0 ||
        initialImageFeaturePoint.y > frame.cols - 1 || initialImageFeaturePoint.y < 0) {
      throw std::logic_error("Feature correspondence is out of image!");
    }

    cv::line(frame, initialImageFeaturePoint, inputImageFeaturePoint, cv::Scalar(255, 255, 255));
  }
}

void RandomForestTracker::makeFeaturePointCorrespondence(
                            const std::vector<Feature>& initialFeaturePoints,
                            const FeaturesCollection& inputImageFeatureCollection,
                            const std::vector<PairsContainer>& classifiedPatches,
                            FeaturesCorrespondence& correspondence) const {
  std::vector<cv::Point2f> initialKeyPoints;
  std::vector<cv::Point2f> inputKeyPoints;

  for (std::size_t i = 0; i < classifiedPatches.size(); ++i) {
    const ClassesAndPropabilities classificationResult = classifiedPatches[i].front();

    if (classificationResult.first == - 1 ||
        classificationResult.second < implementation->parameters.MinimumClassificationConfidence) {
      continue;
    }

    const cv::Point inputRawKeyPoint = inputImageFeatureCollection[i].first.getPoint();
    const cv::Point initialRawKeyPoint = initialFeaturePoints[classificationResult.first].getPoint();

    const cv::Point2f inputKeyPoint(inputRawKeyPoint.x, inputRawKeyPoint.y);
    const cv::Point2f initialKeyPoint(initialRawKeyPoint.x, initialRawKeyPoint.y);

    initialKeyPoints.push_back(initialKeyPoint);
    inputKeyPoints.push_back(inputKeyPoint);
  }

  if (initialKeyPoints.size() < 4) {
    return;
  }

  std::vector<unsigned char> correspondenceMask(initialKeyPoints.size());
  cv::Mat foundHomography = cv::findHomography(initialKeyPoints, inputKeyPoints, correspondenceMask, CV_RANSAC, 5);

  for (std::size_t i = 0; i < correspondenceMask.size(); ++i) {
    if (correspondenceMask[i] == 1) {
      FeaturesPair currentCorrespondence(
        Feature(cv::Point(initialKeyPoints[i].x, initialKeyPoints[i].y)),
        Feature(cv::Point(inputKeyPoints[i].x, inputKeyPoints[i].y)));

      correspondence.push_back(currentCorrespondence);
    }
  }
}

void RandomForestTracker::loadFeaturePointsFromTrainigBase(std::vector<Feature>& loadedFeaturePoints) const {
  if (implementation->trainingBase != 0) {
    common::debug::log("Loading feature points from memory...");

    for (std::size_t i = 0; i < implementation->trainingBase->size(); ++i) {
      loadedFeaturePoints.push_back(Feature((*implementation->trainingBase)[i].first.getPoint()));
    }
  } else {
    common::debug::log("Loading feature points from disk...");

    std::ifstream trainingBaseHeader((implementation->parameters.TrainingBaseFolder + "training-base.txt").c_str());

    if (!trainingBaseHeader.good()) {
      throw std::logic_error("Couldn't load training-base.txt!");
    }

    int featurePointsCount = 0;
    trainingBaseHeader >> featurePointsCount;

    if (featurePointsCount < 1) {
      throw std::logic_error("Invalid feature points count loaded from training-base.txt!");
    }

    trainingBaseHeader.close();

    for (int i = 0; i < featurePointsCount; ++i) {
      std::string folderPath = implementation->parameters.TrainingBaseFolder + toString(i) + "/";

      if (!common::path::directoryExists(folderPath)) {
        throw std::logic_error("Patch folder doesn't exist!");
      }

      loadedFeaturePoints.push_back(Feature(folderPath));
    }
  }

  common::debug::log("Feature points loaded (amount: %d)", loadedFeaturePoints.size());
}

void RandomForestTracker::classifyImage(const cv::Mat& initial, const cv::Mat& frame, cv::Mat& output) {
  if (initial.channels() != frame.channels() || initial.channels() != 1) {
    throw std::logic_error("Initial and input images are not alike (depth, channels count)!");
  }

  createClassificationResultImage(initial, frame, output);

  FeaturePointsExtractor featurePointsExtractor(1000, implementation->parameters.HalfPatchSize);
  featurePointsExtractor.generateFeaturePointsFromSingleImage(frame);

  FeaturesCollection inputImageFeaturesCollection = featurePointsExtractor.getFeatures();

  common::debug::log("Input image feature points count %d \n", inputImageFeaturesCollection.size());
  common::debug::log("Input image patch per feature point %d \n", inputImageFeaturesCollection[0].second.size());

  std::vector<PairsContainer> results;
  classifyPatchesFromCollection(inputImageFeaturesCollection, results);

  std::vector<Feature> loadedFeaturePoints;
  loadFeaturePointsFromTrainigBase(loadedFeaturePoints);

  FeaturesCorrespondence correspondence;
  makeFeaturePointCorrespondence(loadedFeaturePoints, inputImageFeaturesCollection, results, correspondence);

  drawFeaturePointsCorrespondence(loadedFeaturePoints, correspondence, initial, output);
}

bool RandomForestTracker::isTrainingBaseAvailable() const {
  return common::path::directoryExists(implementation->parameters.TrainingBaseFolder);
}

// Protected methods.
void RandomForestTracker::classifierInitialization() {
  common::debug::log("Initializating classifier\n");

  if (!isTrainingBaseAvailable()) {
    common::debug::log("Creating training base\n");
    generateTrainingBase();

    common::debug::log("Writting training base to separate directory\n");
    writeTrainingBaseToFolder();
  }

  common::debug::log("Loading training base from directory\n");
  loadTrainingBaseFromFolder();

  common::debug::log("Training classifier\n");
  trainClassifier();
}

void RandomForestTracker::readPointsFromKeypointFile(const std::string& fileName, std::vector<cv::Point>& points) {
  std::string keypointsFilename = common::path::extractFileName(fileName) + ".keypoints";

  if (common::path::fileExists(keypointsFilename)) {
    std::ifstream input(keypointsFilename.c_str());

    unsigned int n = 0;
    double x = 0.0, y = 0.0, radius = 0.0;

    input >> radius;
    input >> n;

    while (n > 0) {
      input >> x >> y;
      points.push_back(cv::Point(x, y));

      points.push_back(cv::Point(x + radius, y));
      points.push_back(cv::Point(x - radius, y));

      points.push_back(cv::Point(x, y + radius));
      points.push_back(cv::Point(x, y - radius));

      points.push_back(cv::Point(x + radius, y + radius));
      points.push_back(cv::Point(x + radius, y - radius));

      points.push_back(cv::Point(x - radius, y + radius));
      points.push_back(cv::Point(x - radius, y - radius));

      --n;
    }

    input.close();
  }
}

// Public methods.
void RandomForestTracker::process(cv::Mat& frame) {
  cv::cvtColor(frame, actualGrayFrame, CV_BGR2GRAY);
  cv::GaussianBlur(actualGrayFrame, smoothedGrayFrame, GaussianKernelSize, 0.0, 0.0);

  classifyImage(initialImage, smoothedGrayFrame, frame);
}

void RandomForestTracker::fill(const std::vector<std::string>& arguments) {
  if (!implementation->parameters.isValid()) {
    throw std::logic_error("Provided parameters for classificator are insufficient!");
  }
}

void RandomForestTracker::handleFirstFrame(const cv::Mat& firstFrame) {
  implementation->parameters.InitialImage = firstFrame.clone();
}

void RandomForestTracker::handleMovieName(const std::string& movieName) {
  implementation->setPath(movieName);

  // Cropping first frame by keypoints file (if keypoint file exists).
  std::vector<cv::Point> points;
  readPointsFromKeypointFile(movieName, points);

  if (points.size() > 0) {
    cv::Rect boundingRect = cv::boundingRect(points);
    implementation->parameters.InitialImage = implementation->parameters.InitialImage(boundingRect);

    cv::cvtColor(implementation->parameters.InitialImage, initialImage, CV_BGR2GRAY);
  }
}

void RandomForestTracker::afterInitialization() {
  // One-time classifier initialization.
  classifierInitialization();
}