#include <sstream>

#include <opencv2/core/core_c.h>

#include "vision.hpp"

namespace common {
  namespace vision {

    // Primitives.
    void draw_cross(cv::Mat& image, const cv::Point& center, const cv::Scalar& color, int size) {
      cv::line(image, cv::Point(center.x - size, center.y - size),
                      cv::Point(center.x + size, center.y + size), color, 2, CV_AA, 0);
      cv::line(image, cv::Point(center.x + size, center.y - size),
                      cv::Point(center.x - size, center.y + size), color, 2, CV_AA, 0);
    }

    // Parsing input to the OpenCV structures.
    std::vector<cv::Point> extract_points_from_arguments(const std::vector<std::string>& arguments,
                                                         unsigned int start) {
      std::vector<cv::Point> points;
      cv::Point point;

      for (unsigned int count = 0, i = start; i < arguments.size(); ++count, ++i) {
        std::stringstream stream(arguments[i]);

        if (count % 2 == 0) {
          stream >> point.x;
        } else {
          stream >> point.y;
          points.push_back(point);
        }
      }

      return points;
    }

  }
}