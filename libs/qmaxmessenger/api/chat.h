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

#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include "chatmessage.h"
#include "contact.h"

class Chat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Contact owner READ owner NOTIFY chatChanged FINAL)
    Q_PROPERTY(bool hasBots READ hasBots NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 joinTime READ joinTime NOTIFY chatChanged FINAL)
    Q_PROPERTY(int created READ created NOTIFY chatChanged FINAL)
    Q_PROPERTY(QList<ChatMessage*> messages READ messages NOTIFY chatChanged FINAL)
    Q_PROPERTY(ChatMessage* pinnedMessage READ pinnedMessage NOTIFY chatChanged)
    Q_PROPERTY(ChatType type READ type NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 lastFireDelayedErrorTime READ lastFireDelayedErrorTime NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 lastDelayedUpdateTime READ lastDelayedUpdateTime NOTIFY chatChanged FINAL)
    Q_PROPERTY(int prevMessageId READ prevMessageId NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 newMessagesCount READ newMessagesCount NOTIFY chatChanged)
    Q_PROPERTY(Access access READ access NOTIFY chatChanged FINAL)
//TODO options
    Q_PROPERTY(QDateTime modified READ modified NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 lastEventTime READ lastEventTime NOTIFY chatChanged FINAL)

    Q_PROPERTY(Status status READ status NOTIFY chatChanged FINAL)
    Q_PROPERTY(QList<Contact> participants READ participants NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 chatId READ chatId NOTIFY chatChanged FINAL)
    Q_PROPERTY(qint64 chatCid READ chatCid NOTIFY chatChanged FINAL)

//ONLY FOR Type == CHAT
    Q_PROPERTY(QString chatTitle READ chatTitle NOTIFY chatChanged FINAL)
    Q_PROPERTY(QString baseRawIconUrl READ baseRawIconUrl NOTIFY chatChanged FINAL)

//ONLY FOR Type == CHANNEL || Type == CHAT
    Q_PROPERTY(int participantsCount READ participantsCount NOTIFY chatChanged)


public:
    enum ChatType{
        UNKNOWTYPE,
        CHAT,
        DIALOG,
        CHANNEL
    };
    Q_ENUMS(ChatType)

    enum Status{
        UNKNOWSTATUS,
        ACTIVE,
        INACTIVE
    };

    enum Access{
        UNKNOWACCESS,
        PRIVATE,
        PUBLIC
    };

    explicit Chat(QObject *parent = nullptr);
    explicit Chat(QVariantMap jsonObject, QObject *parent = nullptr);
    Chat(const Chat& other, QObject *parent = nullptr);
    Chat& operator=(const Chat& other);

    qint64 chatId() const;
    Status status() const;
    qint64 lastEventTime() const;
    QDateTime modified() const;
    ChatType type() const;
    QList<ChatMessage*> messages() const;

    Contact owner() const;
    bool hasBots() const;
    int created() const;
    qint64 lastFireDelayedErrorTime() const;
    qint64 lastDelayedUpdateTime() const;
    qint64 prevMessageId() const;
    QList<Contact> participants() const;
    qint64 joinTime() const;
    QString chatTitle() const;
    QString baseRawIconUrl() const;
    ChatMessage *pinnedMessage() const;
    qint64 chatCid() const;
    Access access() const;
    int participantsCount() const;
    qint64 newMessagesCount() const;
    void setNewMessagesCount(qint64 count);
    void addMessage(QVariantMap message);

signals:
    void chatChanged();

private:
    qint64 m_chatId;
    Status m_status;
    qint64 m_lastEventTime;
    QDateTime m_modified;
    ChatType m_type;
    QList<ChatMessage*> m_messages;
    Contact m_owner;
    bool m_hasBots;
    int m_created;
    qint64 m_lastFireDelayedErrorTime;
    qint64 m_lastDelayedUpdateTime;
    qint64 m_prevMessageId;
    QList<Contact> m_participants;
    qint64 m_joinTime;
    QString m_chatTitle;
    QString m_baseRawIconUrl;
    ChatMessage *m_pinnedMessage;
    qint64 m_chatCid;
    Access m_access;
    int m_participantsCount;
    qint64 m_newMessagesCount;
};

#endif // CHAT_H
