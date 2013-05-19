#ifndef __CLASSIFICATOR_PARAMETERS_HPP__
#define __CLASSIFICATOR_PARAMETERS_HPP__

#include <string>

class ClassificatorParameters {
  public:
    std::string InitialImagePath;
    std::string TrainingBaseFolder;

    int FeaturePointsCount;
    int HalfPatchSize;

    int MaximumTreeHeight;
    int MinimumElementPerNode;

    int RandomTreesCount;
    double MinimumClassificationConfidence;

    int ClassifierIntensityThreshold;
    int GeneratedRandomPointsCount;

    ClassificatorParameters() {
      FeaturePointsCount = -1;
      HalfPatchSize = -1;

      MaximumTreeHeight = -1;
      MinimumElementPerNode = -1;

      RandomTreesCount = -1;
      MinimumClassificationConfidence = -1;

      ClassifierIntensityThreshold = -1;
      GeneratedRandomPointsCount = -1;
    }

    bool isValid() const {
      return FeaturePointsCount > 0 && HalfPatchSize > 0 && MaximumTreeHeight > 0 &&
             MinimumElementPerNode > 0 && RandomTreesCount > 0 && MinimumClassificationConfidence >= 0 &&
             ClassifierIntensityThreshold >= 0 && GeneratedRandomPointsCount > 0;
    }
};

#endif