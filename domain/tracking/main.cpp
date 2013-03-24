#include <iostream>
#include <string>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

void help() {
  std::cout << "tracking                                           Wojciech Gawroński (2013)" << std::endl;
  std::cout << "Usage:                                                                      " << std::endl;
  std::cout << " ./tracking input_file_AVI                                                  " << std::endl;
  std::cout << "     - input_file_AVI - AVI file                                            " << std::endl;
  std::cout << std::endl;
}

std::string extractFileName(const std::string& input) {
  size_t index = input.find_last_of(".");

  if (index != std::string::npos) {
    return input.substr(0, index);
  }

  return input;
}

int saveMovie(const std::string& input) {
  CvCapture* input_video = cvCaptureFromFile(input.c_str());

  if (input_video == 0) {
      std::cerr << "[!!] Cannot open '" << input << "' AVI file - maybe it isn't a movie file?" << std::endl;
      return -1;
  }

  std::string output = extractFileName(input) + "_tracking_result.avi";

  int frame_width = static_cast<int>(cvGetCaptureProperty(input_video, CV_CAP_PROP_FRAME_WIDTH));
  int frame_height = static_cast<int>(cvGetCaptureProperty(input_video, CV_CAP_PROP_FRAME_HEIGHT));
  int fps = static_cast<int>(cvGetCaptureProperty(input_video, CV_CAP_PROP_FPS));

  int magic_code = CV_FOURCC('M', 'P', '4', '2');
  bool isItColorVideo = true;

  CvVideoWriter *writer = cvCreateVideoWriter(
                            output.c_str(),
                            magic_code,
                            fps,
                            cvSize(frame_width, frame_height),
                            isItColorVideo);

  if (!writer) {
    cvReleaseCapture(&input_video);
    return -2;
  }

  while (true) {
    IplImage* frame = cvQueryFrame(input_video);

    if (!frame) {
      break;
    }

    cvWriteFrame(writer, frame);
  }

  cvReleaseVideoWriter(&writer);
  cvReleaseCapture(&input_video);

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

  return saveMovie(input);
}