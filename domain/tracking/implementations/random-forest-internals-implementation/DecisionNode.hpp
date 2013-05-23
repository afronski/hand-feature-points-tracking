#ifndef __DECISION_NODE_HPP__
#define __DECISION_NODE_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

class DecisionNode {
  public:
    DecisionNode(const cv::Point& firstPoint, const cv::Point& secondPoint, int intensityThreshold);
    DecisionNode(int classCount);

    enum TestResult {
      LessThanThreshold = 0,
      MoreThanThreshold,
      AboutThreshold,

      TestResultEnumSize
    };

    TestResult test(const cv::Mat& image) const;

    void setHistogramData(int classNumber, int count = 1);
    const std::vector<int>& getHistogram() const;

    int getHistogramElementsCount() const { return histogramElementsCount; }

private:
  cv::Point firstPoint;
  cv::Point secondPoint;

  int intensityThreshold;

  bool isLeafNode;
  int classCount;

  std::vector<int> classHistogram;
  int histogramElementsCount;
};

#endif