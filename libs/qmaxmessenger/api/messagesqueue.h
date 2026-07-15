/*
 * Copyright (C) 2025 Chupligin Sergey <neochapay@gmail.com>
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

#ifndef MESSAGESQUEUE_H
#define MESSAGESQUEUE_H

#include <QMutex>
#include <QObject>

#include "connection.h"

class MessagesQueue : public QObject
{
    Q_OBJECT
public:
    explicit MessagesQueue(QObject *parent = nullptr);
    ~MessagesQueue();

    static MessagesQueue *instance();
    int sendMessage(RawApiMessage::OpCode opCode, QVariantMap payload);

signals:
    void readyToSend();
    void connectionClosed();
    void messageReceived(RawApiMessage message);
    void connectionError();

private slots:
    void onConnectionChanged();
    void onMessageReceived(RawApiMessage message);

private:
    QMutex m_lock;
    Connection* m_connection;
    int m_messageId;

    QList<RawApiMessage> m_queue;

    void pushToServer();
};

#endif // MESSAGESQUEUE_H
