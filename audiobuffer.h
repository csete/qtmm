#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QIODevice>
#include <QAudioInput>
#include <QAudioFormat>


class CAudioBuffer : public QIODevice
{
    Q_OBJECT

public:
    CAudioBuffer(const QAudioFormat &format, QObject *parent);
    ~CAudioBuffer();

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    const QAudioFormat m_format;
    quint16 m_maxAmplitude;
    qreal m_level; // 0.0 <= m_level <= 1.0

signals:
    void update();
};


#endif // AUDIOBUFFER_H
