//
//  PlotCurve.qml
//
//  Created by Anna Brewer on 8/2/2019
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

    property var dataResolution: 10000.0

    // indices: 0 = global, 1 = r, 2 = g, 3 = b
    property var index: 0

    property var global: true

    Component.onCompleted: {
        pullFreshValues();
    }

    Timer {
        interval: 15; running: true; repeat: true
        onTriggered: mycanvas.requestPaint()
    }

    Canvas {
        id: mycanvas
        anchors.fill:parent
        
        onPaint: {
            var lineHeight = 12;

            var indexForSamples = (global ? index : index - 1)

            var toeSteps = (global ? toeSamples.length : toeSamples.length / 3);
            var start = indexForSamples * toeSteps;
            var toeSamplesCurrent = toeSamples.slice(start, start + toeSteps);

            var shoulderSteps = (global ? shoulderSamples.length : shoulderSamples.length / 3);
            var start = indexForSamples * shoulderSteps;
            var shoulderSamplesCurrent = shoulderSamples.slice(start, start + shoulderSteps);

            var toeEndCurrent = toeEnd[index];
            var shoulderStartCurrent = shoulderStart[index];
            var shoulderEndCurrent = shoulderEnd[index];

            var rightPoint = shoulderEndCurrent;
            var oneX = (dataResolution / rightPoint) * width;
            var verticalScale = 100.0;
            var oneY = height - verticalScale;

            function plot(ctx) {

                // draw guidelines showing position of (1, 1)

                ctx.beginPath();

                ctx.strokeStyle= "#555555";

                ctx.moveTo(oneX, 0);
                ctx.lineTo(oneX, height);

                ctx.moveTo(0, oneY);
                ctx.lineTo(width, oneY);

                ctx.stroke();

                // draw curve

                var toeRatio = toeEndCurrent / rightPoint;
                var toePixelLength = toeRatio * width;
                var toePixelStepSize = toePixelLength / (toeSteps - 1);

                var shoulderRatioBefore = shoulderStartCurrent / rightPoint;
                var scaledShoulderStart = shoulderRatioBefore * width;
                var shoulderRatio = 1 - shoulderRatioBefore;
                var shoulderPixelLength = shoulderRatio * width;
                var shoulderPixelStepSize = shoulderPixelLength / (shoulderSteps - 1);

                ctx.beginPath();

                ctx.strokeStyle = color;
                ctx.lineWidth = lineWidth;
                ctx.moveTo(0, height); 
                
                for (var i = 0; i < toeSteps; i++) {
                    ctx.lineTo(i * toePixelStepSize, height - (toeSamplesCurrent[i] / verticalScale));
                }
                
                
                for (var i = 0; i < shoulderSteps; i++) {
                    ctx.lineTo(scaledShoulderStart + i * shoulderPixelStepSize, height - (shoulderSamplesCurrent[i] / verticalScale));
                }

                ctx.stroke();
            }
            
            function displayTitle(ctx, text) {
                ctx.fillStyle = "white";
                ctx.textAlign = "left";
                ctx.fillText(text, 0, lineHeight);
            }

            function displayText(ctx, text, x, y) {
                ctx.fillStyle = "grey";
                ctx.textAlign = "left";
                ctx.fillText(text, x, y + lineHeight);
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

            displayTitle(ctx, title);

            displayText(ctx, "(1, 1)", oneX + 5, oneY + 5);
        }
    }
}
