//
// Created by Egor Orachyov on 07.02.2019.
//

#include "Platform/GLFrameBuffer.h"
#include "Platform/GLInclude.h"
#include "Platform/GLProfile.h"

namespace Berserk
{

    namespace Resources
    {

        GLFrameBuffer::~GLFrameBuffer()
        {
            mResourceName.nullify();
        }

        void GLFrameBuffer::initialize(const char *name)
        {
            mWidth = 0;
            mHeight = 0;
            mFrameBufferID = 0;
            mReferenceCount = 0;

            new(&mResourceName) CString(name);
        }

        void GLFrameBuffer::addReference()
        {
            mReferenceCount += 1;
        }

        void GLFrameBuffer::release()
        {
            if (mReferenceCount > 0)
            {
                mReferenceCount -= 1;
            }

            if (mReferenceCount == 0 && mFrameBufferID)
            {
#if PROFILE_GL_PLATFORM
                PUSH("GLFrameBuffer: delete [name: '%s']", mResourceName.get());
#endif

                glDeleteFramebuffers(1, &mFrameBufferID);
                mFrameBufferID = 0;

                for (uint32 i = 0; i < mColorAttachments.getSize(); i++)
                {
                    mColorAttachments[i].release();
                }

                mDepthBuffer.release();

                delete(&mColorAttachments);
                delete(&mResourceName);
            }
        }

        uint32 GLFrameBuffer::getReferenceCount()
        {
            return mReferenceCount;
        }

        uint32 GLFrameBuffer::getMemoryUsage()
        {
            return sizeof(GLFrameBuffer) + mColorAttachments.getMemoryUsage();
        }

        const char* GLFrameBuffer::getName()
        {
            return mResourceName.get();
        }

        void GLFrameBuffer::createFrameBuffer(uint32 width, uint32 height, uint32 colorAttachments)
        {
            if (mFrameBufferID)
            {
                FAIL(false, "An attempt to recreate frame buffer [name: '%s']", mResourceName.get());
            }

            if (colorAttachments < 1)
            {
                FAIL(false, "Color attachments count less than 1 [name: '%s']", mResourceName.get());
            }

            mWidth = width;
            mHeight = height;

            new(&mColorAttachments) ArrayList<GLTexture>(Math::max(colorAttachments, ArrayList<GLTexture>::MIN_INITIAL_SIZE));

            glGenFramebuffers(1, &mFrameBufferID);
        }

        void GLFrameBuffer::attachColorBuffer(IRenderDriver::StorageFormat format, IRenderDriver::SamplerFilter filtering,
                                                      IRenderDriver::SamplerWrapMode wrapping)
        {
            if (!mFrameBufferID)
            {
                FAIL(false, "An attempt to attach buffer to not initialized frame buffer [name: '%s']", mResourceName.get());
            }

            uint32 attachment_i = mColorAttachments.getSize();
            char buffer[CName::STRING_SIZE];
            sprintf(buffer, "ColorAttachment%u", attachment_i);

            GLTexture attachment;
            attachment.initialize(buffer);
            attachment.create(mWidth, mHeight, format);
            attachment.setFiltering(filtering, filtering);
            attachment.setWrapping(wrapping);

            mColorAttachments += attachment;

            glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, attachment.mTextureID, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void GLFrameBuffer::attachDepthBuffer()
        {
            if (!mFrameBufferID)
            {
                FAIL(false, "An attempt to attach buffer to not initialized frame buffer [name: '%s']", mResourceName.get());
            }

            mDepthBuffer.initialize("DepthBuffer");
            mDepthBuffer.create(mWidth, mHeight, IRenderDriver::DEPTH24, IRenderDriver::DEPTH, IRenderDriver::FLOAT, NULL, false);
            mDepthBuffer.setFiltering(IRenderDriver::FILTER_NEAREST, IRenderDriver::FILTER_NEAREST);
            mDepthBuffer.setWrapping(IRenderDriver::WRAP_CLAMP_TO_EDGE);

            glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer.mTextureID, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void GLFrameBuffer::attachDepthStencilBuffer()
        {
            if (!mFrameBufferID)
            {
                FAIL(false, "An attempt to attach buffer to not initialized frame buffer [name: '%s']", mResourceName.get());
            }

            mDepthBuffer.initialize("DepthStencilBuffer");
            mDepthBuffer.create(mWidth, mHeight, IRenderDriver::DEPTH24_STENCIL8, IRenderDriver::DEPTH_AND_STENCIL, IRenderDriver::FLOAT, nullptr, false);
            mDepthBuffer.setFiltering(IRenderDriver::FILTER_NEAREST, IRenderDriver::FILTER_NEAREST);
            mDepthBuffer.setWrapping(IRenderDriver::WRAP_CLAMP_TO_EDGE);
        }

        void GLFrameBuffer::linkBuffers()
        {
            uint32 count = mColorAttachments.getSize();
            uint32 attachments[MAX_SUPPORTED_COLOR_BUFFERS];

            glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);

            for (uint32 i = 0; i < count; i++)
            {
                attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            };

            glDrawBuffers(count, attachments);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                FAIL(false, "GLFrameBuffer: incomplete");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void GLFrameBuffer::bindFrameBuffer()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);
        }

        void GLFrameBuffer::bindColorBuffer(uint32 layer, uint32 textureSlot)
        {
            // !!! WARNING !!!
            //
            // glBindSampler(textureSlot, 0)
            //
            // Unbind any attached sampler object to this
            // texture slot. If there was any sampler binded,
            // our color layer would get wrong filtration properties,
            // therefore sampler2D would not work properly in shader.

            glBindSampler(textureSlot, 0);
            glActiveTexture(GL_TEXTURE0 + textureSlot);
            glBindTexture(mColorAttachments[layer].mTextureType, mColorAttachments[layer].mTextureID);
        }

        void GLFrameBuffer::bindDepthBuffer(uint32 textureSlot)
        {
            // !!! WARNING !!!
            //
            // glBindSampler(textureSlot, 0)
            //
            // Unbind any attached sampler object to this
            // texture slot. If there was any sampler binded,
            // our color layer would get wrong filtration properties,
            // therefore sampler2D would not work properly in shader.

            glBindSampler(textureSlot, 0);
            glActiveTexture(GL_TEXTURE0 + textureSlot);
            glBindTexture(GL_TEXTURE_2D, mDepthBuffer.mTextureID);
        }

        void GLFrameBuffer::bindDepthStencilBuffer(uint32 textureSlot)
        {
            // !!! WARNING !!!
            //
            // glBindSampler(textureSlot, 0)
            //
            // Unbind any attached sampler object to this
            // texture slot. If there was any sampler binded,
            // our color layer would get wrong filtration properties,
            // therefore sampler2D would not work properly in shader.

            glBindSampler(textureSlot, 0);
            glActiveTexture(GL_TEXTURE0 + textureSlot);
            glBindTexture(GL_TEXTURE_2D, mDepthBuffer.mTextureID);
        }

        void GLFrameBuffer::getSize(uint32 &width, uint32 &height)
        {
            width = mWidth;
            height = mHeight;
        }

        uint32 GLFrameBuffer::getGPUMemoryUsage()
        {
            uint32 memory = mDepthBuffer.getGPUMemoryUsage();

            for (uint32 i = 0; i < mColorAttachments.getSize(); i++)
            {
                memory += mColorAttachments[i].getGPUMemoryUsage();
            }

            return memory;
        }

        uint32 GLFrameBuffer::getMaxSupportedColorBuffers()
        {
            return MAX_SUPPORTED_COLOR_BUFFERS;
        }

    } // namespace Resources

} // namespace Berserk