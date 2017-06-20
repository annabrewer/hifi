//
//  Created by Dante Ruiz on 6/1/17.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//


import QtQuick 2.5

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import "../../styles-uit"
import "../../controls"
import "../../controls-uit" as HifiControls


Rectangle {
    id: info


    signal canceled()
    signal restart()

    property int count: 3 
    
    HifiConstants { id: hifi }
    visible: true
    color: hifi.colors.baseGray

    BusyIndicator {
        id: busyIndicator
        width: 350
        height: 350

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 60
        }
        running: true
    }


    HiFiGlyphs {
        id: image
        text: hifi.glyphs.avatar1
        size: 190
        color: hifi.colors.white

        anchors {
            top: busyIndicator.top
            topMargin: 40
            horizontalCenter: busyIndicator.horizontalCenter
        }
    }

    RalewayBold {
        id: statusText
        text: "CALIBRATION STARTING IN"
        size: 16
        color: hifi.colors.blueHighlight

        anchors {
            top: image.bottom
            topMargin: 15
            horizontalCenter: image.horizontalCenter
        }
    }


    RalewayBold {
        id: countDown
        text: info.count
        color: hifi.colors.blueHighlight

        anchors {
            top: statusText.bottom
            topMargin: 12
            horizontalCenter: statusText.horizontalCenter
        }
    }
    

    RalewayBold {
        id: directions

        anchors {
            top: busyIndicator.bottom
            topMargin: 100
            horizontalCenter: parent.horizontalCenter
        }

        color: hifi.colors.white
        size: hifi.fontSizes.rootMenuDisclosure
        text: "please stand in a T-Pose during calibration"
    }

    NumberAnimation {
        id: numberAnimation
        target: info
        property: count
        to: 0
    }

    Row {

        spacing: 20
        anchors {
            top: directions.bottom
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }


        HifiControls.Button {
            width: 120
            height: 30
            color: hifi.buttons.red
            text: "RESTART"

            onClicked: {
                restart();
            }
        }

        HifiControls.Button {
            width: 120
            height: 30
            color: hifi.buttons.black
            text: "CANCEL"

            onClicked: {
                stack.pop();
                canceled();
            }
        }
    }
    
    function start() {
    }
    function callingFunction() {
    }
}
