import QtQuick 2.0

Rectangle {
    property alias text: t.text;
    color: "#262627";
    radius: 2;
    Text {
        id: t;
        height: parent.height
        width: parent.width
        font.family: "Myriad Pro"
        font.pixelSize: 160
        verticalAlignment: Qt.AlignVCenter
        color: "white"
        horizontalAlignment: Text.AlignHCenter
    }
}
