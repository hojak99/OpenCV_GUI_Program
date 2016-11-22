#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/opencv.hpp"
#include <QtCore>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("jangho");
    capture.open(0);

    tmrTimer = new QTimer(this);

    if(count == 0){
        connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameUpdate()));
    }else if(count == 1){

    }
    tmrTimer->start(20);
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::processFrameUpdate()
{
    if(count == 0){
        findCircle(kind);
    }
}


void MainWindow::on_startButton_clicked()
{
   loadingImg = false;

    count = 0;
    if(tmrTimer->isActive()){
        tmrTimer->stop();
        ui->startButton->setText("Find Circle");
    }else{
        tmrTimer->start(20);
        ui->startButton->setText("stop");
    }
}

void MainWindow::findCircle(int kind)
{
    QImage::Format formatType;

    capture.read(originalImg);

    if(originalImg.empty()) return;

    switch (kind) {
    case 0:{
        cv::Mat tmp = originalImg.clone();

        formatType = QImage::Format_RGB888;

        cv::inRange(tmp,cv::Scalar(25,130,168) , cv::Scalar(115,220,255), afterImg);
        cv::GaussianBlur(afterImg, afterImg, cv::Size(9,9), 1.5);
        cv::HoughCircles(afterImg, vecCircle, CV_HOUGH_GRADIENT, 2, afterImg.rows/4, 100, 50, 10, 400);

        for(itrCircle = vecCircle.begin(); itrCircle != vecCircle.end(); itrCircle++){

            ui->textInfor->appendPlainText(QString("Ball position X =") + QString::number((*itrCircle)[0]).rightJustified(4,' ')+
                    QString(", Y = ")+QString::number((*itrCircle)[1]).rightJustified(4, ' ') +
                    QString(", radius = ")+QString::number((*itrCircle)[2], 'f', 3).rightJustified(7, ' '));

            cv::circle(tmp, cv::Point((int)(*itrCircle)[0], (int)(*itrCircle)[1]), 3, cv::Scalar(0,255,0), CV_FILLED);
            cv::circle(tmp, cv::Point((int)(*itrCircle)[0], (int)(*itrCircle)[1]), (int)(*itrCircle)[2], cv::Scalar(0,0,255), 3);
        }
        cv::cvtColor(tmp, tmp, CV_BGR2RGB);

        QImage tmpQimg((uchar*) tmp.data, tmp.cols, tmp.rows, tmp.step, formatType);
        qOriginalimg = tmpQimg;

    }break;

    case 1:{

        cv::Mat tmp = originalImg.clone();
        formatType = QImage::Format_Indexed8;

        cv::inRange(tmp,cv::Scalar(25,130,168) , cv::Scalar(115,220,255), afterImg);
        cv::GaussianBlur(afterImg, afterImg, cv::Size(9,9), 1.5);
        cv::HoughCircles(afterImg, vecCircle, CV_HOUGH_GRADIENT, 2, afterImg.rows/4, 100, 50, 10, 400);

        for(itrCircle = vecCircle.begin(); itrCircle != vecCircle.end(); itrCircle++){

            cv::circle(tmp, cv::Point((int)(*itrCircle)[0], (int)(*itrCircle)[1]), 3, cv::Scalar(0,255,0), CV_FILLED);
            cv::circle(tmp, cv::Point((int)(*itrCircle)[0], (int)(*itrCircle)[1]), (int)(*itrCircle)[2], cv::Scalar(0,0,255), 3);
        }
        cv::cvtColor(tmp, tmp, CV_RGB2GRAY);

        QImage tmpQimg((uchar*) tmp.data, tmp.cols, tmp.rows, tmp.step, formatType);

        qOriginalimg = tmpQimg;

    }break;


    case 2:{
        formatType = QImage::Format_RGB888;

        tmp = originalImg.clone();

        cv::inRange(tmp,cv::Scalar(25,130,168) , cv::Scalar(115,220,255), afterImg);
        cv::GaussianBlur(afterImg, afterImg, cv::Size(9,9), 1.5);
        cv::HoughCircles(afterImg, vecCircle, CV_HOUGH_GRADIENT, 2, afterImg.rows/4, 100, 50, 10, 400);

        for(itrCircle = vecCircle.begin(); itrCircle != vecCircle.end(); itrCircle++){

            cv::circle(tmp, cv::Point((int)(*itrCircle)[0], (int)(*itrCircle)[1]), 3, cv::Scalar(0,255,0), CV_FILLED);
            cv::circle(tmp, cv::Point((int)(*itrCircle)[0], (int)(*itrCircle)[1]), (int)(*itrCircle)[2], cv::Scalar(0,0,255), 3);
        }

        cv::cvtColor(tmp, tmp, CV_BGR2RGB);
        cv::GaussianBlur(tmp, tmp, cv::Size(valueProcessing,valueProcessing), 2.0, 2.0);

        QImage tmpQimg((uchar*) tmp.data, tmp.cols, tmp.rows, tmp.step, formatType);
        qOriginalimg = tmpQimg;
    }break;


    case 3:{
        fingerDetection();
    }break;


    case 4:{
        go_Erode();
    }break;


    case 5:{
        go_Dilate();
    }break;


    default:
        break;

    }

    ui->originalFrame->setPixmap(QPixmap::fromImage(qOriginalimg));
}

