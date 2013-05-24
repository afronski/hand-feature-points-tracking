#include "../../../common/debug.hpp"

#include "AffineTransformationsGenerator.hpp"
#include "CornerFeatureFinder.hpp"
#include "FeaturePointsExtractor.hpp"

// Constructor.
FeaturePointsExtractor::FeaturePointsExtractor(int pointsCount, int patchSize):
  featurePointsCount(pointsCount),
  halfPatchSize(patchSize) {}

// Internal classes.
class SortBySecondElementInPairPredicate {
  public:
    bool operator()(const std::pair<Feature, int>& lhs, const std::pair<Feature, int>& rhs) {
      return lhs.second < rhs.second;
    }
};

// Public methods.
void FeaturePointsExtractor::generateFeaturePoints(const cv::Mat& initialImage) {
  intermediateData.clear();

  FeaturesStore featurePointsStorage;
  extractFeaturesFromTransformedImages(initialImage, featurePointsStorage, intermediateData);

  cv::Size initialImageBoundary = cv::Size(initialImage.cols - 1, initialImage.rows - 1);

  common::debug::log("Filtering prepared features - Correcting by boundaries (%d)\n", featurePointsStorage.size());
  filterCorrectedOutImageFeaturePoints(initialImageBoundary, featurePointsStorage);

  common::debug::log("Filtering prepared features - Removing rare features (%d)\n", featurePointsStorage.size());
  filterRareFeaturePoints(featurePointsStorage);

  common::debug::log("Extracting patches from %d feature(s)\n", featurePointsStorage.size());
  extractPatches(intermediateData, featurePointsStorage);
}

void FeaturePointsExtractor::generateFeaturePointsFromSingleImage(const cv::Mat& image) {
  intermediateData.clear();

  FeaturesStore featurePointsStorage;
  AffineTransformation noTransformation(0, cv::Point(0, 0), 1, 1, 0, 0);

  extractFeaturePointsFromImage(image, &noTransformation, featurePointsStorage);

  ImageAndTransformationStore temporaryStore;
  cv::Mat initialImageCopy = image.clone();

  ImageAndTransformation intermediateDataItem = std::make_pair(initialImageCopy, noTransformation);
  temporaryStore.push_back(intermediateDataItem);

  const cv::Size initialImageBoundary = cv::Size(image.cols - 1, image.rows - 1);

  common::debug::log("Single image - Filtering corrected by boundaries (%d)\n", featurePointsStorage.size());
  filterCorrectedOutImageFeaturePoints(initialImageBoundary, featurePointsStorage);

  common::debug::log("Single image - Filtering rare features (%d)\n", featurePointsStorage.size());
  filterRareFeaturePoints(featurePointsStorage);

  common::debug::log("Single image - Extracting patches from %d feature(s)\n", featurePointsStorage.size());
  extractPatches(temporaryStore, featurePointsStorage);
}

FeaturesCollection FeaturePointsExtractor::getFeatures() const {
  return features;
}

// Private methods.
void FeaturePointsExtractor::filterCorrectedOutImageFeaturePoints(
                                const cv::Size& boundaries,
                                FeaturesStore& features) {
  int count = 0;

  for (int i = features.size() - 1; i >= 0; --i) {
    if (!features[i].correctPointByBoundary(boundaries)) {
      common::debug::log("Removing points outside of boundaries - amount: %d\r", count);

      features.erase(features.begin() + i);
      ++count;
    }
  }

  common::debug::log("\n");
}

void FeaturePointsExtractor::filterRareFeaturePoints(FeaturesStore& features) {
  typedef std::pair<Feature, int> Bucket;
  typedef std::vector<Bucket> Histogram;

  Histogram histogram;
  int count = 0;

  while (!features.empty()) {
    common::debug::log("Removing rare points - amount: %d\r", count);
    histogram.push_back(Bucket(features.back(), 1));
    features.pop_back();

    const Feature& currentFeature = histogram.back().first;

    for (int i = features.size() - 1; i >= 0; --i) {
      if (features[i] == currentFeature) {
        features.erase(features.begin() + i);
        histogram.back().second++;
        ++count;
      }
    }
  }

  common::debug::log("\n");

  SortBySecondElementInPairPredicate sortBySecondPartPredicate;
  std::sort(histogram.begin(), histogram.end(), sortBySecondPartPredicate);

  const std::size_t MaximumCount = static_cast<std::size_t>(featurePointsCount);

  for (std::size_t i = 0; i < histogram.size() && i < MaximumCount; ++i) {
    features.push_back(histogram[i].first);
  }
}

