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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <qwebsocket.h>
#include "rawapimessage.h"

class Connection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    explicit Connection(QObject *parent = nullptr);
    int sendMessage(RawApiMessage message);
    bool connected() const;

signals:
    void connectedChanged();
    void errorReceived(QString errorString);
    void messageReceived(RawApiMessage message);

private slots:
    void onDisconected();
    void onConnected();
    void onError(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    QSslSocket m_socket;
    bool m_connected;
    QByteArray m_receiveBuffer;

    void connectToSocket();
};

#endif // CONNECTION_H
