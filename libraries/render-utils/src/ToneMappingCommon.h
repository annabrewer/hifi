// <!
//  Created by Bradley Austin Davis on 2018/05/25
//  Copyright 2013-2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
// !>

// <@if not testvar@>
// <@def testvar@>

// Hack comment to absorb the extra '//' scribe prepends anna

struct ToneMappingParams {
    ivec4 _toneCurve_globalMode_s1_s2;
    vec4 _exp_2powExp_s0_s1;

    vec4 _toeLnA_toeB_toeScaleY_shoulderOffsetY;
    vec4 _shoulderLnA_shoulderB_shoulderOffsetX_shoulderScaleY;
    vec4 _linearLnA_linearB_linearOffsetX_linearScaleY;
    vec4 _fullCurveX0_fullCurveY0_fullCurveX1_fullCurveY1;
    vec4 _fullCurveW_fullCurveInvW_s0_s1;
};

struct CurveSegment {
    float m_offsetX;
    float m_offsetY;
    float m_scaleX; // always 1 or -1
    float m_scaleY;
    float m_lnA;
    float m_B;
};

struct FullCurve
{
    float m_W;
    float m_invW;

    float m_x0;
    float m_y0;
    float m_x1;
    float m_y1;
};

struct GlobalParams {
    int _toneCurve;
    int _channelMode;
    int _s01;
    int _s02;

    float _exposure;
    float _twoPowExposure;
    float _s10;
    float _s11;
};

struct CurveParams {
    float _toeLnA;
    float _toeB;
    float _toeScaleY;
    float _shoulderOffsetY;

    float _shoulderLnA;
    float _shoulderB;
    float _shoulderOffsetX;
    float _shoulderScaleY;

    float _linearLnA;
    float _linearB;
    float _linearOffsetX;
    float _linearScaleY;

    float _fullCurveX0;
    float _fullCurveY0;
    float _fullCurveX1;
    float _fullCurveY1;

    float _fullCurveW;
    float _fullCurveInvW;
    float _s30;
    float _s31;
};

struct Parameters {
    // same for all curves
    GlobalParams _globals;

    // params to edit when in global mode
    CurveParams _globalParams;

    // params to edit when in spectral mode
    CurveParams _redParams;
    CurveParams _greenParams;
    CurveParams _blueParams;
};

// Hack Comment

// <@if 1@>
// Trigger Scribe include
// <@endif@> <!def that !>

// <@endif@>

// Hack Comment

