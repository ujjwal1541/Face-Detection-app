#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createConnections();
    
    videoProcessor = new VideoProcessor(this);
    faceDetector = new FaceDetector(this);
    connect(videoProcessor, &VideoProcessor::frameReady, this, &MainWindow::updateFrame);
    connect(videoProcessor, &VideoProcessor::frameReady, faceDetector, &FaceDetector::processFrame);
    connect(faceDetector, &FaceDetector::facesDetected, this, &MainWindow::updateFaceCount);
}

MainWindow::~MainWindow()
{
    stopVideo();
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Video display
    videoLabel = new QLabel(this);
    videoLabel->setMinimumSize(640, 480);
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setStyleSheet("QLabel { background-color: black; }");
    mainLayout->addWidget(videoLabel);
    
    // Controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    stopButton->setEnabled(false);
    
    sourceComboBox = new QComboBox(this);
    sourceComboBox->addItem("Webcam", "v4l2src");
    sourceComboBox->addItem("File", "filesrc");
    
    faceCountLabel = new QLabel("Faces detected: 0", this);
    
    controlsLayout->addWidget(startButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addWidget(sourceComboBox);
    controlsLayout->addWidget(faceCountLabel);
    
    mainLayout->addLayout(controlsLayout);
    
    setWindowTitle("Face Detection Application");
    resize(800, 600);
}

void MainWindow::createConnections()
{
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startVideo);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopVideo);
    connect(sourceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::switchSource);
}

void MainWindow::startVideo()
{
    QString source = sourceComboBox->currentData().toString();
    if (source == "filesrc") {
        QString fileName = QFileDialog::getOpenFileName(this,
            "Open Video File", "", "Video Files (*.mp4 *.avi *.mkv)");
        if (fileName.isEmpty()) return;
        videoProcessor->setSource(fileName);
    } else {
        videoProcessor->setSource("0"); // Default webcam
    }
    
    videoProcessor->start();
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    sourceComboBox->setEnabled(false);
}

void MainWindow::stopVideo()
{
    videoProcessor->stop();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    sourceComboBox->setEnabled(true);
    videoLabel->clear();
    faceCountLabel->setText("Faces detected: 0");
}

void MainWindow::switchSource()
{
    if (videoProcessor->isRunning()) {
        stopVideo();
    }
}

void MainWindow::updateFrame(const QImage &frame)
{
    videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(
        videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::updateFaceCount(int count)
{
    faceCountLabel->setText(QString("Faces detected: %1").arg(count));
} 