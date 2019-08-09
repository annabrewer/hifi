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
    Q_PROPERTY(float exposure MEMBER exposure WRITE setExposure );
    Q_PROPERTY(int curve MEMBER curve WRITE setCurve);

    Q_PROPERTY(QVector<qreal> toeStrength MEMBER toeStrength WRITE setToeStrength READ getToeStrength);
    Q_PROPERTY(QVector<qreal> toeLength MEMBER toeLength WRITE setToeLength READ getToeLength);
    Q_PROPERTY(QVector4D shoulderStrength MEMBER shoulderStrength WRITE setShoulderStrength READ getShoulderStrength);
    Q_PROPERTY(QVector4D shoulderLength MEMBER shoulderLength WRITE setShoulderLength READ getShoulderLength);
    Q_PROPERTY(QVector4D shoulderAngle MEMBER shoulderAngle WRITE setShoulderAngle READ getShoulderAngle);
    Q_PROPERTY(QVector4D gamma MEMBER gamma WRITE setGamma READ getGamma);

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
    glm::vec4 gamma = glm::vec4(0.5, 0.5, 0.5, 0.5);

    void setToeStrength(QVector<qreal> newToeStrength) { toeStrength = glm::ivec4(newToeStrength.at(0), newToeStrength.at(1), newToeStrength.at(2), newToeStrength.at(3)); emit dirty(); }
    QVector<qreal> getToeStrength() { QVector<qreal> ret{ toeStrength.x, toeStrength.y, toeStrength.z, toeStrength.w }; return ret;  }

    void setToeLength(QVector<qreal> newToeLength) { toeLength = glm::ivec4(newToeLength.at(0), newToeLength.at(1), newToeLength.at(2), newToeLength.at(3)); emit dirty(); }
    QVector<qreal> getToeLength() { QVector<qreal> ret{ toeLength.x, toeLength.y, toeLength.z, toeLength.w }; return ret; }

    void setShoulderLength(QVector4D newShoulderLength) { shoulderLength = glm::ivec4(newShoulderLength.x(), newShoulderLength.y(), newShoulderLength.z(), newShoulderLength.w()); emit dirty(); }
    QVector4D getShoulderLength() { return QVector4D(shoulderLength.x, shoulderLength.y, shoulderLength.z, shoulderLength.w); }

    void setShoulderStrength(QVector4D newShoulderStrength) { shoulderStrength = glm::ivec4(newShoulderStrength.x(), newShoulderStrength.y(), newShoulderStrength.z(), newShoulderStrength.w()); emit dirty(); }
    QVector4D getShoulderStrength() { return QVector4D(shoulderStrength.x, shoulderStrength.y, shoulderStrength.z, shoulderStrength.w); }

    void setShoulderAngle(QVector4D newShoulderAngle) { shoulderAngle = glm::ivec4(newShoulderAngle.x(), newShoulderAngle.y(), newShoulderAngle.z(), newShoulderAngle.w()); emit dirty(); }
    QVector4D getShoulderAngle() { return QVector4D(shoulderAngle.x, shoulderAngle.y, shoulderAngle.z, shoulderAngle.w); }

    void setGamma(QVector4D newGamma) { gamma = glm::ivec4(newGamma.x(), newGamma.y(), newGamma.z(), newGamma.w()); emit dirty(); }
    QVector4D getGamma() { return QVector4D(gamma.x, gamma.y, gamma.z, gamma.w); }

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

    //QVector<int> sampleCurve(int segmentIndex, float lowerBound, float upperBound);
    QVector<int> ToneMappingConfig::sampleCurve(int segmentIndex);

signals:
    void dirty();
};

class ToneMapAndResample {
public:
    ToneMapAndResample();
    virtual ~ToneMapAndResample() {}

    void render(RenderArgs* args, const gpu::TexturePointer& lightingBuffer, gpu::FramebufferPointer& destinationBuffer);

    void setExposure(float exposure);
    float getExposure() const { return pow(_parametersBuffer.get<Parameters>()._globals._twoPowExposure, 0.5); }

    void setToneCurve(ToneCurve curve);
    ToneCurve getToneCurve() const { return (ToneCurve)_parametersBuffer.get<Parameters>()._globals._toneCurve; }

    void setToeStrength(glm::ivec4 strength);
    //glm::vec4 getToeStrength() const { return userParams.m_toeStrength; }

    void setToeLength(glm::ivec4 length);
    //glm::vec4 getToeLength() const { return userParams.m_toeLength; }

    void setShoulderStrength(glm::ivec4 strength);
    //glm::vec4 getShoulderStrength() const { return userParams.m_shoulderStrength; }

    void setShoulderLength(glm::ivec4 length);
    //glm::vec4 getShoulderLength() const { return userParams.m_shoulderLength; }

    void setShoulderAngle(glm::ivec4 angle);
    //glm::vec4 getShoulderAngle() const { return userParams.m_shoulderAngle; }

    void setGamma(glm::ivec4 gamma);
    //glm::vec4 getGamma() const { return userParams.m_gamma; }

    // Inputs: lightingFramebuffer, destinationFramebuffer
    using Input = render::VaryingSet2<gpu::FramebufferPointer, gpu::FramebufferPointer>;
    using Output = gpu::FramebufferPointer;
    using Config = ToneMappingConfig;
    using JobModel = render::Job::ModelIO<ToneMapAndResample, Input, Output, Config>;

    void configure(const Config& config);
    void run(const render::RenderContextPointer& renderContext, const Input& input, Output& output);

    CurveParamsDirect CalcDirectParamsFromUser(const CurveParamsUser srcParams);

    FullCurve CreateCurve(const CurveParamsDirect srcParams);

    static CurveSegment m_segments[9];
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

    // Class describing the uniform buffer with all the parameters common to the tone mapping shaders
    /*
    typedef struct GlobalParameters {
        int _toneCurve = (int)ToneCurve::Gamma22;
        int _s00;
        int _s01;
        int _s02;

        float _exposure = 0.0f;
        float _twoPowExposure = 1.0f;
        float _s10;
        float _s11;
    } GlobalParams;

    typedef struct CurveParameters {
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
    } CurveParams;

    struct Parameters {
        GlobalParams _globals;
        CurveParams _redParams;
        CurveParams _greenParams;
        CurveParams _blueParams;
    };
    */

    typedef gpu::BufferView UniformBufferView;
    gpu::BufferView _parametersBuffer;

    void init();
};

#endif // hifi_ToneMapAndResample_h
