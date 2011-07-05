#include <QDebug>
//#include <QtMultimedia>
#include <QAudio>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "multimon/multimon.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* select font for text viewer */
#ifdef Q_OS_MAC
    ui->textView->setFont(QFont("Monaco", 12));
#else
    ui->textView->setFont(QFont("Monospace", 9));
#endif

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

    /* initialise decoders */
    afsk12 = new CAfsk12();

    connect(afsk12, SIGNAL(newMessage(QString)), ui->textView, SLOT(appendPlainText(QString)));
}

MainWindow::~MainWindow()
{
    if (ui->actionDecode->isChecked()) {
        audioBuffer->stop();
        audioInput->stop();
        delete audioInput;
    }
    delete audioBuffer;

    delete inputLabel;
    delete inputSelector;
    delete ssi;
    delete afsk12;
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

#if 0
        qDebug() << "----------------------------------------------------";
        qDebug() << "Input device: " << inputDevices.at(inputSelector->currentIndex()).deviceName();
        qDebug() << "      Codecs: " << inputDevices.at(inputSelector->currentIndex()).supportedCodecs();
        qDebug() << "    Channels: " << inputDevices.at(inputSelector->currentIndex()).supportedChannelCounts();
        qDebug() << "Sample rates: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleRates();
        qDebug() << "Sample types: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleTypes();
        qDebug() << "Sample sizes: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleSizes();
        qDebug() << " Byte orders: " << inputDevices.at(inputSelector->currentIndex()).supportedByteOrders();
        qDebug() << "----------------------------------------------------";
#endif

#if 0
        qDebug() << "----------------------------------------------------";
        qDebug() << "      Codec: " << audioFormat.codec();
        qDebug() << " Byte order: " << audioFormat.byteOrder();
        qDebug() << "Sample rate: " << audioFormat.sampleRate();
        qDebug() << "Sample size: " << audioFormat.sampleSize();
        qDebug() << "Sample type: " << audioFormat.sampleType();
        qDebug() << "   Channels: " << audioFormat.channelCount();
        qDebug() << "----------------------------------------------------";
#endif

        /* initialise decoder; looks weird but dmeods were organised in array in multimon */
        afsk12->reset();

        audioInput = new QAudioInput(inputDevices.at(inputSelector->currentIndex()), audioFormat, this);

        /** TODO: cpnnect signals and slots */
        //connect(audioInput, SIGNAL(notify()), SLOT(notified()));
        connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(audioStateChanged(QAudio::State)));
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
void MainWindow::samplesReceived(float *buffer, const int length)
{
    int overlap = 18;
    int i;

    for (i = 0; i < length/*-overlap*/; i++) {
        tmpbuf.append(buffer[i]);
    }

    afsk12->demod(tmpbuf.data(), length);

    /* clear tmpbuf and store "overlap" */
    tmpbuf.clear();
    for (i = length-overlap; i < length; i++) {
        tmpbuf.append(buffer[i]);
    }

}


/*! \brief Slot for audio input state changes.
 *  \param state The new state of the audio input.
 */
void MainWindow::audioStateChanged(QAudio::State state)
{
    qDebug() << "Audio state change: " << state << " ERROR: " << audioInput->error();
#ifdef Q_OS_MAC
    /* On OSX audio stops due to underrun when window is minimized */
    if (state == 3) {
        audioBuffer->stop();
        audioBuffer->start();
        audioInput->start(audioBuffer);
    }
#endif
}

/** \brief Action: About AFSK1200 Decoder
  *
  * This slot is called when the user activates the
  * Help|About menu item (or AFSK1200 Decoder|About on Mac)
  */
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About AFSK1200 Decoder"),
                       tr("<p>AFSK1200 Decoder %1</p>"
                          "<p>A simple AFSK1200 decoder that uses the computer's soundcard "
                          "for input. It can decode AX.25 packets and displays them in a text view.</p>"
                          "<p>AFSK1200 is the modulation mode used by radio amateurs for packet radio "
                          "transmissions, including APRS.</p>"
                          "<p>AFSK1200 decoder is written using the Qt toolkit (see About Qt) and is avaialble "
                          "for Linux, Mac and Windows. You can download the latest version from the "
                          "<a href='http://www.oz9aec.net/'>website</a>."
                          ).arg(VERSION));
}

/** \brief Action: About Qt
  *
  * This slot is called when the user activates the
  *   Help|About Qt menu item (or AFSK Decoder|About Qt on Mac)
  */
void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}
