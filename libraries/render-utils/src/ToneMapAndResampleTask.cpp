//
//  ToneMapAndResampleTask.cpp
//  libraries/render-utils/src
//
//  Created by Anna Brewer on 7/3/19.
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "ToneMapAndResampleTask.h"

#include <gpu/Context.h>
#include <shaders/Shaders.h>

#include "render-utils/ShaderConstants.h"
#include "StencilMaskPass.h"
#include "FramebufferCache.h"

using namespace render;
using namespace shader::gpu::program;
using namespace shader::render_utils::program;

gpu::PipelinePointer ToneMapAndResample::_pipeline;
gpu::PipelinePointer ToneMapAndResample::_mirrorPipeline;
gpu::PipelinePointer ToneMapAndResample::_piecewisePipeline;
gpu::PipelinePointer ToneMapAndResample::_piecewiseMirrorPipeline;

CurveSegment ToneMapAndResample::m_segments[12];
FullCurve ToneMapAndResample::fullCurves[4];
bool ToneMapAndResample::globalMode = true;

ToneMapAndResample::ToneMapAndResample() {
    Parameters parameters;
    _parametersBuffer = gpu::BufferView(std::make_shared<gpu::Buffer>(sizeof(Parameters), (const gpu::Byte*) &parameters));
    userParams = { 0.5, 0.5, 2.0, 0.5, 1.0, 2.2 };
}

void ToneMapAndResample::init() {
    // shared_ptr to gpu::State
    gpu::StatePointer blitState = gpu::StatePointer(new gpu::State());

    blitState->setDepthTest(gpu::State::DepthTest(false, false));
    blitState->setColorWriteMask(true, true, true, true);

    _pipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMapping), blitState));
    _mirrorPipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMapping_mirrored), blitState));
    _piecewisePipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMappingPiecewise), blitState));
    _piecewiseMirrorPipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMappingPiecewise_mirrored), blitState));
}

void ToneMapAndResample::setExposure(float exposure) {
    auto& params = _parametersBuffer.get<Parameters>();
    float twoPowExp = powf(2.0, exposure);
    if (params._globals._twoPowExposure != twoPowExp) {
        _parametersBuffer.edit<Parameters>()._globals._exposure = exposure;
        _parametersBuffer.edit<Parameters>()._globals._twoPowExposure = twoPowExp;
    }
}

void ToneMapAndResample::setToneCurve(ToneCurve curve) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._globals._toneCurve != (int)curve) {
        _parametersBuffer.edit<Parameters>()._globals._toneCurve = (int)curve;
    }
}

void ToneMapAndResample::setChannelMode(ChannelMode mode) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._globals._toneCurve != (int)mode) {
        _parametersBuffer.edit<Parameters>()._globals._channelMode = (int)mode;
    }
}

void ToneMapAndResample::setToeStrength(glm::vec4 strength) {
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;
    for (int i = start; i < end; i++) {
        switch (i) {
            case 0: userParams._globalUserParams.m_toeStrength = strength[i]; break;
            case 1: userParams._redUserParams.m_toeStrength = strength[i]; break;
            case 2: userParams._greenUserParams.m_toeStrength = strength[i]; break;
            case 3: userParams._blueUserParams.m_toeStrength = strength[i]; break;
        }
    }
    _dirty = true;
}

void ToneMapAndResample::setToeLength(glm::vec4 length) {
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;
    for (int i = start; i < end; i++) {
        switch (i) {
            case 0: userParams._globalUserParams.m_toeLength = length[i]; break;
            case 1: userParams._redUserParams.m_toeLength = length[i]; break;
            case 2: userParams._greenUserParams.m_toeLength = length[i]; break;
            case 3: userParams._blueUserParams.m_toeLength = length[i]; break;
        }
    }
    _dirty = true;
}

void ToneMapAndResample::setShoulderStrength(glm::vec4 strength) {
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;
    for (int i = start; i < end; i++) {
        switch (i) {
            case 0: userParams._globalUserParams.m_shoulderStrength = strength[i]; break;
            case 1: userParams._redUserParams.m_shoulderStrength = strength[i]; break;
            case 2: userParams._greenUserParams.m_shoulderStrength = strength[i]; break;
            case 3: userParams._blueUserParams.m_shoulderStrength = strength[i]; break;
        }
    }
    _dirty = true;
}

