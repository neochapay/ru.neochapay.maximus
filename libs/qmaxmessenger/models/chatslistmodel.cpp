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

#include "chatslistmodel.h"

ChatsListModel::ChatsListModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_messQueue(MessagesQueue::instance())
{
    connect(m_messQueue, &MessagesQueue::messageReceived, [=](RawApiMessage message) {
        if(message.opcode() == RawApiMessage::OpCode::LOGIN) {
            loadChatsList(message.payload());
        } else if(message.opcode() == RawApiMessage::OpCode::NOTIF_MARK) {
            handleNewMessageChanges(message.payload());
        } else if(message.opcode() == RawApiMessage::OpCode::NOTIF_MESSAGE
                   || message.opcode() == RawApiMessage::OpCode::MSG_SEND) {
            addNewMessageToChat(message.payload());
            sortChats();
        }

    });

    m_hash.insert(Qt::UserRole, QByteArray("chatId"));
    m_hash.insert(Qt::UserRole + 1, QByteArray("title"));
    m_hash.insert(Qt::UserRole + 2, QByteArray("chatDescription"));
    m_hash.insert(Qt::UserRole + 3, QByteArray("lastMessageTime"));
    m_hash.insert(Qt::UserRole + 4, QByteArray("chatIcon"));
    m_hash.insert(Qt::UserRole + 5, QByteArray("lastEventTime"));
    m_hash.insert(Qt::UserRole + 6, QByteArray("canSendMessage"));
    m_hash.insert(Qt::UserRole + 7, QByteArray("unreadCount"));
}

int ChatsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_chats.count();
}

QVariant ChatsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_chats.size()) {
        return QVariant();
    }

    Chat* item = m_chats.at(index.row());
    if(item == nullptr) {
        return QVariant();
    }
    Contact participant;
    if(item->type() == Chat::DIALOG && item->participants().count() > 0) {
        participant = item->participants().first();
    }
    if (role == Qt::UserRole) {
        return item->chatId();
    } else if (role == Qt::UserRole + 1) {
        if(item->type() == Chat::CHAT || item->type() == Chat::CHANNEL) {
            return item->chatTitle();
        }
        if(item->type() == Chat::DIALOG) {
            if(participant.userId() > 0) {
                if (participant.lastName().isEmpty()) {
                    return participant.firstName();
                } else {
                    return participant.firstName() + " " + participant.lastName();
                }
            }
        }
        return "Unknow user";
    } else if (role == Qt::UserRole + 2) {
        return item->messages().last()->text();
    } else if (role == Qt::UserRole + 3) {
        return item->messages().last()->messageTime();
    } else if (role == Qt::UserRole + 4) {
        if(item->type() == Chat::CHAT || item->type() == Chat::CHANNEL) {
            return item->baseRawIconUrl();
        }
        if(item->type() == Chat::DIALOG) {
            if(participant.userId() > 0) {
                return participant.baseRawUrl();
            }
        }
        return QVariant();
    } else if (role == Qt::UserRole + 5) {
        return item->lastEventTime();
    } else if (role == Qt::UserRole + 6) {
        return item->type() != Chat::CHANNEL;
    } else if (role == Qt::UserRole + 7) {
        return item->newMessagesCount();
    }

    return QVariant();
}

Chat *ChatsListModel::get(int index)
{
    if (index >= m_chats.size() || index < 0) {
        return nullptr;
    }
    return m_chats.at(index);
}

void ChatsListModel::loadChatsList(QVariantMap payload)
{
    QVariantMap chats = payload["chats"].toMap();
    if(chats.count() == 0) {
        qWarning() << Q_FUNC_INFO << "Chats list is empty";
        return;
    }
    beginResetModel();
    m_chats.clear();
    foreach (QVariant cht, chats) {
        Chat* chat = new Chat(cht.toMap());
        m_chats.push_back(chat);
    }

    endResetModel();
}

void ChatsListModel::handleNewMessageChanges(QVariantMap payload)
{
    qint64 newUnreadCount = payload["unread"].toDouble();
    qint64 chatId = payload["chatId"].toDouble();

    for(int i = 0; i < m_chats.count(); ++i) {
        Chat* chat = m_chats.at(i);
        if(chat->chatId() == chatId) {
            chat->setNewMessagesCount(newUnreadCount);
            emit dataChanged(index(i, 0), index(i, 0));
            break;
        }
    }
}

void ChatsListModel::addNewMessageToChat(QVariantMap payload)
{
    for(int i = 0; i < m_chats.count(); ++i) {
        if(payload["chatId"].toDouble() == m_chats.at(i)->chatId()) {
            m_chats.at(i)->addMessage(payload["message"].toMap());
        }
    }
}

void ChatsListModel::sortChats()
{
    beginResetModel();
    std::sort(m_chats.begin(), m_chats.end(), [](Chat* a, Chat* b) {
        return a->messages().last()->messageTime() > b->messages().last()->messageTime();
    });
    endResetModel();
}
