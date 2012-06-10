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

Item {
    id: root

    Component.onCompleted: priv.layoutChildren()
    onChildrenChanged: priv.layoutChildren()
    onWidthChanged: priv.layoutChildren()
    onHeightChanged: priv.layoutChildren()

    Keys.onPressed: {
        if (event.key == Qt.Key_Right || event.key == Qt.Key_Left) {
            if (event.key == Qt.Key_Right || priv.mirrored) {
                var oldIndex = priv.currentButtonIndex()
                if (oldIndex != root.children.length - 1) {
                    priv.tabGroup.currentTab = root.children[oldIndex + 1].tab
                    event.accepted = true
                }
            } else if (event.key == Qt.Key_Left || priv.mirrored) {
                var oldIndex = priv.currentButtonIndex()
                if (oldIndex != 0) {
                    priv.tabGroup.currentTab = root.children[oldIndex - 1].tab
                    event.accepted = true
                }
            }
        }
    }

    focus: true

    QtObject {
        id: priv
        property Item firstButton: root.children.length > 0 ? root.children[0] : null
        property Item firstTab: firstButton ? (firstButton.tab != null ? firstButton.tab : null) : null
        property Item tabGroup: firstTab ? (firstTab.parent ? firstTab.parent.parent : null) : null
        property bool mirrored: root.LayoutMirroring.enabled

        onMirroredChanged: layoutChildren()

        function currentButtonIndex() {
            for (var i = 0; i < root.children.length; ++i) {
                if (root.children[i].tab == tabGroup.currentTab)
                    return i
            }
            return -1
        }

        function layoutChildren() {
            var childCount = root.children.length
            var contentWidth = 0
            var contentHeight = 0
            if (childCount != 0) {
                var itemWidth = root.width / childCount
                var itemIndex = mirrored ? childCount - 1 : 0
                var increment = mirrored ? - 1 : 1

                for (var i = 0; i < childCount; ++i, itemIndex += increment) {
                    var child = root.children[itemIndex]
                    child.x = i * itemWidth
                    child.y = 0
                    child.width = itemWidth
                    child.height = root.height

                    if (child.implicitWidth != undefined) {
                        contentWidth = Math.max(contentWidth, child.implicitWidth * childCount)
                        contentHeight = Math.max(contentHeight, child.implicitHeight)
                    }
                }
            }
            root.implicitWidth = contentWidth
            root.implicitHeight = contentHeight
        }
    }
}
