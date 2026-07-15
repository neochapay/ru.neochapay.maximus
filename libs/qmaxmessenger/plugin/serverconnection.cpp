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

#include "serverconnection.h"
#include <QVariant>
#include <QUuid>

ServerConnection::ServerConnection(QObject *parent)
    : QObject(parent)
    , m_messQueue(MessagesQueue::instance())
    , m_settings(Settings::instance())
    , m_heartBeatTimer(new QTimer(this))
{
    QVariant deviceId = m_settings->value(QString("deviceId"));
    m_userId = m_settings->value(QString("userId"), "-1").toInt();

    if(deviceId.isNull()) {
        deviceId = QUuid::createUuid().toString();
        m_settings->setValue("deviceId", deviceId);
    }
    connect(m_messQueue, &MessagesQueue::readyToSend, this, &ServerConnection::init);
    connect(m_messQueue, &MessagesQueue::connectionClosed, m_heartBeatTimer, &QTimer::stop);
    connect(m_messQueue, &MessagesQueue::messageReceived, this, &ServerConnection::onMessageReceived);
    connect(m_messQueue, &MessagesQueue::connectionError, this, &ServerConnection::connectionError);

    connect(m_heartBeatTimer, &QTimer::timeout, this, &ServerConnection::sendHeartBeatMessage);
}

void ServerConnection::init()
{
    QVariantMap userAgent;
    userAgent["deviceType"] = "ANDROID";
    userAgent["appVersion"] = "26.14.1";
    userAgent["osVersion"] = "Android 14";
    userAgent["timezone"] = "Europe/Moscow";
    userAgent["screen"] = "428dpi 428dpi 1080x2400";
    userAgent["pushDeviceType"] = "GCM";
    userAgent["arch"] = "arm64-v8a";
    userAgent["locale"] = "ru";
    userAgent["buildNumber"] = 6686;
    userAgent["deviceName"] = "Pixel 8";
    userAgent["deviceLocale"] = "ru";

    QVariantMap payload;
    payload["mt_instanceid"] = "13584293248712345678";
    payload["userAgent"] = userAgent;
    payload["clientSessionId"] = 42;
    payload["deviceId"] = m_settings->value(QString("deviceId")).toString().remove('{').remove('}');

    m_messSeq = m_messQueue->sendMessage(RawApiMessage::OpCode::SESSION_INIT, payload);
    //m_heartBeatTimer->start(30000);
}

void ServerConnection::sendHeartBeatMessage()
{
    QVariantMap payload;
    payload["interactive"] = true;
    m_messQueue->sendMessage(RawApiMessage::OpCode::PING, payload);
}

void ServerConnection::onMessageReceived(RawApiMessage message)
{
    if(message.opcode() == RawApiMessage::OpCode::SESSION_INIT) {
        //COUNTRY CODES AND OTHER FOR PHONENUMBERS
        emit readyToLogin();
    }

    if(message.opcode() == RawApiMessage::OpCode::AUTH_REQUEST) {
        if(!message.payload()["token"].toString().isEmpty()) {
            emit tokenReady(message.payload()["token"].toString());
        }
    }

    if(message.opcode() == RawApiMessage::OpCode::AUTH) {
        QVariantMap payload = message.payload();
        if(!payload["passwordChallenge"].isNull()) {
            qDebug() << "READY TO LOGIN";
            emit readyToLogin();
        } else {
            QString trackId = payload["passwordChallenge"].toMap()["trackId"].toString();
            emit requestPassword(trackId);
        }
    }

    if(message.opcode() == RawApiMessage::OpCode::REQUIEST_TOKEN) {
        if(!message.payload()["error"].isNull()) {
            qWarning() << message.payload()["localizedMessage"];
            return;
        }
        QString token = message.payload()["tokenAttrs"].toMap()["LOGIN"].toMap()["token"].toString();
        if(!token.isEmpty()) {
            emit tokenReady(token);
        } else {
            qWarning() << message.payload();
        }
    }
    m_heartBeatTimer->start(30000);
}

void ServerConnection::sendPhone(QString phone)
{
    QVariantMap payload;
    payload["type"] = "START_AUTH";
    payload["language"] = "ru";
    payload["phone"] = phone;

    m_messQueue->sendMessage(RawApiMessage::OpCode::AUTH_REQUEST, payload);
}

void ServerConnection::sendCode(QString code)
{
    QVariantMap payload;
    payload["authTokenType"] = "CHECK_CODE";
    payload["token"] = m_settings->value(QString("token")).toString();
    payload["verifyCode"] = code;

    m_messQueue->sendMessage(RawApiMessage::OpCode::AUTH, payload);
}

void ServerConnection::sendPassword(QString password, QString trackId)
{
    QVariantMap payload;
    payload["password"] = password;
    payload["trackId"] = trackId;

    m_messQueue->sendMessage(RawApiMessage::OpCode::SEND_PASSWORD, payload);
}

void ServerConnection::requestDataSync()
{
    QVariantMap payload;
    payload["chatsCount"] = 40;
    payload["chatsSync"] = 0;
    payload["contactsSync"] = 0;
    payload["draftsSync"] = 0;
    payload["interactive"] = true;
    payload["presenceSync"] = 0;
    payload["token"] = m_settings->value(QString("authToken")).toString();
    m_messQueue->sendMessage(RawApiMessage::OpCode::LOGIN, payload);
}

void ServerConnection::requestContactsByIDs(QList<int> idS)
{
    QVariantMap payload;
    QVariantMap contactIds;
    foreach (int id, idS) {
        contactIds.insert(QString::number(id), id);
    }
    payload["contactIds"] = contactIds;
    m_messQueue->sendMessage(RawApiMessage::OpCode::CONTACT_INFO, payload);
}

void ServerConnection::requestChatById(int chatId, int from, int backward, int forward)
{
    QVariantMap payload;
    payload["chatId"] = chatId;
    payload["from"] = from;
    payload["backward"] = backward;
    payload["forward"] = forward;
    payload["getMessages"] = true;
    m_messQueue->sendMessage(RawApiMessage::OpCode::CHAT_HISTORY, payload);
}

void ServerConnection::refreshToken()
{
    QVariantMap payload;
    int seq = m_messQueue->sendMessage(RawApiMessage::OpCode::REQUIEST_TOKEN, payload);
    connect(m_messQueue, &MessagesQueue::messageReceived, [=](RawApiMessage message) {
        if(message.seq() == seq) {
            QVariantMap payload = message.payload();
            QString lifetimeToken = payload["token"].toString();
            int tokenLifeTime = payload["token_lifetime_ts"].toInt();
            int tokenRefresh = payload["token_refresh_ts"].toInt();

            m_settings->setValue("lifetimeToken", lifetimeToken);
            m_settings->setValue("tokenLifeTime", tokenLifeTime);
            m_settings->setValue("tokenRefresh", tokenRefresh);
        }
    });
}

void ServerConnection::sendMessage(Chat *chat, QString text)
{
    QVariantMap payload;
    payload["chatId"] = chat->chatId();
    payload["notify"] = true;
    QVariantMap message;
    message["text"] = text;
    message["cid"] = QDateTime::currentDateTime().toMSecsSinceEpoch();
    message["attaches"] = QVariantMap();
    message["elements"] = QVariantMap();
    payload["message"] = message;

    m_messQueue->sendMessage(RawApiMessage::OpCode::MSG_SEND, payload);
}
