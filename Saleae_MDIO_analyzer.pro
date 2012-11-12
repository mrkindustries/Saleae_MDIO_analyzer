TEMPLATE = lib

CONFIG -= qt
CONFIG += plugin debug_and_release

TARGET = MDIOAnalyzer

INCLUDEPATH += \
          ./source \
          ../include \
LIBS += \
          -L../lib \
          -lAnalyzer

DEPENDPATH += ./source
	
Release:DESTDIR = release
Release:OBJECTS_DIR = release

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug
	
CXXFLAGS_DEBUG = -O0 -w -fpic -g
CXXFLAGS_RELEASE = -O3 -w -fpic  

SOURCES += \
          source/MDIOAnalyzer.cpp \
          source/MDIOAnalyzerResults.cpp \
          source/MDIOAnalyzerSettings.cpp \
          source/MDIOSimulationDataGenerator.cpp

HEADERS += \
          source/MDIOAnalyzer.h \
          source/MDIOAnalyzerResults.h \
          source/MDIOAnalyzerSettings.h \
          source/MDIOSimulationDataGenerator.h
