#include <iostream>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>

#include "VideoStream.hpp"

namespace common {
  namespace vision {

    struct VideoStream::PIMPL {
      PIMPL() {
        transformer = 0;
      }

      ~PIMPL() {
        if (transformer) {
          delete transformer;
        }
      }

      bool isValid() const {
        return !!transformer;
      }

      cv::VideoCapture input_video;
      cv::VideoWriter output_video;

      FrameTransformer* transformer;

      int codec;
      int fps;

      int width;
      int height;

      cv::Size dimmensions;
    };

    VideoStream::VideoStream(FrameTransformer* transformer) {
      this->_implementation = new VideoStream::PIMPL();
      this->_implementation->transformer = transformer;
    }

    VideoStream::~VideoStream() {
      delete _implementation;
    }

    bool VideoStream::isValid() const {
      return _implementation->isValid();
    }

    void VideoStream::processFrames() {
      cv::Mat input,
              output;

      while (true) {
        _implementation->input_video.read(input);

        if (input.empty()) {
          break;
        }

        output = _implementation->transformer->process(input);
        _implementation->output_video.write(output);
      }
    }

    void VideoStream::open(const std::string& input) {
      inputPath = input;

      _implementation->input_video.open(inputPath);

      _implementation->codec = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FOURCC));
      _implementation->fps = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FPS));
      _implementation->width = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FRAME_WIDTH));
      _implementation->height = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FRAME_HEIGHT));

      _implementation->dimmensions = cv::Size(_implementation->width, _implementation->height);

      if (!_implementation->input_video.isOpened()) {
        throw new std::runtime_error("We can't open input video!");
      }
    }

    void VideoStream::transfer(const std::string& output) {
      outputPath = output;

      _implementation->output_video.open(outputPath,
                                          _implementation->codec,
                                          _implementation->fps,
                                          _implementation->dimmensions,
                                          true);

      if (!_implementation->output_video.isOpened()) {
        throw new std::runtime_error("We can't open output video!");
      }

      if (isValid()) {
        processFrames();

        _implementation->input_video.release();
        _implementation->output_video.release();
      } else {
        throw new std::logic_error("VideoStream parameters are incomplete to proceed with transfer!");
      }
    }

  }
}