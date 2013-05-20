#include "AffineTransformationsGenerator.hpp"
#include "CornerFeatureFinder.hpp"
#include "FeaturePointsExtractor.hpp"

// Constructor.
FeaturePointsExtractor::FeaturePointsExtractor(int pointsCount, int patchSize):
  featurePointsCount(pointsCount),
  halfPatchSize(patchSize) {}

// Public methods.
void FeaturePointsExtractor::generateFeaturePoints(const cv::Mat& initialImage) {
  intermediateData.clear();

  FeaturesStore featurePointsStorage;
  extractFeaturesFromTransformedImages(initialImage, featurePointsStorage, intermediateData );

  cv::Size initialImageBoundary = cv::Size(initialImage.rows - 1, initialImage.cols - 1);

  filterCorrectedOutImageFeaturePoints(initialImageBoundary, featurePointsStorage);
  filterRareFeaturePoints(featurePointsStorage);

  extractPatches(intermediateData, featurePointsStorage);
}

FeaturesCollection FeaturePointsExtractor::getFeatures() const {
  return features;
}

// Private methods.
void FeaturePointsExtractor::filterCorrectedOutImageFeaturePoints(
                                const cv::Size& boundaries,
                                FeaturesStore& features) {
// TODO
}

void FeaturePointsExtractor::filterRareFeaturePoints(FeaturesStore& features) {
// TODO
}

void FeaturePointsExtractor::extractPatches(
                                const ImageAndTransformationStore& intermediateData,
                                FeaturesStore& features) {
// TODO
}

void FeaturePointsExtractor::extractFeaturePointsFromImage(
                                const cv::Mat& image,
                                AffineTransformation* transformation,
                                FeaturesStore& features) {
  FeaturesStore foundFeatures;
  CornerFeatureFinder cornersFinder(featurePointsCount);

  cornersFinder.findCorners(image, foundFeatures);

  for (FeaturesStore::iterator it = foundFeatures.begin(); it != foundFeatures.end(); ++it) {
    Feature currentFeature(transformation->transformPoint(it->getPoint()));
    foundFeatures.push_back(currentFeature);
  }
}

void FeaturePointsExtractor::extractFeaturesFromTransformedImages(
                                const cv::Mat& initialImage,
                                FeaturesStore& features,
                                ImageAndTransformationStore imagesWithTransformations) {
  cv::Point initialImageCenter;

  initialImageCenter.x = initialImage.rows / 2;
  initialImageCenter.y = initialImage.cols / 2;

  AffineTransformationsGenerator affineTransformationsGenerator(initialImageCenter);
  AffineTransformation* currentTransformation = 0;

  while (affineTransformationsGenerator.getNextTransformation(currentTransformation)) {
    cv::Mat* transformedImage = 0;
    cv::Mat* transformationMatrix = 0;

    currentTransformation->transformImage(initialImage, *transformedImage, *transformationMatrix);

    cv::RNG rng = cv::RNG(0x12345);
    cv::Mat noise = transformedImage->clone();

    rng.fill(noise, cv::RNG::UNIFORM, 0, 50);
    cv::add(*transformedImage, noise, *transformedImage);

    AffineTransformation forIntermediate(*transformationMatrix, false);
    ImageAndTransformation intermediateDataItem = std::make_pair(transformedImage->clone(), forIntermediate);
    imagesWithTransformations.push_back(intermediateDataItem);

    AffineTransformation* inverseTransformation = new AffineTransformation(*transformationMatrix, true);
    extractFeaturePointsFromImage(imagesWithTransformations.back().first, inverseTransformation, features);

    delete inverseTransformation;

    delete transformationMatrix;
    transformationMatrix = 0;

    delete currentTransformation;
    currentTransformation = 0;
  }
}