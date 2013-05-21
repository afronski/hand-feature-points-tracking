#include "RandomTreeBuilder.hpp"

// Constructor.
RandomTreeBuilder::RandomTreeBuilder(
                      const FeaturesCollection& featuresCollection,
                      const std::vector<int>& trainingIndices,
                      const ClassificatorParameters& classificatorParameters):
  featuresCollection(featuresCollection),
  trainingIndices(trainingIndices),
  classificatorParameters(classificatorParameters)
{}

// Public methods.
void RandomTreeBuilder::build() {

}

DecisionTree RandomTreeBuilder::getTreeStructure() {
  return DecisionTree();
}