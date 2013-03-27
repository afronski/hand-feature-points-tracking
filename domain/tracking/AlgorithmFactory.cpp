#include <iostream>

#include "PointsMarker.hpp"
#include "AlgorithmFactory.hpp"

static const std::string names[] = {
  PointsMarker::Name
};

static size_t names_length = sizeof(names) / sizeof(names[0]);

AlgorithmFactory::AlgorithmsCollection AlgorithmFactory::algorithms(names, names + names_length);

PointsAwareFrameTransformer* AlgorithmFactory::createAlgorithm(const std::string& method) {
  if (method == PointsMarker::Name) {
    return new PointsMarker();
  }

  return 0;
}

bool AlgorithmFactory::isAlgorithmPresent(const std::string& method) {
  return std::find(algorithms.begin(), algorithms.end(), method) != algorithms.end();
}

void AlgorithmFactory::listAllAlgorithms() {
  for (AlgorithmsCollection::const_iterator it = algorithms.begin(); it != algorithms.end(); ++it) {
    std::cout << *it << std::endl;
  }
}