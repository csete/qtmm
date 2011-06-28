#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QAudioInput>
#include <QList>


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

    QList<QAudioDeviceInfo> inputDevices;  /*! List of available audio input devices. */
    QAudioInput *audioInput;               /*! Audio input object. */

    void createDeviceSelector();

};

#endif // MAINWINDOW_H
