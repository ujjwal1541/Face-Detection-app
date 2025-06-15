#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <QObject>
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <opencv2/opencv.hpp>

class FaceDetector : public QObject
{
    Q_OBJECT

public:
    explicit FaceDetector(QObject *parent = nullptr);
    ~FaceDetector();

public slots:
    void processFrame(const QImage &frame);

signals:
    void facesDetected(int count);

private:
    cv::CascadeClassifier m_faceCascade;
    QDir m_outputDir;
    
    void saveFace(const cv::Mat &face, int frameNumber);
    cv::Mat qImageToCvMat(const QImage &image);
    QImage cvMatToQImage(const cv::Mat &mat);
};

#endif // FACEDETECTOR_H 