#ifndef __RANDOM_FOREST_BUILDER_HPP__
#define __RANDOM_FOREST_BUILDER_HPP__

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
typedef std::vector<DecisionTree*> Trees;

class RandomForestBuilder {
  public:
    RandomForestBuilder(const FeaturesCollection& features, const ClassificatorParameters& parameters);

    void build();
    void cleanUp();

    RandomForest* getRandomForest() const { return forest; }

  private:
    void generateBootStrap(std::vector<int>& set, std::vector<int>& outOfBagSet) const;

    FeaturesCollection featuresCollection;
    ClassificatorParameters classificatorParameters;
    RandomForest* forest;
};

#endif