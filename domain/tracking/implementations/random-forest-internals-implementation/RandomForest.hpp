#ifndef __RANDOM_FOREST_HPP__
#define __RANDOM_FOREST_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

#include "Tree.hpp"
#include "DecisionNode.hpp"

typedef Tree<DecisionNode, DecisionNode::TestResultEnumSize> DecisionTree;
typedef std::vector<DecisionTree*> Trees;

class RandomForest {
  public:
    RandomForest(Trees randomTrees, int classCount);

    void classify(const cv::Mat& image, int& classNumber, double& confidence) const;

  private:
    Trees randomTrees;
    int classCount;

    void findMaximumElementWithIndex(const std::vector<double>& data, double& maximumValue, int& index) const;
};

#endif