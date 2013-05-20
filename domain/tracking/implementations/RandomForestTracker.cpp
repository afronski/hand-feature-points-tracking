#include <fstream>
#include <sstream>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../../common/vision.hpp"
#include "../../common/path.hpp"

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
    parameters.TrainingBaseFolder = "bin/training-base-directory/";

    parameters.FeaturePointsCount = 200;
    parameters.HalfPatchSize = 10;

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

  featureExtractor.generateFeaturePoints(implementation->parameters.InitialImage);

  implementation->deleteTrainingBase();
  implementation->trainingBase = new FeaturesCollection(featureExtractor.getFeatures());
}

void RandomForestTracker::writeTrainingBaseToFolder() {
  if (!implementation->trainingBase || implementation->trainingBase->empty()) {
    throw std::logic_error("Can't write empty training base!");
  }

  const FeaturesCollection& featureCollection = *implementation->trainingBase;
  common::path::makeDir(implementation->parameters.TrainingBaseFolder);

  std::ofstream trainingBaseHeader((implementation->parameters.TrainingBaseFolder + "training-base.txt").c_str());

  if (!trainingBaseHeader.good()) {
    throw std::logic_error("Can't open training base file for write!");
  }

  trainingBaseHeader << featureCollection.size() << " " << featureCollection[0].second.size();
  trainingBaseHeader.close();

  for (std::size_t i = 0; i < featureCollection.size(); ++i) {
    std::string folderPath = implementation->parameters.TrainingBaseFolder + toString(i) + "/";
    common::path::makeDir(folderPath);

    featureCollection[i].first.save(folderPath);

    for (std::size_t k = 0; k < featureCollection[i].second.size(); ++k) {
      std::string imagePath = folderPath + toString(k) + ".bmp";

      if (!imwrite(imagePath, featureCollection[i].second[k])) {
        throw std::runtime_error("Couldn't save patch image.");
      }
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
  if (!isTrainingBaseAvailable()) {
    generateTrainingBase();
    writeTrainingBaseToFolder();
  }

  loadTrainingBaseFromFolder();
  trainClassifier();
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
}

void RandomForestTracker::afterInitialization() {
  // One-time classifier initialization.
  classifierInitialization();
}