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

#ifndef RAWAPIMESSAGE_H
#define RAWAPIMESSAGE_H

#include <QObject>
#include <msgpack.h>

class RawApiMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY rawMessageChanged)
    Q_PROPERTY(OpCode opcode READ opcode WRITE setOpcode NOTIFY rawMessageChanged)
    Q_PROPERTY(int seq READ seq WRITE setSeq NOTIFY rawMessageChanged)
    Q_PROPERTY(int ver READ ver)
    Q_PROPERTY(QVariantMap payload READ payload WRITE setPayload NOTIFY rawMessageChanged)

public:
    enum Type{
        out = 0,
        in = 1
    };

    enum OpCode{
        PING = 1,
        LOG = 5,
        SESSION_INIT = 6,
        PROFILE = 16,
        AUTH_REQUEST = 17,
        AUTH = 18,
        LOGIN = 19,
        LOGOUT = 20,
        SYNC = 21,
        CONFIG = 22,
        AUTH_CONFIRM = 23,
        ASSETS_GET = 26,
        ASSETS_UPDATE = 27,
        ASSETS_GET_BY_IDS = 28,
        ASSETS_ADD = 29,
        CONTACT_INFO = 32,
        CONTACT_UPDATE = 34,
        CONTACT_PRESENCE = 35,
        CONTACT_LIST = 36,
        CONTACT_PHOTOS = 39,
        CONTACT_CREATE = 41,
        REMOVE_CONTACT_PHOTO = 43,
        OWN_CONTACT_SEARCH = 44,
        CONTACT_INFO_EXTERNAL = 45,
        CHAT_INFO = 48,
        CHAT_HISTORY = 49,
        CHAT_MARK = 50,
        CHAT_MEDIA = 51,
        CHAT_DELETE = 52,
        CHAT_LIST = 53,
        CHAT_CLEAR = 54,
        CHAT_UPDATE = 55,
        CHAT_CHECK_LINK = 56,
        CHAT_JOIN = 57,
        CHAT_LEAVE = 58,
        CHAT_MEMBERS = 59,
        CHAT_CLOSE = 61,
        PUBLIC_SEARCH = 60,
        CHAT_CREATE = 63,
        MSG_SEND = 64,
        MSG_TYPING = 65,
        MSG_DELETE = 66,
        MSG_EDIT = 67,
        CHAT_SEARCH = 68,
        MSG_SHARE_PREVIEW = 70,
        MSG_GET = 71,
        MSG_SEARCH_TOUCH = 72,
        MSG_SEARCH = 73,
        MSG_GET_STAT = 74,
        CHAT_SUBSCRIBE = 75,
        VIDEO_CHAT_START = 76,
        VIDEO_CHAT_COMMAND = 78,
        CHAT_MEMBERS_UPDATE = 77,
        PHOTO_UPLOAD = 80,
        STICKER_UPLOAD = 81,
        VIDEO_UPLOAD = 82,
        VIDEO_PLAY = 83,
        MUSIC_PLAY = 84,
        MUSIC_PLAY30 = 85,
        FILE_UPLOAD = 87,
        CHAT_PIN_SET_VISIBILITY = 86,
        FILE_DOWNLOAD = 88,
        LINK_INFO = 89,
        MESSAGE_LINK = 90,
        MSG_CONSTRUCT = 94,
        SESSIONS_INFO = 96,
        SESSIONS_CLOSE = 97,
        PHONE_BIND_REQUEST = 98,
        PHONE_BIND_CONFIRM = 99,
        UNBIND_OK_PROFILE = 100,
        VIDEO_CHAT_JOIN = 102,
        SEND_PASSWORD = 115,
        CHAT_COMPLAIN = 117,
        MSG_SEND_CALLBACK = 118,
        SUSPEND_BOT = 119,
        LOCATION_STOP = 124,
        LOCATION_SEND = 125,
        LOCATION_REQUEST = 126,
        NOTIF_MESSAGE = 128,
        NOTIF_TYPING = 129,
        NOTIF_MARK = 130,
        NOTIF_CONTACT = 131,
        NOTIF_PRESENCE = 132,
        NOTIF_CONFIG = 134,
        NOTIF_CHAT = 135,
        NOTIF_ATTACH = 136,
        NOTIF_VIDEO_CHAT_START = 137,
        NOTIF_VIDEO_CHAT_COMMAND = 138,
        NOTIF_CALLBACK_ANSWER = 143,
        CHAT_BOT_COMMANDS = 144,
        NOTIF_MSG_CONSTRUCT = 146,
        NOTIF_LOCATION = 147,
        NOTIF_LOCATION_REQUEST = 148,
        NOTIF_ASSETS_UPDATE = 150,
        NOTIF_DRAFT = 152,
        NOTIF_DRAFT_DISCARD = 153,
        NOTIF_MSG_DELAYED = 154,
        NOTIF_MSG_REACTIONS_CHANGED = 155,
        NOTIF_MSG_YOU_REACTED = 156,
        REQUIEST_TOKEN = 158,
        DRAFT_SAVE = 176,
        DRAFT_DISCARD = 177,
        MSG_REACT = 178,
        MSG_CANCEL_REACTION = 179,
        MSG_GET_REACTIONS = 180,
        MSG_GET_DETAILED_REACTIONS = 181,
        STICKER_CREATE = 193,
        STICKER_SUGGEST = 194,
        VIDEO_CHAT_MEMBERS = 195,
        CHAT_SEARCH_COUNT_MSG = 197,
        CHAT_SEARCH_COMMON_PARTICIPANTS = 198,
        GET_USER_SCORE = 201,
        AUTH_CALL_INFO = 256,
        ASSETS_REMOVE = 259,
        ASSETS_MOVE = 260,
        ASSETS_LIST_MODIFY = 261,
        AUTH_CHECK_SCENARIO = 263,
        UNKNOW_OPT_CODE = 999
    };
    Q_ENUM(OpCode)

    explicit RawApiMessage(QObject *parent = nullptr);
    RawApiMessage(QByteArray data, QObject *parent = nullptr);
    RawApiMessage(const RawApiMessage& other, QObject *parent = nullptr);
    RawApiMessage& operator=(const RawApiMessage&other);

    Type type() const;
    void setType(Type newType);

    RawApiMessage::OpCode opcode() const;
    void setOpcode(OpCode newOptcode);

    int seq() const;
    void setSeq(int newSeq);

    int ver() { return 10;}

    const QVariantMap &payload() const;
    void setPayload(const QVariantMap &newPayload);

    QByteArray toByteArray();

signals:
    void rawMessageChanged();

private:
    Type m_type;
    OpCode m_opcode;
    int m_seq;
    int m_ver;
    QVariantMap m_payload;
};
QDebug operator<<(QDebug debug, const RawApiMessage &message);
#endif // RAWAPIMESSAGE_H
