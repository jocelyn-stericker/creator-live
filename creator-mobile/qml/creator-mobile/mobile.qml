// Creator Mini
import QtQuick 2.0
//import QtWebKit 2.0

Rectangle {
    id: rect
    width: 1024
    height: 600

    property bool settingsMode : false;
    property bool dark : tool.selected;
    property bool audioMute : true;
    property bool midiEffectsMute : true;
    property bool sequencerEnabled : false;
    property color stdcolor : "darkorange"

    focus: true;

    function onSwipeDown() {
        settingsMode = !settingsMode;
    }

    Rectangle {
        id: settings
        width: rect.width
        height: 0
        color: "white"
        clip: true
        z: 6000;
        opacity: 0;
        focus: true;

        states: [
            State {
                name: "open"; when: settingsMode
                PropertyChanges {
                    target: settings;
                    height: rect.height
                    opacity: 0.95;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "open"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "height"; duration: 500; easing.type: Easing.InOutQuad }
                    NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.InOutQuad }
                }
            }
        ]

        Image{
            id: xSettings
            source: "x.png"
            x: 30-15
            y: 5
            scale: 0.6
            visible: settingsMode
            smooth: true

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    rect.onSwipeDown()
                }
            }
        }

        Text {
            id: settingsLabel;
            x: 10
            y: 50
            width: rect.width < 700 ? 150 : 250;
            color: "grey"
            font.pointSize: rect.width < 700 ? 20 : 28
            horizontalAlignment: Text.AlignHCenter

            font.family: "Myriad Pro"
            text: "Settings"
        }

        Rectangle {
            id: settingsLine
            width: 2;
            x: settingsLabel.x + settingsLabel.width + 10;
            y: settingsLabel.y;
            color: "grey"
        }

        Flickable {
            flickableDirection: Flickable.VerticalFlick
            maximumFlickVelocity: 10
            Text {
                id: fontLabel;
                x: settingsLine.x + settingsLine.width + 10;
                y: 100
                width: rect.width < 700 ? 150 : 300;
                color: "grey"
                font.pixelSize: rect.width < 700 ? 28 : 38
                font.family: "Myriad Pro"
                horizontalAlignment: Text.AlignRight

                text: "Font Size"
            }
        }

        Text {
            text: "Creator Mini is (C) Joshua Netterfield, Daniel Reynolds & Clarisse Schneider";
            wrapMode: Text.WordWrap;
            font.family: "Myriad Pro"
            color: "darkgrey"
            y: rect.height - height - 10
            x: 15;
            width: rect.width - 30;
            font.pixelSize: 16;
        }

    }

    function init() {
        toolView.init();
        launching = false;
    }

    states: [
        State {
            name: "dark"; when: dark
            PropertyChanges {
                target: rect;
                color: "black";
            }
        }
    ]

    transitions: [
        Transition {
            from: ""; to: "dark"; reversible: true
            ParallelAnimation {
                ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
            }
        }
    ]

    Rectangle {
        id: toolbar;
        width: rect.width;
        x: 0;
        y: 0;
        height: 75;
        color: "black";
        enabled: !settingsMode

        Text {
            property bool selected
            x: 30;
            y: parent.height/2 - height/2 + 7;
            id: tool
            color: "white"
            text: toolView.currentItem.text;
            font.family: "Myriad Pro"
            font.bold: false;
            font.pixelSize: 36;
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    tool.selected = !tool.selected;
                }
            }

            states: [
                State {
                    name: "selected"; when: tool.selected
                    PropertyChanges {
                        target: tool;
                        color: stdcolor
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "selected"; reversible: true
                    ParallelAnimation {
                        ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }
        Image {
            id: downArrow
            x: tool.width + tool.x;
            scale: 0.75
            smooth: true;
            source: "down.png"
            opacity: 1;
            enabled: (visible && opacity > 0.0 && !settingsMode);
            y : parent.height/2 - height/2 + 7;

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    tool.selected = true;
                }
            }

            states: [
                State {
                    name: "hidden"; when: tool.selected || chapter.selected || translation.selected
                    PropertyChanges {
                        target: downArrow;
                        opacity: 0;
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "hidden"; reversible: true
                    ParallelAnimation {
                        NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }

        // STATES :/

        Text {
            property bool selected : true
            x: mode2.x - 30;
            y: parent.height/2 - height/2 + 2;
            id: mode1
            color: "white"
            text: "1";
            font.family: "Myriad Pro"
            font.bold: false;
            font.pixelSize: 30;
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mode1.selected = true;
                    mode2.selected = false;
                    mode3.selected = false;
                    mode4.selected = false;
                }
            }

            states: [
                State {
                    name: "selected"; when: mode1.selected
                    PropertyChanges {
                        target: mode1;
                        color: stdcolor
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "selected"; reversible: true
                    ParallelAnimation {
                        ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }

        Text {
            property bool selected : false
            x: mode3.x - 30;
            y: parent.height/2 - height/2 + 7;
            id: mode2
            color: "white"
            text: "2";
            font.family: "Myriad Pro"
            font.bold: false;
            font.pixelSize: 36;
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mode1.selected = false;
                    mode2.selected = true;
                    mode3.selected = false;
                    mode4.selected = false;
                }
            }

            states: [
                State {
                    name: "selected"; when: mode2.selected
                    PropertyChanges {
                        target: mode2;
                        color: stdcolor
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "selected"; reversible: true
                    ParallelAnimation {
                        ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }

        Text {
            property bool selected : false
            x: mode4.x - 30;
            y: parent.height/2 - height/2 + 7;
            id: mode3
            color: "white"
            text: "3";
            font.family: "Myriad Pro"
            font.bold: false;
            font.pixelSize: 36;
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mode1.selected = false;
                    mode2.selected = false;
                    mode3.selected = true;
                    mode4.selected = false;
                }
            }

            states: [
                State {
                    name: "selected"; when: mode3.selected
                    PropertyChanges {
                        target: mode3;
                        color: stdcolor
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "selected"; reversible: true
                    ParallelAnimation {
                        ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }

        Text {
            property bool selected : false
            x: parent.width - width - 30;
            y: parent.height/2 - height/2 + 7;
            id: mode4
            color: "white"
            text: "4";
            font.family: "Myriad Pro"
            font.bold: false;
            font.pixelSize: 36;
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mode1.selected = false;
                    mode2.selected = false;
                    mode3.selected = false;
                    mode4.selected = true;
                }
            }

            states: [
                State {
                    name: "selected"; when: mode4.selected
                    PropertyChanges {
                        target: mode4;
                        color: stdcolor
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "selected"; reversible: true
                    ParallelAnimation {
                        ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }
    }

    // TOOL LIST!!

    ListModel {
        id: toolModel;
        ListElement { name: "Keyboard"; }
        ListElement { name: "MIDI Pad"; }
        ListElement { name: "Effects"; }
        ListElement { name: "Microphone"; }
        ListElement { name: "Sequencer"; }
        ListElement { name: "Looper"; }
        ListElement { name: "Help"; }
    }

    Component {
        id: highlight_tool
        Rectangle {
            color: "#222222"; radius: 2;
            width: parent.width - 70;
            y: toolView.currentItem.y - 2;
            height: 38
            Behavior on y {
                SpringAnimation {
                    spring: 10
                    damping: 1.2
                }
            }
        }
    }

    ListView {
        id: toolView;
        width: parent.width - 20;
        height: parent.height- toolbar.height - 20
        x: 50
        y: toolbar.y + toolbar.height + 10
        z: 75
        opacity: 0;
        currentIndex: 0;
        clip: true;

//        rebound: Transition {
//            NumberAnimation {
//                properties: "y"
//                duration: 400
//                easing.type: Easing.InOutQuad
//            }
//        }

        enabled: (visible && opacity > 0.0 && !settingsMode);

        onCurrentIndexChanged: {
            console.log("index changed:", currentIndex);
        }

        function init() {
            currentItem.activate();
            if (currentIndex==0&&octaveCount<1)
                makeKeys()
        }

        model: toolModel;

        delegate: Text {
            text: name
            font.bold: false;
            font.pixelSize: 36;
            font.family: "Myriad Pro"
            color: "white";
            height: (index == 2 || index == 3 || index ==  5) ? 60 : 40;
            enabled: dark;
            width: parent.width;

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    parent.activate()
                }
            }
            function activate() {
                toolView.currentIndex = index;
                tool.selected = false;
            }
        }

        highlight: highlight_tool;
        highlightFollowsCurrentItem: false;
        focus: false;

        states: [
            State {
                name: "visible"; when: tool.selected
                PropertyChanges {
                    target: toolView;
                    opacity: 1;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "visible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 800; easing.type: Easing.InOutQuad }
                }
            }
        ]
    }

    // HELP

//    WebView {
//        id: helpView;
//        url: "http://nettek.ca"
//        smooth: true
//        opacity: 0.0;
//        enabled: toolView.currentIndex==6;
//        height: rect.height - y;
//        width: parent.width;
//        x: 0;
//        y: toolbar.y + toolbar.height;
//        z: 1;

//        states: [
//            State {
//                name: "visible"; when: toolView.currentIndex == 6 && !dark;
//                PropertyChanges {
//                    target: helpView;
//                    opacity: 1;
//                }
//            }
//        ]

//        transitions: [
//            Transition {
//                from: ""; to: "visible"; reversible: true
//                ParallelAnimation {
//                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
//                }
//            }
//        ]
//     }

    // KEYBOARD

    property int octaveCount: 0
    property int whiteWidth: 100
    property int blackWidth: 60
    property int whiteHeight: 450
    property int blackHeight: 270

    function makeKeys () {
        for (var i=0;i<1;i++)
            keyModel.append({})
        octaveCount++
    }

    function deleteKeys() {
        if (octaveCount <= 1)
            return;
        for (var i=0;i<1;i++)
            keyModel.remove(1);
        octaveCount--;
    }

    Rectangle {
        id: keyboard;
        y: toolbar.y + toolbar.height

        opacity: 0.0;
        enabled: toolView.currentIndex==0;
        height: rect.height -y;
        width: parent.width
        z: 1;
        color: "#262627";

        states: [
            State {
                name: "visible"; when: toolView.currentIndex == 0 && !dark;
                PropertyChanges {
                    target: keyboard;
                    opacity: 1;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "visible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                }
            }
        ]

        Image {
            id: plusbutton
            x: 380
            y: -40
            source: "Images/hereyougofinally.png"
            scale: 0.25
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    parent.opacity= 0.5
                    if (octaveCount<6)
                        makeKeys()
                }
                onReleased: {
                    parent.opacity=1.0
                }
            }
        }

        Image {
            id: minusbutton
            x: 470
            y: -40
            source: "Images/hereyougofinallyminusbutton.png"
            scale: 0.25
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    parent.opacity= 0.5
                    deleteKeys();
                }
                onReleased: {
                    parent.opacity=1.0
                }
            }
        }

        Text {
            x: 485
            y: 15
            font.family: "Myriad Pro"
            font.pointSize: 20
            color: "white"
            text: octaveCount
        }

        ListModel{
            id: keyModel
        }


        ListView {
            x: octaveCount==1?((parent.width-whiteWidth*7*octaveCount)/2):0
            y: 50
            width: parent.width
            height: whiteHeight
            model: keyModel
            delegate: Rectangle {
                width: whiteWidth*7

                C {}
                D {}
                E {}
                F {}
                G {}
                A {}
                B {}
                Csharp {}
                Dsharp{}
                Fsharp{}
                Gsharp{}
                Asharp{}
            }
            onXChanged: {
                console.log(x)
            }

            orientation: ListView.Horizontal
            layoutDirection: Qt.LeftToRight
        }


    }


    // PAD

    Rectangle {
        id: pad;
        y: toolbar.y + toolbar.height
        opacity: 0.0;
        enabled: toolView.currentIndex==1;
        height: rect.height -y;
        width: parent.width
        z: 1;
        color: "#262627";

        states: [
            State {
                name: "visible"; when: toolView.currentIndex == 1 && !dark;
                PropertyChanges {
                    target: pad;
                    opacity: 1;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "visible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                }
            }
        ]

        ListModel {
            id: padModel;
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
            ListElement { xcolor: "darkorange"; }
        }

        GridView {
            width: parent.width - 60;
            height: parent.height - 60;
            x: 32;
            y: 30;
            model: padModel;

            boundsBehavior: Flickable.StopAtBounds;

            cellWidth: width/5 - 1;
            cellHeight: height/3 - 2;

            delegate: Rectangle {
                color: xcolor;
                height: parent.height/3 -2 - 6;
                width: parent.width/5 -1 - 6;
                x: 3;
                y: 3;

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        parent.color = "white";
                    }
                    onReleased: {
                        parent.color = stdcolor;
                    }
                    onCanceled: {
                        parent.color = stdcolor;
                    }
                }
            }

            function updateColor() {
                for (var i = 0; i < count; ++i) {

                }
            }
        }
    }


    // EFFECTS

    Rectangle {
        id: midiEffects;
        y: toolbar.y + toolbar.height
        opacity: 0.0;
        enabled: toolView.currentIndex==2;
        height: rect.height -y;
        width: parent.width
        z: 1;
        color: "#262627";

        Image {
            id: midiEffectsToggle
            source: "plugin.png"
            x: parent.width/2-width/2 - midiEffectsPanel.width/2;
            y: parent.height/2-height/2;
            scale: 1.0
            smooth: true
        }

        Rectangle {
            id: midiEffectsMuteR;
            x: parent.width/2-width/2 - midiEffectsPanel.width/2;
            y: parent.height/2-height/2;
            height: Math.sqrt(record.width*record.width + record.height*record.height)
            width: 8;
            rotation: -45;
            color: "darkorange"
            radius: 5;

            states: [
                State {
                    name: "hidden"; when: !midiEffectsMute;
                    PropertyChanges {
                        target: midiEffectsMuteR;
                        opacity: 0;
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "hidden"; reversible: true
                    ParallelAnimation {
                        NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }

        Rectangle {
            id: midiEffectsPanel
            clip: true;
            width: 0;
            height: parent.height
            x: parent.width - width;
            y: 0;
            color: "darkorange"
            states: [
                State {
                    name: "hidden"; when: !midiEffectsMute;
                    PropertyChanges {
                        target: midiEffectsPanel;
                        width: 392;
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "hidden"; reversible: true
                    ParallelAnimation {
                        NumberAnimation { properties: "width"; duration: 580; easing.type: Easing.InOutQuad }
                    }
                }
            ]


            Text {
                id: meffectText;
                text: "Effects"
                x: 20
                y: 20;
                font.pixelSize: 20;
                font.family: "Myriad Pro"
                color: "white";
            }

            Plus {
                id: meffect1
                y: meffectText.y + meffectText.height + 10;
                x: 30;
                width: 392 - 50;
                height: (parent.height - y - 10)/2 - 5;
                text: "<center>+";
            }
            Plus {
                id: meffect2
                y: meffect1.y + meffect1.height + 10;
                x: 30;
                width: 392 - 50;
                height: (parent.height - y - 10);
                text: "<center>+"
            }
//            Plus {
//                id: micEffect2
//                y: effectText.y + effectText.height;
//                x: micEffect1.width + micEffect1.x + 10;
//                width: 592/2;
//                height: 200;
//                text: ""
//            }
//            Plus {
//                id: micEffect3
//                y: micEffect1.y + micEffect1.height + 10;
//                x: micEffect1.x
//                width: 592/2;
//                height: 200;
//                text: ""
//            }
//            Plus {
//                id: micEffect4
//                y: micEffect2.y + micEffect2.height + 10;
//                x: micEffect2.x
//                width: 592/2;
//                height: 200;
//                text: ""
//            }
        }

        Text {
            text: "<center><font size='6'><B>Tap above to enable MIDI effects.</B></font><br>These change the sound of the keyboard and MIDI pad.";
            y: recordMute.y + recordMute.height - 25;
            wrapMode: Text.WordWrap;
            x: - midiEffectsPanel.width/2;
            width: parent.width;
            color: "white";
            font.family: "Myriad Pro"
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea {
            anchors.fill: midiEffectsToggle
            onClicked: {
                midiEffectsMute = !midiEffectsMute;
            }
        }

        states: [
            State {
                name: "onVisible"; when: toolView.currentIndex == 2 && !dark && !midiEffectsMute
                PropertyChanges {
                    target: midiEffects;
                    color: "white";
                    opacity: 1;
                }
            },
            State {
                name: "offVisible"; when: toolView.currentIndex == 2 && !dark && midiEffectsMute;
                PropertyChanges {
                    target: midiEffects;
                    opacity: 1;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "offVisible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                }
            },
            Transition {
                from: ""; to: "onVisible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                    ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                }
            },
            Transition {
                from: "offVisible"; to: "onVisible"; reversible: true
                ParallelAnimation {
                    ColorAnimation { properties: "color"; duration: 300; easing.type: Easing.InOutQuad }
                }
            }
        ]
    }


    // MICROPHONE

    Rectangle {
        id: microphone;
        y: toolbar.y + toolbar.height
        opacity: 0.0;
        enabled: toolView.currentIndex==3;
        height: rect.height -y;
        width: parent.width
        z: 1;
        color: "#262627";

        Image {
            id: record
            source: "mic.png"
            x: parent.width/2-width/2 - micPanel.width/2;
            y: parent.height/2-height/2;
            scale: 1.0
            smooth: true
        }

        Rectangle {
            id: recordMute;
            x: parent.width/2-width/2 - micPanel.width/2;
            y: parent.height/2-height/2;
            height: Math.sqrt(record.width*record.width + record.height*record.height)
            width: 8;
            rotation: -45;
            color: "darkorange"
            radius: 5;

            states: [
                State {
                    name: "hidden"; when: !audioMute;
                    PropertyChanges {
                        target: recordMute;
                        opacity: 0;
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "hidden"; reversible: true
                    ParallelAnimation {
                        NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.InOutQuad }
                    }
                }
            ]
        }

        Rectangle {
            id: micPanel
            clip: true;
            width: 0;
            height: parent.height
            x: parent.width - width;
            y: 0;
            color: "darkorange"
            states: [
                State {
                    name: "hidden"; when: !audioMute;
                    PropertyChanges {
                        target: micPanel;
                        width: 392;
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "hidden"; reversible: true
                    ParallelAnimation {
                        NumberAnimation { properties: "width"; duration: 580; easing.type: Easing.InOutQuad }
                    }
                }
            ]


            Text {
                id: volumeText;
                text: "Volume"
                x: 20
                y: 20;
                font.pixelSize: 20;
                font.family: "Myriad Pro"
                color: "white";
            }
            Slider {
                id: volumeSliderBox;
                x: 30;
                y: volumeText.y + volumeText.height + 10;
                z: 9901;
                width: 392 - 50;
                height: 42;
                spos: 171;
            }

            Text {
                id: panText;
                text: "Pan"
                x: 20;
                y: volumeSliderBox.y + volumeSliderBox.height + 20
                font.pixelSize: 20;
                font.family: "Myriad Pro"
                color: "white";
            }
            Slider {
                id: panSliderBox;
                x: 30;
                y: panText.y + panText.height + 10;
                z: 9901;
                width: 392 - 50;
                height: 42;
                spos: 171;
            }

            Text {
                id: effectText;
                text: "Effects"
                x: 20
                y: panSliderBox.y + panSliderBox.height + 20;
                font.pixelSize: 20;
                font.family: "Myriad Pro"
                color: "white";
            }

            Plus {
                id: micEffect1
                y: effectText.y + effectText.height + 10;
                x: 30;
                width: 392 - 50;
                height: parent.height - y - 20;
                text: "<center>+"
            }
//            Plus {
//                id: micEffect2
//                y: effectText.y + effectText.height;
//                x: micEffect1.width + micEffect1.x + 10;
//                width: 592/2;
//                height: 200;
//                text: ""
//            }
//            Plus {
//                id: micEffect3
//                y: micEffect1.y + micEffect1.height + 10;
//                x: micEffect1.x
//                width: 592/2;
//                height: 200;
//                text: ""
//            }
//            Plus {
//                id: micEffect4
//                y: micEffect2.y + micEffect2.height + 10;
//                x: micEffect2.x
//                width: 592/2;
//                height: 200;
//                text: ""
//            }
        }

        Text {
            text: "<center><font size='6'><B>Tap above to enable the microphone.</B></font><br>Use headphones to avoid feedback.";
            y: recordMute.y + recordMute.height - 25;
            wrapMode: Text.WordWrap;
            x: - micPanel.width/2;
            width: parent.width;
            color: "white";
            font.family: "Myriad Pro"
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea {
            anchors.fill: record
            onClicked: {
                audioMute = !audioMute;
            }
        }

        states: [
            State {
                name: "onVisible"; when: toolView.currentIndex == 3 && !dark && !audioMute
                PropertyChanges {
                    target: microphone;
                    color: "white";
                    opacity: 1;
                }
            },
            State {
                name: "offVisible"; when: toolView.currentIndex == 3 && !dark && audioMute;
                PropertyChanges {
                    target: microphone;
                    opacity: 1;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "offVisible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                }
            },
            Transition {
                from: ""; to: "onVisible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                    ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                }
            },
            Transition {
                from: "offVisible"; to: "onVisible"; reversible: true
                ParallelAnimation {
                    ColorAnimation { properties: "color"; duration: 300; easing.type: Easing.InOutQuad }
                }
            }
        ]
    }

    // MICROPHONE

    Sequencer {
        id: sequencer;
        y: toolbar.y + toolbar.height
        opacity: 0.0;
        enabled: toolView.currentIndex==4;
        height: rect.height -y;
        width: parent.width
        z: 1;
        color: "#262627";

        states: [
            State {
                name: "onVisible"; when: toolView.currentIndex == 4 && !dark && sequencerEnabled
                PropertyChanges {
                    target: sequencer;
                    color: "white";
                    opacity: 1;
                }
            },
            State {
                name: "offVisible"; when: toolView.currentIndex == 4 && !dark && !sequencerEnabled;
                PropertyChanges {
                    target: sequencer;
                    opacity: 1;
                }
            }
        ]

        transitions: [
            Transition {
                from: ""; to: "offVisible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                }
            },
            Transition {
                from: ""; to: "onVisible"; reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                    ColorAnimation { properties: "color"; duration: 250; easing.type: Easing.InOutQuad }
                }
            },
            Transition {
                from: "offVisible"; to: "onVisible"; reversible: true
                ParallelAnimation {
                    ColorAnimation { properties: "color"; duration: 300; easing.type: Easing.InOutQuad }
                }
            }
        ]
    }
}
