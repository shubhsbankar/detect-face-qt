QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH += C:\opencv\release\install\include
LIBS += -LC:\opencv\release\bin -lopencv_calib3d490 -lopencv_highgui490 -lopencv_features2d490 -lopencv_flann490 -lopencv_video490 -lopencv_videoio490 -lopencv_dnn490 -lopencv_gapi490 -lopencv_imgcodecs490 -lopencv_imgproc490 -lopencv_ml490 -lopencv_objdetect490 -lopencv_photo490 -lopencv_stitching490 -lopencv_core490

DEPENDPATH +=  C:\opencv\release\install\include

