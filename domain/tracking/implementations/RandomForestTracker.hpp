#ifndef __RANDOM_FOREST_TRACKER_HPP__
#define __RANDOM_FOREST_TRACKER_HPP__

#include "../../common/FrameTransformers.hpp"

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
    void classifyImage(const cv::Mat& initial, const cv::Mat& fram, cv::Mat& output);

    bool isTrainingBaseAvailable() const;
};

#endif