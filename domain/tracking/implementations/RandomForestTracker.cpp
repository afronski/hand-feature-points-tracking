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

// PIMPL idiom implementation.
struct RandomForestTracker::PIMPL {
  ClassificatorParameters parameters;
  FeaturesCollection* trainingBase;

  PIMPL() {
    parameters.TrainingBaseFolder = "training-base-directory/";

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
}

void RandomForestTracker::loadTrainingBaseFromFolder() {
}

void RandomForestTracker::trainClassifier() {
}

void RandomForestTracker::classifyImage(const cv::Mat& initial, const cv::Mat& frame, cv::Mat& output) {
}

bool RandomForestTracker::isTrainingBaseAvailable() const {
  return true;
}

// Protected methods.
void RandomForestTracker::classifierInitialization() {
  if (isTrainingBaseAvailable()) {
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

  cv::imshow("classification", frame);
  cv::waitKey(0);
}

void RandomForestTracker::fill(const std::vector<std::string>& arguments) {
  if (!implementation->parameters.isValid()) {
    throw new std::logic_error("Provided parameters for classificator are insufficient!");
  }

  // One-time classifier initialization.
  classifierInitialization();

  cv::namedWindow("classification", CV_WINDOW_AUTOSIZE);
}

void RandomForestTracker::handleFirstFrame(const cv::Mat& firstFrame) {
  implementation->parameters.InitialImage = firstFrame.clone();
}