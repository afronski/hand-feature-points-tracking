openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc", "common" ]

common = Glob("./domain/exporter/main.cpp")

Library("./bin/common", [
  "./domain/common/path.cpp"
])

exporter = Glob("./domain/exporter/main.cpp")
tracking = Glob("./domain/tracking/main.cpp")
kalman   = Glob("./domain/kalman-filter/main.cpp")

Program("./bin/export-first-frame", exporter, LIBS = openCV, LIBPATH = "./bin")
Program("./bin/tracking", tracking, LIBS = openCV, LIBPATH = "./bin")
Program("./bin/kalman", kalman, LIBS = openCV, LIBPATH = "./bin")