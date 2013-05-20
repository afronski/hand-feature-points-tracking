#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>

#include "CornerFeatureFinder.hpp"

CornerFeatureFinder::CornerFeatureFinder(int featuresCount):
  maximumFeaturesCount(featuresCount)
{}

void CornerFeatureFinder::findCorners(const cv::Mat& image, FeaturesStore& features) {
  typedef std::vector<cv::Point2f> PointsCollection;

  PointsCollection corners;
  cv::goodFeaturesToTrack(image, corners, maximumFeaturesCount, 0.01, 2.0);

  if (corners.size() == 0) {
    return;
  }

  for (PointsCollection::iterator it = corners.begin(); it != corners.end(); ++it) {
    cv::Point corner;

    corner.x = static_cast<int>(it->x);
    corner.y = static_cast<int>(it->y);

    features.push_back(Feature(corner));
  }
}