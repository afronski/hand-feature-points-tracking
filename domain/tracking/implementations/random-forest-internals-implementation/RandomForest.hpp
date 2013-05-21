#ifndef __RANDOM_FOREST_HPP__
#define __RANDOM_FOREST_HPP__

#include <vector>

#include "Tree.hpp"
#include "DecisionNode.hpp"

typedef Tree<DecisionNode, DecisionNode::TestResultEnumSize> DecisionTree;
typedef std::vector<DecisionTree> Trees;

class RandomForest {
  public:
    RandomForest(Trees randomTrees, int classCount);

  private:
    Trees randomTrees;
    int classCount;
};

#endif