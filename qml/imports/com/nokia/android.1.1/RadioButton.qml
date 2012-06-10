/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.1
import Qt.labs.components 1.1
import "." 1.1

Item {
    id: root

    // Common Public API
    property alias checked: checkable.checked
    property bool pressed: stateGroup.state == "Pressed" || stateGroup.state == "KeyPressed"
    signal clicked
    property alias text: label.text

    // Symbian specific API
    property alias platformExclusiveGroup: checkable.exclusiveGroup
    property bool platformInverted: false

    QtObject {
        id: internal
        objectName: "internal"
        property color disabledColor: root.platformInverted ? platformStyle.colorDisabledLightInverted
                                                            : platformStyle.colorDisabledLight
        property color pressedColor: root.platformInverted ? platformStyle.colorPressedInverted
                                                           : platformStyle.colorPressed
        property color normalColor: root.platformInverted ? platformStyle.colorNormalLightInverted
                                                          : platformStyle.colorNormalLight

        function toggle() {
            clickedEffect.restart()
            checkable.toggle()
            root.clicked()
        }

        function icon_postfix() {
            if (pressed)
                return "pressed"
            else if (root.checked) {
                if (!root.enabled)
                    return "disabled_selected"
                else
                    return "normal_selected"
            } else {
                if (!root.enabled)
                    return "disabled_unselected"
                else
                    return "normal_unselected"
            }
        }
    }

    StateGroup {
        id: stateGroup

        states: [
            State { name: "Pressed" },
            State { name: "KeyPressed" },
            State { name: "Canceled" }
        ]

        transitions: [
            Transition {
                to: "Pressed"
                ScriptAction { script:  privateStyle.play(Android.BasicItem) }
            },
            Transition {
                from: "Pressed"
                to: ""
                ScriptAction { script: privateStyle.play(Android.CheckBox) }
                ScriptAction { script: internal.toggle() }
            },
            Transition {
                from: "KeyPressed"
                to: ""
                ScriptAction { script: internal.toggle() }
            }
        ]
    }

    implicitWidth: privateStyle.textWidth(label.text, label.font) + platformStyle.paddingMedium + privateStyle.buttonSize
    implicitHeight: privateStyle.buttonSize

    Image {
        id: image
        source: privateStyle.imagePath("qtg_graf_radiobutton_" + internal.icon_postfix(),
                                       root.platformInverted)
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        sourceSize.width: privateStyle.buttonSize
        sourceSize.height: privateStyle.buttonSize

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onPressed: stateGroup.state = "Pressed"
            onReleased: stateGroup.state = ""
            onClicked: stateGroup.state = ""
            onExited: stateGroup.state = "Canceled"
            onCanceled: {
                // Mark as canceled
                stateGroup.state = "Canceled"
                // Reset state. Can't expect a release since mouse was ungrabbed
                stateGroup.state = ""
            }
        }
    }

    Text {
        id: label
        elide: Text.ElideRight
        anchors.left: image.right
        anchors.leftMargin: platformStyle.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        horizontalAlignment: Text.AlignLeft

        font { family: platformStyle.fontFamilyRegular; pixelSize: platformStyle.fontSizeMedium }
        color: root.enabled ? (root.pressed ? internal.pressedColor : internal.normalColor)
                            : internal.disabledColor
    }

    ParallelAnimation {
        id: clickedEffect
        SequentialAnimation {
            PropertyAnimation {
                target: image
                property: "scale"
                from: 1.0
                to: 0.8
                easing.type: Easing.Linear
                duration: 50
            }
            PropertyAnimation {
                target: image
                property: "scale"
                from: 0.8
                to: 1.0
                easing.type: Easing.OutQuad
                duration: 170
            }
        }
    }

    Keys.onPressed: {
        if (!event.isAutoRepeat && (event.key == Qt.Key_Select
                                    || event.key == Qt.Key_Return
                                    || event.key == Qt.Key_Enter)) {
            stateGroup.state = "KeyPressed"
            event.accepted = true
        }
    }

    Keys.onReleased: {
        if (!event.isAutoRepeat && (event.key == Qt.Key_Select
                                    || event.key == Qt.Key_Return
                                    || event.key == Qt.Key_Enter)) {
            stateGroup.state = ""
            event.accepted = true
        }
    }

    Checkable {
        id: checkable
        value: root.text
        enabled: true
    }
}