void FeaturePointsExtractor::extractPatches(
                                ImageAndTransformationStore& intermediateData,
                                FeaturesStore& featuresStorage) {

  common::debug::log("Extracting patches from intermediate data - amount: %d\n", intermediateData.size());

  for(std::size_t i = 0; i < featuresStorage.size(); ++i) {
    features.push_back(FeatureWithFragments(featuresStorage[i], ImagesList()));
    features.back().second.reserve(intermediateData.size());
  }

  int cachedSize = intermediateData.size();

  for(int i = cachedSize - 1; i >= 0; --i) {
    const cv::Mat& currentTransformedImage = intermediateData[i].first;
    const AffineTransformation& currentTransform = intermediateData[i].second;

    for(std::size_t k = 0; k < features.size(); ++k) {
      const cv::Point transformedPoint = currentTransform.transformPoint(features[k].first.getPoint());

      features[k].second.push_back(extractFeaturePointPatch(transformedPoint, currentTransformedImage));
      common::debug::log("Pushing patch to %3d from %3d position in intermediate data\r", k, i);
    }

    intermediateData.erase(intermediateData.begin() + i);
  }

  if (cachedSize > 0) {
    common::debug::log("\n");
  }
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
    features.push_back(currentFeature);
  }
}

void FeaturePointsExtractor::extractFeaturesFromTransformedImages(
                                const cv::Mat& initialImage,
                                FeaturesStore& features,
                                ImageAndTransformationStore& imagesWithTransformations) {
  cv::Point initialImageCenter;

  initialImageCenter.x = initialImage.cols / 2;
  initialImageCenter.y = initialImage.rows / 2;

  AffineTransformationsGenerator affineTransformationsGenerator(initialImageCenter);
  AffineTransformation* currentTransformation = 0;

  std::size_t i = 0;

  while (affineTransformationsGenerator.getNextTransformation(&currentTransformation)) {
    common::debug::log("Calculating transformation %d\r", ++i);

    cv::Mat* transformedImage = 0;
    cv::Mat* transformationMatrix = 0;

    currentTransformation->transformImage(initialImage, &transformedImage, &transformationMatrix);

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

  common::debug::log("\nGenerating transformations is finished - Features amount: %d\n",
                     features.size());
  common::debug::log("Generating transformations is finished - Intermediate data: %d\n",
                     imagesWithTransformations.size());
}

cv::Mat FeaturePointsExtractor::extractFeaturePointPatch(const Feature& featurePoint, const cv::Mat& image) const {
  const cv::Size pathSize = cv::Size(2 * halfPatchSize + 1, 2 * halfPatchSize + 1);
  cv::Mat patch(pathSize, CV_8UC(image.channels()));

  const cv::Point pathCenter = featurePoint.getPoint();
  const int channelCount = image.channels();

  for (int y = pathCenter.y - halfPatchSize, pathY = 0; y <= pathCenter.y + halfPatchSize; ++y, ++pathY) {
    uchar* pointer = 0;

    if (y >= 0 && y < image.rows) {
      pointer = (uchar*)(image.data + y * image.step);
    }

    uchar* patchPointer = (uchar*)(patch.data + pathY * patch.step);

    for(int x = pathCenter.x - halfPatchSize, patchX = 0; x <= pathCenter.x + halfPatchSize; ++x, ++patchX) {
      if (x < 0 || x >= image.cols || y < 0 || y >= image.rows) {
        for (int i = 0; i < channelCount; ++i) {
          patchPointer[channelCount * patchX + i] = 0;
        }
      } else {
        for (int i = 0; i < channelCount; ++i) {
          patchPointer[channelCount * patchX + i] = pointer[channelCount * x + i];
        }
      }
    }
  }

  return patch;
}