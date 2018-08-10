//
// Created by Egor Orachyov on 29.07.2018.
//

#ifndef BERSERKENGINE_GLFRAGMENTLIGHTNING_H
#define BERSERKENGINE_GLFRAGMENTLIGHTNING_H

#include "GLRenderPipeline.h"
#include "Managers/RenderManager.h"
#include "Uniform/LightStructures.h"
#include "Uniform/MaterialStructures.h"
#include "Uniform/TransformationStructures.h"

namespace Berserk
{

    class GLFragmentLightning : public GLRenderPipeline
    {
    private:

        class UniformData : public UBaseTransformation, public ULightsInfo
        {

        };

        class UniformBaseMaterial : public UniformData
        {
        public:
            BaseMaterial Material;
        };

    public:

        GLFragmentLightning();
        virtual ~GLFragmentLightning();

        void init() override;
        void destroy() override;
        GLFrameBufferObject* process(RenderManager *manager, GLFrameBufferObject *object) override;

    private:

        GLGPUProgram mProgram;
        UniformBaseMaterial mUniform;

    };

}

#endif //BERSERKENGINE_GLFRAGMENTLIGHTNING_H