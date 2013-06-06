#include <cmath>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>

#include "floating-point-numbers.hpp"
#include "path.hpp"

#include "memory-usage.hpp"
#include "FrameTransformers.hpp"

// Helpers.
template<typename T>
  void drawAverageTrack(const std::vector<T>& points, cv::Mat& frame) {
    const cv::Scalar DrawingColor = cv::Scalar(255, 255, 255);
    const double CircleRadius = 3.0;

    for (unsigned int i = 0; i < points.size(); ++i) {
      if (i > 0) {
        cv::line(frame, points[i - 1], points[i], DrawingColor);
      }

      cv::circle(frame, points[i], CircleRadius, DrawingColor, -1);
    }
  }

template<typename T, typename R>
  void calculateMean(const std::vector<T>& input, std::size_t meaningfulAmount, std::vector<T>& output) {
    if (input.size() > meaningfulAmount) {
      cv::Mat reducedMean;
      cv::reduce(input, reducedMean, CV_REDUCE_AVG, 1);

      if (!reducedMean.empty()) {
        T mean(reducedMean.at<R>(0,0), reducedMean.at<R>(0,1));
        output.push_back(mean);
      }
    }
  }

template<typename T, typename R>
  void collectErrorsForAveragePoints(
        const std::vector<cv::Point>& keypoints,
        const std::vector<T>& track,
        std::vector<PointNumberAndError>& results) {
    const std::size_t keypointsSize = keypoints.size();

    results.clear();
    results.reserve(keypointsSize);

    for (std::size_t k = 0; k < keypointsSize; ++k) {
      R minimalError = static_cast<R>(5000);
      std::size_t pointNumber = keypointsSize + 1;

      for(std::size_t i = 0; i < track.size(); ++i) {
        const T convertedPoint = T(static_cast<R>(keypoints[k].x), static_cast<R>(keypoints[k].y));
        R actualError = cv::norm(convertedPoint - track[i]);

        if (common::isLessOrEqualThanZero(actualError - minimalError)) {
          minimalError = actualError;
          pointNumber = i;
        }
      }

      results.push_back(std::make_pair(pointNumber, static_cast<double>(minimalError)));
    }
  }

template<typename T>
  double distancePointToLine(const T& start, const T& end, const T& point) {
    double normalLength = hypot(end.x - start.x, end.y - start.y);

    double distance = static_cast<double>((point.x - start.x) *
                                          (end.y - start.y) - (point.y - start.y) *
                                          (end.x - start.x)) / normalLength;
    return std::abs(distance);
  }

template<typename T, typename R>
  void collectErrorsForAveragePaths(
        const std::vector<cv::Point>& keypoints,
        const std::vector<T>& track,
        std::vector<PathNumberAndError>& results) {
    const std::size_t keypointsSize = keypoints.size();

    results.clear();
    results.reserve(keypointsSize);

    for (std::size_t k = 1; k < keypointsSize; ++k) {
      const T convertedPreviousPoint = T(static_cast<R>(keypoints[k - 1].x), static_cast<R>(keypoints[k - 1].y));
      const T convertedPoint = T(static_cast<R>(keypoints[k].x), static_cast<R>(keypoints[k].y));

      R minimalError = static_cast<R>(5000);

      std::size_t previousPathNumber = keypointsSize + 1;
      std::size_t actualPathNumber = keypointsSize + 1;

      for(std::size_t i = 0; i < track.size(); ++i) {
        R actualError = distancePointToLine(convertedPreviousPoint, convertedPoint, track[i]);

        if (common::isLessOrEqualThanZero(actualError - minimalError)) {
          minimalError = actualError;

          previousPathNumber = k - 1;
          actualPathNumber = k;
        }
      }

      results.push_back(
                std::make_pair(
                  std::make_pair(previousPathNumber, actualPathNumber),
                  static_cast<double>(minimalError)));
    }
  }

// Constructor and destructor.
FrameTransformer::FrameTransformer(): boundingRectangle(0) {
  common::getMemoryUsage(virtualMemoryAtStart, residentSetAtStart);
}

FrameTransformer::~FrameTransformer() {
  if (boundingRectangle != 0) {
    delete boundingRectangle;
  }
}

// Public methods.
void FrameTransformer::collectAndDrawAverageTrack(
                          const std::vector<cv::Point2f>& points,
                          std::size_t meaningfulAmount,
                          cv::Mat& frame) {
  averagePointsCalculationTimer.start();

  calculateMean<cv::Point2f, float>(points, meaningfulAmount, averagePoints);
  drawAverageTrack<cv::Point2f>(averagePoints, frame);

  averagePointsCalculationTimer.stop();

  averagePointsCalculationOverhead.push_back(averagePointsCalculationTimer.getElapsedTimeInMilliseconds());
}

void FrameTransformer::collectAndDrawAverageTrack(
                          const std::vector<cv::Point2d>& points,
                          std::size_t meaningfulAmount,
                          cv::Mat& frame) {
  averagePointsCalculationTimer.start();

  calculateMean<cv::Point2d, double>(points, meaningfulAmount, doubleAveragePoints);
  drawAverageTrack<cv::Point2d>(doubleAveragePoints, frame);

  averagePointsCalculationTimer.stop();

  averagePointsCalculationOverhead.push_back(averagePointsCalculationTimer.getElapsedTimeInMilliseconds());
}

