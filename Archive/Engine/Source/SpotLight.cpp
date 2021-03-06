//
// Created by Egor Orachyov on 26.07.2018.
//

#include "Objects/Lights/SpotLight.h"
#include "Managers/SceneManager.h"
#include "Math/UtilityMatrices.h"
#include "Render/RenderSystem.h"
#include <cmath>

namespace Berserk
{

    SpotLight::SpotLight(const CStaticString &name, FLOAT32 lifeTime) : Light(name, lifeTime)
    {
        setLightIntensity(Vector3f(1.0));
        setPosition(Vector3f(0.0));
        setDirection(Vector3f(0,0,-1));
        setOrientation(Vector3f(0,1,0));
        setCutoff(Math::radians(10.0f));
        setInnerCutoff(Math::radians(6.5f));
        setOuterCutoff(Math::radians(10.5f));
        setAttenuationExponent(8);
        setFarShadowPlane(20.0);
    }

    void SpotLight::setLightIntensity(const Vector3f &intensity)
    {
        if (mIsEditable)
        {
            mSpotComponent.mLightIntensity = intensity;
        }
    }

    void SpotLight::setPosition(const Vector3f &position)
    {
        if (mIsEditable)
        {
            mPosition = position;
        }
    }

    void SpotLight::setDirection(const Vector3f &direction)
    {
        if (mIsEditable)
        {
            mDirection = direction;
        }
    }

    void SpotLight::setOrientation(const Vector3f &orientation)
    {
        if (mIsEditable)
        {
            mOrientation = orientation;
        }
    }

    void SpotLight::setInnerCutoff(FLOAT32 angle)
    {
        if (mIsEditable)
        {
            mInnerCutoff = angle;
            mSpotComponent.mEpsilon = cosf(mInnerCutoff) - mSpotComponent.mOuterCutoff;
        }
    }

    void SpotLight::setOuterCutoff(FLOAT32 angle)
    {
        if (mIsEditable)
        {
            mOuterCutoff = angle;
            mSpotComponent.mOuterCutoff = cosf(mOuterCutoff);
            mSpotComponent.mEpsilon = cosf(mInnerCutoff) - mSpotComponent.mOuterCutoff;
        }
    }

    void SpotLight::setCutoff(FLOAT32 angle)
    {
        if (mIsEditable)
        {
            mCutoff = angle;
            mSpotComponent.mCutoff = cosf(angle);
            mSpotComponent.mEpsilon = cosf(mInnerCutoff) - cosf(mOuterCutoff);
            mShadowComponent.mProjection = perspective(mCutoff, 1.0, 1.0, mFarPlane);
        }
    }

    void SpotLight::setAttenuationExponent(FLOAT32 attenuation)
    {
        if (mIsEditable)
        {
            mSpotComponent.mAttenuationExponent = attenuation;
        }
    }

    void SpotLight::setFarShadowPlane(FLOAT32 distance)
    {
        if (mIsEditable)
        {
            mFarPlane = distance;
            mShadowComponent.mProjection = perspective(mCutoff, 1.0, 1.0, mFarPlane);
        }
    }

    const Vector3f& SpotLight::getLightIntensity() const
    {
        return mSpotComponent.mLightIntensity;
    }

    const Vector3f& SpotLight::getPosition() const
    {
        return mPosition;
    }

    const Vector3f& SpotLight::getDirection() const
    {
        return mDirection;
    }

    const Vector3f& SpotLight::getOrientation() const
    {
        return mOrientation;
    }

    FLOAT32 SpotLight::getCutoff() const
    {
        return mCutoff;
    }

    FLOAT32 SpotLight::getInnerCutoff() const
    {
        return mInnerCutoff;
    }

    FLOAT32 SpotLight::getOuterCutoff() const
    {
        return mOuterCutoff;
    }

    FLOAT32 SpotLight::getAttenuationExponent() const
    {
        return mSpotComponent.mAttenuationExponent;
    }

    FLOAT32 SpotLight::getFarShadowPlane() const
    {
        return mFarPlane;
    }

    SpotLightComponent* SpotLight::getComponent()
    {
        return &mSpotComponent;
    }

    ShadowCasterComponent* SpotLight::getShadowCaster()
    {
        return &mShadowComponent;
    }

    void SpotLight::process(FLOAT64 delta, const Matrix4x4f &rootTransformation)
    {
        if (mIsActive)
        {
            processActor(delta, rootTransformation);
            processSpotLight();
        }
    }

    void SpotLight::processSpotLight()
    {
        const Matrix4x4f& ress = mWorldTransformation;

        mSpotComponent.mDirection = (ress * Vector4f(mDirection.x, mDirection.y, mDirection.z, 0)).getNormalized();
        mSpotComponent.mPosition = ress * Vector4f(mPosition.x, mPosition.y, mPosition.z, 1);

        if (mCastShadows)
        {
            Vector3f position = Vector3f(mSpotComponent.mPosition);
            Vector3f target = Vector3f(mSpotComponent.mPosition + mSpotComponent.mDirection);
            Vector3f orientation = Vector3f(ress * Vector4f(mOrientation.x, mOrientation.y, mOrientation.z, 0));
            mShadowComponent.mView = lookAt(position,target, orientation);

            gRenderSystem->queueShadowLightSource(this);
        }
        else
        {
            gRenderSystem->queueLightSource(this);
        }
    }

} // namespace Berserk