void ToneMapAndResample::setShoulderLength(glm::vec4 length) {
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;
    for (int i = start; i < end; i++) {
        switch (i) {
            case 0: userParams._globalUserParams.m_shoulderLength = length[i]; break;
            case 1: userParams._redUserParams.m_shoulderLength = length[i]; break;
            case 2: userParams._greenUserParams.m_shoulderLength = length[i]; break;
            case 3: userParams._blueUserParams.m_shoulderLength = length[i]; break;
        }
    }
    _dirty = true;
}

void ToneMapAndResample::setShoulderAngle(glm::vec4 angle) {
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;
    for (int i = start; i < end; i++) {
        switch (i) {
            case 0: userParams._globalUserParams.m_shoulderAngle = angle[i]; break;
            case 1: userParams._redUserParams.m_shoulderAngle = angle[i]; break;
            case 2: userParams._greenUserParams.m_shoulderAngle = angle[i]; break;
            case 3: userParams._blueUserParams.m_shoulderAngle = angle[i]; break;
        }
    }
    _dirty = true;
}

void ToneMapAndResample::setGamma(glm::vec4 gamma) {
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;
    for (int i = start; i < end; i++) {
        switch (i) {
            case 0: userParams._globalUserParams.m_gamma = gamma[i]; break;
            case 1: userParams._redUserParams.m_gamma = gamma[i]; break;
            case 2: userParams._greenUserParams.m_gamma = gamma[i]; break;
            case 3: userParams._blueUserParams.m_gamma = gamma[i]; break;
        }
    }
    _dirty = true;
}

void ToneMapAndResample::setCurveParams() {
    auto& allParams = _parametersBuffer.edit<Parameters>();

    // in global mode we edit the first set of params, otherwise we edit the next 3 sets: R, G and B
    int start = globalMode ? 0 : 1;
    int end = globalMode ? 1 : 4;

    for (int i = start; i < end; i++) {
        auto& params = i == 0 ? allParams._globalParams : (i == 1 ? allParams._redParams : (i == 2 ? allParams._greenParams: allParams._blueParams));

        CurveSegment toe = m_segments[3 * i + 0];
        CurveSegment linear = m_segments[3 * i + 1];
        CurveSegment shoulder = m_segments[3 * i + 2];

        params._shoulderOffsetX = shoulder.m_offsetX;
        params._shoulderOffsetY = shoulder.m_offsetY;
        params._shoulderLnA = shoulder.m_lnA;
        params._shoulderB = shoulder.m_B;
        params._toeLnA = toe.m_lnA;
        params._toeB = toe.m_B;
        params._linearLnA = linear.m_lnA;
        params._linearB = linear.m_B;
        params._linearOffsetX = linear.m_offsetX;
        params._fullCurveW = fullCurves[i].m_W;
        params._fullCurveInvW = fullCurves[i].m_invW;
        params._fullCurveX0 = fullCurves[i].m_x0;
        params._fullCurveY0 = fullCurves[i].m_y0;
        params._fullCurveX1 = fullCurves[i].m_x1;
        params._fullCurveY1 = fullCurves[i].m_y1;
        params._toeScaleY = toe.m_scaleY;
        params._linearScaleY = linear.m_scaleY;
        params._shoulderScaleY = shoulder.m_scaleY;
    }
}

void ToneMapAndResample::configure(const Config& config) {
    setExposure(config.exposure);
    setToneCurve((ToneCurve)config.curve);
    setChannelMode((ChannelMode)config.channelMode);

    globalMode = (config.channelMode == 0);

    setToeStrength(config.toeStrength);
    setToeLength(config.toeLength);
    setShoulderStrength(config.shoulderStrength);
    setShoulderLength(config.shoulderLength);
    setShoulderAngle(config.shoulderAngle);
    setGamma(config.gamma);
}

