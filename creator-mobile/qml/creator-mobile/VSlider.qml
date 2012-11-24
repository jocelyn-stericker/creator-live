import QtQuick 2.0

Rectangle {
    property alias spos: volumeSlider.y
    opacity: 1;
    radius: 2;
    color: "orange"
    Rectangle {
        anchors.centerIn: parent;
        z: 50;
        width: parent.width;
        height: parent.height;
        radius: 2;
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#242627" }
            GradientStop { position: 1.0; color: "#3c3c3c" }
        }
    }

    z: 9001;

    MouseArea {
        anchors.fill: parent
        onMouseYChanged: {
            volumeSlider.y = mouseY-volumeSlider.height/2;
            if (volumeSlider.y < 5)
                volumeSlider.y = 5;
            if (volumeSlider.y > parent.height- volumeSlider.height - 5)
                volumeSlider.y = parent.height - volumeSlider.height- 5;
        }
    }

    Rectangle {
        id: volumeSlider;
        x: 4;
        y: 150;
        z: 100;
        width: parent.width - 4*2;
        height: 30;
        color: "white"

        radius: 4;
        border.color: "black"
        border.width: 1
    }
}
