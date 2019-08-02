//
//  PropItem.qml
//
//  Created by Sam Gateau on 3/2/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7

import controlsUit 1.0 as HifiControls

PropItem {
    Global { id: global }
    id: root

    // Scalar Prop
    property bool integral: false
    property var numDigits: 2


    property alias min: sliderControl.minimumValue
    property alias max: sliderControl.maximumValue

    property bool showValue: true  
    
    Component.onCompleted: {
    }  

    property var sourceValueVar: root.valueVarGetter()

    function applyValueVarFromWidgets(value) {
        if (!root.readOnly) { 
           root.valueVarSetter(value)
           valueLabel.text = value.toFixed(root.integral ? 0 : root.numDigits);
        }
    }

    PropLabel {
        id: valueLabel
        enabled: root.showValue

        anchors.left: root.splitter.right
        anchors.verticalCenter: root.verticalCenter
        width: root.width * (root.readOnly ? 1.0 : global.valueAreaWidthScale)
        horizontalAlignment: global.valueTextAlign
        height: global.slimHeight
        
        text: root.sourceValueVar.toFixed(root.integral ? 0 : root.numDigits)

        background: Rectangle {
            color: global.color
            border.color: global.colorBorderLight
            border.width: global.valueBorderWidth
            radius: global.valueBorderRadius
        }
    }

    HifiControls.Slider {
        id: sliderControl
        visible: !root.readOnly
        stepSize: root.integral ? 1.0 : 0.0
        anchors.left: valueLabel.right
        anchors.right: root.right
        anchors.verticalCenter: root.verticalCenter
        value: root.sourceValueVar
        onValueChanged: { applyValueVarFromWidgets(value) }
    }

    
}
