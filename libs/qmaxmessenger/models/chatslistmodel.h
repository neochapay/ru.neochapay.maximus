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

#ifndef CHATSLISTMODEL_H
#define CHATSLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <api/messagesqueue.h>
#include <api/chat.h>

class ChatsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatsListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QHash<int, QByteArray> roleNames() const { return m_hash; }
    Q_INVOKABLE Chat* get(int index);

private:
    void loadChatsList(QVariantMap payload);
    void handleNewMessageChanges(QVariantMap payload);
    void addNewMessageToChat(QVariantMap payload);
    void sortChats();
    MessagesQueue* m_messQueue;

    QHash<int, QByteArray> m_hash;
    QList<Chat*> m_chats;
};

#endif // CHATSLISTMODEL_H