void MainWindow::displayImg(QString imagePath)
{
    if(tmrTimer->isActive()){
        tmrTimer->stop();
    }

    try{
        originalImg = cv::imread(imagePath.toStdString());

        cv::resize(originalImg, originalImg, cv::Size(570, 410), 0,0,CV_INTER_NN);

        cv::Mat tmp = originalImg.clone();
        cv::cvtColor(tmp, tmp, CV_BGR2RGB);
        QImage displayImg((uchar*)tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888);

        ui->originalFrame->clear();
        ui->originalFrame->setPixmap(QPixmap::fromImage(displayImg));
    }catch(...){
        ui->originalFrame->setText("error");
    }

}

void MainWindow::go_grayScale()
{
    if(tmrTimer->isActive()){
        kind =1;
    }else{
        changeImg = originalImg.clone();

        cv::cvtColor(changeImg, changeImg, CV_BGR2GRAY);

        QImage displayImg((uchar*) changeImg.data, changeImg.cols, changeImg.rows, changeImg.step, QImage::Format_Indexed8);

        ui->originalFrame->setPixmap(QPixmap::fromImage(displayImg));
    }
}

void MainWindow::return_Img()
{
    if(tmrTimer->isActive())
        kind=0;
    else{
        QImage displayImg((uchar*) originalImg.data, originalImg.cols, originalImg.rows, originalImg.step, QImage::Format_RGB888);
        ui->originalFrame->setPixmap(QPixmap::fromImage(displayImg));
    }
}


void MainWindow::go_gaussianBlur()
{
    if(tmrTimer->isActive())
        kind=2;
    else{
        changeImg=originalImg.clone();


        cv::cvtColor(changeImg, changeImg, CV_BGR2RGB);

        cv::GaussianBlur(changeImg, changeImg, cv::Size(valueProcessing,valueProcessing),2.0,2.0 );

        QImage displayImg((uchar*) changeImg.data, changeImg.cols, changeImg.rows, changeImg.step, QImage::Format_RGB888);
        ui->originalFrame->setPixmap(QPixmap::fromImage(displayImg));

    }
}

void MainWindow::go_FindHand()
{
    if(tmrTimer->isActive())
        kind = 3;
    else{
        fingerDetection();
    }
}

void MainWindow::fingerDetection()
{
    int beforeType;
    int afterType;

    if(loadingImg){
        beforeType = CV_BGR2YCrCb;
    }else{
        beforeType = CV_BGR2YCrCb;
        afterType = CV_BGR2RGB;
    }
    cv::Mat tmpImg;
    changeImg = originalImg.clone();
    element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(11, 11), cv::Point(5,5));

    cv::cvtColor(changeImg, tmpImg, beforeType);
    cv::inRange(tmpImg, cv::Scalar(0,133,7), cv::Scalar(255, 173, 127), tmpImg);

    cv::erode(tmpImg, tmpImg, element);
    cv::dilate(tmpImg, tmpImg, element);

    cv::findContours(tmpImg, contours, hierarchy, CV_RETR_LIST, CV_CLOCKWISE, cv::Point(0,0));

    int largestContour = 0;
    for(int i=0; i<contours.size(); i++){
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])) {
            largestContour = i;
        }
    }
    cv::drawContours(changeImg, contours, largestContour, cv::Scalar(0, 255, 255), 1, 8, std::vector < cv::Vec4i>(), 0, cv::Point());	//contour

    if (!contours.empty()) {
        std::vector<std::vector<cv::Point> > hull(1);

        cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
        cv::drawContours(changeImg, hull, 0, cv::Scalar(0, 255, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());		//� �ձ�

        if (hull[0].size() > 2) {
            cv::Rect boundingBox = cv::boundingRect(hull[0]);
            cv::rectangle(changeImg, boundingBox, cv::Scalar(255, 0, 0));
            cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
            std::vector<cv::Point> validPoints;

            std::vector<int> hullIndex;
            cv::convexHull(cv::Mat(contours[largestContour]), hullIndex);
            std::vector<cv::Vec4i> convexityDefects;
            cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndex, convexityDefects);

            std::cout << convexityDefects.size() << std::endl;

            for (int i = 0; i < convexityDefects.size(); i++) {
                cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
                cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
                cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
                cv::line(changeImg, p1, p3, cv::Scalar(255, 0, 0), 2);
                cv::line(changeImg, p3, p2, cv::Scalar(255, 0, 0), 2);

                double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
                double inA = fingerEnd(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));

                if (angle > -30 && angle < 160 && std::abs(inA) > 20 && std::abs(inA) < 120 && length > 0.1 * boundingBox.height){
                    validPoints.push_back(p1);
                }

            }
            for (int i = 0; i < validPoints.size(); i++){
                cv::circle(changeImg, validPoints[i], 9, cv::Scalar(255, 0, 0), 2);
            }
        }
    }

    if(!loadingImg)
        cv::cvtColor(changeImg, changeImg, afterType);


    QImage displayImg((uchar*) changeImg.data, changeImg.cols, changeImg.rows, changeImg.step, QImage::Format_RGB888);
    qOriginalimg = displayImg;
    ui->originalFrame->setPixmap(QPixmap::fromImage(qOriginalimg));
}

