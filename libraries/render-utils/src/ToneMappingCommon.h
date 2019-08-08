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
    vec4 _shoulderOffsetX_shoulderOffsetY_shoulderLnA_shoulderB;
    vec4 _toeLnA_toeB_linearLnA_linearB;
    vec4 _linearOffsetX_2powExp_w_invW;
    vec4 _x0_y0_x1_y1;
    vec4 _toeScaleY_linearScaleY_shoulderScaleY_s3;
    ivec4 toneCurve_s0_s1_s2;
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

// Hack Comment

// <@if 1@>
// Trigger Scribe include
// <@endif@> <!def that !>

// <@endif@>

// Hack Comment

