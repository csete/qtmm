#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QAudioInput>
#include <QList>
#include "audiobuffer.h"
#include "ssi.h"
#include "multimon/multimon.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionDecode_triggered(bool enabled);
    void samplesReceived(float *buffer, const int length);

private:
    Ui::MainWindow *ui;

    QLabel     *inputLabel;
    QComboBox  *inputSelector;  /*! Audio input delector. */
    QWidget    *ssiSpacer;      /*! Spacer used to right align ssi. */
    CSsi       *ssi;            /*! Input level indicator. */

    QList<QAudioDeviceInfo> inputDevices;  /*! List of available audio input devices. */
    QAudioInput  *audioInput;               /*! Audio input object. */
    QAudioFormat  audioFormat;              /*! Audio format info. */
    CAudioBuffer *audioBuffer;              /*! Audio buffer. */

    demod_state  *afsk1200_state;          /*! AFSK1200 demodulator state. */
    QVarLengthArray<float, 8192> tmpbuf;   /*! Needed to remember "overlap" smples. */

    void createDeviceSelector();
    void initialiseAudio();

    void process(QByteArray buff);

};

#endif // MAINWINDOW_H
