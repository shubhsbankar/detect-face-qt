// FaceDetectionThread.cpp
#include "FaceDetectionThread.h"
#include <QDebug>
#include <QPixmap>
#include <QDir>

FaceDetectionThread::FaceDetectionThread(QObject *parent) : QThread(parent)
{
    QDir currentDir = QDir::current();
    qDebug() << currentDir;
    faceCascade.load("./opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml");
}

void FaceDetectionThread::run()
{
    cv::VideoCapture capture(0); // 0 for default camera
    if (!capture.isOpened())
    {
        qDebug() << "Error opening camera.";
        return;
    }

    while (true)
    {
        cv::Mat frame;
        capture >> frame;

        if (frame.empty())
        {
            qDebug() << "Error capturing frame.";
            break;
        }
        //cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        Q_EMIT frameCaptured(frame);
         QThread::msleep(50); // Adjust sleep time based on your requirements
    }
}
