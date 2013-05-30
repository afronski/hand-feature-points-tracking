#include <cstdlib>
#include <ctime>
#include <cmath>

#include "../../../common/debug.hpp"
#include "../../../common/floating-point-numbers.hpp"

#include "RandomTreeBuilder.hpp"

// Helpers.
int calculateTrainingSetSize(const FeaturesCollection& featuresCollection) {
  const int patchesPerFeaturePoint = featuresCollection.front().second.size();

  return featuresCollection.size() * patchesPerFeaturePoint;
}

int calculatePatchCountPerFeaturePoint(const FeaturesCollection& featuresCollection) {
  const int patchCountPerFeaturePoint = featuresCollection.front().second.size();

  return patchCountPerFeaturePoint;
}

int findPatchSize(const FeaturesCollection& featuresCollection) {
  const int patchSize = featuresCollection.front().second.front().cols;

  return patchSize;
}

void createFeatureIndicesHistogram(
      const std::vector<int>& featureIndicesOccurency,
      std::vector<int>& featureIndexHistogram) {
  featureIndexHistogram.clear();
  featureIndexHistogram.reserve(featureIndicesOccurency.size());

  int currentFeatureIndex = -1;

  for (std::size_t i = 0; i < featureIndicesOccurency.size(); ++i) {
    if (currentFeatureIndex != featureIndicesOccurency[i]) {
      featureIndexHistogram.push_back(1);
      currentFeatureIndex = featureIndicesOccurency[i];
    } else {
      ++featureIndexHistogram.back();
    }
  }
}

// Constructor and destructor.
RandomTreeBuilder::RandomTreeBuilder(
                      const FeaturesCollection& featuresCollection,
                      const std::vector<int>& trainingIndices,
                      const ClassificatorParameters& classificatorParameters):
  trainingSetSize(calculateTrainingSetSize(featuresCollection)),
  patchSize(2 * classificatorParameters.HalfPatchSize + 1),
  patchCountPerFeaturePoint(calculatePatchCountPerFeaturePoint(featuresCollection)),
  resultTreeStructure(new DecisionTree()),
  featuresCollection(featuresCollection),
  trainingIndices(trainingIndices),
  classificatorParameters(classificatorParameters)
{}

// Public methods.
void RandomTreeBuilder::build() {
  addNode(0, DecisionNode::TestResultEnumSize, trainingIndices, 1, false);
}

DecisionTree* RandomTreeBuilder::getTreeStructure() {
  return resultTreeStructure;
}

// Private methods.
double RandomTreeBuilder::calculateEntropy(const std::vector<int>& patchIndices) const
{
  const int patchesCount = patchIndices.size();

  std::vector<int> featureIndicesOccurency;
  featureIndicesOccurency.reserve(patchesCount);

  for (int i = 0; i < patchesCount; ++i) {
    featureIndicesOccurency.push_back(calculateCorrespondingFeaturePointIndex(patchIndices[i]));
  }

  std::vector<int> featureIndicesHistogram;
  std::sort(featureIndicesOccurency.begin(), featureIndicesOccurency.end());
  createFeatureIndicesHistogram(featureIndicesOccurency, featureIndicesHistogram);

  double entropy = 0;
  const double log10Of2 = std::log(2);

  for (std::size_t i = 0; i < featureIndicesHistogram.size(); ++i) {
    const double classFrequency = static_cast<double>(featureIndicesHistogram[i]) / patchesCount;

    entropy -= classFrequency * std::log(classFrequency) / log10Of2;
  }

  return entropy;
}

bool RandomTreeBuilder::arePatchesDominatedBySingleFeaturePoint(const std::vector<int>& patchIndices) const {
  std::vector<int> featureIndicesOccurency;

  for (std::size_t i = 0; i < patchIndices.size(); ++i) {
    featureIndicesOccurency.push_back(calculateCorrespondingFeaturePointIndex(patchIndices[i]));
  }

  std::sort(featureIndicesOccurency.begin(), featureIndicesOccurency.end());

  std::vector<int> featureIndicesHistogram;
  createFeatureIndicesHistogram(featureIndicesOccurency, featureIndicesHistogram);

  int maxElement = 0;
  int sumOfAllElements = 0;

  for (std::size_t i = 0; i < featureIndicesHistogram.size(); ++i) {
    if (maxElement < featureIndicesHistogram[i]) {
      maxElement = featureIndicesHistogram[i];
    }

    sumOfAllElements += featureIndicesHistogram[i];
  }

  const bool areDominated = 4 * maxElement >= 3 * sumOfAllElements;
  return areDominated;
}

