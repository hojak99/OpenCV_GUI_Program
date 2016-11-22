#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "opencv2/opencv.hpp"
#include <iostream>

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    int count=(0);

    Ui::MainWindow *ui;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void processFrameUpdate();
    void findCircle(int kind);
    void displayImg(QString imgPath);

    void go_grayScale();
    void return_Img();

    void go_gaussianBlur();

    void go_FindHand();
    void fingerDetection();
    float fingerEnd(float p1, float p2, float t1, float t2, float q1, float q2);

    void go_Erode();
    void go_Dilate();

private slots:
    void on_startButton_clicked();
    void on_imgLoadButton_clicked();

    void on_radioButton1_clicked();

    void on_radioButton2_clicked();

    void on_processingButton_clicked();

    void on_radioButton3_clicked();

    void on_radioButton4_clicked();

    void on_radioButton5_clicked();

    void on_trackerButton_clicked();

private:
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::VideoCapture capture;
    cv::Mat originalImg;
    cv::Mat afterImg;
    cv::Mat tmp;
    cv::Mat changeImg;

    cv::Mat element;

    QImage qOriginalimg;
    QImage qOriginalImg;
    QString imagePath;

    int valueProcessing = 3;
    int kind=0;

    bool loadingImg = false;

    std::vector <cv::Vec3f> vecCircle;
    std::vector <cv::Vec3f>::iterator itrCircle;

    QTimer* tmrTimer;
};

#endif // MAINWINDOW_H
