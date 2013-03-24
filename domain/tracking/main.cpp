#include <cstdlib>
#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../common/path.hpp"
#include "../common/vision.hpp"
#include "../common/CommandLineInterface.hpp"

class TrackerApplication : public common::CommandLineInterface {
  public:
    typedef std::vector<std::string> strings;

    TrackerApplication(): CommandLineInterface(1) {
      algorithms.push_back("Drawing points (testing)");
    }

  protected:
    virtual void help() {
      std::cout << "tracking                                              Wojciech Gawroński (2013)" << std::endl;
      std::cout << "Usage:                                                                         " << std::endl;
      std::cout << " ./tracking input_file_AVI                                                     " << std::endl;
      std::cout << "     - input_file_AVI - AVI file                                               " << std::endl;
      std::cout << "Or:                                                                            " << std::endl;
      std::cout << " ./tracking --list-algorithms                                                  " << std::endl;
      std::cout << "     --list-algorithms - list all implemented algorithms for tracking  points  " << std::endl;
      std::cout << std::endl;
    }

    virtual int main() {
      std::string input(extractArgument(0));
      std::string method(extractArgument(1));

      points = common::vision::extract_points_from_arguments(*this, 2);

      return saveMovie(input, method);
    }

    virtual void handle_special_case() {
      if (extractArgument(0) == "--list-algorithms") {
        list_algorithms();
        std::exit(0);
      }
    }

  private:
    void list_algorithms() {
      for (strings::const_iterator it = algorithms.begin(); it != algorithms.end(); ++it) {
        std::cout << *it << std::endl;
      }
    }

    int saveMovie(const std::string& input, const std::string& method) {
      std::string output = common::path::extractFileName(input) + "_tracking_result.avi";
      bool should_draw_points = std::find(algorithms.begin(), algorithms.end(), method) != algorithms.end();

      cv::VideoCapture input_video(input);

      if (!input_video.isOpened()) {
        std::cerr << "[!!] Cannot open '" << input << "' AVI file - maybe it isn't a movie file?" << std::endl;
        return -1;
      }

      cv::Size dimmensions = cv::Size(static_cast<int>(input_video.get(CV_CAP_PROP_FRAME_WIDTH)),
                                      static_cast<int>(input_video.get(CV_CAP_PROP_FRAME_HEIGHT)));

      int codec = static_cast<int>(input_video.get(CV_CAP_PROP_FOURCC));
      bool isItColorVideo = true;

      cv::VideoWriter output_video;
      output_video.open(output, codec, input_video.get(CV_CAP_PROP_FPS), dimmensions, isItColorVideo);

      if (!output_video.isOpened()) {
        std::cerr << "[!!] Cannot open '" << output << "' AVI file for write." << std::endl;
        return -2;
      }

      cv::Mat frame;

      while (true) {
        input_video >> frame;

        if (frame.empty()) {
          break;
        }

        if (should_draw_points) {
          for (std::vector<cv::Point>::const_iterator it = points.begin(); it != points.end(); ++it) {
            cv::circle(frame, *it, 1, cv::Scalar(0, 0, 255));
          }
        }

        output_video << frame;
      }

      return 0;
    }

    std::vector<cv::Point> points;
    strings algorithms;
};

int main(int argc, char* argv[]) {
  TrackerApplication cli;

  return cli.run(argc, argv);
}