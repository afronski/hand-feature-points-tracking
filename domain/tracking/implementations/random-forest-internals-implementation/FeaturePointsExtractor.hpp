#ifndef __FEATURE_POINTS_EXTRACTOR_HPP__
#define __FEATURE_POINTS_EXTRACTOR_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

#include "Feature.hpp"
#include "AffineTransformation.hpp"

typedef std::vector<cv::Mat> ImagesList;
typedef std::vector<Feature> FeaturesStore;
typedef std::pair<Feature, ImagesList> FeatureWithFragments;
typedef std::vector<FeatureWithFragments> FeaturesCollection;
typedef std::pair<cv::Mat, AffineTransformation> ImageAndTransformation;
typedef std::vector<ImageAndTransformation> ImageAndTransformationStore;

class FeaturePointsExtractor {
  public:
    FeaturePointsExtractor(int pointsCount, int patchSize);

    void generateFeaturePoints(const cv::Mat& initialImage);
    FeaturesCollection getFeatures() const;

  private:
    int featurePointsCount;
    int halfPatchSize;

    ImageAndTransformationStore intermediateData;
    FeaturesCollection features;

    void extractFeaturesFromTransformedImages(
                                const cv::Mat& initialImage,
                                FeaturesStore& features,
                                ImageAndTransformationStore imagesWithTransformations);

    void extractFeaturePointsFromImage(
                                const cv::Mat& image,
                                AffineTransformation* transformation,
                                FeaturesStore& features);

    void filterCorrectedOutImageFeaturePoints(const cv::Size& boundaries, FeaturesStore& features);
    void filterRareFeaturePoints(FeaturesStore& features);

    void extractPatches(const ImageAndTransformationStore& intermediateData, FeaturesStore& features);

};

#endif