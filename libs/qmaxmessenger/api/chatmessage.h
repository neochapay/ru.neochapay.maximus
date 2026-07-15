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
#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QObject>
#include <QDateTime>
#include <QVariant>

#include "contact.h"
#include "chatmessagereactions.h"

class ChatMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Contact* sender READ sender NOTIFY chatMessageChanged FINAL)
    Q_PROPERTY(qint64 messageID READ messageID NOTIFY chatMessageChanged FINAL)
    Q_PROPERTY(QDateTime messageTime READ messageTime NOTIFY chatMessageChanged FINAL)
    Q_PROPERTY(QString text READ text NOTIFY chatMessageChanged FINAL)
    Q_PROPERTY(QList<ChatMessageElement> elements READ elements NOTIFY chatMessageChanged FINAL)
    Q_PROPERTY(MessageType messageType READ messageType NOTIFY chatMessageChanged FINAL)
    Q_PROPERTY(QVariant extendedData READ extendedData NOTIFY chatMessageChanged)
    // TODO:
    // type
    // attaches
    Q_PROPERTY(int reactionsCount READ reactionsCount NOTIFY chatMessageChanged)
    Q_PROPERTY(QList<ChatMessageReactions*> reactions READ reactions NOTIFY chatMessageChanged)
    Q_PROPERTY(ChatMessage* messageReply READ messageReply NOTIFY chatMessageChanged FINAL)

public:
    struct ChatMessageElement
    {
        QString type;
        int from = 0;
        int length;
    };

    enum MessageType{
        UnknowMessageType,
        TextMessage,
        ControlMessage,
        PhotoMessage,
        VideoMessage,
        GalleryMessage
    };

    Q_ENUMS(MessageType)

    explicit ChatMessage(QObject *parent = nullptr);
    explicit ChatMessage(QVariantMap chatMessageObject, QObject *parent = nullptr);
    ChatMessage(const ChatMessage& other, QObject *parent = nullptr);
    ChatMessage& operator=(const ChatMessage& other);

    Contact* sender() const;
    qint64 messageID() const;
    QDateTime messageTime() const;
    QString text() const;
    QList<ChatMessageElement> elements() const;
    MessageType messageType() const;

    const QVariant &extendedData() const;

    const QList<ChatMessageReactions *> &reactions() const;

    int reactionsCount() const;
    ChatMessage* messageReply() const;

signals:
    void chatMessageChanged();

private:
    Contact* m_sender;
    qint64 m_messageID;
    QDateTime m_messageTime;
    QString m_text;
    QList<ChatMessageElement> m_elements;
    MessageType m_messageType;
    QVariant m_extendedData;
    QList<ChatMessageReactions*> m_reactions;
    int m_reactionsCount;
    ChatMessage* m_messageReply;
};
Q_DECLARE_METATYPE(ChatMessage);
#endif // CHATMESSAGE_H
