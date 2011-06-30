#include <QDebug>
#include <QtMultimedia>
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialiseAudio();
    createDeviceSelector();

    /* create SSI and to toolbar */
    ssiSpacer = new QWidget();
    ssiSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->addWidget(ssiSpacer);
    ssi = new CSsi(this);
    ui->mainToolBar->addWidget(ssi);

    connect(audioBuffer, SIGNAL(update(qreal)), ssi, SLOT(setLevel(qreal)));
    connect(audioBuffer, SIGNAL(newData(float*,int)), this, SLOT(samplesReceived(float*,int)));
}

MainWindow::~MainWindow()
{
    delete inputLabel;
    delete inputSelector;
    delete audioBuffer;
    delete ssi;
    delete ui;
}


/*! \brief Create device selector widget.
 *
 * The device selector widget is a combo box on the main toolbar
 * listing all available input devices.
 */
void MainWindow::createDeviceSelector()
{

    /* Add audio device selector */
    inputSelector = new QComboBox(this);
    inputSelector->setStatusTip(tr("Select audio input device"));

    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
         inputSelector->addItem(deviceInfo.deviceName());

         /* store deviceInfo */
         inputDevices.append(deviceInfo);
    }

    inputLabel = new QLabel(tr(" Input:"), this);
    ui->mainToolBar->insertWidget(ui->actionDecode, inputLabel);
    ui->mainToolBar->insertWidget(ui->actionDecode, inputSelector);

    ui->actionDecode->setToolTip(tr("Start decoder"));

}


/*! \brief Initialise audio related data. */
void MainWindow::initialiseAudio()
{
    audioFormat.setFrequency(22050);
    audioFormat.setChannels(1);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setCodec("audio/pcm");

    audioBuffer  = new CAudioBuffer(audioFormat, this);


}


/*! \brief Decoder status changed
 *  \param enabled True if the decoder has been enabled, false if it has been disabled.
 */
void MainWindow::on_actionDecode_triggered(bool enabled)
{
    if (enabled) {
        ui->statusBar->showMessage(tr("Starting decoder..."));

        /* check that selected input device supports desired format, if not try nearest */
        QAudioDeviceInfo info(inputDevices.at(inputSelector->currentIndex()));
        if (!info.isFormatSupported(audioFormat)) {
            qWarning() << "Default format not supported - trying to use nearest";
            audioFormat = info.nearestFormat(audioFormat);
        }

#if 1
        qDebug() << "Input device: " << inputDevices.at(inputSelector->currentIndex()).deviceName();
        qDebug() << "      Codecs: " << inputDevices.at(inputSelector->currentIndex()).supportedCodecs();
        qDebug() << "    Channels: " << inputDevices.at(inputSelector->currentIndex()).supportedChannelCounts();
        qDebug() << "Sample rates: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleRates();
        qDebug() << "Sample types: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleTypes();
        qDebug() << "Sample sizes: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleSizes();
        qDebug() << " Byte orders: " << inputDevices.at(inputSelector->currentIndex()).supportedByteOrders();
#endif


        audioInput = new QAudioInput(inputDevices.at(inputSelector->currentIndex()), audioFormat, this);

        /** TODO: cpnnect signals and slots */
        //connect(audioInput, SIGNAL(notify()), SLOT(notified()));
        //connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
        audioBuffer->start();
        audioInput->start(audioBuffer);

        /* disable selector and update status tip */
        inputSelector->setEnabled(false);
        ui->actionDecode->setToolTip(tr("Stop decoder"));

        ui->statusBar->showMessage(tr("Decoder running"));
    }
    else {
        ui->statusBar->showMessage(tr("Stopping decoder"));

        /* stop audio processing */
        audioBuffer->stop();
        audioInput->stop();
        /** TODO: disconnect signals and slots */
        delete audioInput;

        /* enable selector and update status tip */
        inputSelector->setEnabled(true);
        ui->actionDecode->setToolTip(tr("Start decoder"));

        ui->statusBar->showMessage(tr("Decoder stopped"));

        /* reset input level indicator */
        ssi->setLevel(0.0);
    }
}


/*! \brief Slot for receiveing new audio samples.
 *  \param data The sample buffer.
 *  \param length The number of samples in the buffer.
 *
 * Calls the afsk1200 decoder.
 */
void MainWindow::samplesReceived(float *data, const int length)
{
    //qDebug() << "Received " << length << " samples";
}
