import os;

environment = Environment(ENV = {
                            'PATH'    : os.environ['PATH'],
                            'TERM'    : os.environ['TERM'],
                            'HOME'    : os.environ['HOME']
                          },
                          CPPFLAGS = [ "-O3", "-Wall", "-Werror" ])


openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc" ]
libraries = openCV + [ "common" ]

environment.Library("./bin/common", [
                      "./domain/common/CommandLineInterface.cpp",
                      "./domain/common/VideoStream.cpp",
                      "./domain/common/path.cpp",
                      "./domain/common/vision.cpp",
                      "./domain/common/floating-point-numbers.cpp"
                    ],
                    LIBS = openCV)

tracking = [
  "./domain/tracking/implementations/PointsMarker.cpp",

  "./domain/tracking/implementations/SparseOpticalFlowTracker.cpp",
  "./domain/tracking/implementations/DenseOpticalFlowTracker.cpp",
  "./domain/tracking/implementations/RandomForestTracker.cpp",

  "./domain/tracking/implementations/random-forest-internals-implementation/AffineTransformation.cpp",
  "./domain/tracking/implementations/random-forest-internals-implementation/CornerFeatureFinder.cpp",
  "./domain/tracking/implementations/random-forest-internals-implementation/AffineTransformationsGenerator.cpp",
  "./domain/tracking/implementations/random-forest-internals-implementation/Feature.cpp",
  "./domain/tracking/implementations/random-forest-internals-implementation/FeaturePointsExtractor.cpp",

  "./domain/tracking/factories/AlgorithmFactory.cpp",
  "./domain/tracking/main.cpp"
]

environment.Program("./bin/tracking", tracking, LIBS = libraries, LIBPATH = "./bin")