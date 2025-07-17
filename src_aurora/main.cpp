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

#include <auroraapp.h>
#include <QtQuick>

#include "plugin/serverconnection.h"
#include "plugin/usersession.h"
#include "models/chatmessagesmodel.h"
#include "models/chatslistmodel.h"
#include "api/chatmessage.h"
#include "api/chatmessagereactions.h"
#include "api/chat.h"
#include "emojimodel.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(Aurora::Application::application(argc, argv));
    application->setOrganizationName(QStringLiteral("ru.neochapay"));
    application->setApplicationName(QStringLiteral("maximus"));

    qmlRegisterType<ServerConnection>("ru.neochapay.maximus", 1, 0, "ServerConnection");
    qmlRegisterType<UserSession>("ru.neochapay.maximus", 1, 0, "UserSession");
    qmlRegisterType<ChatsListModel>("ru.neochapay.maximus", 1, 0, "ChatsListModel");
    qmlRegisterType<ChatMessagesModel>("ru.neochapay.maximus", 1, 0, "ChatMessagesModel");
    qmlRegisterType<ChatMessage>("ru.neochapay.maximus", 1, 0, "ChatMessage");
    qmlRegisterType<ChatMessageReactions>("ru.neochapay.maximus", 1, 0, "ChatMessageReactions");
    qmlRegisterType<Chat>("ru.neochapay.maximus", 1, 0, "Chat");
    qmlRegisterType<EmojiModel>("EmojiModel", 1, 0, "EmojiModel");

    QScopedPointer<QQuickView> view(Aurora::Application::createView());
    view->rootContext()->setContextProperty("version", APP_VERSION);
    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/Maximus.qml")));
    view->show();

    return application->exec();
}