void ToneMapAndResample::run(const RenderContextPointer& renderContext, const Input& input, Output& output) {
    assert(renderContext->args);
    assert(renderContext->args->hasViewFrustum());

    RenderArgs* args = renderContext->args;

    auto& params = _parametersBuffer.get<Parameters>();

    if (_dirty && params._globals._toneCurve == (int)ToneCurve::Piecewise) {

        // in global mode we edit the first set of params, otherwise we edit the next 3 sets: R, G and B
        int start = globalMode ? 0 : 1;
        int end = globalMode ? 1 : 4;

        for (int i = start; i < end; i++) {

            CurveParamsUser currentCurveParams;

            switch (i) {
                case 0: currentCurveParams = userParams._globalUserParams; break;
                case 1: currentCurveParams = userParams._redUserParams; break;
                case 2: currentCurveParams = userParams._greenUserParams; break;
                case 3: currentCurveParams = userParams._blueUserParams; break;
            }

            CurveParamsDirect curveParams = CalcDirectParamsFromUser(currentCurveParams);
            fullCurves[i] = CreateCurve(curveParams, i);
        }
        
        setCurveParams();

        _dirty = false;
    }

    auto lightingBuffer = input.get0()->getRenderBuffer(0);
    auto destinationFramebuffer = input.get1();

    if (!destinationFramebuffer) {
        destinationFramebuffer = args->_blitFramebuffer;
    }

    if (!lightingBuffer || !destinationFramebuffer) {
        return;
    }

    if (!_pipeline) {
        init();
    }

    const auto bufferSize = destinationFramebuffer->getSize();

    auto srcBufferSize = glm::ivec2(lightingBuffer->getDimensions());

    glm::ivec4 destViewport{ 0, 0, bufferSize.x, bufferSize.y };

    gpu::doInBatch("Resample::run", args->_context, [&](gpu::Batch& batch) {
        batch.enableStereo(false);
        batch.setFramebuffer(destinationFramebuffer);

        batch.setViewportTransform(destViewport);
        batch.setProjectionTransform(glm::mat4());
        batch.resetViewTransform();
        batch.setPipeline(params._globals._toneCurve == 4 ?
            (args->_renderMode == RenderArgs::MIRROR_RENDER_MODE ? _piecewiseMirrorPipeline : _piecewisePipeline) :
            (args->_renderMode == RenderArgs::MIRROR_RENDER_MODE ? _mirrorPipeline : _pipeline));
            

        batch.setModelTransform(gpu::Framebuffer::evalSubregionTexcoordTransform(srcBufferSize, args->_viewport));
        batch.setUniformBuffer(render_utils::slot::buffer::ToneMappingParams, _parametersBuffer);
        batch.setResourceTexture(render_utils::slot::texture::ToneMappingColor, lightingBuffer);
        batch.draw(gpu::TRIANGLE_STRIP, 4);
    });

    // Set full final viewport
    args->_viewport = destViewport;

    output = destinationFramebuffer;
}

float ALMOST_ZERO = 0.00001f;
float ALMOST_ONE = 0.99999f;

float MaxFloat(float x, float y) {
    return x > y ? x : y;
}

float MinFloat(float x, float y)
{
    return x < y ? x : y;
}

float Clamp(float x) {
    return MaxFloat(ALMOST_ZERO, MinFloat(ALMOST_ONE, x));
}

// find a function of the form:
//   f(x) = e^(lnA + Bln(x))
// where
//   f(0)   = 0; not really a constraint
//   f(x0)  = y0
//   f'(x0) = m
vec2 SolveAB(float x0, float y0, float m) {
    float B = (m * x0) / y0;
    float lnA = log(y0) - B * log(x0);
    return glm::vec2(lnA, B);
}

// convert to y=mx+b
vec2 AsSlopeIntercept(float x0, float x1, float y0, float y1) {
    float dy = (y1 - y0);
    float dx = (x1 - x0);
    float m;
    if (dx == 0)
        m = 1.0f;
    else
        m = dy / dx;

    float b = y0 - x0 * m;

    return glm::vec2(m, b);
}

// f(x) = (mx+b)^g
// f'(x) = gm(mx+b)^(g-1)
float EvalDerivativeLinearGamma(float m, float b, float g, float x) {
    float ret = g * m * pow(m * x + b, g - 1.0f);
    return ret;
}

float EvalCurveSegment(CurveSegment curve, float x) {
    float x0 = (x - curve.m_offsetX) * curve.m_scaleX;
    float y0 = 0.0f;

    // log(0) is undefined but our function should evaluate to 0. There are better ways to handle this,
    // but it's doing it the slow way here for clarity.
    if (x0 > 0)
    {
        y0 = exp(curve.m_lnA + curve.m_B * log(x0));
    }

    return y0 * curve.m_scaleY + curve.m_offsetY;
}


