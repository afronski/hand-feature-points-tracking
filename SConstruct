environment = Environment(CPPFLAGS = "-O3 -Wall")

openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc" ]
libraries = openCV + [ "common" ]

common = [ "./domain/exporter/main.cpp" ]

environment.Library("./bin/common", [
                              "./domain/common/CommandLineInterface.cpp",
                              "./domain/common/VideoStream.cpp",
                              "./domain/common/path.cpp",
                              "./domain/common/vision.cpp"
                            ],
                            LIBS = openCV)

exporter = [ "./domain/exporter/main.cpp" ]
kalman   = [ "./domain/kalman-filter/main.cpp" ]
tracking = [
    "./domain/tracking/implementations/PointsMarker.cpp",
    "./domain/tracking/factories/AlgorithmFactory.cpp",
    "./domain/tracking/main.cpp"
]

environment.Program("./bin/export-first-frame", exporter, LIBS = libraries, LIBPATH = "./bin")
environment.Program("./bin/tracking", tracking, LIBS = libraries, LIBPATH = "./bin")
environment.Program("./bin/kalman", kalman, LIBS = libraries, LIBPATH = "./bin")