//
//  ToneMapAndResample.h
//  libraries/render-utils/src
//
//  Created by Anna Brewer on 7/3/19.
//  Copyright 2019 High Fidelity, Inc.
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

struct CurveParamsUser
{
    float m_toeStrength = 0.5; // as a ratio
    float m_toeLength = 0.5; // as a ratio
    float m_shoulderStrength = 2.0; // as a ratio
    float m_shoulderLength = 0.5; // in F stops
    float m_shoulderAngle = 1.0; // as a ratio

    float m_gamma = 2.2;
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
    Q_PROPERTY(float exposure MEMBER exposure WRITE setExposure );
    Q_PROPERTY(int curve MEMBER curve WRITE setCurve);
    Q_PROPERTY(float toeStrength MEMBER toeStrength WRITE setToeStrength);
    Q_PROPERTY(float toeLength MEMBER toeLength WRITE setToeLength);
    Q_PROPERTY(float shoulderStrength MEMBER shoulderStrength WRITE setShoulderStrength);
    Q_PROPERTY(float shoulderLength MEMBER shoulderLength WRITE setShoulderLength);
    Q_PROPERTY(float shoulderAngle MEMBER shoulderAngle WRITE setShoulderAngle);
    Q_PROPERTY(float gamma MEMBER gamma WRITE setGamma);
    Q_PROPERTY(QVector<int> toeSamples MEMBER toeSamples READ getToeSamples);
    Q_PROPERTY(QVector<int> shoulderSamples MEMBER shoulderSamples READ getShoulderSamples);
    Q_PROPERTY(QVector<int> toeEnd MEMBER toeEnd READ getToeEnd);
    Q_PROPERTY(QVector<int> shoulderStart MEMBER shoulderStart READ getShoulderStart);
    Q_PROPERTY(QVector<int> shoulderEnd MEMBER shoulderEnd READ getShoulderEnd);
    Q_PROPERTY(float dataResolution MEMBER dataResolution READ getDataResolution);

public:
    ToneMappingConfig() : render::Job::Config(true) {}

    void setExposure(float newExposure) { exposure = newExposure; emit dirty(); }
    void setCurve(int newCurve) { curve = std::max((int)ToneCurve::None, std::min((int)ToneCurve::Piecewise, newCurve)); emit dirty(); }
    void setToeStrength(float newToeStrength) { toeStrength = newToeStrength; emit dirty(); }
    void setToeLength(float newToeLength) { toeLength = newToeLength; emit dirty(); }
    void setShoulderStrength(float newShoulderStrength) { shoulderStrength = newShoulderStrength; emit dirty(); }
    void setShoulderLength(float newShoulderLength) { shoulderLength = newShoulderLength; emit dirty(); }
    void setShoulderAngle(float newShoulderAngle) { shoulderAngle = newShoulderAngle; emit dirty(); }
    void setGamma(float newGamma) { gamma = newGamma; emit dirty(); }

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

    int dataResolution{ 10000 };
    float getDataResolution() { return dataResolution; }

    float exposure{ 0.0f };
    float toeStrength{ 0.5f };
    float toeLength{ 0.5f };

    float shoulderStrength{ 0.5f };
    float shoulderLength{ 0.5f };
    float shoulderAngle{ 1.0f };
    float gamma{ 2.2f };

    float x0{ 0.0f };
    float y0{ 0.0f };
    float x1{ 1.0f };
    float y1{ 1.0f };

    int curve{ (int)ToneCurve::Gamma22 };

    QVector<int> sampleCurve(int segmentIndex, float lowerBound, float upperBound);

signals:
    void dirty();
};

class ToneMapAndResample {
public:
    ToneMapAndResample();
    virtual ~ToneMapAndResample() {}

    void render(RenderArgs* args, const gpu::TexturePointer& lightingBuffer, gpu::FramebufferPointer& destinationBuffer);

    void setExposure(float exposure);
    float getExposure() const { return pow(_parametersBuffer.get<Parameters>()._twoPowExposure, 0.5); }

    void setToneCurve(ToneCurve curve);
    ToneCurve getToneCurve() const { return (ToneCurve)_parametersBuffer.get<Parameters>()._toneCurve; }

    void setToeStrength(float strength);
    float getToeStrength() const { return userParams.m_toeStrength; }

    void setToeLength(float strength);
    float getToeLength() const { return userParams.m_toeLength; }

    void setShoulderStrength(float strength);
    float getShoulderStrength() const { return userParams.m_shoulderStrength; }

    void setShoulderLength(float strength);
    float getShoulderLength() const { return userParams.m_shoulderLength; }

    void setShoulderAngle(float strength);
    float getShoulderAngle() const { return userParams.m_shoulderAngle; }

    void setGamma(float strength);
    float getGamma() const { return userParams.m_gamma; }

    // Inputs: lightingFramebuffer, destinationFramebuffer
    using Input = render::VaryingSet2<gpu::FramebufferPointer, gpu::FramebufferPointer>;
    using Output = gpu::FramebufferPointer;
    using Config = ToneMappingConfig;
    using JobModel = render::Job::ModelIO<ToneMapAndResample, Input, Output, Config>;

    void configure(const Config& config);
    void run(const render::RenderContextPointer& renderContext, const Input& input, Output& output);

    CurveParamsDirect CalcDirectParamsFromUser(const CurveParamsUser srcParams);

    FullCurve CreateCurve(const CurveParamsDirect srcParams);

    static CurveSegment m_segments[3];
    static FullCurve fullCurve;

protected:
    static gpu::PipelinePointer _pipeline;
    static gpu::PipelinePointer _mirrorPipeline;
    static gpu::PipelinePointer _piecewisePipeline;
    static gpu::PipelinePointer _piecewiseMirrorPipeline;

    gpu::FramebufferPointer _destinationFrameBuffer;

    float _factor{ 2.0f };

    gpu::FramebufferPointer getResampledFrameBuffer(const gpu::FramebufferPointer& sourceFramebuffer);

    bool _dirty;

    void setCurveParams(FullCurve curve);

    CurveParamsUser userParams;

private:
    gpu::PipelinePointer _blitLightBuffer;

    // Class describing the uniform buffer with all the parameters common to the tone mapping shaders
    class Parameters {
    public:
        float _shoulderOffsetX;
        float _shoulderOffsetY;
        float _shoulderLnA;
        float _shoulderB;

        float _toeLnA;
        float _toeB;
        float _linearLnA;
        float _linearB;

        float _linearOffsetX;
        float _twoPowExposure = 1.0f;
        float _fullCurveW;
        float _fullCurveInvW;

        float _fullCurveX0;
        float _fullCurveY0;
        float _fullCurveX1;
        float _fullCurveY1;

        float _toeScaleY;
        float _linearScaleY;
        float _shoulderScaleY;
        float _s3;

        int _toneCurve = (int)ToneCurve::Gamma22;

        Parameters() {}
    };

    typedef gpu::BufferView UniformBufferView;
    gpu::BufferView _parametersBuffer;

    void init();
};

#endif // hifi_ToneMapAndResample_h
