#ifndef __FEATURE_POINTS_EXTRACTOR_HPP__
#define __FEATURE_POINTS_EXTRACTOR_HPP__

#include "Feature.hpp"

typedef std::vector<cv::Mat> ImagesList;
typedef std::pair<Feature, ImagesList> FeatureWithFragments;
typedef std::vector<FeatureWithFragments> FeaturesCollection;

class FeaturePointsExtractor {
  public:
    FeaturePointsExtractor(int pointsCount, int patchSize):
      featurePointsCount(pointsCount), halfPatchSize(patchSize) {
    }

    void generateFeaturePoints(const cv::Mat& initialImage) {};
    void getFeatures(FeaturesCollection* featuresCollection) {};

  private:
    int featurePointsCount;
    int halfPatchSize;
};

#endif