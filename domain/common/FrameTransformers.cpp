#include "memory-usage.hpp"
#include "FrameTransformers.hpp"

FrameTransformer::FrameTransformer() {
  common::getMemoryUsage(virtualMemoryAtStart, residentSetAtStart);
}

void FrameTransformer::beforeFrame(cv::Mat& frame) {
  processingFrameTimer.start();
}

void FrameTransformer::afterFrame(cv::Mat& frame) {
  processingFrameTimer.stop();
  processedFramesTiming.push_back(processingFrameTimer.getElapsedTimeInMilliseconds());

  memoryMeasureOverheadTimer.start();

  double virtualMemory, residentSet;
  common::getMemoryUsage(virtualMemory, residentSet);

  processedFramesVirtualMemoryUsage.push_back(virtualMemory - virtualMemoryAtStart);
  processedFramesResidentMemoryUsage.push_back(residentSet - residentSetAtStart);

  memoryMeasureOverheadTimer.stop();

  memoryMeasureOverhead.push_back(memoryMeasureOverheadTimer.getElapsedTimeInMilliseconds());
}

Dictionary FrameTransformer::getResults() const {
  Dictionary results;

  results.insert(std::make_pair("memoryMeasureOverheadTime", common::toString(memoryMeasureOverhead)));
  results.insert(std::make_pair("timesForProcessingEachFrame", common::toString(processedFramesTiming)));

  results.insert(std::make_pair("virtualMemoryUsage", common::toString(processedFramesVirtualMemoryUsage)));
  results.insert(std::make_pair("residentSetMemoryUsage", common::toString(processedFramesResidentMemoryUsage)));

  return results;
}