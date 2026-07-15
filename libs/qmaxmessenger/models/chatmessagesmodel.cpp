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

#include "chatmessagesmodel.h"
#include "qjsonobject.h"

#include <QJsonArray>

ChatMessagesModel::ChatMessagesModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_messQueue(MessagesQueue::instance())
    , m_canFetchMore(true)
{
    connect(m_messQueue, &MessagesQueue::messageReceived, this, &ChatMessagesModel::messagesHandler);

    m_hash.insert(Qt::UserRole, QByteArray("messageIcon"));
    m_hash.insert(Qt::UserRole+1, QByteArray("messageName"));
    m_hash.insert(Qt::UserRole+2, QByteArray("messageText"));
    m_hash.insert(Qt::UserRole+3, QByteArray("messageType"));
    m_hash.insert(Qt::UserRole+4, QByteArray("messageSenderId"));
    m_hash.insert(Qt::UserRole+5, QByteArray("messageExtendedData"));
    m_hash.insert(Qt::UserRole+6, QByteArray("reactionsCount"));
    m_hash.insert(Qt::UserRole+7, QByteArray("reactions"));
    m_hash.insert(Qt::UserRole+8, QByteArray("messageTime"));
    m_hash.insert(Qt::UserRole+9, QByteArray("replyToMessageId"));
    m_hash.insert(Qt::UserRole+10, QByteArray("replySenderName"));
    m_hash.insert(Qt::UserRole+11, QByteArray("replyMessageText"));
    m_hash.insert(Qt::UserRole+12, QByteArray("replyMessageType"));
    m_hash.insert(Qt::UserRole+13, QByteArray("replyMessageExtendedData"));
}

ChatMessagesModel::~ChatMessagesModel()
{
    m_messages.clear();
    disconnect(m_messQueue, &MessagesQueue::messageReceived, this, &ChatMessagesModel::messagesHandler);
}


void ChatMessagesModel::messagesHandler(RawApiMessage message)
{
    if(message.opcode() == RawApiMessage::OpCode::CHAT_HISTORY) {
        loadMessagesList(message.payload());
    }

    if(message.opcode() == RawApiMessage::OpCode::NOTIF_MESSAGE
        || message.opcode() == RawApiMessage::OpCode::MSG_SEND) {
        if(m_chat == nullptr) {
            return;
        }

        if(message.payload()["chatId"].toDouble() == m_chat->chatId()) {
            pushNewMessageToList(message.payload()["message"].toMap());
        }
    }
}

int ChatMessagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_messages.count();
}

QVariant ChatMessagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_messages.size()) {
        return QVariant();
    }

    ChatMessage* item = m_messages.at(index.row());
    if (role == Qt::UserRole) {
        return item->sender()->baseRawUrl();
    } else if (role == Qt::UserRole + 1) {
        return item->sender()->name();
    } else if (role == Qt::UserRole + 2) {
        return item->text();
    } else if (role == Qt::UserRole + 3) {
        return item->messageType();
    } else if (role == Qt::UserRole + 4) {
        return item->sender()->userId();
    } else if (role == Qt::UserRole + 5) {
        return item->extendedData();
    } else if (role == Qt::UserRole + 6) {
        return item->reactionsCount();
    } else if (role == Qt::UserRole + 7) {
        QVariantList list;
        for (ChatMessageReactions* reaction : item->reactions()) {
            list.append(QVariant::fromValue(reaction));
        }
        return list;
    } else if (role == Qt::UserRole + 8) {
        return QVariant::fromValue(item->messageTime());
    } else if (role == Qt::UserRole + 9) {
        return item->messageReply() != Q_NULLPTR ? item->messageReply()->messageID() : 0;
    } else if (role == Qt::UserRole + 10) {
        return item->messageReply() != Q_NULLPTR ? item->messageReply()->sender()->name() : "";
    } else if (role == Qt::UserRole + 11) {
        return item->messageReply() != Q_NULLPTR ? item->messageReply()->text() : "";
    } else if (role == Qt::UserRole + 12) {
        return item->messageReply() != Q_NULLPTR ? item->messageReply()->messageType() : 0;
    } else if (role == Qt::UserRole + 13) {
        return item->messageReply() != Q_NULLPTR ? item->messageReply()->extendedData() : "";
    }

    return QVariant();
}

bool ChatMessagesModel::canFetchMore(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_canFetchMore;
}

void ChatMessagesModel::fetchMore(const QModelIndex &index)
{
    Q_UNUSED(index);
    qint64 lastEventTime = 0;
    if(m_chat == nullptr) {
        return;
    }

    if(m_messages.count() > 0) {
        ChatMessage* firstInModel = m_messages.first();
        if(firstInModel == nullptr) {
            return;
        }

        lastEventTime = firstInModel->messageTime().toMSecsSinceEpoch();
    } else {
        lastEventTime = m_chat->lastEventTime();
    }
    requsetChat(lastEventTime);
}


void ChatMessagesModel::requsetChat(qint64 lastEventTime)
{
    if(m_chat == nullptr) {
        return;
    }

    QVariantMap payload;
    payload["backward"] = 30;
    payload["forward"] = 0;
    payload["getMessages"] = true;
    payload["chatId"] = m_chat->chatId();
    payload["from"] = lastEventTime;

    m_messQueue->sendMessage(RawApiMessage::OpCode::CHAT_HISTORY, payload);
}

void ChatMessagesModel::loadMessagesList(QVariantMap payload)
{
    QVariantMap messages = payload["messages"].toMap();
    if(messages.empty()) {
        return;
    }

    if(messages.count() < 30) {
        m_canFetchMore = false;
    }

    beginInsertRows(QModelIndex(), 0, messages.count() - 1);
    QList<ChatMessage*> newMessages;

    foreach(QVariant mess, messages) {
        ChatMessage* m = new ChatMessage(mess.toMap());
        if(m == nullptr) {
            continue;
        }

        if(m->messageID() > 0) {
            newMessages.push_back(m);
        }
    }

    foreach(ChatMessage* m, m_messages) {
        newMessages.push_back(m);
    }
    m_messages = newMessages;

    endInsertRows();
    emit chatLoaded();
}

void ChatMessagesModel::pushNewMessageToList(QVariantMap message)
{
    ChatMessage* m = new ChatMessage(message);
    if(m == nullptr || m->messageID() < 1) {
        return;
    }
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_messages.push_back(m);
    m_chat->addMessage(message);
    endInsertRows();
}


Chat *ChatMessagesModel::chat() const
{
    return m_chat;
}

void ChatMessagesModel::setChat(Chat *newChat)
{
    if (m_chat == newChat)
        return;
    m_chat = newChat;
    emit chatChanged();
}
