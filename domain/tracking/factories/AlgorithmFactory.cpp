#include <iostream>

#include "../implementations/PointsMarker.hpp"
#include "../implementations/LucasKanadeTracker.hpp"
#include "../implementations/BackgroundExtractor.hpp"

#include "AlgorithmFactory.hpp"

static const std::string names[] = {
  PointsMarker::Name,
  LucasKanadeTracker::Name,
  BackgroundExtractor::Name
};

static size_t names_length = sizeof(names) / sizeof(names[0]);

AlgorithmFactory::AlgorithmsCollection AlgorithmFactory::algorithms(names, names + names_length);

PointsAwareFrameTransformer* AlgorithmFactory::createAlgorithm(const std::string& method) {
  if (method == PointsMarker::Name) {
    return new PointsMarker();
  }

  if (method == LucasKanadeTracker::Name) {
    return new LucasKanadeTracker();
  }

  if (method == BackgroundExtractor::Name) {
    return new BackgroundExtractor();
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