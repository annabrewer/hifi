//
//  markerTipEntityScript.js
//  examples/homeContent/markerTipEntityScript
//
//  Created by Eric Levin on 2/17/15.
//  Copyright 2016 High Fidelity, Inc.
//

//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html



(function() {
    Script.include("../../libraries/utils.js");

    var MAX_POINTS_PER_STROKE = 40;

    MarkerTip = function() {
        _this = this;
        _this.MARKER_TEXTURE_URL = "https://s3-us-west-1.amazonaws.com/hifi-content/eric/textures/markerStroke.png";
        this.strokeForwardOffset = 0.0005;
        this.STROKE_FORWARD_OFFSET_INCRERMENT = 0.00001;
    };

    MarkerTip.prototype = {

        continueNearGrab: function() {

            _this.continueHolding();
        },

        continueEquip: function() {
            _this.continueHolding();
        },

        continueHolding: function() {
            // cast a ray from marker and see if it hits anything

            var props = Entities.getEntityProperties(_this.entityID, ["position", "rotation"]);

            var pickRay = {
                origin: props.position,
                direction: Quat.getFront(props.rotation)
            }

            var intersection = Entities.findRayIntersection(pickRay, true, [_this.whiteboard]);

            if (intersection.intersects) {

                this.paint(intersection.intersection)
            }
        },

        newStroke: function(position) {
            _this.strokeBasePosition = position;
            _this.currentStroke = Entities.addEntity({
                type: "PolyLine",
                name: "marker stroke",
                dimensions: {
                    x: 10,
                    y: 10,
                    z: 10
                },
                position: position,
                textures: _this.MARKER_TEXTURE_URL,
                color: {red: 0, green: 10, blue: 200}
            });

            _this.linePoints = [];
            _this.normals = [];
            _this.strokeWidths = [];
        },

        paint: function(position) {
            if (!_this.currentStroke) {
                _this.newStroke(position);
            }

            var localPoint = Vec3.subtract(position, this.strokeBasePosition);
            // localPoint = Vec3.sum(localPoint, Vec3.multiply(_this.whiteboardNormal, _this.strokeForwardOffset));
            _this.strokeForwardOffset += _this.STROKE_FORWARD_OFFSET_INCRERMENT;

            _this.linePoints.push(localPoint);
            _this.normals.push(_this.whiteboardNormal);
            this.strokeWidths.push(0.02);

            Entities.editEntity(_this.currentStroke, {
                linePoints: _this.linePoints,
                normals: _this.normals,
                strokeWidths: _this.strokeWidths
            });

            if (_this.linePoints.length > MAX_POINTS_PER_STROKE) {
                _this.currentStroke = null;
            }
        },

        preload: function(entityID) {
            this.entityID = entityID;
            print("EBL PRELOAD");
        },

        setWhiteboard: function(myId, data) {
            _this.whiteboard = JSON.parse(data[0]);
            var props = Entities.getEntityProperties(_this.whiteboard, ["rotation"]);
            _this.whiteboardNormal = Vec3.multiply(Quat.getRight(props.rotation), -1);
        }
    };

    // entity scripts always need to return a newly constructed object of our type
    return new MarkerTip();
});