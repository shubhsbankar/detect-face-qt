#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPushButtonClicked();

    void on_cameraOnOffBut_clicked();

    void on_cameraOnOffBut_released();

    void on_detectEmotion_clicked();

    void on_selectImage_clicked();

    void on_detectFace_clicked();

private:
    Ui::MainWindow *ui;
    cv::VideoCapture videoCapture;
    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyeCascade;
    cv::CascadeClassifier noseCascade;
    cv::CascadeClassifier mouthCascade;
    cv::Mat faceROI;
    cv::Mat image;
    QString filePath;
    void detectAndDrawFaces();
    void loadImage(bool detectFace = false);
};
#endif // MAINWINDOW_H