CurveParamsDirect ToneMapAndResample::CalcDirectParamsFromUser(const CurveParamsUser srcParams) {
    CurveParamsDirect dstParams;// = CurveParamsDirect(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    // This is not actually the display gamma. It's just a UI space to avoid having to 
    // enter small numbers for the input.
    float perceptualGamma = 2.2f;

    // constraints
    float toeLength = pow(Clamp(srcParams.m_toeLength), perceptualGamma);
    float toeStrength = Clamp(srcParams.m_toeStrength);
    float shoulderAngle = Clamp(srcParams.m_shoulderAngle);
    float shoulderLength = Clamp(srcParams.m_shoulderLength);
    float shoulderStrength = Clamp(srcParams.m_shoulderStrength);

    // apply base params

    // toe goes from 0 to 0.5
    float x0 = toeLength * .5f;
    float y0 = (1.0f - toeStrength) * x0; // lerp from 0 to x0

    float remainingY = 1.0f - y0;

    float initialW = x0 + remainingY;

    float y1_offset = (1.0f - shoulderLength) * remainingY; // distance from end of toe to start of shoulder
    float x1 = x0 + y1_offset; // we constrain linear section to have a slope of 1
    float y1 = y0 + y1_offset;

    // filmic shoulder strength is in F stops
    float extraW = exp2f(shoulderStrength) - 1.0f; // note that shoulder length does not affect white point

    float W = initialW + extraW;

    dstParams.m_x0 = x0;
    dstParams.m_y0 = y0;
    dstParams.m_x1 = x1;
    dstParams.m_y1 = y1;
    dstParams.m_W = W;

    // bake the linear to gamma space conversion
    dstParams.m_gamma = 2.2 / srcParams.m_gamma;

    dstParams.m_overshootX = (dstParams.m_W * 2.0f) * shoulderAngle * shoulderStrength;
    dstParams.m_overshootY = 0.5f * shoulderAngle * shoulderStrength;

    return dstParams;
}

FullCurve ToneMapAndResample::CreateCurve(const CurveParamsDirect srcParams, int index) {
    CurveParamsDirect params = srcParams;

    FullCurve dstCurve;// = FullCurve(1.0, 1.0, 0.25, 0.25, 0.75, 0.75);

    dstCurve.m_W = srcParams.m_W;
    dstCurve.m_invW = 1.0f / srcParams.m_W;

    // normalize params to 1.0 range
    params.m_W = 1.0f;
    params.m_x0 /= srcParams.m_W;
    params.m_x1 /= srcParams.m_W;
    params.m_overshootX = srcParams.m_overshootX / srcParams.m_W;

    float toeM = 0.0f;
    float shoulderM = 0.0f;
    float endpointM = 0.0f;

    vec2 mb = AsSlopeIntercept(params.m_x0, params.m_x1, params.m_y0, params.m_y1);
    float m = mb[0];
    float b = mb[1];

    float g = srcParams.m_gamma;

    // base function of linear section plus gamma is
    // y = (mx+b)^g

    // which we can rewrite as
    // y = exp(g*ln(m) + g*ln(x+b/m))

    // and our evaluation function is (skipping the if parts):
    /*
        float x0 = (x - m_offsetX)*m_scaleX;
        y0 = exp(m_lnA + m_B*log(x0));
        return y0*m_scaleY + m_offsetY;
    */

    CurveSegment midSegment;
    midSegment.m_offsetX = -(b / m);
    midSegment.m_offsetY = 0.0f;
    midSegment.m_scaleX = 1.0f;
    midSegment.m_scaleY = 1.0f;
    midSegment.m_lnA = g * log(m);
    midSegment.m_B = g;

    m_segments[index * 3 + 1] = midSegment;

    toeM = EvalDerivativeLinearGamma(m, b, g, params.m_x0);
    shoulderM = EvalDerivativeLinearGamma(m, b, g, params.m_x1);

    // apply gamma to endpoints
    params.m_y0 = MaxFloat(1e-5f, pow(params.m_y0, params.m_gamma));
    params.m_y1 = MaxFloat(1e-5f, pow(params.m_y1, params.m_gamma));

    params.m_overshootY = pow(1.0f + params.m_overshootY, params.m_gamma) - 1.0f;

    dstCurve.m_x0 = params.m_x0;
    dstCurve.m_x1 = params.m_x1;
    dstCurve.m_y0 = params.m_y0;
    dstCurve.m_y1 = params.m_y1;

    // toe section

    CurveSegment toeSegment;
    toeSegment.m_offsetX = 0;
    toeSegment.m_offsetY = 0.0f;
    toeSegment.m_scaleX = 1.0f;
    toeSegment.m_scaleY = 1.0f;

    vec2 toeAB = SolveAB(params.m_x0, params.m_y0, toeM);
    toeSegment.m_lnA = toeAB[0];
    toeSegment.m_B = toeAB[1];
    m_segments[index * 3] = toeSegment;

    // shoulder section

    // use the simple version that is usually too flat 
    CurveSegment shoulderSegment;

    float x0 = (1.0f + params.m_overshootX) - params.m_x1;
    float y0 = (1.0f + params.m_overshootY) - params.m_y1;

    vec2 shoulderAB = SolveAB(x0, y0, shoulderM);
    float lnA = shoulderAB[0];
    float B = shoulderAB[1];

    shoulderSegment.m_offsetX = (1.0f + params.m_overshootX);
    shoulderSegment.m_offsetY = (1.0f + params.m_overshootY);

    shoulderSegment.m_scaleX = -1.0f;
    shoulderSegment.m_scaleY = -1.0f;
    shoulderSegment.m_lnA = lnA;
    shoulderSegment.m_B = B;

    m_segments[index * 3 + 2] = shoulderSegment;

    // Normalize so that we hit 1.0 at our white point. We wouldn't have do this if we 
    // skipped the overshoot part.

    // evaluate shoulder at the end of the curve
    float scale = EvalCurveSegment(m_segments[index * 3 + 2], 1.0f);
    float invScale = 1.0f / scale;

    // linear and toe have an offset of zero so we don't need to scale their offsets, as we do for the shoulder

    m_segments[index * 3].m_scaleY *= invScale;

    m_segments[index * 3 + 1].m_scaleY *= invScale;

    m_segments[index * 3 + 2].m_offsetY *= invScale;
    m_segments[index * 3 + 2].m_scaleY *= invScale;

    return dstCurve;
}

