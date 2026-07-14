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

#include "rawapimessage.h"

#include <QJsonDocument>
#include <QMetaEnum>

RawApiMessage::RawApiMessage(QObject *parent)
    : QObject(parent)
{
}

RawApiMessage::RawApiMessage(QString jsonString, QObject *parent)
    : QObject(parent)
{
    QJsonObject mess = QJsonDocument::fromJson(jsonString.toUtf8()).object();
    m_ver = mess.value("ver").toInt();
    QMetaEnum opMeta = QMetaEnum::fromType<OpCode>();
    int opcode = mess.value("opcode").toInt();
    if (opMeta.valueToKey(opcode) != nullptr) {
        m_opcode = static_cast<OpCode>(opcode);
    } else {
        m_opcode = UNKNOW_OPT_CODE;
    }

    m_seq = mess.value("seq").toInt();
    if(mess.value("cmd").toInt() == 0) {
        m_type = RawApiMessage::Type::out;
    } else {
        m_type = RawApiMessage::Type::in;
    }
    m_payload = mess.value("payload").toObject();
}

RawApiMessage::RawApiMessage(const RawApiMessage &other, QObject *parent)
    : QObject(parent)
{
    m_ver = other.m_ver;
    m_opcode = other.m_opcode;
    m_seq = other.m_seq;
    m_type = other.m_type;
    m_payload = other.m_payload;
}

RawApiMessage &RawApiMessage::operator=(const RawApiMessage & other)
{
    m_ver = other.m_ver;
    m_opcode = other.m_opcode;
    m_seq = other.m_seq;
    m_type = other.m_type;
    m_payload = other.m_payload;
    return *this;
}

RawApiMessage::Type RawApiMessage::type() const
{
    return m_type;
}

void RawApiMessage::setType(Type newType)
{
    if (m_type == newType)
        return;
    m_type = newType;
    emit rawMessageChanged();
}

RawApiMessage::OpCode RawApiMessage::opcode() const
{
    return m_opcode;
}

void RawApiMessage::setOpcode(OpCode newOptcode)
{
    if (m_opcode == newOptcode)
        return;
    m_opcode = newOptcode;
    emit rawMessageChanged();
}

int RawApiMessage::seq() const
{
    return m_seq;
}

void RawApiMessage::setSeq(int newSeq)
{
    if (m_seq == newSeq)
        return;
    m_seq = newSeq;
    emit rawMessageChanged();
}

const QJsonObject &RawApiMessage::payload() const
{
    return m_payload;
}

void RawApiMessage::setPayload(const QJsonObject &newPayload)
{
    if (m_payload == newPayload)
        return;
    m_payload = newPayload;
    emit rawMessageChanged();
}

QString RawApiMessage::toJsonString()
{
    QJsonObject message;
    message["ver"] = ver();
    if(m_type == RawApiMessage::Type::out) {
        message["cmd"] = 0;
    } else {
        message["cmd"] = 1;
    }
    message["seq"] = m_seq;
    message["opcode"] = m_opcode;
    message["payload"] = m_payload;

    QJsonDocument doc(message);
    return doc.toJson(QJsonDocument::Compact);
}

void RawApiMessage::formJsonString(QString jsonString)
{
    Q_UNUSED(jsonString)
}
