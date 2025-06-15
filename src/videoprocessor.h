#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QObject>
#include <QString>
#include <QImage>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class VideoProcessor : public QObject
{
    Q_OBJECT

public:
    explicit VideoProcessor(QObject *parent = nullptr);
    ~VideoProcessor();

    void setSource(const QString &source);
    void start();
    void stop();
    bool isRunning() const { return m_running; }

signals:
    void frameReady(const QImage &frame);

private:
    GstElement *m_pipeline;
    GstElement *m_appsink;
    QString m_source;
    bool m_running;
    
    static GstFlowReturn newFrameCallback(GstElement *sink, gpointer data);
    void processFrame(GstSample *sample);
    void createPipeline();
    void destroyPipeline();
};

#endif // VIDEOPROCESSOR_H 