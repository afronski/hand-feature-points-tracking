openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc" ]
libraries = openCV + [ "common" ]

common = Glob("./domain/exporter/main.cpp")

Library("./bin/common", [
                          "./domain/common/path.cpp",
                          "./domain/common/primitives.cpp"
                        ],
                        LIBS = openCV)

exporter = Glob("./domain/exporter/main.cpp")
tracking = Glob("./domain/tracking/main.cpp")
kalman   = Glob("./domain/kalman-filter/main.cpp")

Program("./bin/export-first-frame", exporter, LIBS = libraries, LIBPATH = "./bin")
Program("./bin/tracking", tracking, LIBS = libraries, LIBPATH = "./bin")
Program("./bin/kalman", kalman, LIBS = libraries, LIBPATH = "./bin")