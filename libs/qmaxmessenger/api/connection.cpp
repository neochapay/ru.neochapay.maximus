/*
 * Copyright (C) 2025-2026 Chupligin Sergey <neochapay@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "connection.h"
#include "qsslcipher.h"

#include <QDataStream>

Connection::Connection(QObject *parent)
    : QObject(parent)
    , m_connected(false)
{
    connect(&m_socket, &QSslSocket::connected, this, &Connection::onConnected);
    connect(&m_socket,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this,
            SLOT(onError(QAbstractSocket::SocketError)));
    connect(&m_socket, &QSslSocket::disconnected, this, &Connection::onDisconected);
    connect(&m_socket, &QSslSocket::readyRead, this, &Connection::onReadyRead);

    connectToSocket();
}

int Connection::sendMessage(RawApiMessage message)
{
    qDebug() << Q_FUNC_INFO << message;

    QByteArray payload = message.toByteArray();
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint8)message.ver();
    out << static_cast<quint8>(message.type());
    out << (quint16)message.seq();
    out << static_cast<quint16>(message.opcode());
    out << static_cast<quint32>(payload.size());

    packet.append(payload);

    int seq = m_socket.write(packet);
    m_socket.flush();
    return seq;
}

void Connection::onConnected()
{
    if(!m_connected) {
        m_connected = true;
        emit connectedChanged();
    }
}


void Connection::onDisconected()
{
    if(m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
    connectToSocket();
}


void Connection::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    qDebug() << Q_FUNC_INFO << m_socket.errorString();
    emit errorReceived(m_socket.errorString());

    if(m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
}

void Connection::onReadyRead()
{
    QByteArray data = m_socket.readAll();
    m_receiveBuffer.append(data);

    while (m_receiveBuffer.size() >= 10)
    {
        QDataStream in(&m_receiveBuffer, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::BigEndian);

        quint8 ver;
        quint8 cmd;
        quint16 seq;
        quint16 opcode;
        quint32 packedLen;

        in >> ver;
        in >> cmd;
        in >> seq;
        in >> opcode;
        in >> packedLen;

        quint8 flags = (packedLen >> 24) & 0xFF;
        quint32 payloadLen = packedLen & 0x00FFFFFF;

        const int headerSize = 10;
        const int frameSize = headerSize + payloadLen;

        if (m_receiveBuffer.size() < frameSize)
        {
            qDebug() << "WAIT:" << m_receiveBuffer.size() << "/" << frameSize;
            return;
        }

        QByteArray frame =  m_receiveBuffer.left(frameSize);
        m_receiveBuffer.remove(0, frameSize);

        RawApiMessage message(frame);

        qDebug() << Q_FUNC_INFO << message;
        emit messageReceived(message);
    }
}

void Connection::connectToSocket()
{
    m_socket.connectToHostEncrypted(
        "api.oneme.ru",
        443
    );
}

bool Connection::connected() const
{
    return m_connected;
}
