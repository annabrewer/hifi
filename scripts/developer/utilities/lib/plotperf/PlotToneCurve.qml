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

    // The title of the graph
    property string title

    // The object used as the default source object for the prop plots
    property var object

    property var backgroundOpacity: 0.6

    // Plots is an array of plot descriptor
    // a default plot descriptor expects the following object:
    //     prop: [ {
    //                object: {}             // Optional: this is the object from which the prop will be fetched, 
    //                                          if nothing than the object from root is used
    //                prop:"bufferCPUCount", // Needed the name of the property from the object to feed the plot
    //                label: "CPU",          // Optional: Label as displayed on the plot
    //                color: "#00B4EF"       // Optional: Color of the curve
    //                unit: "km/h"           // Optional: Unit added to the value displayed, if nothing then the default unit is used
    //                scale: 1               // Optional: Extra scaling used to represent the value, this scale is combined with the global scale.
    //            },

    property var points

    // Default value scale used to define the max value of the chart
    property var valueScale: 1

    // Default number of digits displayed
    property var valueNumDigits: 0
    
    property var valueMax : 1
    property var valueMin : 0

    property var displayMinAt0 : true
    property var _displayMaxValue : 1
    property var _displayMinValue : 0

    property var tick : 0
    property var color : "#FFFF00"
    /*
    Component.onCompleted: {
        createValues();   
    }
    */
    
    
    function createValues() {
        // Wait until values are created to begin pulling
        if (!points) { return; }

        _displayMaxValue = 1.5;
        _displayMinValue = 0;

        mycanvas.requestPaint()
    }

    onTriggerChanged: createValues() 

    Timer {
        interval: 100; running: true; repeat: true
        onTriggered: createValues()
    }

    Canvas {
        id: mycanvas
        anchors.fill:parent
        
        onPaint: {
            var lineHeight = 12;

            function displayValue(val, unit) {
                 return (val / root.valueScale).toFixed(root.valueNumDigits)
            }

            function pixelFromVal(val) {
                return lineHeight + (height - lineHeight) * (1 - (0.99) * (val - _displayMinValue) / (_displayMaxValue - _displayMinValue));
            }
            function valueFromPixel(pixY) {
                return _displayMinValue + (((pixY - lineHeight) / (height - lineHeight) - 1) * (_displayMaxValue - _displayMinValue) / (-0.99));
            }

            function plot(ctx) {
                ctx.beginPath();
                ctx.strokeStyle= color; // Green path
                ctx.lineWidth="2";
                ctx.moveTo(0, 0); 
                   
                for (var i = 0; i < 3; i++) { 
                    //ctx.lineTo(i * 30, pixelFromVal(points[i]["prop"])); 
                    ctx.lineTo(i * 30, pixelFromVal(1)); 
                }

                ctx.stroke();
            }

            function displayBackground(ctx) {
                ctx.fillStyle = Qt.rgba(0, 0, 0, root.backgroundOpacity);
                ctx.fillRect(0, 0, width, height);
            }

            function displayMaxZeroMin(ctx) {
                var maxY = pixelFromVal(_displayMaxValue);
                
                ctx.strokeStyle= "LightSlateGray";
                ctx.lineWidth="1";
                ctx.beginPath();
                ctx.moveTo(0, maxY); 
                ctx.lineTo(width, maxY);
                ctx.stroke();

                if (_displayMinValue != 0) { 
                    var zeroY = pixelFromVal(0);
                    var minY = pixelFromVal(_displayMinValue);
                    ctx.beginPath();
                    ctx.moveTo(0, zeroY); 
                    ctx.lineTo(width, zeroY);
                    ctx.moveTo(0, minY); 
                    ctx.lineTo(width, minY);
                    ctx.stroke();
                } 
            }
            
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.font="12px Verdana";

            displayBackground(ctx);
                
            displayMaxZeroMin(ctx);

            plot(ctx)
        }
    }

    MouseArea {
        id: hitbox
        anchors.fill: mycanvas

        onClicked: {
            resetMax();
            resetMin();
        }
    }
}
