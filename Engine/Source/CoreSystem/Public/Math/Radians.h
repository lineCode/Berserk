//
// Created by Egor Orachyov on 05.02.2019.
//

#ifndef BERSERK_RADIANS_H
#define BERSERK_RADIANS_H

#include "Misc/Types.h"
#include "Math/MathUtility.h"
#include "Strings/StaticString.h"
#include "Misc/UsageDescriptors.h"

namespace Berserk
{

    class Radians final
    {
    public:

        Radians();

        Radians(const Radians& angle) = default;

        explicit Radians(float32 angle);

        explicit Radians(const Degrees& angle);

        ~Radians() = default;

        Degrees degrees() const;

        float32 get() const { return mAngle; }

        CName toString() const;

    public:

        float32 mAngle;

    };

} // namespace Berserk

#endif //BERSERK_RADIANS_H