void FrameTransformer::handleFirstFrame(const cv::Mat& firstFrame) {
  boundary = firstFrame.size();
}

void FrameTransformer::handleMovieName(const std::string& movieName) {
  calculateBoundingRectangle(movieName);
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

  results.insert(std::make_pair("averagePointsCalculationOverheadTime",
                                common::toString(averagePointsCalculationOverhead)));

  results.insert(std::make_pair("frameWidth", common::toString(boundary.width)));
  results.insert(std::make_pair("frameHeight", common::toString(boundary.height)));

  results.insert(std::make_pair("boundingRectangleWidth", common::toString(boundingRectangle->width)));
  results.insert(std::make_pair("boundingRectangleHeight", common::toString(boundingRectangle->height)));
  results.insert(std::make_pair("boundingRectangleX", common::toString(boundingRectangle->x)));
  results.insert(std::make_pair("boundingRectangleY", common::toString(boundingRectangle->y)));

  results.insert(std::make_pair("baseKeypoints", common::toString(baseKeypoints)));

  if (averagePoints.size() > 0) {
    collectErrorsForAveragePoints<cv::Point2f, float>(baseKeypoints, averagePoints, collectedErrors);
    collectErrorsForAveragePaths<cv::Point2f, float>(baseKeypoints, averagePoints, collectedPathErrors);

    results.insert(std::make_pair("averagePoints", common::toString(averagePoints)));
    results.insert(std::make_pair("averagePointsAmount", common::toString(averagePoints.size())));
  } else if (doubleAveragePoints.size() > 0) {
    collectErrorsForAveragePoints<cv::Point2d, double>(baseKeypoints, doubleAveragePoints, collectedErrors);
    collectErrorsForAveragePaths<cv::Point2d, double>(baseKeypoints, doubleAveragePoints, collectedPathErrors);

    results.insert(std::make_pair("averagePoints", common::toString(doubleAveragePoints)));
    results.insert(std::make_pair("averagePointsAmount", common::toString(doubleAveragePoints.size())));
  }

  results.insert(std::make_pair("collectedErrors", common::toString(collectedErrors)));
  results.insert(std::make_pair("collectedErrorsAmount", common::toString(collectedErrors.size())));

  results.insert(std::make_pair("collectedPathErrors", common::toString(collectedPathErrors)));
  results.insert(std::make_pair("collectedPathErrorsAmount", common::toString(collectedPathErrors.size())));

  return results;
}

bool FrameTransformer::contains(const cv::Point2f& point) const {
  if (boundingRectangle != 0) {
    return boundingRectangle->contains(point);
  }

  return true;
}

bool FrameTransformer::contains(const cv::Point2d& point) const {
  if (boundingRectangle != 0) {
    return boundingRectangle->contains(point);
  }

  return true;
}

bool FrameTransformer::contains(const cv::Point& point) const {
  if (boundingRectangle != 0) {
    return boundingRectangle->contains(point);
  }

  return true;
}

// Private methods.
void FrameTransformer::calculateBoundingRectangle(const std::string& movieName) {
  readKeypointsFromFile(movieName, keypoints, baseKeypoints);

  if (keypoints.size() > 0) {
    boundingRectangle = new cv::Rect();
    *boundingRectangle = cv::boundingRect(keypoints);
  }
}

void FrameTransformer::readKeypointsFromFile(
                          const std::string& fileName,
                          std::vector<cv::Point>& points,
                          std::vector<cv::Point>& baseKeypoints) {
  std::string keypointsFilename = common::path::extractFileName(fileName) + ".keypoints";

  if (common::path::fileExists(keypointsFilename)) {
    std::ifstream input(keypointsFilename.c_str());

    unsigned int n = 0;
    double x = 0.0, y = 0.0;

    input >> keypointRadius;
    input >> n;

    while (n > 0) {
      input >> x >> y;
      points.push_back(cv::Point(x, y));
      baseKeypoints.push_back(cv::Point(x, y));

      if (x + keypointRadius < boundary.width) {
        points.push_back(cv::Point(x + keypointRadius, y));
      }

      if (x - keypointRadius >= 0.0) {
        points.push_back(cv::Point(x - keypointRadius, y));
      }

      if (y + keypointRadius < boundary.height) {
        points.push_back(cv::Point(x, y + keypointRadius));
      }

      if (y - keypointRadius >= 0.0) {
        points.push_back(cv::Point(x, y - keypointRadius));
      }

      if (x + keypointRadius < boundary.width && y + keypointRadius < boundary.height) {
        points.push_back(cv::Point(x + keypointRadius, y + keypointRadius));
      }

      if (x + keypointRadius < boundary.width && y - keypointRadius >= 0) {
        points.push_back(cv::Point(x + keypointRadius, y - keypointRadius));
      }

      if (x - keypointRadius >= 0.0 && y + keypointRadius < boundary.height) {
        points.push_back(cv::Point(x - keypointRadius, y + keypointRadius));
      }

      if (x - keypointRadius >= 0.0 && y - keypointRadius >= 0.0) {
        points.push_back(cv::Point(x - keypointRadius, y - keypointRadius));
      }

      --n;
    }

    input.close();
  }
}