#include "../../../common/debug.hpp"
#include "../../../common/floating-point-numbers.hpp"

#include "RandomForest.hpp"

// Constructor.
RandomForest::RandomForest(Trees randomTrees, int classCount):
  randomTrees(randomTrees),
  classCount(classCount)
{}

// Public methods.
void RandomForest::classify(const cv::Mat& image, int& classNumber, double& confidence) const {
  std::vector<double> histogramOfSums;

  histogramOfSums.clear();
  histogramOfSums.insert(histogramOfSums.begin(), classCount, 0);

  for (std::size_t i = 0; i < randomTrees.size(); ++i) {
    const DecisionTree* currentDecisionTree = randomTrees[i];

    TreeWalker<DecisionNode, DecisionNode::TestResultEnumSize> currentWalker(currentDecisionTree);
    currentWalker.setCurrentNode(currentDecisionTree->getRoot());

    while (!currentWalker.isLeafNode()) {
      const DecisionNode& node = currentWalker.getCurrentNode();
      const DecisionNode::TestResult childNumber = node.test(image);

      currentWalker.moveToChildNode(childNumber);
    }

    const DecisionNode& leafNode = currentWalker.getCurrentNode();
    const std::vector<int>& leafHistogram = leafNode.getHistogram();
    const int currentHistogramElementsCount = leafNode.getHistogramElementsCount();

    if (currentHistogramElementsCount == 0) {
      continue;
    }

    for (int k = 0; k < classCount; ++k) {
      histogramOfSums[k] += static_cast<double>(leafHistogram[k]) / currentHistogramElementsCount;
    }
  }

  double maximumElement = 0;
  findMaximumElementWithIndex(histogramOfSums, maximumElement, classNumber);

  if (common::isEqualToZero(maximumElement)) {
    classNumber = -1;
    confidence = 0;
    return;
  }

  confidence = maximumElement / randomTrees.size();
}

// Private methods.
void RandomForest::findMaximumElementWithIndex(
                      const std::vector<double>& data,
                      double& maximumValue,
                      int& index) const {
  double currentMaximumValue = DBL_MIN;
  int currentIndex = -1;

  for (std::size_t i = 0; i < data.size(); ++i) {
    if (currentMaximumValue <= data[i]) {
      currentMaximumValue = data[i];
      currentIndex = i;
    }
  }

  maximumValue = currentMaximumValue;
  index = currentIndex;
}