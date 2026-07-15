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

#ifndef CHATMESSAGESMODEL_H
#define CHATMESSAGESMODEL_H

#include <QAbstractListModel>
#include <QObject>

#include <api/messagesqueue.h>
#include <api/chatmessage.h>
#include <api/chat.h>

class ChatMessagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Chat* chat READ chat WRITE setChat NOTIFY chatChanged)

public:
    explicit ChatMessagesModel(QObject *parent = nullptr);
    virtual ~ChatMessagesModel();
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override { return m_hash; }
    Q_INVOKABLE void requsetChat(qint64 lastEventTime);

    Chat *chat() const;
    void setChat(Chat *newChat);

protected:
    bool canFetchMore(const QModelIndex& index = QModelIndex()) const override;
    void fetchMore(const QModelIndex& index = QModelIndex()) override;

signals:
    void chatLoaded();
    void chatChanged();

private slots:
    void messagesHandler(RawApiMessage message);

private:
    void loadMessagesList(QVariantMap payload);
    void pushNewMessageToList(QVariantMap message);

    MessagesQueue* m_messQueue;

    QHash<int, QByteArray> m_hash;
    QList<ChatMessage*> m_messages;
    Chat *m_chat;
    bool m_canFetchMore;
};

#endif // CHATMESSAGESMODEL_H
