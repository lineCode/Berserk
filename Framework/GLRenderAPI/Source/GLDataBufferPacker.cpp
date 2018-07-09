//
// Created by Egor Orachyov on 04.07.2018.
//

#include "Buffers/GLDataBufferPacker.h"

#include "../../Core/Logging/LogMessages.h"

namespace Berserk
{

    GLDataBufferPacker::GLDataBufferPacker()
    {
        mBuffer = NULL;
        mIsInitialized = false;
        mIsPacked = false;
        mTotalSize = 0;
        mStride = 0;
        mCount = 0;
    }

    GLDataBufferPacker::~GLDataBufferPacker()
    {
        PUSH("Delete Data Packer %p with buffer %p\n", this, mBuffer);
        destroy();
    }

    void GLDataBufferPacker::init()
    {
        mMetaData.init(4); // initial capacity
        mIsInitialized = true;
    }

    void GLDataBufferPacker::destroy()
    {
        reset();
    }

    void GLDataBufferPacker::pack()
    {
        if (!isInitialized())
        {
            WARNING("An attempt to use do not initialized packer");
            return;
        }
        if (isPacked())
        {
            WARNING("Vertex data addition to already packed buffer");
            return;
        }
        if (mMetaData.getSize() == 0)
        {
            WARNING("Cannot pack NULL data");
            return;
        }

        mBuffer = mem_alloc(mTotalSize);
        uint32 offset = 0;

        for(uint32 i = 0; i < mMetaData.getSize(); i++)
        {
            VertexData* current = mMetaData.getPointer(i);

            for(uint32 j = 0; j < mCount; j++)
            {
                memcpy((int8*)mBuffer + mStride * j + offset, (int8*)current->data + current->size * j, current->size);
            }

            current->offset = (void*)offset;
            offset += current->size;
        }

        mIsPacked = true;
    }

    void GLDataBufferPacker::reset()
    {
        if (mBuffer)
        {
            mem_free(mBuffer);
        }
        mMetaData.empty();
        mBuffer = NULL;
        mIsInitialized = false;
        mIsPacked = false;
        mTotalSize = 0;
        mStride = 0;
        mCount = 0;
    }

    void GLDataBufferPacker::addVertexData(void *data, uint32 size, uint32 perVertexCount, uint32 count, uint32 attributeIndex,
                                           GLDataType type, GLNormalization usage)
    {
        if (!isInitialized())
        {
            WARNING("An attempt to use do not initialized packer");
            return;
        }
        if (isPacked())
        {
            WARNING("Vertex data addition to already packed buffer");
            return;
        }

        if (mMetaData.getSize() == 0)
        {
            VertexData vd = {data, size, perVertexCount, attributeIndex, type, (GLboolean)usage, NULL};
            mMetaData.add(vd);
            mTotalSize += size * count;
            mStride += size;
            mCount = count;
        }
        else if (mCount != count)
        {
            ERROR("Count of element in the buffer do not incompatible with previously got value");
        }
        else
        {
            VertexData vd = {data, size, perVertexCount, attributeIndex, type, (GLboolean)usage, NULL};
            mMetaData.add(vd);
            mTotalSize += size * count;
            mStride += size;
        }
    }

    void GLDataBufferPacker::addVertexData(Vector2f* data, uint32 count, uint32 attributeIndex, GLNormalization usage)
    {
        addVertexData((void*)data, sizeof(Vector2f), 2, count, attributeIndex, GLDataType::GLPT_FLOAT, usage);
    }

    void GLDataBufferPacker::addVertexData(Vector3f* data, uint32 count, uint32 attributeIndex, GLNormalization usage)
    {
        addVertexData((void*)data, sizeof(Vector3f), 3, count, attributeIndex, GLDataType::GLPT_FLOAT, usage);
    }

    void GLDataBufferPacker::addVertexData(Vector4f* data, uint32 count, uint32 attributeIndex, GLNormalization usage)
    {
        addVertexData((void*)data, sizeof(Vector4f), 4, count, attributeIndex, GLDataType::GLPT_FLOAT, usage);
    }

    bool GLDataBufferPacker::isInitialized() const
    {
        return mIsInitialized;
    }

    bool GLDataBufferPacker::isPacked() const
    {
        return mIsPacked;
    }

    uint32 GLDataBufferPacker::getStride() const
    {
        return mStride;
    }

    uint32 GLDataBufferPacker::getCount() const
    {
        return mCount;
    }

    uint32 GLDataBufferPacker::getBuffersCount() const
    {
        return mMetaData.getSize();
    }

    uint32 GLDataBufferPacker::getTotalBufferSize() const
    {
        return mTotalSize;
    }

    const void* GLDataBufferPacker::getBuffer() const
    {
        return mBuffer;
    }

} // namespace Berserk