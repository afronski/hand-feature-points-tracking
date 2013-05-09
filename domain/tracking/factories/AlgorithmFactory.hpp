#ifndef __ALGORITHM_FACTORY_HPP__
#define __ALGORITHM_FACTORY_HPP__

#include <string>
#include <vector>

#include "../../common/FrameTransformers.hpp"

class AlgorithmFactory {
  public:
    static ArgumentsAwareFrameTransformer* createAlgorithm(const std::string& method);

    static bool isAlgorithmPresent(const std::string& method);
    static void listAllAlgorithms();

  private:
    typedef std::vector<std::string> AlgorithmsCollection;

    static AlgorithmsCollection algorithms;
};

#endif