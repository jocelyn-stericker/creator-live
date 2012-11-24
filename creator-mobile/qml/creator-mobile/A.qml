// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0

Rectangle{
    x: whiteWidth*5
    width: whiteWidth
    height: whiteHeight
    color: "white"
    border.width: 3
    border.color: "black"

    Transition {
    NumberAnimation { properties: "color"; easing.type: Easing.InOutQuad; duration: 3000 }
    }
    MouseArea {
        anchors.fill:parent
        onPressed: {
            parent.color="lightgrey";
        }
        onReleased: {
            parent.color="white";
        }
        onCanceled: {
            parent.color="white"
        }
    }
}
