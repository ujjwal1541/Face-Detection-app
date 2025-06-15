#include "videoprocessor.h"
#include <QDebug>

VideoProcessor::VideoProcessor(QObject *parent)
    : QObject(parent)
    , m_pipeline(nullptr)
    , m_appsink(nullptr)
    , m_running(false)
{
    // Initialize GStreamer
    gst_init(nullptr, nullptr);
}

VideoProcessor::~VideoProcessor()
{
    stop();
    gst_deinit();
}

void VideoProcessor::setSource(const QString &source)
{
    m_source = source;
}

void VideoProcessor::start()
{
    if (m_running) return;
    
    createPipeline();
    
    if (m_pipeline) {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qDebug() << "Failed to start pipeline";
            return;
        }
        m_running = true;
    }
}

void VideoProcessor::stop()
{
    if (!m_running) return;
    
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        destroyPipeline();
    }
    
    m_running = false;
}

void VideoProcessor::createPipeline()
{
    QString pipelineStr;
    if (m_source.startsWith("v4l2src")) {
        pipelineStr = QString("v4l2src device=/dev/video%1 ! "
                            "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! "
                            "videoconvert ! "
                            "appsink name=sink emit-signals=true sync=false")
                            .arg(m_source.mid(1));
    } else {
        pipelineStr = QString("filesrc location=\"%1\" ! "
                            "decodebin ! "
                            "videoconvert ! "
                            "appsink name=sink emit-signals=true sync=false")
                            .arg(m_source);
    }
    
    m_pipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), nullptr);
    if (!m_pipeline) {
        qDebug() << "Failed to create pipeline";
        return;
    }
    
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "sink");
    if (!m_appsink) {
        qDebug() << "Failed to get appsink element";
        return;
    }
    
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(newFrameCallback), this);
}

void VideoProcessor::destroyPipeline()
{
    if (m_appsink) {
        gst_object_unref(m_appsink);
        m_appsink = nullptr;
    }
    
    if (m_pipeline) {
        gst_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

GstFlowReturn VideoProcessor::newFrameCallback(GstElement *sink, gpointer data)
{
    VideoProcessor *processor = static_cast<VideoProcessor*>(data);
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
    
    if (sample) {
        processor->processFrame(sample);
        gst_sample_unref(sample);
    }
    
    return GST_FLOW_OK;
}

void VideoProcessor::processFrame(GstSample *sample)
{
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *structure = gst_caps_get_structure(caps, 0);
        
        int width, height;
        gst_structure_get_int(structure, "width", &width);
        gst_structure_get_int(structure, "height", &height);
        
        QImage frame(map.data, width, height, QImage::Format_RGB888);
        QImage copy = frame.copy(); // Create a deep copy
        
        emit frameReady(copy);
        
        gst_buffer_unmap(buffer, &map);
    }
} 