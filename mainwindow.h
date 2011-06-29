#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QAudioInput>
#include <QList>
#include "audiobuffer.h"
#include "ssi.h"


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

    void createDeviceSelector();
    void initialiseAudio();

};

#endif // MAINWINDOW_H
