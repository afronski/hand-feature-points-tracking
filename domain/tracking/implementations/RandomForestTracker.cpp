#include <fstream>
#include <sstream>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../../common/vision.hpp"
#include "../../common/path.hpp"
#include "../../common/debug.hpp"

#include "RandomForestTracker.hpp"

#include "random-forest-internals-implementation/Feature.hpp"
#include "random-forest-internals-implementation/FeaturePointsExtractor.hpp"
#include "random-forest-internals-implementation/ClassificatorParameters.hpp"

// Aliases.
typedef std::vector<cv::Mat> ImagesList;
typedef std::pair<Feature, ImagesList> FeatureWithFragments;
typedef std::vector<FeatureWithFragments> FeaturesCollection;

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

  PIMPL() {
    trainingBase = 0;

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
}

void RandomForestTracker::trainClassifier() {
}

void RandomForestTracker::classifyImage(const cv::Mat& initial, const cv::Mat& frame, cv::Mat& output) {
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
  }
}

void RandomForestTracker::afterInitialization() {
  // One-time classifier initialization.
  classifierInitialization();
}