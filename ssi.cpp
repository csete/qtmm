/****************************************************************************
**
** This file is based on the multimedia examples from the Qt toolkit
** and heavily modified by Alexandru Csete.
**
** The original code is:
**
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

#include <QPainter>

#include "ssi.h"

CSsi::CSsi(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    //setAutoFillBackground(true);

    setToolTip(tr("Input level indicator"));

    m_level = 0;
    m_alpha = 0.15;
    setMinimumHeight(30);
    setMinimumWidth(150);

    m_gradient = QLinearGradient(0, 0, 150, 0);
    //m_gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
    m_gradient.setColorAt(0.4, Qt::darkGreen);
    m_gradient.setColorAt(0.75, QColor(0xCB, 0xB8, 0x15, 0xFF));
    m_gradient.setColorAt(1.0, QColor(0xD0, 0x1C, 0x12, 0xFF));

    m_brush = QBrush(m_gradient);
}

void CSsi::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.drawRect(QRect(painter.viewport().left() + 10,
                           painter.viewport().top() + 9,
                           painter.viewport().right() - 20,
                           painter.viewport().bottom() - 18));
    if (m_level == 0.0)
        return;

    //painter.setPen(Qt::darkGreen);

    painter.setPen(QPen(m_brush, 1.0));

    int pos = ((painter.viewport().right()-20)-(painter.viewport().left() + 11)) * m_level;
    for (int i = 0; i < 10; ++i)
    {
        int x1 = painter.viewport().left() + 11;
        int y1 = painter.viewport().top() + 10 + i;
        int x2 = painter.viewport().left() + 20 + pos;
        int y2 = painter.viewport().top() + 10 + i;
        if (x2 < painter.viewport().left() + 10)
            x2 = painter.viewport().left() + 10;

        painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));
    }
}

void CSsi::setAlpha(qreal value)
{
    if ((value > 1.0) || (value < 0.0))
        return;

    m_alpha = value;
}

/*
 * Set new level.
 *
 * If the new level is greater than the current level apply it immediately, if it is
 * lower apply it through an exponential filter. This gives it smoother motion. The time
 * constant of the filter is set using setAlpha().
 */
void CSsi::setLevel(qreal value)
{
    if ((value > 1.0) || (value < 0.0))
        return;

    if (value > m_level)
    {
        m_level = value;
    }
    else
    {
        /* apply exponential filter */
        m_level = m_alpha * value + (1.0 - m_alpha) * m_level;
        if (m_level > 1.0)
            m_level = 1.0;
    }
    repaint();
}
