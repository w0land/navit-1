import QtQuick 2.0

Item {
    id: locationInfoTop
    x: 0
    y: 0
    width: 400
    //  width: 400
    height: 100
    visible: true
    signal backToMapRequest()
    property bool isFavorite : false
    property bool canNavigate : false
    property string locationName: locationComponent ? locationComponent.itemText : ""
    property string locationDescriptionString: locationComponent ? locationComponent.description : ""
    property var locationComponent: null

    property bool extraInfoVisible: false

    signal requestHideBars()

    Behavior on opacity {NumberAnimation {}}

    Item {
        id: mainItem
        anchors.fill: parent
        Item {
            id: buttonsBar
            width: 60
            height: parent.height

            Rectangle {
                id: rectangle2
                color: "#242424"
                anchors.fill: parent

                MouseArea {
                    anchors.fill: parent
                    onClicked: {}
                }

                Image {
                    id: image1
                    x: 18
                    y: 30
                    width: 25
                    height: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "back_icon_white_lg.png"
                }
            }
        }

        Item {
            id: item1
            anchors.left: buttonsBar.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.leftMargin: 0
            visible: extraInfoVisible
            Rectangle {
                id: rectangle1
                color: "#fbfbfb"
                anchors.fill: parent

                 Image {
                    id: image3
                    x: 58
                    y: 21
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    source: "location_marker_icon.png"
                }

                Text {
                    id: locationTitle
                    x: 43
                    y: 21
                    text: locationName
                    font.pixelSize: 26
                }

                Text {
                    id: locationDescription
                    x: 43
                    text: locationDescriptionString
                    anchors.top: locationTitle.bottom
                    anchors.topMargin: 5
                    font.pixelSize: 15
                }

                Text {
                    id: locationTime
                    x: 302
                    y: 36
                    color: "#09bcdf"
                    text: {
                        var str;
                        if (navigationProxy.eta === -1) {
                            str = qsTr("Unknown eta");
                        } else {
                            if (navigationProxy.eta < 60) {
                                str = navigationProxy.eta + " sec";
                            } else {
                                var minutes = Math.floor(navigationProxy.eta / 60);
                                str = minutes + " min";
                            }
                        }
                        return str;
                    }
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: 12
                }

                Text {
                    id: locationDistance
                    x: 284
                    y: 58
                    color: "#09bcdf"
                    text: {
                        if (navigationProxy.distanceToDestination === -1) {
                            return qsTr("Unknown distance")
                        } else {
                            var destStr;
                            if (navigationProxy.distanceToDestination > 1000 ) {
                                var dist = Number(navigationProxy.distanceToDestination/1000).toFixed(1);
                                destStr = dist + " km";
                            } else {
                                destStr = navigationProxy.distanceToDestination + " m";
                            }
                            return destStr;
                        }
                    }
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: 12
                }

            }
        }
    }

    Item {
        anchors.top: mainItem.bottom
        anchors.right: parent.right
        width: 100
        height: 40
        visible: extraInfoVisible
        MouseArea {
            anchors.fill: parent
            onClicked: {
                navigationProxy.stopNavigation()
            }
        }
        Image {
            width: 76
            height: 20
            anchors.centerIn: parent
            sourceSize.width: 60
            sourceSize.height: 50
            source: "blue_forward_button_long_bg.png"
        }
        Text {
            anchors.centerIn: parent
            text: "Cancel"
            color: "white"
            font.pixelSize: 16
        }
    }
}
