/****************************************************************************
**
** This file is based on the multimedia examples from the Qt toolkit
** and heavily modified by Alexandru Csete.
**
** The original code is:
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QDebug>
#include <QtCore/qendian.h>

#include "audiobuffer.h"


const int BufferSize = 4096;

CAudioBuffer::CAudioBuffer(const QAudioFormat &format, QObject *parent)
    :   QIODevice(parent)
    ,   m_format(format)
    ,   m_maxAmplitude(0)
    ,   m_level(0.0)

{
    switch (m_format.sampleSize())
    {
    case 8:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;

    case 16:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;
    default:
        qWarning() << "Unsupported audio format.";
        break;
    }

    qDebug() << "CAudioBuffer::m_maxAmplitude = " << m_maxAmplitude;
}

CAudioBuffer::~CAudioBuffer()
{
}

void CAudioBuffer::start()
{
    open(QIODevice::WriteOnly);
}

void CAudioBuffer::stop()
{
    close();
}

qint64 CAudioBuffer::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 CAudioBuffer::writeData(const char *data, qint64 len)
{

    if (m_maxAmplitude)
    {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);
        const int channelBytes = m_format.sampleSize() / 8;
        const int sampleBytes = m_format.channelCount() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        /* clear buffer */
        m_buffer.clear();

        qint16 maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for (int i = 0; i < numSamples; ++i)
        {
            for(int j = 0; j < m_format.channelCount(); ++j)
            {
                qint16 value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    value = *reinterpret_cast<const quint8*>(ptr);
                }
                else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                }
                else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                }
                else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        //value = qAbs(qFromLittleEndian<qint16>(ptr));
                        value = qFromLittleEndian<qint16>(ptr);
                    else
                        //value = qAbs(qFromBigEndian<qint16>(ptr));
                        value = qFromBigEndian<qint16>(ptr);
                }

                m_buffer.append(((float)value)*1.0/m_maxAmplitude);
                //qDebug() << ((float)value)/m_maxAmplitude;

                maxValue = qMax(qAbs(value), maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, m_maxAmplitude);
        m_level = qreal(maxValue) / m_maxAmplitude;

        /* send signal to decoder */
        emit newData(m_buffer.data(), m_buffer.size());
    }

    /* send signal to SSI */
    emit update(m_level);

    return len;
}
