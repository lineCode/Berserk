//
// Created by Egor Orachyov on 07.02.2019.
//

#ifndef BERSERK_ISAMPLER_H
#define BERSERK_ISAMPLER_H

#include "Misc/Bits.h"
#include "Misc/Types.h"
#include "Platform/ISampler.h"
#include "Resource/IResource.h"
#include "Misc/UsageDescriptors.h"
#include "Platform/IRenderDriver.h"

namespace Berserk
{

    class GRAPHICS_API ISampler : public IResource
    {
    public:

        /**
         * Creates sampler with chosen params
         * @param min Minification filtering
         * @param max Magnification filtering
         * @param wrap Wrapping mod for texture
         */
        virtual void create(IRenderDriver::SamplerFilter min,
                            IRenderDriver::SamplerFilter max,
                            IRenderDriver::SamplerWrapMode wrap) = 0;

        /**
         * Set filtering params
         * @param min Minification filtering
         * @param max Magnification filtering
         */
        virtual void setFiltering(IRenderDriver::SamplerFilter min,
                                  IRenderDriver::SamplerFilter max) = 0;

        /**
         * Set wrapping mode
         * @param wrap Wrapping mod for texture
         */
        virtual void setWrapping(IRenderDriver::SamplerWrapMode wrap) = 0;

        /**
         * Enable border color using for texture
         * @param color Chosen border color
         */
        virtual void setBorderColor(const Vec4f& color) = 0;

    };

} // namespace Berserk

#endif //BERSERK_ISAMPLER_H