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
import QtGraphicalEffects 1.0

import ru.neochapay.maximus 1.0
import EmojiModel 1.0
import "../jsFunctions/emoji.js" as EmojiFunc


Page {
    id: chatListPage
    objectName: "chatsListPage"
    allowedOrientations: Orientation.All
    property alias chatTitle: header.title

    PageHeader {
        id: header
        objectName: "pageHeader"
        title: qsTr("Chat")
    }

    property var currentChat
    property bool showPopup: false


    // Background dimmer
    Rectangle {
        visible: showPopup
        anchors.fill: parent
        z: 10
        color: "#80000000"

        MouseArea {
            anchors.fill: parent
            onClicked:{
                showPopup = false
            }
        }
    }

    Rectangle {
        visible: showPopup
        width: parent.width
        height: parent.height / 3
        z: 11
        radius: chatMessageImage.width / 2
        anchors{
            bottom: sendMessageRow.top
            left: parent.left
            leftMargin: Theme.paddingSmall
            rightMargin: Theme.paddingSmall
        }

        color: "white"
        border.color: "gray"

        EmojiPicker {
            id: emojiPicker
            model: emojiModel
            editor: messageTextField
            anchors.fill: parent
        }
    }

    BusyIndicator {
        id: spinner
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        enabled: true
    }

    ChatMessagesModel{
        id: chatMessagesModel
        chat: currentChat
        onChatLoaded: {
            spinner.enabled = false
            messagesListView.needToUpdate = true
        }
    }

    EmojiModel {
        id: emojiModel
        iconsPath: '../emojiSvgs/'
        iconsType: '.svg'
    }

    SilicaListView {
        id: messagesListView
        width: parent.width
        height: parent.height - header.height - sendMessageRow.height
        anchors.top:  header.bottom
        model: chatMessagesModel
        clip: true

        MouseArea {
            anchors.fill: parent
            z: -1  // Put behind delegates
            onClicked: messagesListView.forceActiveFocus()
        }
        property bool needToUpdate: true

        onCountChanged: {
            messagesListView.currentIndex = count - 1
        }

        delegate: Loader{
            z: 1
            Component.onCompleted: {
                if(messageType == ChatMessage.TextMessage || messageType == ChatMessage.PhotoMessage) {
                    source = "../components/TextMessageItem.qml";
                } else if (messageType == ChatMessage.ControlMessage) {
                    source = "../components/ControlMessageItem.qml";
                } else {
                    source = "../components/UnsupportedMessage.qml"
                }
            }
        }
    }

    Item{
        id: sendMessageRow
        width: parent.width
        height: visible ? Theme.iconSizeLarge : 0
        anchors.bottom: chatListPage.bottom
        visible: currentChat.type !== Chat.CHANNEL;

        TextEdit{
            id: messageTextField
            clip: true
            width: parent.width - sendButton.width - emojiButton.width - Theme.paddingSmall*4
            height: parent.height - Theme.paddingSmall*2
            verticalAlignment:  TextEdit.AlignVCenter
            wrapMode: TextEdit.NoWrap
            textFormat: TextEdit.RichText
            color: Theme.secondaryColor
            anchors{
                top: parent.top
                topMargin: Theme.paddingSmall
                left: emojiButton.right
                leftMargin: Theme.paddingSmall
            }

            Keys.onReturnPressed: event.accepted = true
            Keys.onEnterPressed: event.accepted = true

            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: Theme.paddingSmall
                }
                height: 2  // Underline thickness
                color: messageTextField.activeFocus ? Theme.highlightColor : Theme.primaryColor  // Blue when focused, gray otherwise

                Behavior on color {
                    ColorAnimation { duration: 200 }  // Smooth color transition
                }
            }
        }


        IconButton {
            id: emojiButton
            width: sendButton.width
            height: sendButton.height
            icon.fillMode: Image.PreserveAspectFit
            icon.source: "../icons/emoji-smiley.svg?" + (pressed
                                                         ? Theme.highlightColor
                                                         : Theme.primaryColor)
            icon.width: width * 0.4
            icon.height: height * 0.4
            anchors{
                top: parent.top
                topMargin: Theme.paddingSmall
                left: parent.left
                leftMargin: Theme.paddingSmall
            }
            onClicked: {
                showPopup = true
                //console.log("emoji!!")
            }
        }


        IconButton {
            id: sendButton
            width: parent.height - Theme.paddingSmall * 2
            height: parent.height - Theme.paddingSmall * 2
            icon.source: "image://theme/icon-m-send?" + (pressed
                                                         ? Theme.highlightColor
                                                         : Theme.primaryColor)
            anchors{
                top: parent.top
                topMargin: Theme.paddingSmall
                left: messageTextField.right
                leftMargin: Theme.paddingSmall
            }
            onClicked: {
                //console.log("text a ", extractTextWithEmojis(messageTextField.text));
                serverConnection.sendMessage(currentChat, EmojiFunc.extractTextWithEmojis(messageTextField.text))
                messageTextField.text = ""
            }
        }
    }

}
