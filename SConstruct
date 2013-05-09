environment = Environment(CPPFLAGS = "-O3 -Wall")

openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc" ]
libraries = openCV + [ "common" ]

environment.Library("./bin/common", [
                              "./domain/common/CommandLineInterface.cpp",
                              "./domain/common/VideoStream.cpp",
                              "./domain/common/path.cpp",
                              "./domain/common/vision.cpp"
                            ],
                            LIBS = openCV)

tracking = [
    "./domain/tracking/implementations/PointsMarker.cpp",
    "./domain/tracking/implementations/SparseOpticalFlowTracker.cpp",
    "./domain/tracking/implementations/DenseOpticalFlowTracker.cpp",
    "./domain/tracking/factories/AlgorithmFactory.cpp",
    "./domain/tracking/main.cpp"
]

environment.Program("./bin/tracking", tracking, LIBS = libraries, LIBPATH = "./bin")