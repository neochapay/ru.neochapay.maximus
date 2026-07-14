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

#include "contactsstorage.h"
#include <QJsonArray>

static ContactsStorage* contactsStorageInstance = 0;

ContactsStorage::ContactsStorage(QObject *parent)
    : QObject{parent}
    , m_messQueue(MessagesQueue::instance())
    , m_requestTimer(new QTimer(this))
{
    connect(m_messQueue, &MessagesQueue::messageReceived, [=](RawApiMessage message) {
        if(message.opcode() == RawApiMessage::OpCode::LOGIN
            || message.opcode() == RawApiMessage::OpCode::CONTACT_INFO) {
            loadContactsList(message.payload());
        }
    });

    connect(m_requestTimer, &QTimer::timeout, this, &ContactsStorage::requestContacts);
    m_requestTimer->start(1000);
}

ContactsStorage *ContactsStorage::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (!contactsStorageInstance) {
        contactsStorageInstance = new ContactsStorage();
    }
    return contactsStorageInstance;
}

Contact *ContactsStorage::get(qint64 userId)
{
    if(m_contacts.contains(userId)) {
        return m_contacts.value(userId);
    } else {
        if(!m_contactsIdsForRequest.contains(userId)) {
            m_contactsIdsForRequest.push_back(userId);
        }
        return nullptr;
    }
}

void ContactsStorage::requestContact(int userId)
{
    if(!m_contactsIdsForRequest.contains(userId)) {
        m_contactsIdsForRequest.push_back(userId);
    }
}

void ContactsStorage::loadContactsList(QJsonObject payload)
{
    QJsonArray contacts = payload["contacts"].toArray();
    if(contacts.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Contacts list is empty";
        return;
    }

    QList<int> updatesIds;
    foreach (QJsonValue cont, contacts) {
        Contact* contact = new Contact(cont.toObject());
        if(contact == nullptr) {
            continue;
        }
        int contactId = contact->userId();
        if(contactId < 1) {
            qWarning() << "Wrong contact with wrond id";
            continue;
        }

        m_contacts.insert(contactId, contact);
        updatesIds.push_back(contactId);
    }

    if(!updatesIds.empty()) {
        emit contactStorageUpdated(updatesIds);
    }
}

void ContactsStorage::requestContacts()
{
    if(m_contactsIdsForRequest.empty()) {
        return;
    }

    QJsonArray contactIds;
    foreach (int contactId, m_contactsIdsForRequest) {
        contactIds << contactId;
    }
    QJsonObject payload;
    payload["contactIds"] = contactIds;

    m_messQueue->sendMessage(RawApiMessage::OpCode::CONTACT_INFO, payload);
    m_contactsIdsForRequest.clear();
}
