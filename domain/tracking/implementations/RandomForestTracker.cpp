#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>

#include <opencv2/highgui/highgui.hpp>

#include "../../common/vision.hpp"
#include "../../common/path.hpp"

#include "RandomForestTracker.hpp"

#include "random-forest-internals-implementation/ClassificatorParameters.hpp"

// Constants.
const std::string RandomForestTracker::Name = "Random Forest Tracker";

const cv::Scalar RandomForestTracker::DrawingColor = cv::Scalar(0, 255, 0);
const cv::Size RandomForestTracker::GaussianKernelSize = cv::Size(7, 7);

// PIMPL idiom implementation.
struct RandomForestTracker::PIMPL {
  ClassificatorParameters parameters;

  PIMPL() {
    parameters.InitialImagePath = "train.png";
    parameters.TrainingBaseFolder = "hand_features/";

    parameters.FeaturePointsCount = 200;
    parameters.HalfPatchSize = 10;

    parameters.MaximumTreeHeight = 30;
    parameters.MinimumElementPerNode = 100;

    parameters.RandomTreesCount = 30;
    parameters.MinimumClassificationConfidence = 0.1;

    parameters.ClassifierIntensityThreshold = 10;
    parameters.GeneratedRandomPointsCount = 30;
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

}

void RandomForestTracker::writeTrainingBaseToFolder() {

}
void RandomForestTracker::loadTrainingBaseFromFolder() {

}

void RandomForestTracker::trainClassifier() {

}

void RandomForestTracker::classifyImage(const cv::Mat& initial, const cv::Mat& frame, cv::Mat& output) {
}

// Protected methods.
void RandomForestTracker::classifierInitialization() {
  generateTrainingBase();

  writeTrainingBaseToFolder();
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