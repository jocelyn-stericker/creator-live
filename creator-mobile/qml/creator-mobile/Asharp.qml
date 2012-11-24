// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0

Rectangle {
    width: blackWidth
    height: blackHeight
    x: (whiteWidth-width)/2-(width/6)+(whiteWidth/2) + whiteWidth*5
    color: "black"
    border.width: 3
    border.color: "black"

    MouseArea {
        anchors.fill:parent
        onPressed: {
            parent.color="#1C1C1C";
        }
        onReleased: {
            parent.color="black";
        }

}
}
