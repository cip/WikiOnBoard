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
import "." 1.1

CommonDialog {
    id: root
    objectName: "root"

    property string message
    property string acceptButtonText
    property string rejectButtonText
    property alias icon: root.titleIcon // for backwards compatibility

    onAcceptButtonTextChanged: internal.updateButtonTexts()
    onRejectButtonTextChanged: internal.updateButtonTexts()

    onStatusChanged: {
        if (status == DialogStatus.Open)
            scrollBar.flash()
    }

    onButtonClicked: {
        if (acceptButtonText && index == 0)
            accepted()
        else
            rejected()
    }

    content: Item {
        height: {
            if (root.height > 0)
                return Math.min(Math.max(privateStyle.dialogMinSize, root.height) - privateTitleHeight - privateButtonsHeight, root.platformContentMaximumHeight)
            else
                return Math.min(label.paintedHeight, root.platformContentMaximumHeight)
        }
        width: parent.width

        Item {
            anchors {
                top: parent.top; topMargin: platformStyle.paddingLarge
                bottom: parent.bottom; bottomMargin: platformStyle.paddingLarge
                left: parent.left; leftMargin: platformStyle.paddingLarge
                right: parent.right
            }

            Flickable {
                id: flickable
                width: parent.width
                height: parent.height
                anchors { left: parent.left; top: parent.top }
                contentHeight: label.paintedHeight
                flickableDirection: Flickable.VerticalFlick
                clip: true
                interactive: contentHeight > height

                Text {
                    id: label
                    anchors { right: parent.right; rightMargin: privateStyle.scrollBarThickness }
                    width: flickable.width - privateStyle.scrollBarThickness
                    font { family: platformStyle.fontFamilyRegular; pixelSize: platformStyle.fontSizeMedium }
                    color: root.platformInverted ? platformStyle.colorNormalLightInverted
                                                 : platformStyle.colorNormalLight
                    wrapMode: Text.WordWrap
                    text: root.message
                    horizontalAlignment: Text.AlignLeft
                }
            }

            ScrollBar {
                id: scrollBar
                height: parent.height
                anchors { top: flickable.top; right: flickable.right }
                flickableItem: flickable
                interactive: false
                orientation: Qt.Vertical
                platformInverted: root.platformInverted
            }
        }
    }

    QtObject {
        id: internal

        function updateButtonTexts() {
            var newButtonTexts = []
            if (acceptButtonText)
                newButtonTexts.push(acceptButtonText)
            if (rejectButtonText)
                newButtonTexts.push(rejectButtonText)
            root.buttonTexts = newButtonTexts
        }
    }
}
