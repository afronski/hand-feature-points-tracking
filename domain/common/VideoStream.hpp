#ifndef __VIDEO_STREAM_HPP__
#define __VIDEO_STREAM_HPP__

#include <string>

#include "Timer.hpp"
#include "FrameTransformers.hpp"

namespace common {
  namespace vision {

    class VideoStream {
      public:
        VideoStream();
        ~VideoStream();

        void add(FrameTransformer* transformer);

        void open(const std::string& input);
        void transfer(const std::string& output);

        Dictionary getResults();

      protected:
        virtual void beforeVideo();
        virtual void afterVideo();

      private:
        bool isValid() const;
        void processFrames();

        common::Timer processingVideoTimer;
        cv::Mat getFirstFrame() const;

        std::string inputPath;
        std::string outputPath;

        struct PIMPL;
        PIMPL* _implementation;
    };
  }
}

#endif