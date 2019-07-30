// "WaveVR SDK 
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#define  LOG_TAG    "hellovr_jni"
#include <jni.h>
#include <log.h>
#include <Context.h>
#include <hellovr.h>
#include <unistd.h>
#include <wvr/wvr.h>

#include <android/log.h>

#include <QtGui/QGuiApplication>
#include <QtCore/QTimer>
#include <QtCore/QFileInfo>
#include <QtAndroidExtras/QAndroidJniObject>

#include <Trace.h>

#include <QtGui/QWindow>

// DEBUG can be disabled when the Activity ask.
extern bool gDebug;
extern bool gDebugOld;
extern bool gMsaa;
extern bool gScene;
extern bool gSceneOld;

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    if (!message.isEmpty()) {
        const char* local = message.toStdString().c_str();
        switch (type) {
            case QtDebugMsg:
                __android_log_write(ANDROID_LOG_DEBUG, "Interface", local);
                break;
            case QtInfoMsg:
                __android_log_write(ANDROID_LOG_INFO, "Interface", local);
                break;
            case QtWarningMsg:
                __android_log_write(ANDROID_LOG_WARN, "Interface", local);
                break;
            case QtCriticalMsg:
                __android_log_write(ANDROID_LOG_ERROR, "Interface", local);
                break;
            case QtFatalMsg:
            default:
                __android_log_write(ANDROID_LOG_FATAL, "Interface", local);
                abort();
        }
    }
}

int main(int argc, char *argv[]) {
    //setupHifiApplication("gpuFramePlayer");
    QGuiApplication app(argc, argv);
    auto oldMessageHandler = qInstallMessageHandler(messageHandler);
    DependencyManager::set<tracing::Tracer>();
    QWindow window;
    QTimer::singleShot(10, []{
        __android_log_write(ANDROID_LOG_WARN, "QQQ", "notifyLoadComplete");
        //AndroidHelper::instance().notifyLoadComplete();
    });
    __android_log_write(ANDROID_LOG_WARN, "QQQ", "Exec");
    app.exec();
    __android_log_write(ANDROID_LOG_WARN, "QQQ", "Exec done");
    qInstallMessageHandler(oldMessageHandler);
}

int render(int argc, char *argv[]) {
    LOGENTRY();
    LOGI("HelloVR main, new MainApplication ");
    MainApplication *app = new MainApplication();
    LOGI("HelloVR main, start call app->initVR()");
    if (!app->initVR()) {
        LOGW("HelloVR main, initVR fail,start call app->shutdownVR()");
        app->shutdownVR();
        return 1;
    }

    if (!app->initGL()) {
        LOGW("HelloVR main, initGL failed, start call app->shutdownVR()");
        app->shutdownGL();
        app->shutdownVR();
        return 1;
    }

    while (1) {
        if (app->handleInput())
            break;

        if (app->renderFrame()) {
            LOGE("Unknown render error. Quit.");
            break;
        }

        app->updateHMDMatrixPose();
    }

    app->shutdownGL();
    app->shutdownVR();

    return 0;
}

extern "C" {
    JNIEXPORT void JNICALL Java_io_highfidelity_frameplayer_ViveFramePlayer_init(JNIEnv * env, jobject act, jobject am);
    JNIEXPORT void JNICALL Java_io_highfidelity_frameplayer_ViveFramePlayer_setFlag(JNIEnv * env, jclass clazz, jint flag);
};



JNIEXPORT void JNICALL Java_io_highfidelity_frameplayer_ViveFramePlayer_init(JNIEnv * env, jobject activityInstance, jobject assetManagerInstance) {
    LOGI("MainActivity_init: call  Context::getInstance()->init");
    Context::getInstance()->init(env, assetManagerInstance);
    LOGI("register WVR main when library loading");
    //WVR_RegisterMain(main);
}

JNIEXPORT void JNICALL Java_io_highfidelity_frameplayer_ViveFramePlayer_setFlag(JNIEnv * env, jclass clazz, jint flag) {
    // Flags are defined in java Activity.
    gDebug = (flag & 0x1) != 0;
    LOGD("gDebug = %d", gDebug ? 1 : 0);
    gMsaa = (flag & 0x2) != 0;
    LOGD("gMsaa = %d", gMsaa ? 1 : 0);
    gScene = (flag & 0x4) != 0;
    LOGD("gScene = %d", gScene ? 1 : 0);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    Context *ctx = new Context(vm);
    if (!ctx) return JNI_VERSION_1_6;

    return JNI_VERSION_1_6;
}

jint JNI_OnUnLoad(JavaVM* vm, void* reserved) {
    delete Context::getInstance();
}

