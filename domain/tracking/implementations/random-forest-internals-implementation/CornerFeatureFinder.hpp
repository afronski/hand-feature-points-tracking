#ifndef __CORNER_FEATURE_FINDER_HPP__
#define __CORNER_FEATURE_FINDER_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

#include "Feature.hpp"

typedef std::vector<Feature> FeaturesStore;

class CornerFeatureFinder {
  public:
    CornerFeatureFinder(int featuresCount);

    void findCorners(const cv::Mat& image, FeaturesStore& features);

  private:
    int maximumFeaturesCount;
};

#endif