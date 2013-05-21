#ifndef __DECISION_NODE_HPP__
#define __DECISION_NODE_HPP__

class DecisionNode {
  public:
    enum TestResult {
      LessThanThreshold = 0,
      MoreThanThreshold,
      AboutThreshold,

      TestResultEnumSize
    };
};

#endif