## Process this file with automake to produce Makefile.in
bin_PROGRAMS = cdva

cdva_SOURCES = cdva.h cdva.cpp FileManager.h FileManager.cpp CdvaException.h \
	LogManager.h LogManager.cpp Buffer.h Buffer.cpp CdvaImpl.h CdvaImpl.cpp
cdva_LDADD = -lcdvs_main -lopencv_highgui -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_features2d
