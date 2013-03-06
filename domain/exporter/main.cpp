#include <iostream>
#include <string>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

void help() {
  std::cout << "first-frame-exporter                               Wojciech Gawroński (2013)" << std::endl;
  std::cout << "Usage:                                                                      " << std::endl;
  std::cout << " ./first-frame-exporter input_file_AVI [output_file_PNG]                    " << std::endl;
  std::cout << "     - input_file_AVI - AVI file                                            " << std::endl;
  std::cout << "     - output_file_PNG - PNG file (default: input_file_AVI_first_frame.png) " << std::endl;
  std::cout << std::endl;
}

std::string extractFileName(const std::string& input) {
  size_t index = input.find_last_of(".");

  if (index != std::string::npos) {
    return input.substr(0, index);
  }

  return input;
}

int grabFrame(std::string input, std::string output) {
  CvCapture* input_video = cvCaptureFromFile(input.c_str());

  if (input_video == 0) {
      std::cerr << "[!!] Cannot open '" << input << "' AVI file - maybe it isn't a movie file?" << std::endl;
      return -1;
  }

  IplImage *first_frame = cvQueryFrame(input_video);
  cvSaveImage(output.c_str(), first_frame);

  return 0;
}

int main(int argc, char* argv[]) {
  // Missing arguments.
  if (argc < 2) {
    help();

    return -127;
  }

  // Getting parameters.
  std::string input(argv[1]);
  std::string output;

  if (argc < 3) {
    output = extractFileName(input) + "_first_frame.png";
  } else {
    output = std::string(argv[2]);
  }

  return grabFrame(input, output);
}