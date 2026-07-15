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
#include <QDataStream>
#include <QDebugStateSaver>
#include <QMetaEnum>

RawApiMessage::RawApiMessage(QObject *parent)
    : QObject(parent)
{
}

RawApiMessage::RawApiMessage(QByteArray data, QObject *parent)
    : QObject(parent)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::BigEndian);

    quint8 ver;
    quint8 cmd;
    quint16 seq;
    quint16 opcode;
    quint32 payloadLen;

    in >> ver;
    in >> cmd;
    in >> seq;
    in >> opcode;
    in >> payloadLen;

    m_ver = ver;
    m_seq = seq;

    QByteArray payload = data.mid(10);
    QMetaEnum opMeta = QMetaEnum::fromType<OpCode>();

    if (opMeta.valueToKey(opcode) != nullptr) {
        m_opcode = static_cast<OpCode>(opcode);
    } else {
        qWarning() << "WRONG OPCODE" << opcode;
        m_opcode = UNKNOW_OPT_CODE;
    }

    m_type = (cmd == 0)
                 ? RawApiMessage::out
                 : RawApiMessage::in;

    m_payload = MsgPack::unpack(payload).toMap();
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

QDebug operator<<(QDebug debug, const RawApiMessage &message)
{
    QDebugStateSaver saver(debug);

    debug.nospace()
        << "RawApiMessage("
        << "type=" << static_cast<int>(message.type())
        << ", opcode=" << message.opcode()
        << ", seq=" << message.seq()
        << ", payload=" << message.payload()
        << ")";

    return debug;
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

const QVariantMap &RawApiMessage::payload() const
{
    return m_payload;
}

void RawApiMessage::setPayload(const QVariantMap &newPayload)
{
    if (m_payload == newPayload)
        return;
    m_payload = newPayload;
    emit rawMessageChanged();
}

QByteArray RawApiMessage::toByteArray()
{
    QVariantMap message;
    message["ver"] = ver();
    if(m_type == RawApiMessage::Type::out) {
        message["cmd"] = 0;
    } else {
        message["cmd"] = 1;
    }
    message["seq"] = m_seq;
    message["opcode"] = m_opcode;
    message["payload"] = m_payload;

    return MsgPack::pack(m_payload);
}
