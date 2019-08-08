//
//  PlotPerf.qml
//  examples/utilities/render/plotperf
//
//  Created by Sam Gateau on 3//2016
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//
import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    id: root
    width: parent.width
    height: 100

    property var color: "#FFFFFF"

    property var lineWidth: 1 

    // The title of the graph
    property string title

    property var backgroundOpacity: 0.6

    property var toeSamples
    property var shoulderSamples

    property var toeEnd
    property var shoulderStart
    property var shoulderEnd

    property var dataResolution

    Component.onCompleted: {
        pullFreshValues();
    }

    Timer {
        interval: 100; running: true; repeat: true
        onTriggered: mycanvas.requestPaint()
    }

    Canvas {
        id: mycanvas
        anchors.fill:parent
        
        onPaint: {
            var lineHeight = 12;

            function plot(ctx) {
                var verticalScale = 100.0;
                var rightPoint = shoulderEnd;
                var one = (dataResolution / rightPoint) * width;

                var toeSteps = toeSamples.length;
                var toeRatio = toeEnd / rightPoint;
                var toePixelLength = toeRatio * width;
                var toePixelStepSize = toePixelLength / (toeSteps - 1);

                var shoulderSteps = shoulderSamples.length;
                var shoulderRatioBefore = shoulderStart / rightPoint;
                var scaledShoulderStart = shoulderRatioBefore * width;
                var shoulderRatio = 1 - shoulderRatioBefore;
                var shoulderPixelLength = shoulderRatio * width;
                var shoulderPixelStepSize = shoulderPixelLength / (shoulderSteps - 1);

                ctx.beginPath();

                ctx.strokeStyle= "#555555";

                ctx.moveTo(one, 0);
                ctx.lineTo(one, height);

                ctx.moveTo(0, height - verticalScale);
                ctx.lineTo(width, height - verticalScale);

                ctx.stroke();

                ctx.beginPath();

                ctx.strokeStyle= color;
                ctx.lineWidth=lineWidth;
                ctx.moveTo(0, height); 

                for (var i = 0; i < toeSteps; i++) {
                    ctx.lineTo(i * toePixelStepSize, height - (toeSamples[i] / verticalScale));
                }
                
                for (var i = 0; i < shoulderSteps; i++) {
                    ctx.lineTo(scaledShoulderStart + i * shoulderPixelStepSize, height - (shoulderSamples[i] / verticalScale));
                }

                ctx.stroke();
            }
            
            function displayTitle(ctx, text) {
                ctx.fillStyle = "grey";
                ctx.textAlign = "right";
               
                ctx.fillStyle = "white";
                ctx.textAlign = "left";
                ctx.fillText(text, 0, lineHeight);
            }

            function displayBackground(ctx) {
                ctx.fillStyle = Qt.rgba(0, 0, 0, root.backgroundOpacity);
                ctx.fillRect(0, 0, width, height);
            }
            
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.font="12px Verdana";

            displayBackground(ctx);
                
            plot(ctx);

            displayTitle(ctx, title)
        }
    }
}
