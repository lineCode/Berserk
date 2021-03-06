//
// Created by Egor Orachyov on 16.02.2019.
//

#ifndef BERSERK_VERTEXTYPES_H
#define BERSERK_VERTEXTYPES_H

#include "Math/MathInclude.h"
#include "Misc/UsageDescriptors.h"

namespace Berserk
{

    struct GRAPHICS_API Vertf
    {
        Vec3f position;
    };

    struct GRAPHICS_API VertPNf
    {
        Vec3f position;
        Vec3f normal;
    };

    struct GRAPHICS_API VertPTf
    {
        Vec3f position;
        Vec2f texcoords;
    };

    struct GRAPHICS_API VertPNTf
    {
        Vec3f position;
        Vec3f normal;
        Vec2f texcoords;
    };

    struct GRAPHICS_API VertPNTBTf
    {
        Vec3f position;
        Vec3f normal;
        Vec3f tangent;
        Vec3f bitangent;
        Vec2f texcoords;
    };

} // namespace Berserk

#endif //BERSERK_VERTEXTYPES_H
