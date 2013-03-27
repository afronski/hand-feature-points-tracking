environment = Environment(CPPFLAGS = "-O3 -Wall")

openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc" ]
libraries = openCV + [ "common" ]

common = Glob("./domain/exporter/main.cpp")

environment.Library("./bin/common", [
                              "./domain/common/CommandLineInterface.cpp",
                              "./domain/common/VideoStream.cpp",
                              "./domain/common/path.cpp",
                              "./domain/common/vision.cpp"
                            ],
                            LIBS = openCV)

exporter = Glob("./domain/exporter/main.cpp")
kalman   = Glob("./domain/kalman-filter/main.cpp")
tracking = [
    "./domain/tracking/PointsMarker.cpp",
    "./domain/tracking/AlgorithmFactory.cpp",
    "./domain/tracking/main.cpp"
]

environment.Program("./bin/export-first-frame", exporter, LIBS = libraries, LIBPATH = "./bin")
environment.Program("./bin/tracking", tracking, LIBS = libraries, LIBPATH = "./bin")
environment.Program("./bin/kalman", kalman, LIBS = libraries, LIBPATH = "./bin")