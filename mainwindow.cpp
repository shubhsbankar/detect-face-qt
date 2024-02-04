#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    faceCascade.load("./opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml");
    mouthCascade.load("./opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml");
    mFaceDetectionThread = new FaceDetectionThread(this);
    // Add this line to the constructor or setup function
    connect(mFaceDetectionThread, &FaceDetectionThread::finished, this, &MainWindow::onFaceDetectionThreadFinished);
    connect(mFaceDetectionThread, &FaceDetectionThread::frameCaptured, this, &MainWindow::onFrameCaptured);
    qDebug() << getpid();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPushButtonClicked()
{
    if (mIsCameraOn)
    {
        if (mFaceDetectionThread->isRunning())
        {
            mFaceDetectionThread->terminate();
            mFaceDetectionThread->wait();
            mIsCameraOn = false;
            qDebug() << "Camera stopped.";
        }
    }
    else
    {
        if (!mFaceDetectionThread->isRunning())
        {
            mFaceDetectionThread->start();
            mIsCameraOn = true;
            qDebug() << "Camera started.";
        }
    }
}
void MainWindow::on_cameraOnOffBut_clicked()
{
    onPushButtonClicked();
}

QString MainWindow::detectEmotionDlib(const cv::Mat &face) {
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> sp;  // Load a shape predictor model
    dlib::cv_image<dlib::bgr_pixel> dlibImage(face);
    std::vector<dlib::rectangle> detectedFaces = detector(dlibImage);
    dlib::rectangle faceRect;
    if (!detectedFaces.empty()) {
         faceRect = detectedFaces[0];
        dlib::full_object_detection landmarks = sp(dlibImage, faceRect);
        // Here, we just return a placeholder string based on the face's left and right landmarks
        int leftEyeX = landmarks.part(36).x();
        int rightEyeX = landmarks.part(45).x();
        int mouthX = (landmarks.part(48).x() + landmarks.part(54).x()) / 2;

        if (mouthX < leftEyeX && mouthX < rightEyeX) {
            return "Surprise";
        } else if (mouthX < leftEyeX && mouthX > rightEyeX) {
            return "Happy";
        } else {
            return "Neutral";
        }
        // Your code to handle the detected face
    } else {
        qDebug() << "No face detected.";
    }

    return QString();
}

void MainWindow::on_detectEmotion_clicked()
{
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(grayImage, grayImage);

    // Detect faces in the image
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayImage, faces, 1.1, 3, 0, cv::Size(30, 30));

    // Draw rectangles around detected faces
    for (const cv::Rect &faceRect : faces)
    {
        QString emotion = detectEmotionDlib(faceROI(faceRect));
        qDebug() << emotion;
        emotion += " " + ui->emotionLabel->text();
        ui->emotionLabel->setText(emotion);
        cv::rectangle(image, faceRect, cv::Scalar(0, 255, 0), 2);
    }
 }

void MainWindow::loadImage(bool detectFace)
{
    if (!filePath.isEmpty())
    {
        // Load the selected image
        cv::Mat originalImage = cv::imread(filePath.toStdString());
        // Resize the image to a specific width and height
        QSize labelSize = ui->cameraLabel->size();
        cv::resize(originalImage, image, cv::Size(labelSize.width(), labelSize.height()));
        if (!originalImage.empty())
        {
            if (detectFace)
            {
                detectAndDrawFaces();
            }
            QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);
            // Display the processed image in the QLabel widget
             ui->cameraLabel->setPixmap(QPixmap::fromImage(qImage));
        }
        else
        {
            qCritical() << "Error: Failed to load the image.";
        }
    }
}
void MainWindow::on_selectImage_clicked()
{
    // Open a file dialog to select an image
    filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
    loadImage();
    ui->emotionLabel->setText("");
}

void MainWindow::detectAndDrawFaces()
{
    // Convert the image to grayscale for face detection
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(grayImage, grayImage);

    // Detect faces in the image
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayImage, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Draw rectangles around detected faces
    for (const cv::Rect &faceRect : faces)
    {
        qDebug() << "Got face : ";
        cv::rectangle(image, faceRect, cv::Scalar(255, 0, 0), 2);
        faceROI = image;
    }
}


void MainWindow::on_detectFace_clicked()
{
    ui->emotionLabel->setText("");
    loadImage(true);
}


// Add this slot to the MainWindow class implementation
void MainWindow::onFaceDetectionThreadFinished()
{
    ui->cameraLabel->clear();
    ui->emotionLabel->setText("");
    qInfo() << "Face detection thread finished.";
}

// Show all the frames captured from the webcam
void MainWindow::onFrameCaptured(cv::Mat frame)
{
    image = frame;
    // Calling function to detect the face
    detectAndDrawFaces();
    // Showing the frames on the main window
    cv::Mat rgbMat;
    cv::cvtColor(frame, rgbMat, cv::COLOR_BGR2RGB);
    QImage img(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(img));
}
