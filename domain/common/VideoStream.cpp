#include <iostream>
#include <stdexcept>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>

#include "debug.hpp"
#include "converters.hpp"
#include "VideoStream.hpp"

namespace common {
  namespace vision {

    // Functors.
    class TransformerDelete {
      public:
        void operator() (FrameTransformer* transformer) {
          if (transformer) {
            delete transformer;
          }
        }
    };

    class FrameProcessor {
      public:
        FrameProcessor(cv::Mat& frame): frameReference(frame) {}

        void operator() (FrameTransformer* transformer) {
          transformer->beforeFrame(frameReference);
          transformer->process(frameReference);
          transformer->afterFrame(frameReference);
        }

      private:
        cv::Mat& frameReference;
    };

    class ParametersProcessor {
      public:
        ParametersProcessor(const cv::Mat& frame, const std::string& fileName):
          frameReference(frame),
          movieName(fileName)
        {}

        void operator() (FrameTransformer* transformer) {
          transformer->handleFirstFrame(frameReference);
          transformer->handleMovieName(movieName);
          transformer->afterInitialization();
        }

      private:
        cv::Mat frameReference;
        std::string movieName;
    };

    // Protected implementation for class internals.
    struct VideoStream::PIMPL {
      typedef std::vector<FrameTransformer*> TransformersList;
      typedef TransformersList::iterator TransformersIterator;

      ~PIMPL() {
        TransformerDelete deleteTransform;

        std::for_each(transformers.begin(), transformers.end(), deleteTransform);
      }

      bool isValid() const {
        return transformers.size() > 0;
      }

      cv::VideoCapture input_video;
      cv::VideoWriter output_video;

      TransformersList transformers;

      double duration;
      int frameCounter;

      int codec;
      int fps;

      int width;
      int height;

      cv::Size dimmensions;
    };

    VideoStream::VideoStream() {
      this->_implementation = new VideoStream::PIMPL();
    }

    VideoStream::~VideoStream() {
      delete _implementation;
    }

    bool VideoStream::isValid() const {
      return _implementation->isValid();
    }

    void VideoStream::add(FrameTransformer* transformer) {
      this->_implementation->transformers.push_back(transformer);
    }

    Dictionary VideoStream::getResults() {
      Dictionary results;
      std::string processingTime(common::toString(processingVideoTimer.getElapsedTimeInMilliseconds()));

      results.insert(std::make_pair("totalVideoProcessingTime", processingTime));
      results.insert(std::make_pair("fps", common::toString(_implementation->fps)));
      results.insert(std::make_pair("frameCounter", common::toString(_implementation->frameCounter)));
      results.insert(std::make_pair("videoDuration", common::toString(_implementation->duration)));

      return results;
    }

    void VideoStream::beforeVideo() {
      processingVideoTimer.start();
    }

    void VideoStream::afterVideo() {
      processingVideoTimer.stop();
    }

    void VideoStream::processFrames() {
      cv::Mat frame;

      beforeVideo();

      while (true) {
        _implementation->input_video.read(frame);

        if (frame.empty()) {
          afterVideo();
          break;
        }

        FrameProcessor frameProcessor(frame);
        std::for_each(_implementation->transformers.begin(), _implementation->transformers.end(), frameProcessor);

        _implementation->output_video.write(frame);
      }
    }

    cv::Mat VideoStream::getFirstFrame() const {
      cv::Mat frame;

      _implementation->input_video.read(frame);
      _implementation->input_video.set(CV_CAP_PROP_POS_FRAMES, 0);

      if (frame.empty()) {
        throw std::logic_error("VideoStream cannot obtain first frame of animation!");
      }

      return frame;
    }

    void VideoStream::open(const std::string& input) {
      inputPath = input;

      _implementation->input_video.open(inputPath);

      _implementation->codec = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FOURCC));
      _implementation->fps = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FPS));
      _implementation->width = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FRAME_WIDTH));
      _implementation->height = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FRAME_HEIGHT));
      _implementation->frameCounter = static_cast<int>(_implementation->input_video.get(CV_CAP_PROP_FRAME_COUNT));

      _implementation->duration = (static_cast<double>(_implementation->frameCounter) /
                                   static_cast<double>(_implementation->fps)) * 1000.0;

      _implementation->dimmensions = cv::Size(_implementation->width, _implementation->height);

      if (!_implementation->input_video.isOpened()) {
        throw std::runtime_error("We can't open input video!");
      }

      // Send first frame and movie name to all transformers.
      ParametersProcessor sendParameters(getFirstFrame(), inputPath);
      std::for_each(_implementation->transformers.begin(), _implementation->transformers.end(), sendParameters);
    }

    void VideoStream::transfer(const std::string& output) {
      outputPath = output;

      _implementation->output_video.open(outputPath,
                                          _implementation->codec,
                                          _implementation->fps,
                                          _implementation->dimmensions,
                                          true);

      if (!_implementation->output_video.isOpened()) {
        throw std::runtime_error("We can't open output video!");
      }

      if (isValid()) {
        processFrames();

        _implementation->input_video.release();
        _implementation->output_video.release();

        common::debug::print("[END] Video stream processed successfully!\n");
      } else {
        throw std::logic_error("VideoStream parameters are incomplete to proceed with transfer!");
      }
    }

  }
}