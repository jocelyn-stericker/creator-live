import QtQuick 2.0

Rectangle {
    id: iaWindow;
    width: 1020
    height: 200
    color: "white"
    opacity: 0.0
    states: [
        State {
            name: "on"; when: visible
            PropertyChanges {
                target: iaWindow;
                opacity: 1.0;
            }
        }
    ]

    transitions: [
        Transition {
            from: ""; to: "on"; reversible: false
            ParallelAnimation {
                NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.InOutQuad }
            }
        }
    ]


    Text {
        x: 10
        y: 5
        color: "black"
        text: "Insert"
        font.pointSize: 20;
        font.family: "HandelGotDLig";
    }

    ListModel {
        id: appModel;
        ListElement { intid: "SEQUENCER"; xtext: "Sequencer"; xpng: "icons/app_seq.png"; }
        ListElement { intid: "LOOPER"; xtext: "Looper"; xpng: "icons/app_loop.png"; }
        ListElement { intid: "SAMPLER"; xtext: "Sampler"; xpng: "icons/app_sampler.png"; }
        ListElement { intid:" Mixer"; xtext: "Mixer"; xpng: "icons/app_mixer.png"}
        ListElement { intid: "Pitch Shift"; xtext: "Pitch Shift"; xpng: "icons/app_pitch.png"; }
        ListElement { intid: "Reverb"; xtext: "Reverb" }
        ListElement { intid: "Flanger"; xtext: "Flanger" }
        ListElement { intid: "Phaser"; xtext: "Phaser" }
        ListElement { intid: "Filter"; xtext: "Filter"}
        ListElement { intid: "BPM Detection"; xtext: "BPM"}
        ListElement { intid: "VST"; xtext: "VST Effect"}
    }

    Rectangle {
        x: parent.width - width - 10;
        y: 7;
        height: 35;
        width: 100
        z: 900
        Text {
            id: cancelText
            width: parent.width - 20;
            x: 10;
            y: 5;
            text: "Cancel"
            color: "white"
            font.family: "HandelGotDLig";
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 16

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    selector.cancel();
                }
            }
        }
        color: "darkred"
    }

    GridView {
        id: appView
        x: 10;
        y: 40;
        width: parent.width - 10
        height: parent.height - 40;
        model: appModel;

        header: Rectangle {
            height: 20;
        }

        cellWidth: width/5;
        cellHeight: width/5
        clip: true;

        delegate: Rectangle {
            width: parent.width/5 - 10
            height: parent.width/5 - 10;
            color: "#262627"
            Image {
                source: xpng
                x: 0;
                width: parent.width;
                y: 0;
                height: parent.height
            }

            Text {
                color: "white"
                y: parent.height/2 - height/2
                x: 10;
                width: parent.width - 20;
                horizontalAlignment: Text.AlignHCenter
                text: xtext;
                font.family: "HandelGotDLig";
                font.pointSize: 16
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    selector.go(intid);
                }
            }
        }

        Rectangle {
            width: parent.width;
            height: 20;
            x: 0;
            y: parent.height - 20;
            gradient: Gradient{
                GradientStop {position:0.0; color:"transparent"}
                GradientStop {position:1.0; color: "white"}
            }
        }

        Rectangle {
            width: parent.width;
            height: 20;
            x: 0;
            y: 0;
            gradient: Gradient{
                GradientStop {position:0.0; color: "white"}
                GradientStop {position:1.0; color:"transparent"}
            }
        }
    }
}
