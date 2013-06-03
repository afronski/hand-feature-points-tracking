#include "DecisionNode.hpp"

// Constants.
const cv::Point NoPoint = cv::Point(-1, -1);
const int NotFound = -1;

// Constructors.
DecisionNode::DecisionNode(const cv::Point& firstPoint, const cv::Point& secondPoint, int intensityThreshold):
  firstPoint(firstPoint),
  secondPoint(secondPoint),
  intensityThreshold(intensityThreshold),
  isLeafNode(false),
  classCount(NotFound)
{}

DecisionNode::DecisionNode(int classCount):
  firstPoint(NoPoint),
  secondPoint(NoPoint),
  intensityThreshold(NotFound),
  isLeafNode(true),
  classCount(classCount),
  histogramElementsCount(0)
{
  classHistogram.insert(classHistogram.begin(), classCount, 0);
}

// Public methods.
DecisionNode::TestResult DecisionNode::test(const cv::Mat& image) const {
  unsigned char firstPointIntensity = image.at<unsigned char>(firstPoint.y, firstPoint.x);
  unsigned char secondPointIntensity = image.at<unsigned char>(secondPoint.y, secondPoint.x);

  const int intensityDifference = static_cast<int>(firstPointIntensity - secondPointIntensity);

  if (intensityDifference < -intensityThreshold) {
    return LessThanThreshold;
  } else if (intensityDifference > intensityThreshold) {
    return MoreThanThreshold;
  }

  return AboutThreshold;
}

void DecisionNode::setHistogramData(int classNumber, int count) {
  classHistogram[classNumber] += count;
  histogramElementsCount += count;
}

const std::vector<int>& DecisionNode::getHistogram() const {
  return classHistogram;
}