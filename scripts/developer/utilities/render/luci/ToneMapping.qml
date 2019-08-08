//
//  ToneMapping.qml
//
//  Created by Sam Gateau on 4/17/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7
import QtQuick.Controls 1.4
import "../../lib/prop" as Prop
import "../../lib/plotperf"

Column {

    function evalEvenHeight() {
        // Why do we have to do that manually ? cannot seem to find a qml / anchor / layout mode that does that ?
        return (height - spacing * (children.length - 1)) / children.length
    }

    anchors.left: parent.left
    anchors.right: parent.right 
    
    Prop.PropEnum {
        label: "Tone Curve"
        object: Render.getConfig("RenderMainView.ToneMapping")
        property: "curve"
        enums: [
                    "RGB",
                    "SRGB",
                    "Reinhard",
                    "Filmic",
                    "Piecewise Filmic"
                ]
        anchors.left: parent.left
        anchors.right: parent.right 
        onValueChanged: {
        
        }
    }
    Prop.PropScalar {
        label: "Exposure"
        object: Render.getConfig("RenderMainView.ToneMapping")
        property: "exposure"
        min: -4
        max: 4
        anchors.left: parent.left
        anchors.right: parent.right 
    }
}
