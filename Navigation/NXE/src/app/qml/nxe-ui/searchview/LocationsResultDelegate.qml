import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import '../common'

Item {
    id: delegateroot
    width: LocationResultListView.width
    height: 50

    signal locationClicked(var itemId)

    MouseArea {
        anchors.fill: parent

        onClicked: {
            locationClicked(itemId);
        }
    }
    Row {
        id: row1
        anchors.fill: parent
        Item {
            id: item1
            width: parent.width
            height: parent.height
            NText {
                id: mainTextItem
                width: parent.width * 2 / 3
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: itemText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: starImage.right
                anchors.leftMargin: 5
            }

            Image {
                id: starImage
                width: favorite === true ? 20 : 0
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                source: "../images/star_icon_solid_white.png"
            }

            NText {
                id: descriptionText
                text: description !== undefined ? description : ""
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 5
            }
        }
    }
    Rectangle {
        width: parent.width
        height: 1
        color: "#45a6c3"
        anchors.bottom: parent.bottom
    }

}
