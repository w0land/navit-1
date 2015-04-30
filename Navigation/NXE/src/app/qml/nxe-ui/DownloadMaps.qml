import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Page {
    id: page1

    property var maps
    property string currentDownloadMap
    property int currentDownloadIndex: 0

    Component.onCompleted: {
        currentDownloadMap = maps[currentDownloadIndex];
        navitProxy.downloadMap(currentDownloadMap);
    }

    function downloadNextMap() {
        currentDownloadIndex++;

        if (currentDownloadIndex === maps.length) {
            // push FTUMapDownloadCompleted.qml
            navitProxy.ftu = false;
            rootStack.clear();
            rootStack.push({item: Qt.resolvedUrl("MainPage.qml")})
        } else {
            currentDownloadMap = maps[currentDownloadIndex];
        }
    }

    Connections {
        target: navitProxy
        onMapDownloadError: {
            console.error("An error during map download");
        }

        onMapDownloadProgress: {
            var progress = now/total;
            console.debug("Progress for " + map + " value=" + progress);
            var nowInt = parseInt(now,10);
            var totalInt = parseInt(total,10);
            if (map === currentDownloadMap) {
                progressBarItem.value = progress;
                bytesTextItem.text = Math.floor(now/(1024*1024)) + "MB/" + Math.floor(total/(1024*1024)) + "MB";
            }
        }

        onMapDownloadFinished: {
            downloadNextMap();
        }
    }

    Rectangle {
        id: rect
        width: parent.width
        height: 300
        color: "#09bcdf"
        anchors.verticalCenter: parent.verticalCenter
    }

    Image {
        id: icon
        anchors.bottomMargin: -31
        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.height: 64
        sourceSize.width: 64
        source: "download_hex_icon.png"
        anchors.bottom: rect.top
    }

    Column {
        spacing: 10
        anchors {
            top: icon.bottom
            topMargin: 10
            bottom: rect.bottom
            left: rect.left
            leftMargin: 10
            right: rect.right
            rightMargin: 10
        }
        Text {
            id: headerText
            color: "white"
            text: "Downloading (" + (currentDownloadIndex + 1) +" of " + maps.length + ")"
            font.pointSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ProgressBar {
            id: progressBarItem
            width: parent.width
            height: 15
            value: 0
            style: ProgressBarStyle {
                background: Rectangle {
                    radius: 2
                    color: "#467C8A"
                    border.color: "#467C8A"
                    border.width: 1
                    implicitWidth: 200
                    implicitHeight: 8
                }
                progress: Rectangle {
                    color: "#BABABA"
                    border.color: "#BABABA"
                }
            }
        }
        Item {
            width: parent.width
            height: 20
            Text {
                text: currentDownloadMap
                font.pointSize: 16
                color: "white"
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                id: bytesTextItem
                text: "N/A"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
        }

        Item {
            width: parent.width
            height: 20
            Text {
                text: "Time remaining"
                font.pointSize: 16
                color: "white"
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "1.3 min"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
        }
    }

    Item {
        id: item1
        width: 100
        height: 62
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: rect.bottom
    }
}
