//
// Created by Egor Orachyov on 04.07.2018.
//

#include <Render/RenderDriver.h>
#include "Render/GLRenderSystem.h"

#include "Pipeline/GLDeferredShading.h"
#include "Pipeline/GLShadowMap.h"
#include "Pipeline/GLAmbientOcclusion.h"
#include "Pipeline/GLLightShafts.h"
#include "Pipeline/GLPhongDeferred.h"
#include "Pipeline/GLPhongShadow.h"
#include "Pipeline/GLPhongModel.h"
#include "Pipeline/GLToneMap.h"
#include "Pipeline/GLGaussianBloom.h"
#include "Pipeline/GLScreenRender.h"
#include "Misc/Buffers.h"
#include "Misc/Platform.h"
#include "Misc/Delete.h"

namespace Berserk
{

    GLRenderSystem::GLRenderSystem()
    {
        mRenderCamera = nullptr;
        mGlobalLight = nullptr;
        mAmbientLight = Vector3f(0);
    }

    GLRenderSystem::~GLRenderSystem()
    {
        destroy();
    }

    void GLRenderSystem::init(const ConfigTable& table)
    {
        printf("Size of gl render system %lu\n", sizeof(GLRenderSystem));

        if (!glfwInit())
        {
            ERROR("Cannot initialize GLFW library");
            return;
        }

        if (table.getUInt32("MSAA"))
        { glfwWindowHint(GLFW_SAMPLES, table.getUInt32("MSAAValue")); }
        else
        { glfwWindowHint(GLFW_SAMPLES, 0); }

        #ifdef PLATFORM_MAC
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        #else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #endif

        mWindowHandle = glfwCreateWindow(table.getUInt32("WindowWidth"),
                                         table.getUInt32("WindowHeight"),
                                         table.getChar("ApplicationName"),
                                         nullptr, nullptr);

        if (mWindowHandle == nullptr)
        {
            ERROR("Cannot create glfw window");
            return;
        }

        if (table.getUInt32("WindowMaximize"))
        { glfwMaximizeWindow(mWindowHandle); }

        glfwMakeContextCurrent(mWindowHandle);
        glfwGetWindowPos(mWindowHandle, &mWindowPosX, &mWindowPosY);
        glfwGetWindowSize(mWindowHandle, &mWindowWidth, &mWindowHeight);
        glfwGetFramebufferSize(mWindowHandle, &mPixelWindowWidth, &mPixelWindowHeight);

        mOldPixelWindowWidth = mPixelWindowWidth;
        mOldPixelWindowHeight = mPixelWindowHeight;

        if (glewInit() != GLEW_OK)
        {
            ERROR("Cannot initialize GLEW library");
            return;
        }

        mUseToneMap = (bool)table.getUInt32("ToneMap");
        mUseGaussianBloom = (bool)table.getUInt32("GaussianBloom");
        mUseSSAO = (bool)table.getUInt32("SSAO");
        mUseLightShafts = false;

        mStageIn = nullptr;
        mStageOut = nullptr;

        mWindowName = CStaticString(table.getChar("ApplicationName"));
        mRenderCamera = nullptr;
        mGlobalLight = nullptr;
        mAmbientLight = Vector3f(0.1);
        mClearColor = Vector4f(0.0);
        mBorderColor = Vector3f(0.0);
        mExposure = table.getFloat32("Exposure");
        mLuminanceThresh = table.getFloat32("LuminanceThresh");
        mGammaCorrection = (FLOAT32)1 / table.getFloat32("GammaCorrection");

        mIsReSized = false;
        mShadowQuality = ShadowInfo::SI_QUALITY_MEDIUM;
        mShadowMapSize = ShadowInfo::SI_MAP_SIZE_QUALITY_MEDIUM;

        mSSAOScreenBufferPart = table.getFloat32("SSAOBufferScale");
        mSSAORadius = table.getFloat32("SSAORadius");;

        mLightShaftsBufferPart = table.getFloat32("LightShaftsBufferScale");
        mLightShaftsExposure = table.getFloat32("LightShaftsExposure");
        mLightShaftsDecay = table.getFloat32("LightShaftsDecay");

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_SPOT_SHADOW_SOURCES; i++)
            mSpotDepthMap[i].create(mShadowMapSize, mShadowMapSize, ShadowInfo::SI_SPOT_MAP_SLOT0 + i);

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_POINT_SHADOW_SOURCES; i++)
            mPointDepthMap[i].create(mShadowMapSize, ShadowInfo::SI_POINT_MAP_SLOT0 + i);

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_DIR_SHADOW_SOURCES; i++)
            mDirectionalDepthMap[i].create(mShadowMapSize, mShadowMapSize, ShadowInfo::SI_DIR_MAP_SLOT0 + i);


        mSpotShadowSources.init(ShadowInfo::SI_MAX_SPOT_SHADOW_SOURCES);
        mPointShadowSources.init(ShadowInfo::SI_MAX_POINT_SHADOW_SOURCES);
        mDirectionalShadowSources.init(ShadowInfo::SI_MAX_DIR_SHADOW_SOURCES);

        mSpotShadowSources.lock();
        mPointShadowSources.lock();
        mDirectionalShadowSources.lock();

        mSpotLightSources.init(LightInfo::LI_MAX_SPOT_LIGHTS);
        mPointLightSources.init(LightInfo::LI_MAX_POINT_LIGHTS);
        mDirectionalLightSources.init(LightInfo::LI_MAX_DIRECTIONAL_LIGHTS);

        mSpotLightSources.lock();
        mPointLightSources.lock();
        mDirectionalLightSources.lock();

        mRenderNodeSources.init();
        mRenderNodeList.init();
        mScreenPlane.init();

        mDeferredStage = new GLDeferredShading();
        mDeferredStage->init();
        mShadowMapStage = new GLShadowMap();
        mShadowMapStage->init();
        mAmbientOcclusionStage = new GLAmbientOcclusion();
        mAmbientOcclusionStage->init();
        mLightShaftsStage = new GLLightShafts();
        mLightShaftsStage->init();
        mDeferredPhongShadowStage = new GLPhongDeferred();
        mDeferredPhongShadowStage->init();
        mPhongShadowStage = new GLPhongShadow();
        mPhongShadowStage->init();
        mPhongModelStage = new GLPhongModel();
        mPhongModelStage->init();
        mToneMapStage = new GLToneMap();
        mToneMapStage->init();
        mGaussianBloomStage = new GLGaussianBloom();
        mGaussianBloomStage->init();
        mScreenRenderStage = new GLScreenRender();
        mScreenRenderStage->init();

        mGBuffer.init((UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);
        mSSAOBuffer.create((UINT32)(mSSAOScreenBufferPart * mPixelWindowWidth),
                           (UINT32)(mSSAOScreenBufferPart * mPixelWindowHeight));

        mRGB32FBuffer1.init((UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);
        mRGB32FBuffer1.addTexture(GLInternalTextureFormat::GLTF_RGB32F, GLWrapping::GLW_CLAMP_TO_EDGE, GLFiltering::GLF_NEAREST, 0, 0);
        mRGB32FBuffer1.addDepthBuffer();
        mRGB32FBuffer1.setShaderAttachments();

        mRGB32FBuffer2.init((UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);
        mRGB32FBuffer2.addTexture(GLInternalTextureFormat::GLTF_RGB32F, GLWrapping::GLW_CLAMP_TO_EDGE, GLFiltering::GLF_NEAREST, 0, 0);
        mRGB32FBuffer2.addDepthBuffer();
        mRGB32FBuffer2.setShaderAttachments();

        getContextInfo();
    }

    void GLRenderSystem::destroy()
    {
        if (mWindowHandle)
        {
            glfwDestroyWindow(mWindowHandle);
            mWindowHandle = nullptr;
        }
        if (mDeferredStage)
        {
            mDeferredStage->destroy();
            SAFE_DELETE(mDeferredStage);
        }
        if (mShadowMapStage)
        {
            mShadowMapStage->destroy();
            SAFE_DELETE(mShadowMapStage);
        }
        if (mAmbientOcclusionStage)
        {
            mAmbientOcclusionStage->destroy();
            SAFE_DELETE(mAmbientOcclusionStage);
        }
        if (mLightShaftsStage)
        {
            mLightShaftsStage->destroy();
            SAFE_DELETE(mLightShaftsStage);
        }
        if (mDeferredPhongShadowStage)
        {
            mDeferredPhongShadowStage->destroy();
            SAFE_DELETE(mDeferredPhongShadowStage);
        }
        if (mPhongShadowStage)
        {
            mPhongShadowStage->destroy();
            SAFE_DELETE(mPhongShadowStage);
        }
        if (mPhongModelStage)
        {
            mPhongModelStage->destroy();
            SAFE_DELETE(mPhongModelStage);
        }
        if (mToneMapStage)
        {
            mToneMapStage->destroy();
            SAFE_DELETE(mToneMapStage);
        }
        if (mGaussianBloomStage)
        {
            mGaussianBloomStage->destroy();
            SAFE_DELETE(mGaussianBloomStage);
        }
        if (mScreenRenderStage)
        {
            mScreenRenderStage->destroy();
            SAFE_DELETE(mScreenRenderStage);
        }

        mGBuffer.destroy();
        mRGB32FBuffer1.destroy();
        mRGB32FBuffer2.destroy();

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_SPOT_SHADOW_SOURCES; i++)
            mSpotDepthMap[i].destroy();

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_POINT_SHADOW_SOURCES; i++)
            mPointDepthMap[i].destroy();

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_DIR_SHADOW_SOURCES; i++)
            mDirectionalDepthMap[i].destroy();

        glfwTerminate();
    }

    void GLRenderSystem::validate()
    {
        PUSH("GLRenderSystem: validation");

        if (mRenderCamera == nullptr)
        { PUSH("Render Camera is not attached"); }
        else
        { PUSH("Attached render camera with name %s", mRenderCamera->getName().getChars()); }
    }

    void GLRenderSystem::preMainLoop()
    {
        PUSH("GLRenderSystem: pre-main loop stage");
    }

    void GLRenderSystem::preUpdate()
    {
        glfwGetWindowPos(mWindowHandle, &mWindowPosX, &mWindowPosY);
        glfwGetWindowSize(mWindowHandle, &mWindowWidth, &mWindowHeight);
        glfwGetFramebufferSize(mWindowHandle, &mPixelWindowWidth, &mPixelWindowHeight);

        /////////////////////////
        ///       DEBUG       ///
        /////////////////////////

        static auto current = 0.0;
        auto elapsed = 0.0;

        auto tmp = glfwGetTime();
        const auto should = 1.0 / 30.0;

        elapsed = tmp - current;

        while (elapsed < should)
        {
            tmp = glfwGetTime();
            elapsed = tmp - current;
        }

        current = glfwGetTime();

        printf("FPS %2.1lf\n",1.0 / elapsed);
    }

    void GLRenderSystem::postUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(mWindowHandle);

        if (mOldPixelWindowWidth != mPixelWindowWidth || mOldPixelWindowHeight != mOldPixelWindowHeight)
        {
            PUSH("Was re-sized");

            mRGB32FBuffer1.destroy();
            mRGB32FBuffer1.init((UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);
            mRGB32FBuffer1.addTexture(GLInternalTextureFormat::GLTF_RGB32F, GLWrapping::GLW_CLAMP_TO_EDGE, GLFiltering::GLF_NEAREST, 0, 0);
            mRGB32FBuffer1.addDepthBuffer();
            mRGB32FBuffer1.setShaderAttachments();

            mRGB32FBuffer2.destroy();
            mRGB32FBuffer2.init((UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);
            mRGB32FBuffer2.addTexture(GLInternalTextureFormat::GLTF_RGB32F, GLWrapping::GLW_CLAMP_TO_EDGE, GLFiltering::GLF_NEAREST, 0, 0);
            mRGB32FBuffer2.addDepthBuffer();
            mRGB32FBuffer2.setShaderAttachments();

            mGBuffer.destroy();
            mGBuffer.init((UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);

            mSSAOBuffer.destroy();
            mSSAOBuffer.create((UINT32)(mSSAOScreenBufferPart * mPixelWindowWidth),
                               (UINT32)(mSSAOScreenBufferPart * mPixelWindowHeight));

            mRenderCamera->setViewport(0, 0, (UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);

            mOldPixelWindowWidth = mPixelWindowWidth;
            mOldPixelWindowHeight = mPixelWindowHeight;

            mIsReSized = true;
        }
        else
        {
            mIsReSized = false;
        }
    }

    void GLRenderSystem::postMainLoop()
    {
        PUSH("GLRenderSystem: post-main loop stage");
    }

    void GLRenderSystem::renderPass1()
    {
        if (mRenderCamera)
        {
            mStageIn = &mRGB32FBuffer1;
            mStageOut = &mRGB32FBuffer2;

            mDeferredStage->execute();
            mShadowMapStage->execute();

            if (mUseSSAO)
            {
                mAmbientOcclusionStage->execute();
            }

            swap();
            mDeferredPhongShadowStage->execute();

            if (mUseGaussianBloom)
            {
                swap();
                mGaussianBloomStage->execute();
            }

            if (mUseLightShafts)
            {
                swap();
                mLightShaftsStage->execute();
            }

            if (mUseToneMap)
            {
                swap();
                mToneMapStage->execute();
            }

            swap();
            mScreenRenderStage->execute();
        }
        else
        {
            PUSH("GLRenderSystem: Camera is not attached");
        }

        mSpotShadowSources.clean();
        mPointShadowSources.clean();
        mDirectionalShadowSources.clean();

        mSpotLightSources.clean();
        mPointLightSources.clean();
        mDirectionalLightSources.clean();

        mRenderNodeSources.clean();
    }

    void GLRenderSystem::renderPass2()
    {
        mSpotLightSources.clean();
        mPointLightSources.clean();
        mDirectionalLightSources.clean();
    }

    const CString& GLRenderSystem::getName() const
    {
        return mName;
    }

    const CString& GLRenderSystem::getRenderName() const
    {
        return mRenderName;
    }

    const CString& GLRenderSystem::getShadingLanguageName() const
    {
        return mShadingLanguage;
    }

    void GLRenderSystem::printContextInfo() const
    {
        // debug only feature

        const GLubyte * renderer = glGetString(GL_RENDERER);
        const GLubyte * vendor = glGetString(GL_VENDOR);
        const GLubyte * version = glGetString(GL_VERSION);
        const GLubyte * glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        GLint minor, major;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        OPEN_BLOCK("GL Context info");
        PUSH_BLOCK("GL Vendor            : %s", vendor);
        PUSH_BLOCK("GL Renderer          : %s", renderer);
        PUSH_BLOCK("GL Version (string)  : %s", version);
        PUSH_BLOCK("GL Version (integer) : %d.%d", major, minor);
        PUSH_BLOCK("GLSL Version         : %s", glslVersion);
        CLOSE_BLOCK();

        GLint numOfExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numOfExtensions);

        OPEN_BLOCK("GL Extensions");
        for(int i = 0; i < numOfExtensions; i++)
        {
            PUSH_BLOCK("%s", glGetStringi(GL_EXTENSIONS, i));
        }
        CLOSE_BLOCK();
    }

    void GLRenderSystem::enableToneMap(bool setIn)
    {
        mUseToneMap = setIn;
    }

    void GLRenderSystem::enableGaussianBloom(bool setIn)
    {
        mUseGaussianBloom = setIn;
    }

    void GLRenderSystem::enableSSAO(bool setIn)
    {
        mUseSSAO = setIn;
    }

    void GLRenderSystem::enableLightShafts(bool setIn)
    {
        mUseLightShafts = setIn;
    }

    bool GLRenderSystem::isEnabledToneMap()
    {
        return mUseToneMap;
    }

    bool GLRenderSystem::isEnabledGaussianBloom()
    {
        return mUseGaussianBloom;
    }

    bool GLRenderSystem::isEnabledSSAO()
    {
        return mUseSSAO;
    }

    bool GLRenderSystem::isEnabledLightShafts()
    {
        return mUseLightShafts;
    }

    void GLRenderSystem::setRenderCamera(Camera *camera)
    {
        ASSERT(camera, "GLRenderSystem: Attempt to pass nullptr render camera");
        mRenderCamera = camera;
        mRenderCamera->setViewport(0, 0, (UINT32)mPixelWindowWidth, (UINT32)mPixelWindowHeight);
    }

    void GLRenderSystem::setGlobalLight(GlobalLight *light)
    {
        ASSERT(light, "GLRenderSystem: Attempt to pass nullptr global light");
        mGlobalLight = light;
    }

    void GLRenderSystem::setAmbientLight(const Vector3f& light)
    {
        mAmbientLight = light;
    }

    void GLRenderSystem::setClearColor(const Vector4f &color)
    {
        mClearColor = color;
    }

    void GLRenderSystem::setBorderColor(const Vector3f &color)
    {
        mBorderColor = color;
    }

    void GLRenderSystem::setShadowQuality(ShadowInfo quality)
    {
        if (quality != mShadowQuality) setUpShadowMaps(quality);
    }

    void GLRenderSystem::setSSAOBufferSize(FLOAT32 partOfScreen)
    {
        ASSERT(partOfScreen > 0.01, "GLRenderSystem: SSAO buffer size part should be more than 0.01");
        mSSAOScreenBufferPart = partOfScreen;
    }

    void GLRenderSystem::setSSAORadius(FLOAT32 radius)
    {
        ASSERT(radius > 0.001, "GLRenderSystem: SSAO radius should be more than 0.001");
        mSSAORadius = radius;
    }

    void GLRenderSystem::setLightShaftsBufferSize(FLOAT32 partOfScreen)
    {
        ASSERT(partOfScreen > 0.01, "GLRenderSystem: Light Shafts buffer size part should be more than 0.01");
        mLightShaftsBufferPart = partOfScreen;
    }

    void GLRenderSystem::setLightShaftsExposure(FLOAT32 exposure)
    {
        mLightShaftsExposure = exposure;
    }

    void GLRenderSystem::setLightShaftsDecay(FLOAT32 decay)
    {
        mLightShaftsDecay = decay;
    }

    void GLRenderSystem::setWindowName(const CStaticString &name)
    {
        mWindowName = name;
        glfwSetWindowTitle(mWindowHandle, name.getChars());
    }

    Camera* GLRenderSystem::getRenderCamera()
    {
        return mRenderCamera;
    }

    GlobalLight* GLRenderSystem::getGlobalLight()
    {
        return mGlobalLight;
    }

    const Vector3f& GLRenderSystem::getAmbientLightSource() const
    {
        return mAmbientLight;
    }

    const Vector4f& GLRenderSystem::getClearColor() const
    {
        return mClearColor;
    }

    const Vector3f& GLRenderSystem::getBorderColor() const
    {
        return mBorderColor;
    }

    ShadowInfo GLRenderSystem::getShadowQuality() const
    {
        return mShadowQuality;
    }

    UINT32 GLRenderSystem::getShadowMapSize() const
    {
        return mShadowMapSize;
    }

    FLOAT32 GLRenderSystem::getSSAOBufferSize() const
    {
        return mSSAOScreenBufferPart;
    }

    FLOAT32 GLRenderSystem::getSSAORadius() const
    {
        return mSSAORadius;
    }

    FLOAT32 GLRenderSystem::getLightShaftsBufferSize() const
    {
        return mLightShaftsBufferPart;
    }

    FLOAT32 GLRenderSystem::getLightShaftsExposure() const
    {
        return mLightShaftsExposure;
    }

    FLOAT32 GLRenderSystem::getLightShaftsDecay() const
    {
        return mLightShaftsDecay;
    }

    const CStaticString& GLRenderSystem::getWindowName() const
    {
        return CStaticString(mWindowName.getChars());
    }

    void GLRenderSystem::setExposure(FLOAT32 exposure)
    {
        mExposure = exposure;
    }

    void GLRenderSystem::setLuminanceThresh(FLOAT32 luminance)
    {
        mLuminanceThresh = luminance;
    }

    void GLRenderSystem::setGammaCorrection(FLOAT32 gamma)
    {
        mGammaCorrection = (FLOAT32)1.0 / gamma;
    }

    FLOAT32 GLRenderSystem::getExposure() const
    {
        return mExposure;
    }

    FLOAT32 GLRenderSystem::getLuminanceThresh() const
    {
        return mLuminanceThresh;
    }

    FLOAT32 GLRenderSystem::getGammaCorrection() const
    {
        return (FLOAT32)1.0 / mGammaCorrection;
    }

    UINT32 GLRenderSystem::getWindowWidth() const
    {
        return (UINT32)mWindowWidth;
    }

    UINT32 GLRenderSystem::getWindowHeight() const
    {
        return (UINT32)mWindowHeight;
    }

    void GLRenderSystem::getWindowSize(UINT32 &width, UINT32 &height) const
    {
        width = (UINT32)mWindowWidth;
        height = (UINT32)mWindowHeight;
    }

    UINT32 GLRenderSystem::getPixelWindowWidth() const
    {
        return (UINT32)mPixelWindowWidth;
    }

    UINT32 GLRenderSystem::getPixelWindowHeight() const
    {
        return (UINT32)mPixelWindowHeight;
    }

    void GLRenderSystem::getPixelWindowSize(UINT32& width, UINT32& height) const
    {
        width = (UINT32)mPixelWindowWidth;
        height = (UINT32)mPixelWindowHeight;
    }

    UINT32 GLRenderSystem::getOldPixelWindowWidth() const
    {
        return (UINT32)mOldPixelWindowWidth;
    }

    UINT32 GLRenderSystem::getOldPixelWindowHeight() const
    {
        return (UINT32)mOldPixelWindowHeight;
    }

    void GLRenderSystem::getOldPixelWindowSize(UINT32& width, UINT32& height) const
    {
        width = (UINT32)mOldPixelWindowWidth;
        height = (UINT32)mOldPixelWindowHeight;
    }

    UINT32 GLRenderSystem::getWindowPosX() const
    {
        return (UINT32)mWindowPosX;
    }

    UINT32 GLRenderSystem::getWindowPosY() const
    {
        return (UINT32)mWindowPosY;
    }

    void GLRenderSystem::getWindowPos(UINT32 &posX, UINT32 &posY) const
    {
        posX = (UINT32)mWindowPosX;
        posY = (UINT32)mWindowPosY;
    }

    bool GLRenderSystem::isReSized()
    {
        return mIsReSized;
    }

    void GLRenderSystem::queueShadowLightSource(SpotLight *light)
    {
        ASSERT(light, "GLRenderSystem: An attempt to pass NULL spot light");
        mSpotShadowSources.add(light);
    }

    void GLRenderSystem::queueShadowLightSource(PointLight *light)
    {
        ASSERT(light, "GLRenderSystem: An attempt to pass NULL point light");
        mPointShadowSources.add(light);
    }

    void GLRenderSystem::queueShadowLightSource(DirectionalLight *light)
    {
        ASSERT(light, "GLRenderSystem: An attempt to pass NULL directional light");
        mDirectionalShadowSources.add(light);
    }

    void GLRenderSystem::queueLightSource(SpotLight* light)
    {
        ASSERT(light, "GLRenderSystem: An attempt to pass NULL spot light");
        mSpotLightSources.add(light);
    }

    void GLRenderSystem::queueLightSource(PointLight* light)
    {
        ASSERT(light, "GLRenderSystem: An attempt to pass NULL point light");
        mPointLightSources.add(light);
    }

    void GLRenderSystem::queueLightSource(DirectionalLight* light)
    {
        ASSERT(light, "GLRenderSystem: An attempt to pass NULL directional light");
        mDirectionalLightSources.add(light);
    }

    void GLRenderSystem::queueRenderNode(RenderNode *node)
    {
        ASSERT(node, "GLRenderSystem: An attempt to pass NULL render node");
        mRenderNodeSources.add(node);
    }

    List<SpotLight *> & GLRenderSystem::getSpotShadowSources()
    {
        return mSpotShadowSources;
    }

    List<PointLight *> & GLRenderSystem::getPointShadowSources()
    {
        return mPointShadowSources;
    }

    List<DirectionalLight *> & GLRenderSystem::getDirectionalShadowSources()
    {
        return mDirectionalShadowSources;
    }

    List<SpotLight*>& GLRenderSystem::getSpotLightSources()
    {
        return mSpotLightSources;
    }

    List<PointLight*>& GLRenderSystem::getPointLightSources()
    {
        return mPointLightSources;
    }

    List<DirectionalLight*>& GLRenderSystem::getDirectionalLightSources()
    {
        return mDirectionalLightSources;
    }

    List<RenderNode *> & GLRenderSystem::getRenderNodeSources()
    {
        return mRenderNodeSources;
    }

    GBuffer* GLRenderSystem::getGBuffer()
    {
        return (GBuffer*)&mGBuffer;
    }

    SSAOBuffer* GLRenderSystem::getSSAOBuffer()
    {
        return (SSAOBuffer*)&mSSAOBuffer;
    }

    DepthMap* GLRenderSystem::getDirDepthMaps()
    {
        return (DepthMap*)mDirectionalDepthMap;
    }

    DepthMap* GLRenderSystem::getSpotDepthMaps()
    {
        return (DepthMap*)mSpotDepthMap;
    }

    CubeDepthMap* GLRenderSystem::getPointDepthMaps()
    {
        return (CubeDepthMap*)mPointDepthMap;
    }

    RenderNode* GLRenderSystem::createRenderNode()
    {
        GLRenderNode node;
        mRenderNodeList.add(node);
        return &mRenderNodeList.getLast();
    }

    void GLRenderSystem::deleteRenderNode(RenderNode* node)
    {
        mRenderNodeList.iterate(true);
        while (mRenderNodeList.iterate())
        {
            if (mRenderNodeList.getCurrent() == *dynamic_cast<GLRenderNode*>(node))
            {
                node->destroy();
                mRenderNodeList.remove(*dynamic_cast<GLRenderNode*>(node));
                return;
            }
        }
    }

    TextureManager& GLRenderSystem::getTextureManagerRef()
    {
        return mTextureManager;
    }

    TextureManager* GLRenderSystem::getTextureManagerPtr()
    {
        return &mTextureManager;
    }

    MaterialManager& GLRenderSystem::getMaterialManagerRef()
    {
        return mMaterialManager;
    }

    MaterialManager* GLRenderSystem::getMaterialManagerPtr()
    {
        return &mMaterialManager;
    }

    RenderMeshManager& GLRenderSystem::getRenderMeshManagerRef()
    {
        return mRenderMeshManager;
    }

    RenderMeshManager* GLRenderSystem::getRenderMeshManagerPtr()
    {
        return &mRenderMeshManager;
    }

    GLScreenPlane* GLRenderSystem::getScreenPlane()
    {
        return &mScreenPlane;
    }

    GLFrameBufferObject* GLRenderSystem::getStageInBuffer()
    {
        return mStageIn;
    }

    GLFrameBufferObject* GLRenderSystem::getStageOutBuffer()
    {
        return mStageOut;
    }

    void GLRenderSystem::getContextInfo()
    {
        const GLubyte * renderer = glGetString(GL_RENDERER);
        const GLubyte * vendor = glGetString(GL_VENDOR);
        const GLubyte * version = glGetString(GL_VERSION);
        const GLubyte * glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        GLint minor, major;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        CHAR buffer[Buffers::SIZE_512] = {'\0'};

        sprintf(buffer, "%s %s", renderer, version);
        mName = CString((const CHAR*)buffer);

        sprintf(buffer, "%s", vendor);
        mRenderName = CString((const CHAR*)buffer);

        sprintf(buffer, "GLSL %s", glslVersion);
        mShadingLanguage = CString((const CHAR*)buffer);
    }

    void GLRenderSystem::setUpShadowMaps(ShadowInfo quality)
    {
        if (quality == ShadowInfo::SI_QUALITY_LOW)
        {
            mShadowMapSize = ShadowInfo::SI_MAP_SIZE_QUALITY_LOW;
        }
        else if (quality == ShadowInfo::SI_QUALITY_MEDIUM)
        {
            mShadowMapSize = ShadowInfo::SI_MAP_SIZE_QUALITY_MEDIUM;
        }
        else if (quality == ShadowInfo::SI_QUALITY_HIGH)
        {
            mShadowMapSize = ShadowInfo::SI_MAP_SIZE_QUALITY_HIGH;
        }
        else
        {
            ERROR("GLRenderSystem: Wrong shadow quality identifier. Shadow's quality is not changed");
            return;
        }

        mShadowQuality = quality;

        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_SPOT_SHADOW_SOURCES; i++)
        {
            mSpotDepthMap[i].destroy();
            mSpotDepthMap[i].create(mShadowMapSize, mShadowMapSize, ShadowInfo::SI_SPOT_MAP_SLOT0 + i);
        }
        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_POINT_SHADOW_SOURCES; i++)
        {
            mPointDepthMap[i].destroy();
            mPointDepthMap[i].create(mShadowMapSize, ShadowInfo::SI_POINT_MAP_SLOT0 + i);
        }
        for(UINT32 i = 0; i < ShadowInfo::SI_MAX_DIR_SHADOW_SOURCES; i++)
        {
            mDirectionalDepthMap[i].destroy();
            mDirectionalDepthMap[i].create(mShadowMapSize, mShadowMapSize, ShadowInfo::SI_DIR_MAP_SLOT0 + i);
        }
    }

    void GLRenderSystem::swap()
    {
        GLFrameBufferObject* tmp;
        tmp = mStageIn;
        mStageIn = mStageOut;
        mStageOut = tmp;
    }

} // namespace Berserk