//
//  ToneMapAndResample.h
//  libraries/render-utils/src
//
//  Created by Anna Brewer on 7/3/19.
//  Copyright 2019 High Fidelity, Inc.
//
//  Piecewise tone curve was adapted from John Hable's sample code:
//  https://github.com/johnhable/fw-public.git
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ToneMapAndResample_h
#define hifi_ToneMapAndResample_h

#include <DependencyManager.h>
#include <NumericalConstants.h>

#include <gpu/Resource.h>
#include <gpu/Pipeline.h>
#include <render/Forward.h>
#include <render/DrawTask.h>

#include "ToneMappingCommon.h"

enum class ToneCurve {
    // Different tone curve available
    None,
    Gamma22,
    Reinhard,
    Filmic,
    Piecewise
};

enum class ChannelMode {
    // Different tone curve available
    Global,
    Spectral
};

struct CurveParamsUser
{
    float m_toeStrength = 0.5; // as a ratio
    float m_toeLength = 0.5; // as a ratio
    float m_shoulderStrength = 2.0; // as a ratio
    float m_shoulderLength = 0.5; // in F stops
    float m_shoulderAngle = 1.0; // as a ratio

    float m_gamma = 2.2;
};

struct AllCurveParamsUser
{
    CurveParamsUser _globalUserParams;
    CurveParamsUser _redUserParams;
    CurveParamsUser _greenUserParams;
    CurveParamsUser _blueUserParams;
};

struct CurveParamsDirect {
    float m_x0;
    float m_y0;
    float m_x1;
    float m_y1;

    float m_W;

    float m_overshootX;
    float m_overshootY;

    float m_gamma;
};

class ToneMappingConfig : public render::Job::Config {
    Q_OBJECT
    Q_PROPERTY(float exposure MEMBER exposure WRITE setExposure);
    Q_PROPERTY(int curve MEMBER curve WRITE setCurve);

    Q_PROPERTY(float toeStrengthALL READ getToeStrengthALL WRITE setToeStrengthALL);
    Q_PROPERTY(float toeLengthALL READ getToeLengthALL WRITE setToeLengthALL);
    Q_PROPERTY(float shoulderStrengthALL READ getShoulderStrengthALL WRITE setShoulderStrengthALL);
    Q_PROPERTY(float shoulderLengthALL READ getShoulderLengthALL WRITE setShoulderLengthALL);
    Q_PROPERTY(float shoulderAngleALL READ getShoulderAngleALL WRITE setShoulderAngleALL);
    Q_PROPERTY(float gammaALL READ getGammaALL WRITE setGammaALL);

    Q_PROPERTY(float toeStrengthR READ getToeStrengthR WRITE setToeStrengthR);
    Q_PROPERTY(float toeLengthR READ getToeLengthR WRITE setToeLengthR);
    Q_PROPERTY(float shoulderStrengthR READ getShoulderStrengthR WRITE setShoulderStrengthR);
    Q_PROPERTY(float shoulderLengthR READ getShoulderLengthR WRITE setShoulderLengthR);
    Q_PROPERTY(float shoulderAngleR READ getShoulderAngleR WRITE setShoulderAngleR);
    Q_PROPERTY(float gammaR READ getGammaR WRITE setGammaR);

    Q_PROPERTY(float toeStrengthG READ getToeStrengthG WRITE setToeStrengthG);
    Q_PROPERTY(float toeLengthG READ getToeLengthG WRITE setToeLengthG);
    Q_PROPERTY(float shoulderStrengthG READ getShoulderStrengthG WRITE setShoulderStrengthG);
    Q_PROPERTY(float shoulderLengthG READ getShoulderLengthG WRITE setShoulderLengthG);
    Q_PROPERTY(float shoulderAngleG READ getShoulderAngleG WRITE setShoulderAngleG);
    Q_PROPERTY(float gammaG READ getGammaG WRITE setGammaG);

    Q_PROPERTY(float toeStrengthB READ getToeStrengthB WRITE setToeStrengthB);
    Q_PROPERTY(float toeLengthB READ getToeLengthB WRITE setToeLengthB);
    Q_PROPERTY(float shoulderStrengthB READ getShoulderStrengthB WRITE setShoulderStrengthB);
    Q_PROPERTY(float shoulderLengthB READ getShoulderLengthB WRITE setShoulderLengthB);
    Q_PROPERTY(float shoulderAngleB READ getShoulderAngleB WRITE setShoulderAngleB);
    Q_PROPERTY(float gammaB READ getGammaB WRITE setGammaB);

