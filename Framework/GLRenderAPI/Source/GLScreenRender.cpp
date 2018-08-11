//
// Created by Egor Orachyov on 10.08.2018.
//

#include "Pipeline/GLScreenRender.h"
#include "Render/GLRenderDriver.h"
#include "Render/GLRenderSystem.h"

namespace Berserk
{
    void GLScreenRender::init()
    {
        mProgram.init();
        mProgram.compileShader("../GLRenderAPI/Shaders/PostProcess/GLSLScreenRender.vert", GLShaderType::GLST_VERTEX);
        mProgram.compileShader("../GLRenderAPI/Shaders/PostProcess/GLSLScreenRender.frag", GLShaderType::GLST_FRAGMENT);
        mProgram.link();
        mProgram.validate();

        mUniform = mProgram.getUniformLocation("Screen");
    }

    void GLScreenRender::destroy()
    {
        mProgram.destroy();
    }

    void GLScreenRender::execute()
    {
        auto render = dynamic_cast<GLRenderSystem*>(gRenderSystem);
        auto driver = dynamic_cast<GLRenderDriver*>(gRenderDriver);

        mProgram.use();

        render->getStageInBuffer()->useAsUniformData();

        driver->setDefaultBuffer();
        driver->setClearColor(render->getClearColor());
        driver->clearBuffer();
        driver->enableDepthTest(false);
        driver->setViewPort(0, 0, render->getPixelWindowWidth(), render->getOldPixelWindowHeight());

        mProgram.setUniform(mUniform, 0);

        render->getScreenPlane()->use();
        render->getStageOutBuffer()->disable();
    }

} // namespace Berserk