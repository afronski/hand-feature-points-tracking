#ifndef __RANDOM_FOREST_TRACKER_HPP__
#define __RANDOM_FOREST_TRACKER_HPP__

#include <opencv2/core/core.hpp>

#include "../../common/FrameTransformers.hpp"

#include "random-forest-internals-implementation/Feature.hpp"

typedef std::vector<cv::Mat> ImagesList;
typedef std::pair<Feature, ImagesList> FeatureWithFragments;
typedef std::vector<FeatureWithFragments> FeaturesCollection;

typedef std::pair<int, double> ClassesAndPropabilities;
typedef std::vector<ClassesAndPropabilities> PairsContainer;

typedef std::pair<Feature, Feature> FeaturesPair;
typedef std::vector<FeaturesPair> FeaturesCorrespondence;

class RandomForestTracker : public ArgumentsAwareFrameTransformer {
  public:
    RandomForestTracker();
    ~RandomForestTracker();

    static const std::string Name;

    static const cv::Scalar DrawingColor;
    static const cv::Size GaussianKernelSize;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<std::string>& arguments);

    virtual void handleFirstFrame(const cv::Mat& firstFrame);
    virtual void handleMovieName(const std::string& movieName);

    virtual void afterInitialization();

  protected:
    virtual void classifierInitialization();

  private:
    struct PIMPL;
    PIMPL* implementation;

    cv::Mat initialImage;
    cv::Mat actualGrayFrame;
    cv::Mat smoothedGrayFrame;

    void generateTrainingBase();

    void writeTrainingBaseToFolder();
    void loadTrainingBaseFromFolder();

    void trainClassifier();
    void classifyImage(const cv::Mat& initial, const cv::Mat& frame, cv::Mat& output);

    bool isTrainingBaseAvailable() const;

    void readPointsFromKeypointFile(const std::string& fileName, std::vector<cv::Point>& points);

    void createClassificationResultImage(const cv::Mat& initial, const cv::Mat& frame, cv::Mat& output);
    void classifyPatchesFromCollection(const FeaturesCollection& featuresStore, std::vector<PairsContainer>& results);
    void loadFeaturePointsFromTrainigBase(std::vector<Feature>& loadedFeaturePoints) const;

    void makeFeaturePointCorrespondence(
          const std::vector<Feature>& initialFeaturePoints,
          const FeaturesCollection& inputImageFeatureCollection,
          const std::vector<PairsContainer>& classifiedPatches,
          FeaturesCorrespondence& corespondance ) const;

    void drawFeaturePointsCorrespondence(
          std::vector<Feature>& trainingBaseFeaturePoints,
          const FeaturesCorrespondence& correspondence,
          const cv::Mat& initialImage,
          cv::Mat& frame) const;
};

#endif