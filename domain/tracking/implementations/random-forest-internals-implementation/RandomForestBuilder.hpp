#ifndef __RANDOM_FOREST_BUILDER_HPP__
#define __RANDOM_FOREST_BUILDER_HPP__

#include <opencv2/core/core.hpp>

#include "Feature.hpp"
#include "RandomForest.hpp"
#include "ClassificatorParameters.hpp"

typedef std::vector<cv::Mat> ImagesList;
typedef std::pair<Feature, ImagesList> FeatureWithFragments;
typedef std::vector<FeatureWithFragments> FeaturesCollection;

class RandomForestBuilder {
  public:
    RandomForestBuilder(const FeaturesCollection& features, const ClassificatorParameters& parameters);

    void build();
    RandomForest getRandomForest() const { return forest; }

  private:
    RandomForest forest;

    FeaturesCollection featuresCollection;
    ClassificatorParameters classificatorParameters;
};

#endif