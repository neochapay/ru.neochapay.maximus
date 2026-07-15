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

#include "contact.h"
#include "contactsstorage.h"
#include <QJsonObject>
#include <QJsonArray>

Contact::Contact(QObject *parent)
    : QObject{parent}
    , m_baseRawUrl("")
    , m_userId(-1)
    , m_name("")
    , m_contactStorage(ContactsStorage::instance())
{
}

Contact::Contact(QVariantMap object, QObject *parent)
    : QObject{parent}
    , m_baseRawUrl("")
    , m_userId(-1)
    , m_name("")
    , m_contactStorage(ContactsStorage::instance())
{
    m_userId = object["id"].toDouble();
    m_accountStatus = object["accountStatus"].toInt();
    m_baseRawUrl = object["baseRawUrl"].toString();
    m_baseUrl = object["baseUrl"].toString();

    QVariantMap names = object["names"].toMap().first().toMap();
    m_name = names["name"].toString();
    m_firstName = names["firstName"].toString();
    m_lastName = names["lastName"].toString();
    m_type = names["type"].toString();

    m_phone = object["phone"].toInt();
    m_photoId = object["photoId"].toInt();
    m_updateTime = object["updateTime"].toInt();

    //makeConnection();
}

Contact::Contact(int userId, QObject *parent)
    : QObject{parent}
    , m_baseRawUrl("")
    , m_userId(-1)
    , m_name("")
    , m_contactStorage(ContactsStorage::instance())
{
    Contact* fromStorage = m_contactStorage->get(userId);
    if(fromStorage != nullptr) {
        m_userId = fromStorage->userId();
        m_accountStatus = fromStorage->accountStatus();
        m_baseRawUrl = fromStorage->baseRawUrl();
        m_baseUrl = fromStorage->baseUrl();
        m_name = fromStorage->name();
        m_firstName = fromStorage->firstName();
        m_lastName = fromStorage->lastName();
        m_type = fromStorage->type();
        m_phone = fromStorage->phone();
        m_photoId = fromStorage->photoId();
        m_updateTime = fromStorage->updateTime();
    } else {
        m_userId = userId;
        m_contactStorage->requestContact(userId);
    }
}

Contact::Contact(const Contact &other, QObject *parent)
    : QObject{parent}
    , m_baseRawUrl("")
    , m_userId(-1)
    , m_name("")
    , m_contactStorage(ContactsStorage::instance())
{
    m_userId = other.userId();
    m_accountStatus = other.accountStatus();
    m_baseRawUrl = other.baseRawUrl();
    m_baseUrl = other.baseUrl();
    m_name = other.name();
    m_firstName = other.firstName();
    m_lastName = other.lastName();
    m_type = other.type();
    m_phone = other.phone();
    m_photoId = other.photoId();
    m_updateTime = other.updateTime();
    makeConnection();
}

Contact &Contact::operator=(const Contact &other)
{
    m_userId = other.userId();
    m_accountStatus = other.accountStatus();
    m_baseRawUrl = other.baseRawUrl();
    m_baseUrl = other.baseUrl();
    m_name = other.name();
    m_firstName = other.firstName();
    m_lastName = other.lastName();
    m_type = other.type();
    m_phone = other.phone();
    m_photoId = other.photoId();
    m_updateTime = other.updateTime();
    makeConnection();
    return *this;
}

int Contact::accountStatus() const
{
    return m_accountStatus;
}

QString Contact::baseRawUrl() const
{
    return m_baseRawUrl;
}

QString Contact::baseUrl() const
{
    return m_baseUrl;
}

qint64 Contact::userId() const
{
    return m_userId;
}

QString Contact::name() const
{
    return m_name;
}

QString Contact::firstName() const
{
    return m_firstName;
}

QString Contact::lastName() const
{
    return m_lastName;
}

QString Contact::type() const
{
    return m_type;
}

int Contact::phone() const
{
    return m_phone;
}

int Contact::photoId() const
{
    return m_photoId;
}

int Contact::updateTime() const
{
    return m_updateTime;
}

void Contact::makeConnection()
{
    connect(m_contactStorage, &ContactsStorage::contactStorageUpdated, this, [=](QList<int> idForUpdate) {
        if(idForUpdate.contains(m_userId)) {
            Contact* fromStorage = m_contactStorage->get(m_userId);
            if(fromStorage != nullptr) {
                m_userId = fromStorage->userId();
                m_accountStatus = fromStorage->accountStatus();
                m_baseRawUrl = fromStorage->baseRawUrl();
                m_baseUrl = fromStorage->baseUrl();
                m_name = fromStorage->name();
                m_firstName = fromStorage->firstName();
                m_lastName = fromStorage->lastName();
                m_type = fromStorage->type();
                m_phone = fromStorage->phone();
                m_photoId = fromStorage->photoId();
                m_updateTime = fromStorage->updateTime();

                emit contactChanged();
            } else {
                qWarning() << "CONTACT IS NULL!!!";
            }

        }
    });
}
