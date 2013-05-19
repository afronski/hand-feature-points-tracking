#include <iostream>

#include "../implementations/PointsMarker.hpp"
#include "../implementations/SparseOpticalFlowTracker.hpp"
#include "../implementations/DenseOpticalFlowTracker.hpp"
#include "../implementations/RandomForestTracker.hpp"

#include "AlgorithmFactory.hpp"

static const std::string names[] = {
  PointsMarker::Name,
  SparseOpticalFlowTracker::Name,
  DenseOpticalFlowTracker::Name,
  RandomForestTracker::Name
};

static size_t names_length = sizeof(names) / sizeof(names[0]);

AlgorithmFactory::AlgorithmsCollection AlgorithmFactory::algorithms(names, names + names_length);

ArgumentsAwareFrameTransformer* AlgorithmFactory::createAlgorithm(const std::string& method) {
  if (method == PointsMarker::Name) {
    return new PointsMarker();
  }

  if (method == SparseOpticalFlowTracker::Name) {
    return new SparseOpticalFlowTracker();
  }

  if (method == DenseOpticalFlowTracker::Name) {
    return new DenseOpticalFlowTracker();
  }

  if (method == RandomForestTracker::Name) {
    return new RandomForestTracker();
  }

  return 0;
}

bool AlgorithmFactory::isAlgorithmPresent(const std::string& method) {
  return std::find(algorithms.begin(), algorithms.end(), method) != algorithms.end();
}

void AlgorithmFactory::listAllAlgorithms() {
  for (AlgorithmsCollection::const_iterator it = algorithms.begin(); it != algorithms.end(); ++it) {
    if (it->find("testing") == std::string::npos) {
      std::cout << *it << std::endl;
    }
  }
}