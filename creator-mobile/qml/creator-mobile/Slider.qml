import QtQuick 2.0

Rectangle {
    property alias spos: volumeSlider.xt
    opacity: 1;
    radius: 2;
    color: "transparent"
    Rectangle {
        rotation: 90;
        anchors.centerIn: parent;
        z: 50;
        width: parent.height;
        height: parent.width;
        radius: 2;
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#262627" }
            GradientStop { position: 1.0; color: "#3c3c3c" }
        }
    }

    z: 9001;

    MouseArea {
        anchors.fill: parent
        onMouseXChanged: {
            volumeSlider.xt = mouseX;

            if (Math.abs(volumeSlider.xt - parent.width/2) < 30) {
                volumeSlider.xt = parent.width/2;
            }

            if (volumeSlider.xt < 5 + volumeSlider.width/2)
                volumeSlider.xt = 5 + volumeSlider.width/2;
            if (volumeSlider.xt > parent.width - volumeSlider.width/2 - 5)
                volumeSlider.xt = parent.width - volumeSlider.width/2 - 5;
        }
    }

    Rectangle {
        id: volumeSlider;
        property int xt : 300;
        x: xt - width/2;
        y: 4;
        z: 100;
        height: parent.height - 4*2;
        width: 30;
        color: "white"

        radius: 4;
        border.color: "black"
        border.width: 1
    }

}