    Q_PROPERTY(QVector<int> toeSamples MEMBER toeSamples READ getToeSamples);
    Q_PROPERTY(QVector<int> shoulderSamples MEMBER shoulderSamples READ getShoulderSamples);
    Q_PROPERTY(QVector<int> toeEnd MEMBER toeEnd READ getToeEnd);
    Q_PROPERTY(QVector<int> shoulderStart MEMBER shoulderStart READ getShoulderStart);
    Q_PROPERTY(QVector<int> shoulderEnd MEMBER shoulderEnd READ getShoulderEnd);
    Q_PROPERTY(float dataResolution MEMBER dataResolution READ getDataResolution);
    Q_PROPERTY(int channelMode MEMBER channelMode);

public:
    ToneMappingConfig() : render::Job::Config(true) {}

    void setExposure(float newExposure) { exposure = newExposure; emit dirty(); }
    void setCurve(int newCurve) { curve = std::max((int)ToneCurve::None, std::min((int)ToneCurve::Piecewise, newCurve)); emit dirty(); }

    glm::vec4 toeStrength = glm::vec4(0.5, 0.5, 0.5, 0.5);
    glm::vec4 toeLength = glm::vec4(0.5, 0.5, 0.5, 0.5);
    glm::vec4 shoulderStrength = glm::vec4(0.5, 0.5, 0.5, 0.5);
    glm::vec4 shoulderLength = glm::vec4(0.5, 0.5, 0.5, 0.5);
    glm::vec4 shoulderAngle = glm::vec4(0.5, 0.5, 0.5, 0.5);
    glm::vec4 gamma = glm::vec4(2.2, 2.2, 2.2, 2.2);

    void setToeStrengthALL(float newToeStrength) { toeStrength.x = newToeStrength; emit dirty(); }
    float getToeStrengthALL() { return toeStrength.x; }

    void setToeLengthALL(float newToeLength) { toeLength.x = newToeLength; emit dirty(); }
    float getToeLengthALL() { return toeLength.x; }

    void setShoulderStrengthALL(float newShoulderStrength) { shoulderStrength.x = newShoulderStrength; emit dirty(); }
    float getShoulderStrengthALL() { return shoulderStrength.x; }

    void setShoulderLengthALL(float newShoulderLength) { shoulderLength.x = newShoulderLength;  emit dirty(); }
    float getShoulderLengthALL() { return shoulderLength.x; }

    void setShoulderAngleALL(float newShoulderAngle) { shoulderAngle.x = newShoulderAngle; emit dirty(); }
    float getShoulderAngleALL() { return shoulderAngle.x; }

    void setGammaALL(float newGamma) { gamma.x = newGamma; emit dirty(); }
    float getGammaALL() { return gamma.x; }


    void setToeStrengthR(float newToeStrength) { toeStrength.y = newToeStrength; emit dirty(); }
    float getToeStrengthR() { return toeStrength.y; }

    void setToeLengthR(float newToeLength) { toeLength.y = newToeLength; emit dirty(); }
    float getToeLengthR() { return toeLength.y; }

    void setShoulderStrengthR(float newShoulderStrength) { shoulderStrength.y = newShoulderStrength; emit dirty(); }
    float getShoulderStrengthR() { return shoulderStrength.y; }

    void setShoulderLengthR(float newShoulderLength) { shoulderLength.y = newShoulderLength;  emit dirty(); }
    float getShoulderLengthR() { return shoulderLength.y; }

    void setShoulderAngleR(float newShoulderAngle) { shoulderAngle.y = newShoulderAngle; emit dirty(); }
    float getShoulderAngleR() { return shoulderAngle.y; }

    void setGammaR(float newGamma) { gamma.y = newGamma; emit dirty(); }
    float getGammaR() { return gamma.y; }


    void setToeStrengthG(float newToeStrength) { toeStrength.z = newToeStrength; emit dirty(); }
    float getToeStrengthG() { return toeStrength.z; }

    void setToeLengthG(float newToeLength) { toeLength.z = newToeLength; emit dirty(); }
    float getToeLengthG() { return toeLength.z; }

    void setShoulderStrengthG(float newShoulderStrength) { shoulderStrength.z = newShoulderStrength; emit dirty(); }
    float getShoulderStrengthG() { return shoulderStrength.z; }

    void setShoulderLengthG(float newShoulderLength) { shoulderLength.z = newShoulderLength;  emit dirty(); }
    float getShoulderLengthG() { return shoulderLength.z; }

    void setShoulderAngleG(float newShoulderAngle) { shoulderAngle.z = newShoulderAngle; emit dirty(); }
    float getShoulderAngleG() { return shoulderAngle.z; }

