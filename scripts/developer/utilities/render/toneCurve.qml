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
//import "ChannelMode"

Column {
    id: toneCurve;   
    anchors.margins: 10

    Prop.PropBoolDropdown {
        id: channelMode
        label: "Channel Mode"
        object: Render.getConfig("RenderMainView.ToneMapping")
        property: "channelMode"
        enums: [
                    "Global",
                    "Spectral",
                ]
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Row {
        anchors.left: parent.left
        width: parent.width
        spacing: 5

        Repeater {
            id: rep
            
            model: { if (channelMode.value) return ["Red:#FF001A:1:R","Green:#009036:2:G","Blue:#009EE0:3:B"] 
                    else return ["Global:#FFFFFF:0:ALL"] }

            Item {
                width: (parent.parent.width - (rep.count == 1 ? 0 : 10)) / rep.count
                height: 400 

                Column {
                    width: parent.width - (rep.count == 1 ? 0 : 5)
                    spacing: 5
    
                    PlotCurve {
                        title: "Tone Curve " + modelData.split(":")[0]
                        width: parent.width
                        height: 160
                        color: modelData.split(":")[1]
                        lineWidth: 2
                        toeSamples: Render.getConfig("RenderMainView.ToneMapping").toeSamples
                        shoulderSamples: Render.getConfig("RenderMainView.ToneMapping").shoulderSamples
                        toeEnd: Render.getConfig("RenderMainView.ToneMapping").toeEnd
                        shoulderStart: Render.getConfig("RenderMainView.ToneMapping").shoulderStart
                        shoulderEnd: Render.getConfig("RenderMainView.ToneMapping").shoulderEnd
                        dataResolution: Render.getConfig("RenderMainView.ToneMapping").dataResolution
                        index: modelData.split(":")[2]
                        global: !channelMode.value
                    }

                    Repeater {
                        // modelData in here refers to the modelData of the parent repeater
                        // modelData.split(":")[3] will be "R", "G", or "B" or "ALL" for global
                        model: [
                            modelData.split(":")[3] + ":Toe Length:toeLength",
                            modelData.split(":")[3] + ":Toe Strength:toeStrength",
                            modelData.split(":")[3] + ":Shoulder Strength:shoulderStrength",
                            modelData.split(":")[3] + ":Shoulder Length:shoulderLength",
                            modelData.split(":")[3] + ":Shoulder Angle:shoulderAngle"
                        ]

                        Prop.PropScalar {
                            label: modelData.split(":")[1]
                            object: Render.getConfig("RenderMainView.ToneMapping")
                            property: modelData.split(":")[2] + modelData.split(":")[0]
                            min: 0
                            max: 1
                            anchors.left: parent.left
                            anchors.right: parent.right 
                        }
                    }

                    Prop.PropScalar {
                        label: "Gamma"
                        object: Render.getConfig("RenderMainView.ToneMapping")
                        property: "gamma" + modelData.split(":")[3]
                        numDigits: 1
                        min: 1
                        max: 3
                        anchors.left: parent.left
                        anchors.right: parent.right 
                    }
                }
            }
        }
    }
}
