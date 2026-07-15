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

#ifndef USERSESSION_H
#define USERSESSION_H

#include <QObject>
#include <api/messagesqueue.h>
#include <api/settings.h>
#include <api/contact.h>

class UserSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int userId READ userId NOTIFY userIdChanged FINAL)
    Q_PROPERTY(QString authToken READ authToken NOTIFY authTokenChanged)

public:
    explicit UserSession(QObject *parent = nullptr);
    int userId() const;
    Q_INVOKABLE void coldStart();
    Q_INVOKABLE void storeToken(QString token);
    Q_INVOKABLE void logout();

    QString authToken();

signals:
    void userIdChanged();
    void authTokenChanged();
    void userLogin();
    void userLogout();

private:
    void goNavigation(int from, int to);

    void updateSessionData(QVariantMap payload);
    void updateOnStartData(QVariantMap payload);
    void updateProfile(QVariantMap profile);

    MessagesQueue* m_messQueue;
    Settings* m_settings;
    int m_sessionId;
    int m_eventPrevTime;
    int m_actionId;

    Contact *m_userProfile;
};

#endif // USERSESSION_H
