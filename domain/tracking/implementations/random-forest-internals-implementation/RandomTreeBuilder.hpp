#ifndef __RANDOM_TREE_BUILDER_HPP__
#define __RANDOM_TREE_BUILDER_HPP__

#include <opencv2/core/core.hpp>

#include "Tree.hpp"
#include "DecisionNode.hpp"

#include "Feature.hpp"
#include "RandomForest.hpp"
#include "ClassificatorParameters.hpp"

typedef std::vector<cv::Mat> ImagesList;
typedef std::pair<Feature, ImagesList> FeatureWithFragments;
typedef std::vector<FeatureWithFragments> FeaturesCollection;

typedef Tree<DecisionNode, DecisionNode::TestResultEnumSize> DecisionTree;
typedef std::vector<DecisionTree> Trees;

class RandomTreeBuilder {
  public:
    RandomTreeBuilder(
      const FeaturesCollection& featuresCollection,
      const std::vector<int>& trainingIndices,
      const ClassificatorParameters& classificatorParameters);

    void build();

    DecisionTree getTreeStructure();

  private:
    const FeaturesCollection& featuresCollection;
    const std::vector<int>& trainingIndices;
    const ClassificatorParameters& classificatorParameters;
};

#endif