float MainWindow::fingerEnd(float p1, float p2, float t1, float t2, float q1, float q2)
{
    float dist1 = std::sqrt((p1 - q1)*(p1 - q1) + (p2 - q2)*(p2 - q2));
    float dist2 = std::sqrt((t1 - q1)*(t1 - q1) + (t2 - q2)*(t2 - q2));

    float aX, aY;
    float bX, bY;
    float cX, cY;

    cX = q1;
    cY = q2;

    if (dist1 < dist2) {
        bX = p1;
        bY = p2;
        aX = t1;
        aY = t2;
    }
    else {
        bX = t1;
        bY = t2;
        aX = p1;
        aY = p2;
    }

    float r1 = cX - aX;
    float r2 = cY - aY;
    float z1 = bX - aX;
    float z2 = bY - aY;

    float result = std::acos((z1*r1 + z1*r2) / (std::sqrt(z1*z1 + z2*z2) * std::sqrt(r1*r1 + r2*r2)));

    result = result * 180 / CV_PI;

    return result;
}

void MainWindow::go_Erode()
{
    kind = 4;

    cv::Mat tmp;
    cv::erode(originalImg, tmp, cv::Mat(valueProcessing, valueProcessing, CV_8UC1, cv::Scalar(255)));

    cv::cvtColor(tmp, tmp, CV_BGR2RGB);

    QImage displayImg((uchar*) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888);
    qOriginalimg = displayImg;
    ui->originalFrame->setPixmap(QPixmap::fromImage(qOriginalimg));
}

void MainWindow::go_Dilate()
{
    kind =5;
    cv::Mat origin = originalImg.clone();
    cv::Mat tmp;

    cv::dilate(origin, origin, cv::Mat(valueProcessing, valueProcessing, CV_8UC1, cv::Scalar(255)));

    cv::cvtColor(origin, origin, CV_BGR2RGB);

    QImage displayImg((uchar*) origin.data, origin.cols, origin.rows, origin.step, QImage::Format_RGB888);
    qOriginalimg = displayImg;
    ui->originalFrame->setPixmap(QPixmap::fromImage(qOriginalimg));
}


void MainWindow::on_imgLoadButton_clicked()
{
    loadingImg = true;
    count = 1;
    imagePath = ui->imagePathEdit->text();
    qDebug() << "이미지 경로:"<<imagePath;

    displayImg(imagePath);
}

void MainWindow::on_processingButton_clicked()
{
    QString tmp = ui->valueEdit->text();
    valueProcessing = tmp.toInt();

    if(valueProcessing % 2 == 0)
        valueProcessing += 1;

    if(ui->radioButton2->isChecked())
        go_gaussianBlur();

    if(ui->radioButton4->isChecked())
        go_Erode();

    if(ui->radioButton5->isChecked())
        go_Dilate();

    qDebug() <<"valueProcessing : "<<valueProcessing;
}


void MainWindow::on_radioButton1_clicked()
{
    if(ui->radioButton1->isChecked())
        go_grayScale();
    else if(!ui->radioButton1->isChecked())
        return_Img();
}

void MainWindow::on_radioButton2_clicked()
{
    if(ui->radioButton2->isChecked())
        go_gaussianBlur();
    else if(!ui->radioButton2->isChecked())
        return_Img();

}

void MainWindow::on_radioButton3_clicked()
{
    if(ui->radioButton3->isChecked())
        go_FindHand();
    else if(!ui->radioButton3->isChecked())
        return_Img();
}

void MainWindow::on_radioButton4_clicked()
{
    if(ui->radioButton4->isChecked())
        go_Erode();
    else if(!ui->radioButton4->isChecked())
        return_Img();
}

void MainWindow::on_radioButton5_clicked()
{
    if(ui->radioButton5->isChecked())
        go_Dilate();
    else if(!ui->radioButton5->isChecked())
        return_Img();
}

void MainWindow::on_trackerButton_clicked()
{

}
