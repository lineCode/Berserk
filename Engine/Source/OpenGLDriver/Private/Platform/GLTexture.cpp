//
// Created by Egor Orachyov on 07.02.2019.
//

#include "Platform/GLTexture.h"
#include "Platform/GLInclude.h"
#include "Platform/GLRenderDriver.h"
#include "Logging/LogMacros.h"

namespace Berserk
{

    void GLTexture::initialize(const char* name)
    {
        mWidth = 0;
        mHeight = 0;
        mGenMipMaps = false;

        mSampler = nullptr;
        mTextureID = 0;
        mReferenceCount = 0;

        mResourceName = name;
    }

    void GLTexture::addReference()
    {
        mReferenceCount += 1;
    }

    void GLTexture::release()
    {
        if (mReferenceCount > 0)
        {
            mReferenceCount -= 1;
        }

        if (mReferenceCount == 0 && mTextureID)
        {
            PUSH("GLTexture: delete [name: '%s']", mResourceName.get());

            glDeleteTextures(1, &mTextureID);
            mTextureID = 0;
        }
    }

    uint32 GLTexture::getReferenceCount()
    {
        return mReferenceCount;
    }

    uint32 GLTexture::getMemoryUsage()
    {
        return sizeof(GLTexture);
    }

    const char* GLTexture::getName()
    {
        return mResourceName.get();
    }

    void GLTexture::create(uint32 width,
                           uint32 height,
                           IRenderDriver::StorageFormat storageFormat,
                           IRenderDriver::DataType dataType)
    {
        create(width, height, storageFormat, IRenderDriver::RGB, dataType, nullptr, false);
    }

    void GLTexture::create(uint32 width,
                           uint32 height,
                           IRenderDriver::StorageFormat storageFormat,
                           IRenderDriver::PixelFormat pixelFormat,
                           IRenderDriver::DataType pixelType,
                           void *data,
                           bool genMipMaps)
    {
        auto trg = GLRenderDriver::TEXTURE_2D;
        auto str = GLRenderDriver::getStorageFormat(storageFormat);
        auto pxf = GLRenderDriver::getPixelFormat(pixelFormat);
        auto pxt = GLRenderDriver::getDataType(pixelType);

        glGenTextures(1, &mTextureID);
        glBindTexture(trg, mTextureID);

        glTexImage2D(trg, 0, str, width, height, 0, pxf, pxt, data);

        if (genMipMaps) glGenerateMipmap(trg);

        mWidth = width;
        mHeight = height;
        mGenMipMaps = genMipMaps;

        mTextureType = trg;
        mStorageFormat = str;
        mPixelFormat = pxf;

        glBindTexture(trg, 0);
    }

    void GLTexture::bind(ISampler *sampler)
    {
        mSampler = sampler;
    }

    void GLTexture::bind(uint32 textureSlot)
    {
        mSampler->bind(textureSlot);
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(mTextureType, mTextureID);
    }

    void GLTexture::getData(uint32 depth, IRenderDriver::PixelFormat format, IRenderDriver::DataType type, void *data)
    {
        glBindTexture(mTextureType, mTextureID);
        glGetTexImage(mTextureType,
                      depth,
                      GLRenderDriver::getPixelFormat(format),
                      GLRenderDriver::getDataType(type),
                      data);
    }

    void GLTexture::getSize(uint32 &width, uint32 &height)
    {
        width = mWidth;
        height = mHeight;
    }

    void GLTexture::setFiltering(IRenderDriver::SamplerFilter min, IRenderDriver::SamplerFilter mag)
    {
        glBindTexture(mTextureType, mTextureID);
        glTexParameteri(mTextureType, GL_TEXTURE_MIN_FILTER, GLRenderDriver::getSamplerFilter(min));
        glTexParameteri(mTextureType, GL_TEXTURE_MAG_FILTER, GLRenderDriver::getSamplerFilter(mag));
        glBindTexture(mTextureType, 0);
    }

    void GLTexture::setWrapping(IRenderDriver::SamplerWrapMode wrap)
    {
        glBindTexture(mTextureType, mTextureID);

                                           glTexParameteri(mTextureType,
                                                           GL_TEXTURE_WRAP_S,
                                                           GLRenderDriver::getSamplerWrapMode(wrap));
        if (mTextureType == GL_TEXTURE_2D) glTexParameteri(mTextureType,
                                                           GL_TEXTURE_WRAP_T,
                                                           GLRenderDriver::getSamplerWrapMode(wrap));
        if (mTextureType == GL_TEXTURE_3D) glTexParameteri(mTextureType,
                                                           GL_TEXTURE_WRAP_R,
                                                           GLRenderDriver::getSamplerWrapMode(wrap));

        glBindTexture(mTextureType, 0);
    }

    void GLTexture::setBorderColor(const Vec4f &color)
    {
        glBindTexture(mTextureType, mTextureID);
        glTexParameterfv(mTextureType, GL_TEXTURE_BORDER_COLOR, (float32*)&color);
        glBindTexture(mTextureType, 0);
    }

    bool GLTexture::getMipMapsGen()
    {
        return mGenMipMaps;
    }

    uint32 GLTexture::getHandle()
    {
        return mTextureID;
    }

    uint32 GLTexture::getWidth()
    {
        return mWidth;
    }

    uint32 GLTexture::getHeight()
    {
        return mHeight;
    }

    uint32 GLTexture::getGPUMemoryUsage()
    {
        uint32 size = 0;

        switch (mStorageFormat)
        {
            case GLRenderDriver::RGB8:
                size = 4;
                break;

            case GLRenderDriver::RGBA8:
                size = 4;
                break;

            case GLRenderDriver::RGB16F:
                size = 8;
                break;

            case GLRenderDriver::RGBA16F:
                size = 8;
                break;

            case GLRenderDriver::RGB32F:
                size = 12;
                break;

            case GLRenderDriver::RGBA32F:
                size = 16;
                break;

            case GLRenderDriver::DEPTH24:
                size = 4;
                break;

            case GLRenderDriver::DEPTH24_STENCIL8:
                size = 4;
                break;

            default:
                FAIL(false, "Unsupported format [name: '%s']", mResourceName.get());
        }

        return size * mWidth * mHeight;
    }

} // namespace Berserk