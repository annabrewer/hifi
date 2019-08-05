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
import "../lib/prop" as Prop
import "../lib/plotperf"


Column {
    id: render2;   
    
    //color: hifi.colors.baseGray;

    //anchors.fill: parent
    //anchors.left: parent.right
    //anchors.right: parent.right  

    anchors.margins: 10

    Row {
        anchors.left: parent.left
        width: parent.width

        spacing: 20

        function itemIndex(item) {
            if (item.parent == null)
                return -1
            var siblings = item.parent.children
            for (var i = 0; i < siblings.length; i++)
                if (siblings[i] == item)
                    return i
            return -1 //will never happen
        }

        function previousItem(item) {
            if (item.parent == null)
                return null
            var index = itemIndex(item)
            return (index > 0)? item.parent.children[itemIndex(item) - 1]: null
        }

        /*
        Repeater {
            model: [
            ]
        
        }
        */

        Column {
            width: parent.width / 2
            spacing: 5
            anchors.left: parent.left
            function evalEvenHeight() {
                // Why do we have to do that manually ? cannot seem to find a qml / anchor / layout mode that does that ?
                return (height - spacing * (children.length - 1)) / children.length
            }
    
            PlotCurve {
                title: "Tone Curve"
                width: parent.width - 5
                object: parent.drawOpaqueConfig
                valueScale: 1
                valueNumDigits: 3
                height: 160
                points: 
                [
                    {
                        object: Render.getConfig("RenderMainView.ToneMapping"),
                        prop: "toeLength"
                    },
                    {
                        object: Render.getConfig("RenderMainView.ToneMapping"),
                        prop: "toeStrength"
                    },
                    {
                        object: Render.getConfig("RenderMainView.ToneMapping"),
                        prop: "shoulderLength"
                    }
                ]
            }
            
    
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
            Prop.PropScalar {
                label: "Toe Strength"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "toeStrength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
    
            Prop.PropScalar {
                label: "Toe Length"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "toeLength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
            Prop.PropScalar {
                label: "Shoulder Strength"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "shoulderStrength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
    
            Prop.PropScalar {
                label: "Shoulder Length (Stops)"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "shoulderLength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
    
            Prop.PropScalar {
                label: "Shoulder Angle"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "shoulderAngle"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
            Prop.PropScalar {
                label: "Gamma"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "gamma"
                numDigits: 1
                min: 1
                max: 3
                anchors.left: parent.left
                anchors.right: parent.right 
            }
        }
        Column {
            width: parent.width / 2
            anchors.right: parent.right
            spacing: 5
            
            PlotCurve {
                title: "Tone Curve"
                object: parent.drawOpaqueConfig
                valueScale: 1
                valueNumDigits: 3
                height: 160
                width: parent.width - 5
                points: 
                [
                    {
                        object: Render.getConfig("RenderMainView.ToneMapping"),
                        prop: "toeLength"
                    },
                    {
                        object: Render.getConfig("RenderMainView.ToneMapping"),
                        prop: "toeStrength"
                    },
                    {
                        object: Render.getConfig("RenderMainView.ToneMapping"),
                        prop: "shoulderLength"
                    }
                ]
            }
            
    
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
            Prop.PropScalar {
                label: "Toe Strength"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "toeStrength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
    
            Prop.PropScalar {
                label: "Toe Length"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "toeLength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
            Prop.PropScalar {
                label: "Shoulder Strength"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "shoulderStrength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
    
            Prop.PropScalar {
                label: "Shoulder Length (Stops)"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "shoulderLength"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
    
            Prop.PropScalar {
                label: "Shoulder Angle"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "shoulderAngle"
                min: 0
                max: 1
                anchors.left: parent.left
                anchors.right: parent.right 
            }
            Prop.PropScalar {
                label: "Gamma"
                object: Render.getConfig("RenderMainView.ToneMapping")
                property: "gamma"
                numDigits: 1
                min: 1
                max: 3
                anchors.left: parent.left
                anchors.right: parent.right 
            }
        }
    }
}
