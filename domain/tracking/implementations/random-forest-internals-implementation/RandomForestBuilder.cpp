#include "../../../common/debug.hpp"

#include "RandomTreeBuilder.hpp"
#include "RandomForestBuilder.hpp"

// Constructor and destructor.
RandomForestBuilder::RandomForestBuilder(
                        const FeaturesCollection& features,
                        const ClassificatorParameters& parameters):
  featuresCollection(features),
  classificatorParameters(parameters),
  forest(0)
{}

// Public methods.
void RandomForestBuilder::build() {
  common::debug::log("Creating %d random tree(s)\n", classificatorParameters.RandomTreesCount);

  Trees trainedRandomTrees;
  trainedRandomTrees.reserve(classificatorParameters.RandomTreesCount);

  for (int i = 0; i < classificatorParameters.RandomTreesCount; ++i) {
    std::vector<int> trainingSet;
    std::vector<int> outOfBagSet;

    generateBootStrap(trainingSet, outOfBagSet);

    RandomTreeBuilder treeBuilder(featuresCollection, trainingSet, classificatorParameters);
    treeBuilder.build();

    trainedRandomTrees.push_back(treeBuilder.getTreeStructure());
    common::debug::log("[%d] Trained Random Tree...\n", i + 1);
  }

  cleanUp();
  forest = new RandomForest(trainedRandomTrees, featuresCollection.size());
}

void RandomForestBuilder::cleanUp() {
  if (forest != 0) {
    delete forest;
    forest = 0;
  }
}

// Private methods.
void RandomForestBuilder::generateBootStrap(std::vector<int>& set, std::vector<int>& outOfBagSet) const {
  srand(time(0));

  const std::size_t trainingSetSize = featuresCollection.size() * featuresCollection.front().second.size();

  std::vector<bool> usedTrainingElements;
  usedTrainingElements.insert(usedTrainingElements.begin(), trainingSetSize, false);

  for (std::size_t i = 0; i < trainingSetSize; ++i) {
    set.push_back(rand() % trainingSetSize);
    usedTrainingElements[set.back()] = true;
  }

  for (std::size_t i = 0; i < trainingSetSize; ++i) {
    if (usedTrainingElements[i] == false) {
      outOfBagSet.push_back(i);
    }
  }
}