    void setGammaG(float newGamma) { gamma.z = newGamma; emit dirty(); }
    float getGammaG() { return gamma.z; }


    void setToeStrengthB(float newToeStrength) { toeStrength.w = newToeStrength; emit dirty(); }
    float getToeStrengthB() { return toeStrength.w; }

    void setToeLengthB(float newToeLength) { toeLength.w = newToeLength; emit dirty(); }
    float getToeLengthB() { return toeLength.w; }

    void setShoulderStrengthB(float newShoulderStrength) { shoulderStrength.w = newShoulderStrength; emit dirty(); }
    float getShoulderStrengthB() { return shoulderStrength.w; }

    void setShoulderLengthB(float newShoulderLength) { shoulderLength.w = newShoulderLength;  emit dirty(); }
    float getShoulderLengthB() { return shoulderLength.w; }

    void setShoulderAngleB(float newShoulderAngle) { shoulderAngle.w = newShoulderAngle; emit dirty(); }
    float getShoulderAngleB() { return shoulderAngle.w; }

    void setGammaB(float newGamma) { gamma.w = newGamma; emit dirty(); }
    float getGammaB() { return gamma.w; }

    QVector<int> toeSamples;
    QVector<int> getToeSamples();

    QVector<int> shoulderSamples;
    QVector<int> getShoulderSamples();

    QVector<int> toeEnd;
    QVector<int> getToeEnd();

    QVector<int> shoulderStart;
    QVector<int> getShoulderStart();

    QVector<int> shoulderEnd;
    QVector<int> getShoulderEnd();

    int toeResolution{ 50 };
    int shoulderResolution{ 50 };

    float dataResolution{ 10000.0f };
    float getDataResolution() { return dataResolution; }

    float exposure{ 0.0f };

    float x0{ 0.0f };
    float y0{ 0.0f };
    float x1{ 1.0f };
    float y1{ 1.0f };

    int curve{ (int)ToneCurve::Gamma22 };

    int channelMode{ (int)ChannelMode::Global };

    QVector<int> ToneMappingConfig::sampleCurve(int segmentIndex);

signals:
    void dirty();
};

class ToneMapAndResample {
public:
    ToneMapAndResample();
    virtual ~ToneMapAndResample() {}

    void render(RenderArgs* args, const gpu::TexturePointer& lightingBuffer, gpu::FramebufferPointer& destinationBuffer);

    void setChannelMode(ChannelMode mode);

    void setExposure(float exposure);
    float getExposure() const { return pow(_parametersBuffer.get<Parameters>()._globals._twoPowExposure, 0.5); }

    void setToneCurve(ToneCurve curve);
    ToneCurve getToneCurve() const { return (ToneCurve)_parametersBuffer.get<Parameters>()._globals._toneCurve; }

    void setToeStrength(glm::vec4 strength);

    void setToeLength(glm::vec4 length);

    void setShoulderStrength(glm::vec4 strength);

    void setShoulderLength(glm::vec4 length);

    void setShoulderAngle(glm::vec4 angle);

    void setGamma(glm::vec4 gamma);

    // Inputs: lightingFramebuffer, destinationFramebuffer
    using Input = render::VaryingSet2<gpu::FramebufferPointer, gpu::FramebufferPointer>;
    using Output = gpu::FramebufferPointer;
    using Config = ToneMappingConfig;
    using JobModel = render::Job::ModelIO<ToneMapAndResample, Input, Output, Config>;

    void configure(const Config& config);
    void run(const render::RenderContextPointer& renderContext, const Input& input, Output& output);

    CurveParamsDirect CalcDirectParamsFromUser(const CurveParamsUser srcParams);

    FullCurve CreateCurve(const CurveParamsDirect srcParams, int index);

    static CurveSegment m_segments[12];
    static FullCurve fullCurves[4];

    static bool globalMode;

protected:
    static gpu::PipelinePointer _pipeline;
    static gpu::PipelinePointer _mirrorPipeline;
    static gpu::PipelinePointer _piecewisePipeline;
    static gpu::PipelinePointer _piecewiseMirrorPipeline;

    gpu::FramebufferPointer _destinationFrameBuffer;

    float _factor{ 2.0f };

    gpu::FramebufferPointer getResampledFrameBuffer(const gpu::FramebufferPointer& sourceFramebuffer);

    bool _dirty;

    void setCurveParams();

    AllCurveParamsUser userParams;

private:
    gpu::PipelinePointer _blitLightBuffer;

    typedef gpu::BufferView UniformBufferView;
    gpu::BufferView _parametersBuffer;

    void init();
};

#endif // hifi_ToneMapAndResample_h
