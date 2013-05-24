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
typedef std::vector<DecisionTree*> Trees;

class RandomTreeBuilder {
  public:
    RandomTreeBuilder(
      const FeaturesCollection& featuresCollection,
      const std::vector<int>& trainingIndices,
      const ClassificatorParameters& classificatorParameters);

    void build();

    DecisionTree* getTreeStructure();

    // Internal class.
    struct NodeParameters {
      cv::Point FirstPoint;
      cv::Point SecondPoint;
      int IntensityThreshold;

      NodeParameters() {
        FirstPoint = cv::Point(-1, -1);
        SecondPoint = cv::Point(-1, -1);
        IntensityThreshold = -1;
      }

      NodeParameters& operator=(const NodeParameters& other) {
        FirstPoint = other.FirstPoint;
        SecondPoint = other.SecondPoint;
        IntensityThreshold = other.IntensityThreshold;

        return *this;
      }

      bool IsValid() const {
        const cv::Point invalidPoint = cv::Point(-1, -1);

        return !(FirstPoint == invalidPoint) && !(SecondPoint == invalidPoint) && IntensityThreshold >= 0;
      }
    };

  private:
    const int trainingSetSize;
    const int patchSize;
    const int patchCountPerFeaturePoint;

    DecisionTree* resultTreeStructure;

    const FeaturesCollection& featuresCollection;
    const std::vector<int>& trainingIndices;
    const ClassificatorParameters& classificatorParameters;

    void addNode(
          const DecisionNode* parent,
          DecisionNode::TestResult childNumber,
          const std::vector<int>& patchIndices,
          int treeHeight,
          bool forceLeaf = false);

    bool arePatchesDominatedBySingleFeaturePoint(const std::vector<int>& patchIndices) const;
    bool findBestNodeTest(NodeParameters& result, const std::vector<int>& patchIndices) const;

    int calculateCorrespondingFeaturePointIndex(int globalPatchNumber) const;
    int calculateInternalPatchIndex(int patchIndex, int featureIndex) const;

    void filterIndicesByTestResult(
          const DecisionNode& tester,
          const std::vector<int>& patchIndices,
          DecisionNode::TestResult targetTestResult,
          std::vector<int>& filteredIndices) const;

    double calculateInformationGain(const std::vector<int>& patchIndices, const NodeParameters& params) const;
    double calculateEntropy(const std::vector<int>& patchIndices) const;
};

#endif