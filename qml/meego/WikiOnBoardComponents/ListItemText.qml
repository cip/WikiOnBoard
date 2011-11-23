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

Text {
    id: root
    property string mode: "normal"
    property string role: "Title"
    property bool platformInverted: false

    // Also role "Heading" taken into account although not explicitely used in evaluations below
    /*font {
       // family: platformStyle.fontFamilyRegular
       //     pixelSize: (role == "Title" || role == "SelectionTitle") ? platformStyle.fontSizeLarge : platformStyle.fontSizeSmall
       // weight: (role == "SubTitle" || role == "SelectionSubTitle") ? Font.Light : Font.Normal
    }*/
    //color: internal.normalColor
    elide: Text.ElideRight
    horizontalAlignment: root.role != "Heading" ? Text.AlignLeft : Text.AlignRight

    // Performance optimization:
    // Use value assignment when property changes instead of binding to js function
    //onModeChanged: { color = internal.getColor() }

    QtObject {
        id: internal

           /*        // Performance optmization:
        // Use tertiary operations even though it doesn't look that good
        property color colorMid: root.platformInverted ? platformStyle.colorNormalMidInverted
                                                       : platformStyle.colorNormalMid
        property color colorLight: root.platformInverted ? platformStyle.colorNormalLightInverted
                                                         : platformStyle.colorNormalLight
        property color normalColor: (root.role == "SelectionSubTitle" || root.role == "SubTitle")
                                    ? colorMid : colorLight

        function getColor() {
            if (root.mode == "pressed")
                return root.platformInverted ? platformStyle.colorPressedInverted
                                             : platformStyle.colorPressed
            else if (root.mode == "highlighted")
                return root.platformInverted ? platformStyle.colorHighlightedInverted
                                             : platformStyle.colorHighlighted
            else if (root.mode == "disabled")
                return root.platformInverted ? platformStyle.colorDisabledLightInverted
                                             : platformStyle.colorDisabledLight
            else
                return normalColor
        }*/
    }
}
