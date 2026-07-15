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

#ifndef CONTACTSSTORAGE_H
#define CONTACTSSTORAGE_H

#include "api/contact.h"
#include "api/messagesqueue.h"
#include <QObject>
#include <QMutex>
#include <QTimer>

class ContactsStorage : public QObject
{
    Q_OBJECT

public:
    explicit ContactsStorage(QObject *parent = nullptr);
    static ContactsStorage *instance();
    Contact* get(qint64 userId);
    void requestContact(int userId);

signals:
    void contactStorageUpdated(QList<int> updatesIds);

private:
    void loadContactsList(QVariantMap payload);
    void requestContacts();

    QList<int> m_contactsIdsForRequest;
    QMutex m_lock;
    MessagesQueue* m_messQueue;
    QMap<qint64 , Contact*> m_contacts;
    QTimer* m_requestTimer;
};

#endif // CONTACTSSTORAGE_H
