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

#include "messagesqueue.h"
#include <QMutexLocker>

static MessagesQueue* messagesQueueInstance = 0;

MessagesQueue::MessagesQueue(QObject *parent)
    : QObject(parent)
    , m_connection(new Connection())
    , m_messageId(0)
{
    QMutexLocker locker(&m_lock);
    connect(m_connection, &Connection::connectedChanged, this, &MessagesQueue::onConnectionChanged);
    connect(m_connection, &Connection::messageReceived, this, &MessagesQueue::onMessageReceived);
    connect(m_connection, &Connection::errorReceived, this, &MessagesQueue::connectionError);
}

MessagesQueue::~MessagesQueue()
{
}

MessagesQueue* MessagesQueue::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (!messagesQueueInstance) {
        messagesQueueInstance = new MessagesQueue();
    }
    return messagesQueueInstance;
}

int MessagesQueue::sendMessage(RawApiMessage::OpCode opCode, QVariantMap payload)
{
    int seq = m_messageId;
    RawApiMessage mess;
    mess.setSeq(seq);
    mess.setOpcode(opCode);
    mess.setPayload(payload);
    mess.setType(RawApiMessage::Type::out);

    if(m_connection->sendMessage(mess) > 0) {
        m_messageId++;
        return seq;
    }
    qWarning() << "Message not send";
    return -1;
}

void MessagesQueue::onConnectionChanged()
{
    if(m_connection->connected()) {
        emit readyToSend();
    } else {
        emit connectionClosed();
    }
}

void MessagesQueue::onMessageReceived(RawApiMessage message)
{
    emit messageReceived(message);
}
