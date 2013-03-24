#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../common/path.hpp"
#include "../common/CommandLineInterface.hpp"

class FirstFrameExporter : public common::CommandLineInterface {
  public:
    FirstFrameExporter(): CommandLineInterface(1) {}

  protected:
    virtual void help() {
      std::cout << "first-frame-exporter                               Wojciech Gawroński (2013)" << std::endl;
      std::cout << "Usage:                                                                      " << std::endl;
      std::cout << " ./first-frame-exporter input_file_AVI [output_file_PNG]                    " << std::endl;
      std::cout << "     - input_file_AVI - AVI file                                            " << std::endl;
      std::cout << "     - output_file_PNG - PNG file (default: input_file_AVI_first_frame.png) " << std::endl;
      std::cout << std::endl;
    }

    virtual int main() {
      std::string input(extractArgument(0));
      std::string output;

      if (size() < 2) {
        output = common::path::extractFileName(input) + "_first_frame.png";
      } else {
        output = std::string(extractArgument(1));
      }

      return grabFrame(input, output);
    }

  private:
    int grabFrame(std::string input, std::string output) {
      cv::VideoCapture input_video(input);

      if (!input_video.isOpened()) {
        std::cerr << "[!!] Cannot open '" << input << "' AVI file - maybe it isn't a movie file?" << std::endl;
        return -1;
      }

      cv::Mat first_frame;
      input_video >> first_frame;

      cv::imwrite(output, first_frame);

      return 0;
    }
};

int main(int argc, char* argv[]) {
  FirstFrameExporter cli;

  return cli.run(argc, argv);
}