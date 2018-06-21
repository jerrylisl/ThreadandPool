TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lpthread

SOURCES += main.cpp \
    Thread.cpp \
    ThreadPool.cpp

HEADERS += \
    Condition.h \
    Mutex.h \
    Thread.h \
    Base.h \
    ThreadPool.h

