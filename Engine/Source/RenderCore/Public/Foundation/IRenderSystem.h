//
// Created by Egor Orachyov on 24.03.2019.
//

#ifndef BERSERK_IRENDERSYSTEM_H
#define BERSERK_IRENDERSYSTEM_H

#include <Systems/ISystem.h>
#include <Foundation/RenderInfo.h>
#include <Foundation/RenderSettings.h>
#include <Foundation/RenderPassInfo.h>

namespace Berserk::Render
{
    using namespace Engine;

    /**
     * Interface for the rendering system of the engine. Provides
     * exhaustive functionality and initializes its subsystems.
     *
     * Allows to register components, process them, capture screens.
     * Also allows to configure all the effects, available in the rendering engine
     */
    class ENGINE_API IRenderSystem : public ISystem
    {
    public:

        /** Provide minimal required interface for memory operations */
        GENERATE_CLASS_BODY(IRenderSystem);

        /** Do actually nothing */
        explicit IRenderSystem(const ISystemInitializer& systemInitializer) : ISystem(systemInitializer) {}

        /** Do actually nothing */
        ~IRenderSystem() override = default;

    public:

        /** Register a light source which will affect on scene lightning */
        virtual void registerComponent(SpotLightComponent *component) = 0;

        /** Register a light source which will affect on scene lightning */
        virtual void registerComponent(PointLightComponent *component) = 0;

        /** Register a light source which will affect on scene lightning */
        virtual void registerComponent(DirectionalLightComponent *component) = 0;

        /** Register a primitive component to be rendered */
        virtual void registerComponent(StaticMeshComponent *component) = 0;

    public:

        /** @return Current frame number */
        uint64 getCurrentFrameNumber() { return mCurrentFrameNumber; }

        /** @return General render system settings info [can modify] */
        RenderSettings& getSettings() { return mSettings; }

        /** @return Fog effect info [can modify] */
        RenderInfoDistantFog& getDistantFogInfo() { return mDistantFog; }

        /** @return Tone Mapping info [can modify] */
        RenderInfoToneMapping& getToneMappingInfo() { return mToneMapping; }

    protected:

        /** Frames counter */
        uint64 mCurrentFrameNumber = 0;

        /** General settings of the render system */
        RenderSettings mSettings;

        /** Info about fog effect */
        RenderInfoDistantFog mDistantFog;

        /** Info about tone mapping and gamma correction */
        RenderInfoToneMapping mToneMapping;

    };

} // namespace Berserk

#endif //BERSERK_IRENDERSYSTEM_H
