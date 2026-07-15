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

#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

class ContactsStorage;
class Contact : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int accountStatus READ accountStatus NOTIFY contactChanged FINAL)
    Q_PROPERTY(QString baseRawUrl READ baseRawUrl NOTIFY contactChanged FINAL)
    Q_PROPERTY(QString baseUrl READ baseUrl NOTIFY contactChanged FINAL)
    Q_PROPERTY(qint64 userId READ userId NOTIFY contactChanged FINAL)
    Q_PROPERTY(QString name READ name NOTIFY contactChanged FINAL)
    Q_PROPERTY(QString firstName READ firstName NOTIFY contactChanged FINAL)
    Q_PROPERTY(QString lastName READ lastName NOTIFY contactChanged FINAL)
    Q_PROPERTY(QString type READ type NOTIFY contactChanged FINAL)
    Q_PROPERTY(int phone READ phone NOTIFY contactChanged FINAL)
    Q_PROPERTY(int photoId READ photoId NOTIFY contactChanged FINAL)
    Q_PROPERTY(int updateTime READ updateTime NOTIFY contactChanged FINAL)

public:
    explicit Contact(QObject *parent = nullptr);
    explicit Contact(QVariantMap object, QObject *parent = nullptr);
    explicit Contact(int userId, QObject *parent = nullptr);
    Contact(const Contact &other, QObject *parent = nullptr);
    Contact& operator=(const Contact&other);

    int accountStatus() const;
    QString baseRawUrl() const;
    QString baseUrl() const;
    qint64 userId() const;
    QString name() const;
    QString firstName() const;
    QString lastName() const;
    QString type() const;
    int phone() const;
    int photoId() const;
    int updateTime() const;

signals:
    void contactChanged();

private:
    void makeConnection();

    int m_accountStatus;
    QString m_baseRawUrl;
    QString m_baseUrl;
    qint64 m_userId;
    QString m_name = "";
    QString m_firstName;
    QString m_lastName;
    QString m_type;
    int m_phone;
    int m_photoId;
    int m_updateTime;

    ContactsStorage* m_contactStorage;
};

#endif // CONTACT_H
