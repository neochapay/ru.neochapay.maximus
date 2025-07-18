

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
import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    objectName: "entherCodePage"
    allowedOrientations: Orientation.All

    PageHeader {
        id: header
        objectName: "pageHeader"
        title: qsTr("Enther code")
    }

    SilicaFlickable {
        anchors.fill: parent

        TextField {
            id: codeNumberField
            placeholderText: qsTr("XXXXXX")
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            anchors.centerIn: parent
        }

        Button {
            id: sendCodeButton
            text: qsTr("Send")
            width: codeNumberField.width
            enabled: codeNumberField.text.length == 6

            anchors.top: codeNumberField.bottom

            onClicked: {
                serverConnection.sendCode(codeNumberField.text)
            }
        }
    }
}
