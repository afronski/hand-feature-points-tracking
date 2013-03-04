openCV = [ "opencv_core", "opencv_highgui", "opencv_video", "opencv_imgproc" ]

exporter = Glob("./domain/exporter/main.cpp")
tracking = Glob("./domain/tracking/main.cpp")

Program("./bin/export-first-frame", exporter, LIBS = openCV)
Program("./bin/tracking", tracking, LIBS = openCV)
