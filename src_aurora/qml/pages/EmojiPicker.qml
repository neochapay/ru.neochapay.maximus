import QtQuick 2.0
import QtQuick.Layouts 1.0
import EmojiModel 1.0

Item {
    id: container
    property var editor
    property EmojiModel model
    property var categories: ['Smileys & Emotion', 'People & Body', 'Animals & Nature',
        'Food & Drink', 'Activities', 'Travel & Places', 'Objects', 'Symbols', 'Flags']
    property var searchModel: ListModel {}
    property bool searchMode: false
    property int skinColor: -1
    function changeSkinColor(index) {
        if (index !== skinColors.current) {
            skinColors.itemAt(skinColors.current + 1).scale = 0.6
            skinColors.itemAt(index + 1).scale = 1
            skinColors.current = index
            container.skinColor = index
        }
    }
    function refreshSearchModel() {
        searchModel.clear()
        var searchResult = model.search(searchField.text, skinColor)
        for (var i = 0; i < searchResult.length; ++i) {
            searchModel.append({path: searchResult[i]})
        }
    }
    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: container.categories
            spacing: 30
            clip: true
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            interactive: true


            delegate: GridLayout {
                id: grid
                property string category: container.searchMode ? 'Search Result' : modelData
                property int columnCount: list.width / 70
                property int sc: grid.category === 'People & Body' ? container.skinColor : -1
                columns: columnCount
                columnSpacing: 8

                Text {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    text: grid.category
                    color: Qt.rgba(0, 0, 0, 0.5)
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignLeft
                    Layout.columnSpan: grid.columnCount != 0 ? grid.columnCount : 1
                }
                Repeater {
                    model: container.searchMode ? container.searchModel : container.model.count(grid.category)
                    delegate: Rectangle  {
                        property alias es: emojiSvg
                        Layout.preferredWidth: 60
                        Layout.preferredHeight: 60
                        radius: 60
                        color: mouseArea.containsMouse ? '#e6e6e6' : '#ffffff'
                        Image {
                            id: emojiSvg
                            source: container.searchMode ? path : container.model.path(grid.category, index, grid.sc)
                            sourceSize: Qt.size(50, 50)
                            anchors.centerIn: parent
                            asynchronous: true
                        }
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                var tag = "<img src = '%1' width = '40' height = '40' align = 'top'>" + '<img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII=" width="5" height="1"/>'
                                container.editor.insert(container.editor.cursorPosition, tag.arg(emojiSvg.source))
                            }
                        }
                    }
                }
            }
            /*onContentYChanged: {
                var index = list.indexAt(0, contentY + 15)
                if (index !== -1 && index !== cateIcons.current) {
                    cateIcons.itemAt(index).source = 'icons/' + cateIcons.blueSvg[index]
                    cateIcons.itemAt(cateIcons.current).source = 'icons/' + cateIcons.blackSvg[cateIcons.current]
                    cateIcons.current = index
                }
            }*/
        }
    }
}
