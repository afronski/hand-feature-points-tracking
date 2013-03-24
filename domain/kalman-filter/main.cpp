#include <string>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

struct mouse_info_struct {
  int x;
  int y;
};

mouse_info_struct mouse_info = { -1, -1 },
                  last_mouse;

std::vector<cv::Point>  mousePoints,
                    kalmanPoints;

bool shouldWeExit(char keyCode) {
  return keyCode == 27 || keyCode == 'q' || keyCode == 'Q';
}

void on_mouse(int event, int x, int y, int flags, void* param) {
  last_mouse = mouse_info;

  mouse_info.x = x;
  mouse_info.y = y;
}

void drawCross(cv::Mat& image, const cv::Point& center, const cv::Scalar& color, int d) {
  cv::line(image, cv::Point(center.x - d, center.y - d),
                  cv::Point(center.x + d, center.y + d), color, 2, CV_AA, 0);
  cv::line(image, cv::Point(center.x + d, center.y - d),
                  cv::Point(center.x - d, center.y + d), color, 2, CV_AA, 0);
}

int main (int argc, char* argv[]) {
  std::string window_name = "Kalman Filter";

  cv::Mat image(500, 500, CV_8UC3);
  cv::KalmanFilter filter(6, 2, 0);
  cv::Mat_<float> measurement(2,1);

  char keyCode = -1;

  measurement.setTo(cv::Scalar(0));

  cv::namedWindow(window_name.c_str());
  cv::setMouseCallback(window_name.c_str(), on_mouse, 0);

  for(;;) {
    if (mouse_info.x < 0 || mouse_info.y < 0) {
      cv::imshow(window_name.c_str(), image);
      cv::waitKey(30);

      continue;
    }

    filter.statePre.at<float>(0) = mouse_info.x;
    filter.statePre.at<float>(1) = mouse_info.y;

    filter.statePre.at<float>(2) = 0;
    filter.statePre.at<float>(3) = 0;
    filter.statePre.at<float>(4) = 0;
    filter.statePre.at<float>(5) = 0;

    filter.transitionMatrix = *(cv::Mat_<float>(6, 6) << 1, 0, 1, 0, 0.5, 0,
                                                         0, 1, 0, 1, 0,   0.5,
                                                         0, 0, 1, 0, 1,   0,
                                                         0, 0, 0, 1, 0,   1,
                                                         0, 0, 0, 0, 1,   0,
                                                         0, 0, 0, 0, 0,   1   );

    filter.measurementMatrix = *(cv::Mat_<float>(2, 6) << 1, 0, 1, 0, 0.5, 0,
                                                          0, 1, 0, 1, 0,   0.5);

    cv::setIdentity(filter.measurementMatrix);
    cv::setIdentity(filter.processNoiseCov, cv::Scalar::all(1e-4));
    cv::setIdentity(filter.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(filter.errorCovPost, cv::Scalar::all(0.1));

    mousePoints.clear();
    kalmanPoints.clear();

    for(;;) {
      cv::Mat prediction = filter.predict();
      cv::Point predictPt(prediction.at<float>(0), prediction.at<float>(1));

      measurement(0) = mouse_info.x;
      measurement(1) = mouse_info.y;

      cv::Point measPt(measurement(0), measurement(1));
      mousePoints.push_back(measPt);

      cv::Mat estimated = filter.correct(measurement);
      cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));
      kalmanPoints.push_back(statePt);

      image = cv::Scalar::all(0);
      drawCross(image, statePt, cv::Scalar(255, 255, 255), 5);
      drawCross(image, measPt, cv::Scalar(0, 0, 255), 5);

      for (int i = 0; i < mousePoints.size() - 1; i++) {
        cv::line(image, mousePoints[i], mousePoints[i+1], cv::Scalar(255, 255, 0), 1);
      }

      for (int i = 0; i < kalmanPoints.size() - 1; i++) {
        cv::line(image, kalmanPoints[i], kalmanPoints[i+1], cv::Scalar(0, 255, 0), 1);
      }

      cv::imshow(window_name.c_str(), image);
      keyCode = cv::waitKey(100);

      if (keyCode > 0) {
          break;
      }
    }

    if (shouldWeExit(keyCode)) {
        break;
    }
  }

  return 0;
}