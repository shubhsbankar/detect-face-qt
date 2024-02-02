#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     faceCascade.load("C:/opencv/sources/data/haarcascades_cuda/haarcascade_frontalface_default.xml");
    eyeCascade.load("C:/opencv/sources/data/haarcascades_cuda/haarcascade_eye.xml");
    noseCascade.load("C:/opencv/sources/data/haarcascades_cuda/haarcascade_mcs_nose.xml");
    mouthCascade.load(cv::samples::findFile("C:/opencv/sources/data/haarcascades_cuda/haarcascade_smile.xml"));

    qDebug() << "Hello Shubham : " << getpid();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPushButtonClicked()
{
    videoCapture.open(0);
    if (!videoCapture.isOpened())
    {
        qDebug() << "Error opening camera.";
    }
    cv::Mat frame;
    videoCapture >> frame;

    if (!frame.empty())
    {
        image = frame;
        filePath = "temp.png";
        cv::imwrite(filePath.toStdString(), frame);
        // cv::Mat grayFrame;
        // cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        // cv::equalizeHist(grayFrame, grayFrame);

        // std::vector<cv::Rect> faces;
        // faceCascade.detectMultiScale(grayFrame, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        // for (const auto &face : faces)
        // {
        //     cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        //     faceROI = grayFrame(face);
        // }

        //cv::imshow("Face Recognition", frame);
        //detectAndDrawFaces();
        cv::Mat rgbMat;
        cv::cvtColor(frame, rgbMat, cv::COLOR_BGR2RGB);
        QImage img(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
        ui->cameraLabel->setPixmap(QPixmap::fromImage(img));
    }
}


void MainWindow::on_cameraOnOffBut_clicked()
{
    onPushButtonClicked();
}


void MainWindow::on_cameraOnOffBut_released()
{
    videoCapture.release();
}



void MainWindow::on_detectEmotion_clicked()
{
    std::vector<cv::Rect> mouths;
    mouthCascade.detectMultiScale(faceROI, mouths, 1.8, 20, 0, cv::Size(20, 20));

    // Check if a mouth is detected
    if (!mouths.empty()) {
        // Assume the person is happy if a mouth is detected
        QString emotion = "Happy";

        // Display the emotion label on the GUI
        ui->emotionLabel->setText(emotion);
    } else {
        // No mouth detected, assume neutral emotion
        QString emotion = "Neutral";

        // Display the emotion label on the GUI
        ui->emotionLabel->setText(emotion);
    }
}

void MainWindow::loadImage(bool detectFace)
{
    if (!filePath.isEmpty())
    {
        // Load the selected image
        cv::Mat originalImage = cv::imread(filePath.toStdString());

        if (!originalImage.empty())
        {
            // Resize the image to fit within the QLabel while maintaining aspect ratio
            QSize labelSize = ui->cameraLabel->size();
            //cv::Mat resizedImage;
            double aspectRatio = static_cast<double>(originalImage.cols) / originalImage.rows;
            int newWidth = static_cast<int>(labelSize.height() * aspectRatio);
            cv::resize(originalImage, image, cv::Size(newWidth, labelSize.height()));
            if (detectFace)
            {
                detectAndDrawFaces();
            }
            // Convert the resized image to a QImage
            QImage img(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);

            // Display the processed image in the QLabel widget
            ui->cameraLabel->setPixmap(QPixmap::fromImage(img));
        }
        else
        {
            qDebug() << "Error: Failed to load the image.";
        }
    }
}
void MainWindow::on_selectImage_clicked()
{
    // Open a file dialog to select an image
    filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
    loadImage();
}

void MainWindow::detectAndDrawFaces()
{
    // Convert the image to grayscale for face detection
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(grayImage, grayImage);

    // Detect faces in the image
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayImage, faces, 1.1, 3, 0, cv::Size(30, 30));

    // Draw rectangles around detected faces
    for (const cv::Rect &faceRect : faces)
    {
        cv::rectangle(image, faceRect, cv::Scalar(255, 0, 0), 2);
    }
}


void MainWindow::on_detectFace_clicked()
{
    loadImage(true);
}

