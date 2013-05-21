#include "../../../common/debug.hpp"

#include "RandomForest.hpp"

RandomForest::RandomForest(Trees randomTrees, int classCount):
  randomTrees(randomTrees),
  classCount(classCount)
{}