void RandomTreeBuilder::addNode(
                          const DecisionNode* parent,
                          DecisionNode::TestResult childNumber,
                          const std::vector<int>& patchIndices,
                          int treeHeight,
                          bool forceLeaf) {
  DecisionNode* newNode = 0;

  const bool hasToAddLeafNode =
                patchIndices.size() <= static_cast<std::size_t>(classificatorParameters.MinimumElementPerNode) ||
                treeHeight >= classificatorParameters.MaximumTreeHeight ||
                forceLeaf ||
                arePatchesDominatedBySingleFeaturePoint(patchIndices);

  if (hasToAddLeafNode) {
    newNode = new DecisionNode(featuresCollection.size());

    for (std::size_t i = 0; i < patchIndices.size(); ++i) {
      newNode->setHistogramData(calculateCorrespondingFeaturePointIndex(patchIndices[i]));
    }

    common::debug::log("There are %d patch(es) at a leaf node\n", patchIndices.size());
  } else {
    NodeParameters nodeParameters;

    if (!findBestNodeTest(nodeParameters, patchIndices)) {
      addNode(parent, childNumber,  patchIndices, treeHeight, true);
      return;
    }

    newNode = new DecisionNode(
                    nodeParameters.FirstPoint,
                    nodeParameters.SecondPoint,
                    nodeParameters.IntensityThreshold);
  }

  resultTreeStructure->addNode(newNode, parent, childNumber);

  if (!hasToAddLeafNode) {
    for (int i = 0; i < DecisionNode::TestResultEnumSize; ++i) {
      std::vector<int> childIndices;

      filterIndicesByTestResult(*newNode, patchIndices, static_cast<DecisionNode::TestResult>(i), childIndices);
      addNode(newNode, static_cast<DecisionNode::TestResult>(i), childIndices, treeHeight + 1, false);
    }
  }
}

int RandomTreeBuilder::calculateCorrespondingFeaturePointIndex(int globalPatchNumber) const {
  return globalPatchNumber / patchCountPerFeaturePoint;
}

int RandomTreeBuilder::calculateInternalPatchIndex(int patchIndex, int featureIndex) const {
  return patchIndex - featureIndex * patchCountPerFeaturePoint;
}

double RandomTreeBuilder::calculateInformationGain(
                              const std::vector<int>& patchIndices,
                              const NodeParameters& params) const {
  const DecisionNode decisionNode(params.FirstPoint, params.SecondPoint, params.IntensityThreshold);
  const int intialPatchCount = patchIndices.size();

  double informationGain = calculateEntropy(patchIndices);
  std::vector<int> currentIndexSet;

  for (int i = 0; i < DecisionNode::TestResultEnumSize; ++i) {
    filterIndicesByTestResult(decisionNode, patchIndices, static_cast<DecisionNode::TestResult>(i), currentIndexSet);

    if (currentIndexSet.empty()) {
      continue;
    }

    const double currentFrequency = static_cast<double>( currentIndexSet.size() ) / intialPatchCount;
    const double currentSetEntropy = calculateEntropy( currentIndexSet );

    informationGain -=  currentFrequency *  currentSetEntropy;
  }

  return informationGain;
}


bool RandomTreeBuilder::findBestNodeTest(NodeParameters& result, const std::vector<int>& patchIndices) const {
  std::srand(std::time(0));

  double currentMaxInformationGain = -1;
  NodeParameters currentBestNodeParameters;

  int generatedUsefulParametersCount = 0;
  int generatedParametersCount = 0;

  const int maxGeneratedParametersCount = 3 * patchSize * patchSize / 4;

  while (generatedUsefulParametersCount <= classificatorParameters.GeneratedRandomPointsCount &&
         generatedParametersCount < maxGeneratedParametersCount ) {
    NodeParameters currentNodeParams;

    currentNodeParams.FirstPoint = cv::Point(std::rand() % patchSize, std::rand() % patchSize);
    currentNodeParams.SecondPoint = cv::Point(std::rand() % patchSize, std::rand() % patchSize);
    currentNodeParams.IntensityThreshold = classificatorParameters.ClassifierIntensityThreshold;

    const double currentInformationGain = calculateInformationGain(patchIndices, currentNodeParams);

    if (currentInformationGain > currentMaxInformationGain) {
      currentMaxInformationGain = currentInformationGain;
      currentBestNodeParameters = currentNodeParams;
    }

    if (currentInformationGain > 0) {
      generatedUsefulParametersCount++;
    }

    generatedParametersCount++;
  }

  common::debug::log("Finished because we found best node split:\n");
  common::debug::log("  currentMaxInformationGain = %f\n", currentMaxInformationGain);

  if (common::isLessOrEqualThanZero(currentMaxInformationGain)) {
    return false;
  }

  result = currentBestNodeParameters;
  return true;
}

void RandomTreeBuilder::filterIndicesByTestResult(
                            const DecisionNode& tester,
                            const std::vector<int>& patchIndices,
                            DecisionNode::TestResult targetTestResult,
                            std::vector<int>& filteredIndices) const {
  filteredIndices.clear();

  for (std::size_t i = 0; i < patchIndices.size(); ++i) {
    const int featureIndex = calculateCorrespondingFeaturePointIndex(patchIndices[i]);
    const int internalPatchIndex = calculateInternalPatchIndex(patchIndices[i], featureIndex);

    if (tester.test(featuresCollection[featureIndex].second[internalPatchIndex]) == targetTestResult) {
      filteredIndices.push_back(patchIndices[i]);
    }
  }
}