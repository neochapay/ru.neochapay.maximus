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

#include "usersession.h"

#include <QCoreApplication>

UserSession::UserSession(QObject *parent)
    : QObject{parent}
    , m_messQueue(MessagesQueue::instance())
    , m_settings(Settings::instance())
    , m_sessionId(QDateTime::currentMSecsSinceEpoch()/1000)
    , m_actionId(0)
    , m_userProfile(new Contact())
{
    connect(m_messQueue, &MessagesQueue::messageReceived, [=](RawApiMessage message) {
        //FROM LOGIN DATA
        if(message.opcode() == RawApiMessage::OpCode::AUTH || message.opcode() == RawApiMessage::OpCode::REQUIEST_TOKEN) {
            updateSessionData(message.payload());
        }
        //FROM UPDATE ON START DATA
        if(message.opcode() == RawApiMessage::OpCode::LOGIN) {
            updateOnStartData(message.payload());
        }
        //FROM START
        if(message.opcode() == RawApiMessage::OpCode::SESSION_INIT) {
            coldStart();
        }
    });
}

int UserSession::userId() const
{
    return m_userProfile->userId();
}

void UserSession::coldStart()
{
    m_actionId++;
    m_eventPrevTime = QDateTime::currentMSecsSinceEpoch()/1000;
    QVariantMap params;
    params["session_id"] = m_sessionId;
    params["action_id"] = m_actionId;
    params["screen_to"] = 51;

    QVariantMap event;
    event["event"] = "COLD_START";
    event["type"] = "NAV";
    event["time"] = m_eventPrevTime;
    event["params"] = params;

    QVariantMap payload;
    payload["events"] = event;

    int seq = m_messQueue->sendMessage(RawApiMessage::OpCode::LOG, payload);
    connect(m_messQueue, &MessagesQueue::messageReceived, [=](RawApiMessage message) {
        if(seq == message.seq()) {
            qDebug() << "COLD START!";
        }
    });
}

void UserSession::storeToken(QString token) {
    m_settings->setValue("token", token);
}

void UserSession::logout()
{
    m_settings->reset();
    QCoreApplication::quit();
}

void UserSession::goNavigation(int from, int to)
{
    m_actionId++;
    QVariantMap params;
    params["action_id"] = m_actionId;
    params["prev_time"] = m_eventPrevTime;
    params["screen_from"] = from;
    params["screen_to"] = to;
    params["session_id"] = m_sessionId;

    m_eventPrevTime = QDateTime::currentMSecsSinceEpoch()/1000;
    QVariantMap event;
    event["event"] = "GO";
    event["type"] = "NAV";
    event["time"] = m_eventPrevTime;
    event["params"] = params;
    event["userId"] = m_userProfile->userId();

    QVariantMap payload;
    payload["events"] = event;
    int seq = m_messQueue->sendMessage(RawApiMessage::OpCode::LOG, payload);
    connect(m_messQueue, &MessagesQueue::messageReceived, [=](RawApiMessage message) {
        if(seq == message.seq()) {
            qDebug() << "PROFILE READY!";
        }
    });
}

void UserSession::updateSessionData(QVariantMap payload)
{
    QString authToken = payload["tokenAttrs"].toMap()["LOGIN"].toMap()["token"].toString();
    if(authToken.isEmpty()) {
        qWarning() << "Auth token is empty!";
        return;
    }
    m_settings->setValue("authToken", authToken);
    QVariantMap profile = payload["profile"].toMap();

    updateProfile(profile);
    emit userLogin();
}

void UserSession::updateOnStartData(QVariantMap payload)
{
    QVariantMap profile = payload["profile"].toMap();
    updateProfile(profile);
}

void UserSession::updateProfile(QVariantMap profile)
{
    Contact* userProfile = new Contact(profile);
    if(userProfile->userId() == 0) {
        qWarning() << "UserProfile ID is 0";
        return;
    }
    if(userProfile->userId() != m_userProfile->userId()) {
        m_userProfile = userProfile;
        emit userIdChanged();
   }
}

QString UserSession::authToken()
{
    QString authToken = m_settings->value("authToken").toString();;
    return authToken ;
}
