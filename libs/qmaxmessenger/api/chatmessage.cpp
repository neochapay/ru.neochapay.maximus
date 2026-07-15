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

#include "chatmessage.h"
#include <QJsonArray>

ChatMessage::ChatMessage(QObject *parent)
    : QObject{parent}
    , m_reactionsCount(0)
    , m_messageReply(Q_NULLPTR)
{
}

ChatMessage::ChatMessage(QVariantMap chatMessageObject, QObject *parent)
    : QObject{parent}
    , m_messageType(UnknowMessageType)
    , m_reactionsCount(0)
    , m_messageReply(Q_NULLPTR)
{
    m_sender = new Contact(chatMessageObject["sender"].toInt());
    m_messageID = chatMessageObject["id"].toString().toDouble();
    m_text = chatMessageObject["text"].toString();
    m_messageTime = QDateTime::fromMSecsSinceEpoch(chatMessageObject["time"].toDouble());

    if(chatMessageObject.contains("link")) {
        QVariantMap chatReply = chatMessageObject["link"].toMap()["message"].toMap();
        m_messageReply = new ChatMessage(chatReply);
    }


    QVariantMap elements = chatMessageObject["elements"].toMap();
    foreach (QVariant element, elements) {
        QVariantMap e = element.toMap();
        ChatMessageElement chatMessageElement;
        chatMessageElement.type = e["type"].toString();
        chatMessageElement.from = e["from"].toInt();
        chatMessageElement.length = e["length"].toInt();
        m_elements.push_back(chatMessageElement);
    }

    if(!m_text.isEmpty()) {
        m_messageType = TextMessage;
    }
//Attaches
    QVariantMap attaches = chatMessageObject["attaches"].toMap();
    foreach (QVariant attach, attaches) {
        QVariantMap attachObject = attach.toMap();
        QString type = attachObject["_type"].toString();
        if(type == "CONTROL") {
            m_messageType = ControlMessage;
        } else if(type == "PHOTO") {
            m_messageType = PhotoMessage;
            m_extendedData = attachObject["baseUrl"].toString();
        }
    }

//Reactions
    if(chatMessageObject.contains("reactionInfo")) {
        m_reactionsCount = chatMessageObject["reactionInfo"].toMap()["totalCount"].toInt();
        QVariantMap reactions = chatMessageObject["reactionInfo"].toMap()["counters"].toMap();
        foreach(QVariant reaction, reactions) {
            int reactionsCount = reaction.toMap()["count"].toInt();
            QString reactionsString = reaction.toMap()["reaction"].toString();
            ChatMessageReactions* r = new ChatMessageReactions(reactionsCount, reactionsString);
            m_reactions.push_back(r);
        }
    }
}

ChatMessage::ChatMessage(const ChatMessage &other, QObject *parent)
    : QObject{parent}
    , m_messageType(UnknowMessageType)
{
    m_sender = other.sender();
    m_messageID = other.messageID();
    m_text = other.text();
    m_messageTime = other.messageTime();
    m_elements = other.elements();
    m_messageType = other.messageType();
    m_messageReply = other.messageReply();
    m_reactions = other.reactions();
    m_reactionsCount = other.reactionsCount();
}

ChatMessage &ChatMessage::operator=(const ChatMessage &other)
{
    m_sender = other.sender();
    m_messageID = other.messageID();
    m_text = other.text();
    m_messageTime = other.messageTime();
    m_elements = other.elements();
    m_messageType = other.messageType();
    m_messageReply = other.messageReply();
    m_reactions = other.reactions();
    m_reactionsCount = other.reactionsCount();
    return *this;
}

Contact *ChatMessage::sender() const
{
    return m_sender;
}

qint64 ChatMessage::messageID() const
{
    return m_messageID;
}

QDateTime ChatMessage::messageTime() const
{
    return m_messageTime;
}

QString ChatMessage::text() const
{
    return m_text;
}

QList<ChatMessage::ChatMessageElement> ChatMessage::elements() const
{
    return m_elements;
}

ChatMessage::MessageType ChatMessage::messageType() const
{
    return m_messageType;
}

const QVariant &ChatMessage::extendedData() const
{
    return m_extendedData;
}

const QList<ChatMessageReactions *> &ChatMessage::reactions() const
{
    return m_reactions;
}

int ChatMessage::reactionsCount() const
{
    return m_reactionsCount;
}

ChatMessage* ChatMessage::messageReply() const
{
    return m_messageReply;
}
