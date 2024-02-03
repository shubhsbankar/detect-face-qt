#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <Python.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     faceCascade.load("C:/opencv/sources/data/haarcascades_cuda/haarcascade_frontalface_default.xml");
    eyeCascade.load("C:/opencv/sources/data/haarcascades_cuda/haarcascade_eye.xml");
    noseCascade.load("C:/opencv/sources/data/haarcascades_cuda/haarcascade_mcs_nose.xml");
    mouthCascade.load(cv::samples::findFile("C:/opencv/sources/data/haarcascades_cuda/haarcascade_smile.xml"));
    //emotionNet = cv::dnn::readNetFromTensorflow("model.pb");
    //showFullScreen();
    qDebug() << "Hello Shubham : " << getpid();
    mFaceDetectionThread = new FaceDetectionThread(this);
    // Add this line to the constructor or setup function
    connect(mFaceDetectionThread, &FaceDetectionThread::finished, this, &MainWindow::onFaceDetectionThreadFinished);
    connect(mFaceDetectionThread, &FaceDetectionThread::frameCaptured, this, &MainWindow::onFrameCaptured);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// void MainWindow::onPushButtonClicked()
// {
//     videoCapture.open(0);
//     if (!videoCapture.isOpened())
//     {
//         qDebug() << "Error opening camera.";
//     }
//     cv::Mat frame;
//     videoCapture >> frame;

//     if (!frame.empty())
//     {
//         image = frame;
//         filePath = "temp.png";
//         cv::imwrite(filePath.toStdString(), frame);
//         // cv::Mat grayFrame;
//         // cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
//         // cv::equalizeHist(grayFrame, grayFrame);

//         // std::vector<cv::Rect> faces;
//         // faceCascade.detectMultiScale(grayFrame, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

//         // for (const auto &face : faces)
//         // {
//         //     cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
//         //     faceROI = grayFrame(face);
//         // }

//         //cv::imshow("Face Recognition", frame);
//         //detectAndDrawFaces();
//         cv::Mat rgbMat;
//         cv::cvtColor(frame, rgbMat, cv::COLOR_BGR2RGB);
//         QImage img(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
//         ui->cameraLabel->setPixmap(QPixmap::fromImage(img));
//         // QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
//         // ui->centralwidget->setLayout(layout);
//     }
// }

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
QImage MainWindow::putImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
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
            if (filePath.compare("temp.png") != 0)
            {
            QSize labelSize = ui->cameraLabel->size();
            //cv::Mat resizedImage;
            double aspectRatio = static_cast<double>(originalImage.cols) / originalImage.rows;
            int newWidth = static_cast<int>(labelSize.height() * aspectRatio);
            cv::resize(originalImage, image, cv::Size(newWidth, labelSize.height()));
            }
            else
            {
                image = originalImage;
            }
            //image = originalImage;
            //ui->cameraLabel->setFixedSize(image.cols,image.rows);
            if (detectFace)
            {
                detectAndDrawFaces();
            }
            // Convert the resized image to a QImage
            //QImage img(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);

            // Display the processed image in the QLabel widget
            ui->cameraLabel->setPixmap(QPixmap::fromImage(putImage(image)));
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
    faceCascade.detectMultiScale(grayImage, faces, 1.1, 3, 0, cv::Size(30, 30));

    // Draw rectangles around detected faces
    for (const cv::Rect &faceRect : faces)
    {
        cv::rectangle(image, faceRect, cv::Scalar(255, 0, 0), 2);
       // cv::Mat faceROI = image(faceRect);

       //  // Resize the face image to match the input size of the emotion model
       //  cv::Mat blob = cv::dnn::blobFromImage(faceROI, 1.0, cv::Size(48, 48), cv::Scalar(0, 0, 0), false, false);

       //  emotionNet.setInput(blob);
       //  cv::Mat emotions = emotionNet.forward();

       //  // Find the index of the most probable emotion
       //  cv::Point maxLoc;
       //  cv::minMaxLoc(emotions, nullptr, nullptr, nullptr, &maxLoc);

       //  int emotionIndex = maxLoc.x;

       //  qDebug() << "Emotion is : " << emotionLabels.at(emotionIndex);
        // Analyze emotion for each frame
        QString emotion = analyzeEmotion(image);
        setWindowTitle("Qt with deepface - Emotion: " + emotion);
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
    qDebug() << "Face detection thread finished.";
}

void MainWindow::onFrameCaptured(cv::Mat frame)
{
    image = frame;
    detectAndDrawFaces();
    cv::Mat rgbMat;
    cv::cvtColor(frame, rgbMat, cv::COLOR_BGR2RGB);
    QImage img(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(img));
}


QString MainWindow::analyzeEmotion(const cv::Mat &frame)
{
    Py_Initialize();

    // Convert OpenCV frame to temporary image file
    QString tempImagePath = QDir::tempPath() + "/temp_image.jpg";
    cv::imwrite(tempImagePath.toStdString(), frame);

    // Convert QString to Python string
    qDebug() << tempImagePath.toStdString().c_str();
    qDebug() << PyBytes_Check(tempImagePath.toStdString().c_str());
    PyObject *pImagePath = PyUnicode_DecodeFSDefault(tempImagePath.toStdString().c_str());

    // Import the Python script
    PyObject *pName = PyUnicode_DecodeFSDefault("emotion_detection");
    PyObject *pModule = PyImport_Import(pName);
    Py_XDECREF(pName);

    if (pModule != NULL) {
        // Call the Python function
        PyObject *pFunc = PyObject_GetAttrString(pModule, "analyze_emotion");
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject *pValue = PyObject_CallFunctionObjArgs(pFunc, pImagePath, NULL);
            Py_XDECREF(pFunc);

            if (pValue != NULL) {
                // Convert Python string to QString
                QString emotion = QString::fromStdString(PyUnicode_AsUTF8(pValue));
                Py_XDECREF(pValue);

                // Remove temporary image file
                QFile::remove(tempImagePath);

                return emotion;
            }
        }
    }

    // Handle errors or return an empty string
    PyErr_Print();
    Py_Finalize();
    return QString();
}
