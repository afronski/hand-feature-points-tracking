#include "FrameTransformers.hpp"

void FrameTransformer::beforeFrame(cv::Mat& frame) {
  processingFrameTimer.start();
}

void FrameTransformer::afterFrame(cv::Mat& frame) {
  processingFrameTimer.stop();
  processedFramesTiming.push_back(processingFrameTimer.getElapsedTimeInMilliseconds());
}

Dictionary FrameTransformer::getResults() const {
  Dictionary results;

  results.insert(std::make_pair("timesForProcessingEachFrame", common::toString(processedFramesTiming)));

  return results;
}