QVector<int> ToneMappingConfig::getToeSamples() {
    return sampleCurve(0);
}

QVector<int> ToneMappingConfig::getShoulderSamples() {
    return sampleCurve(2);
}

QVector<int> ToneMappingConfig::sampleCurve(int segmentIndex) {

    if (!(segmentIndex == 0 || segmentIndex == 2)) {
        QVector<int> ret;
        return ret; // should never get here
    }
    
    int start = ToneMapAndResample::globalMode ? 0 : 1;
    int end = ToneMapAndResample::globalMode ? 1 : 4;

    QVector<int> samples;

    for (int i = start; i < end; i++) {

        CurveSegment segment = ToneMapAndResample::m_segments[i * 3 + segmentIndex];

        float lowerBound;
        float upperBound;

        if (segmentIndex == 0) {
            lowerBound = 0.0;
            upperBound = ToneMapAndResample::fullCurves[i].m_x0;
        }
        else if (segmentIndex == 2) {
            lowerBound = ToneMapAndResample::fullCurves[i].m_x1;
            upperBound = ToneMapAndResample::fullCurves[i].m_W;
        }

        float stepSize = (upperBound - lowerBound) / ((segmentIndex == 0 ? toeResolution : shoulderResolution) - 1);

        for (int i = 0; i < (segmentIndex == 0 ? toeResolution : shoulderResolution); i++) {
            float eval = EvalCurveSegment(segment, lowerBound + i * stepSize);
            float evalLarger = eval * dataResolution;
            int sample = (int)evalLarger;
            samples.append(sample);
        }
    }

    return samples;
}

QVector<int> ToneMappingConfig::getToeEnd() {
    QVector<int> ret;
    for (int i = 0; i < 4; i++) {
        ret.append(ToneMapAndResample::fullCurves[i].m_x0 * dataResolution);
    }
    return ret;
}

QVector<int> ToneMappingConfig::getShoulderStart() {
    QVector<int> ret;
    for (int i = 0; i < 4; i++) {
        ret.append(ToneMapAndResample::fullCurves[i].m_x1 * dataResolution);
    }
    return ret;
}

QVector<int> ToneMappingConfig::getShoulderEnd() {
    QVector<int> ret;
    for (int i = 0; i < 4; i++) {
        ret.append(ToneMapAndResample::fullCurves[i].m_W * dataResolution);
    }
    return ret;
}
