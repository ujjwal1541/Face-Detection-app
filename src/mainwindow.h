#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFileDialog>
#include "videoprocessor.h"
#include "facedetector.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startVideo();
    void stopVideo();
    void switchSource();
    void updateFrame(const QImage &frame);
    void updateFaceCount(int count);

private:
    QWidget *centralWidget;
    QLabel *videoLabel;
    QLabel *faceCountLabel;
    QPushButton *startButton;
    QPushButton *stopButton;
    QComboBox *sourceComboBox;
    
    VideoProcessor *videoProcessor;
    FaceDetector *faceDetector;
    
    void setupUI();
    void createConnections();
};

#endif // MAINWINDOW_H 