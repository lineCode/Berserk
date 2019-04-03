//
// Created by Egor Orachyov on 01.04.2019.
//

#ifndef BERSERK_SCENECOMPONENT_H
#define BERSERK_SCENECOMPONENT_H

#include <Components/IEntityComponent.h>
#include <Math/MathInclude.h>

namespace Berserk::EntitySystem
{

    /**
     * A SceneComponent has a transform and supports attachment, but has no rendering or
     * collision capabilities. Allows to create transformation hierarchy. Needed for
     * audio, physics, rendering components and entities for scene placement.
     */
    class SceneComponent : public IEntityComponent
    {
    public:

        GENERATE_CLASS_BODY(SceneComponent);

        /**
         * Creates as default (0,0,0) position transformation
         * with 1.0f scale factor and (0,0,0) rotation
         */
        explicit SceneComponent(const IObjectInitializer& objectInitializer);

        /** Do actually nothing */
        ~SceneComponent() override = default;

    public:

        /**
         * Rotate via Euler angles
         *
         * @note Angles should be in radians
         * @note Watch param description to exclude misunderstanding because of
         *       relating (roll, yaw, pitch) and rotation axises (oX,oY,oZ)
         *
         * @param roll  oX Clockwise rotation angle
         * @param yaw   oY Clockwise rotation angle
         * @param pitch oZ Clockwise rotation angle
         */
        void addLocalRotation(float32 roll, float32 yaw, float32 pitch);

        /**
         * Rotate around an axis and angle
         *
         * @note Clockwise rotation
         * @warning Axis should be not 0 length
         *
         * @param axis  The vector of rotation
         * @param angle The angle of rotation in radians
         */
        void addLocalRotation(const Vec3f &axis, float32 angle);

        /**
         * Adds to `this` rotation around oX
         * @note  Angle should be in radians
         * @param roll oX Clockwise rotation angle
         */
        void addLocalRotationX(float32 roll);

        /**
         * Adds to `this` rotation around oY
         * @note  Angle should be in radians
         * @param yaw oY Clockwise rotation angle
         */
        void addLocalRotationY(float32 yaw);

        /**
         * Adds to `this` rotation around oZ
         * @note  Angle should be in radians
         * @param pitch oZ Clockwise rotation angle
         */
        void addLocalRotationZ(float32 pitch);

        /** Add translation to the original translation */
        void addLocalTranslation(const Vec3f &translation);

        /** Add to original scale this scale argument */
        void addLocalScale(float32 factor);

        /** Convert to internal matrix form */
        void update();

        /** Update with root [needed for global transform] */
        void update(const SceneComponent &root);

    public:

        /** @return Reference to scale to modify it */
        float32& scale() { return mScale; }

        /** @return Reference to rotation to modify it */
        Quatf& rotation() { return mRotation; }

        /** @return Reference to translation to modify it */
        Vec3f& translation() { return mTranslation; }

        /** @return Result transform in local space */
        Mat4x4f toLocalSpace() { return mLocalTransform; }

        /** @return Result transform in global space */
        Mat4x4f toGlobalSpace() { return mGlobalTransform; }

    private:

        /** Scale factor around x,y,z axes */
        float32 mScale;

        /** Simple translation vector */
        Vec3f mTranslation;

        /** Rotation around an arbitrary axis */
        Quatf mRotation;

        /** Matrix representation of component transformation [updated in update() function] */
        Mat4x4f mLocalTransform;

        /** Absolute transformation of this component relatively to its root */
        Mat4x4f mGlobalTransform;

        /** Root of trasformation in scene hierarchy */
        SceneComponent* root;

    };

} // namespace Berserk::EntitySystem

#endif //BERSERK_SCENECOMPONENT_H