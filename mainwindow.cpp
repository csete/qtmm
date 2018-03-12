/*
 *      mainwindow.cpp -- Main application window for AFSK1200 demodulator
 *
 *      Copyright (C) 2011 Alexandru Csete (oz9aec at gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <QAudio>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    ui->statusBar->showMessage(tr("Decoder ready - select and input source then press start"));
}

MainWindow::~MainWindow()
{
    if (ui->actionDecode->isChecked())
    {
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

/*
 * Create device selector widget.
 *
 * The device selector widget is a combo box on the main toolbar
 * listing all available input devices.
 */
void MainWindow::createDeviceSelector()
{
    /* Add audio device selector */
    inputSelector = new QComboBox(this);
    inputSelector->setToolTip(tr("Select audio input device"));
    connect(inputSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(inputSelectionChanged(int)));

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
    audioFormat.setSampleRate(22050);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setCodec("audio/pcm");

    audioBuffer  = new CAudioBuffer(audioFormat, this);
}

/*
 * Input selection has changed.
 * This slot is activated when the user selects a new input device
*/
void MainWindow::inputSelectionChanged(int index)
{
    Q_UNUSED(index);

    /* check whether decoder is running, if so, restart */
    if (ui->actionDecode->isChecked())
    {
        ui->actionDecode->toggle();
        ui->actionDecode->toggle();
    }

    /* if decoder is not running there is nothing to do */
}

/*
 * Decoder status changed
 * If enabled=true if the decoder has been enabled, false if it has been disabled.
 */
void MainWindow::on_actionDecode_toggled(bool enabled)
{
    if (enabled)
    {
        ui->statusBar->showMessage(tr("Starting decoder..."));

        /* check that selected input device supports desired format, if not try nearest */
        QAudioDeviceInfo info(inputDevices.at(inputSelector->currentIndex()));
        if (!info.isFormatSupported(audioFormat))
        {
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

        /** TODO: connect signals and slots */
        //connect(audioInput, SIGNAL(notify()), SLOT(notified()));
        connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(audioStateChanged(QAudio::State)));
        audioBuffer->start();
        audioInput->start(audioBuffer);

        ui->actionDecode->setToolTip(tr("Stop decoder"));
        ui->statusBar->showMessage(tr("Decoder running"));
    }
    else
    {
        ui->statusBar->showMessage(tr("Stopping decoder"));

        /* stop audio processing */
        audioBuffer->stop();
        audioInput->stop();
        /** TODO: disconnect signals and slots */
        delete audioInput;

        ui->actionDecode->setToolTip(tr("Start decoder"));
        ui->statusBar->showMessage(tr("Decoder stopped"));

        /* reset input level indicator */
        ssi->setLevel(0.0);
    }
}

/*
 * Slot for receiveing new audio samples.
 *   data is the sample buffer.
 *   length is the number of samples in the buffer.
 */
void MainWindow::samplesReceived(float *buffer, const int length)
{
    int overlap = 18;
    int i;

    for (i = 0; i < length; i++)
    {
        tmpbuf.append(buffer[i]);
    }

    afsk12->demod(tmpbuf.data(), length);

    /* clear tmpbuf and store "overlap" */
    tmpbuf.clear();
    for (i = length-overlap; i < length; i++)
    {
        tmpbuf.append(buffer[i]);
    }
}

/*
 * Slot for audio input state changes.
 *   state is the new state of the audio input.
 */
void MainWindow::audioStateChanged(QAudio::State state)
{
    qDebug() << "Audio state change: " << state << " ERROR: " << audioInput->error();
#ifdef Q_OS_MAC
    /* On OSX audio stops due to underrun when window is minimized */
    if (state == 3)
    {
        audioBuffer->stop();
        audioBuffer->start();
        audioInput->start(audioBuffer);
    }
#endif
}

void MainWindow::on_actionClear_triggered()
{
    ui->textView->clear();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    QDir::homePath(),
                                                    tr("Text Files (*.txt)"));

    if (fileName.isEmpty())
    {
        qDebug() << "Save as cancelled by user";
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error creating file: " << fileName;
        return;
    }

    QTextStream out(&file);
    out << ui->textView->toPlainText();
    file.close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       tr("About AFSK1200 decoder"),
                       tr("<p>AFSK1200 Decoder %1</p>"
                          "<p>A simple AFSK1200 decoder that uses the computer's soundcard "
                          "for input. It can decode AX.25 packets and displays them in a text view.</p>"
                          "<p>AFSK1200 is the modulation mode used by radio amateurs for packet radio "
                          "transmissions, including APRS.</p>"
                          "<p>AFSK1200 decoder is written using the Qt toolkit (see About Qt) and is avaialble "
                          "for Linux, Mac and Windows. You can download the latest version from the "
                          "<a href='http://qtmm.sf.net/'>application website</a>."
                          ).arg(VERSION));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}
