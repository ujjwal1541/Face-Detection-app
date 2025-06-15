#include "facedetector.h"
#include <QDebug>

FaceDetector::FaceDetector(QObject *parent)
    : QObject(parent)
{
    // Load the face cascade classifier
    QString cascadePath = "haarcascade_frontalface_default.xml";
    if (!m_faceCascade.load(cascadePath.toStdString())) {
        qDebug() << "Error: Could not load face cascade classifier";
    }
    
    // Create output directory for saved faces
    m_outputDir = QDir("faces");
    if (!m_outputDir.exists()) {
        m_outputDir.mkpath(".");
    }
}

FaceDetector::~FaceDetector()
{
}

void FaceDetector::processFrame(const QImage &frame)
{
    // Convert QImage to cv::Mat
    cv::Mat frameMat = qImageToCvMat(frame);
    cv::Mat grayFrame;
    
    // Convert to grayscale
    cv::cvtColor(frameMat, grayFrame, cv::COLOR_BGR2GRAY);
    
    // Detect faces
    std::vector<cv::Rect> faces;
    m_faceCascade.detectMultiScale(grayFrame, faces, 1.1, 3, 0, cv::Size(30, 30));
    
    // Draw rectangles around faces and save them
    static int frameNumber = 0;
    for (const auto& face : faces) {
        // Draw rectangle
        cv::rectangle(frameMat, face, cv::Scalar(0, 255, 0), 2);
        
        // Extract and save face
        cv::Mat faceROI = frameMat(face);
        saveFace(faceROI, frameNumber++);
    }
    
    // Convert back to QImage and emit signal
    QImage processedFrame = cvMatToQImage(frameMat);
    emit facesDetected(faces.size());
}

void FaceDetector::saveFace(const cv::Mat &face, int frameNumber)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString filename = m_outputDir.filePath(QString("face_%1_%2.jpg").arg(timestamp).arg(frameNumber));
    cv::imwrite(filename.toStdString(), face);
}

cv::Mat FaceDetector::qImageToCvMat(const QImage &image)
{
    switch (image.format()) {
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        return cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.bits(), image.bytesPerLine());
    case QImage::Format_RGB888:
        return cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.bits(), image.bytesPerLine());
    default:
        QImage converted = image.convertToFormat(QImage::Format_RGB888);
        return cv::Mat(converted.height(), converted.width(), CV_8UC3, (void*)converted.bits(), converted.bytesPerLine());
    }
}

QImage FaceDetector::cvMatToQImage(const cv::Mat &mat)
{
    if (mat.type() == CV_8UC3) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    } else if (mat.type() == CV_8UC4) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
    }
    return QImage();
} 