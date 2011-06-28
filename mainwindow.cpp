#include <QDebug>
#include <QtMultimedia>
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createDeviceSelector();

}

MainWindow::~MainWindow()
{
    delete inputLabel;
    delete inputSelector;
    delete ui;
}


/*! \brief Create device selector widget
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

    ui->actionDecode->setStatusTip(tr("Start decoder"));

}


/*! \brief Decoder status changed
 *  \param enabled True if the decoder has been enabled, false if it has been disabled.
 */
void MainWindow::on_actionDecode_triggered(bool enabled)
{
    if (enabled) {

        /* set up audio format */
        /** FIXME: this never changes and should be set up in the constructor checking for support */
        QAudioFormat format;
        format.setFrequency(22050);
        format.setChannels(1);
        //format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::Float);

#if 0
        qDebug() << "Input device: " << inputDevices.at(inputSelector->currentIndex()).deviceName();
        qDebug() << "      Codecs: " << inputDevices.at(inputSelector->currentIndex()).supportedCodecs();
        qDebug() << "    Channels: " << inputDevices.at(inputSelector->currentIndex()).supportedChannelCounts();
        qDebug() << "Sample rates: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleRates();
        qDebug() << "Sample types: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleTypes();
        qDebug() << "Sample sizes: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleSizes();
#endif

        audioInput = new QAudioInput(inputDevices.at(inputSelector->currentIndex()), format);

        /* disable selector and update status tip */
        inputSelector->setEnabled(false);
        ui->actionDecode->setStatusTip(tr("Stop decoder"));
    }
    else {
        /* enable selector and update status tip */
        inputSelector->setEnabled(true);
        ui->actionDecode->setStatusTip(tr("Start decoder"));

        delete audioInput;
    }
}

