//
// Created by Egor Orachyov on 07.02.2019.
//

#ifndef BERSERK_GLGPUBUFFER_H
#define BERSERK_GLGPUBUFFER_H

#include "Platform/IGPUBuffer.h"
#include "Strings/String.h"

namespace Berserk
{

    namespace Resources
    {

        /**
         * OpenGL platform implementation of gpu buffer object
         */
        class GRAPHICS_API GLGPUBuffer : public IGPUBuffer
        {
        public:

            ~GLGPUBuffer();

            /** @copydoc IResource::initialize() */
            void initialize(const char* name) override;

            /** @copydoc IResource::addReference() */
            void addReference() override;

            /** @copydoc IResource::release() */
            void release() override;

            /** @copydoc IResource::getReferenceCount() */
            uint32 getReferenceCount() override;

            /** @copydoc IResource::getMemoryUsage() */
            uint32 getMemoryUsage() override;

            /** @copydoc IResource::getName() */
            const char* getName() override;

        public:

            /** @copydoc IGPUBuffer::create() */
            void create(uint32 verticesCount,
                        VertexType vertexType,
                        void* vertices,
                        uint32 indicesCount,
                        uint16* indices) override;

            /** @copydoc IGPUBuffer::setDrawingProperties() */
            void draw(uint32 count,
                      IRenderDriver::PrimitiveType primitiveType,
                      IRenderDriver::DataType indicesType) override;

            /** @copydoc IGPUBuffer::draw() */
            void draw() override;

            /** @copydoc IGPUBuffer::getVertexType() */
            VertexType getVertexType() override;

            /** @copydoc IGPUBuffer::getVertexType() */
            uint32 getVertexCount() override;

            /** @copydoc IGPUBuffer::getGPUMemoryUsage() */
            uint32 getGPUMemoryUsage() override;

        protected:

            friend class GLBufferManager;

            uint32 mVertexArrayObject;      // Buffer VAO handle
            uint32 mElementBufferObject;    // Buffer VBO handle for vertexes' attributes
            uint32 mVertexBufferObject;     // Buffer EBO handle for indices of vertexes

            uint32 mReferenceCount;

            uint32 mIndicesCount;
            uint32 mVerticesCount;

            uint32 mPrimitiveMode;
            uint32 mIndicesType;

            VertexType mVertexType;
            IRenderDriver::DataType mIndexType;

            CString mResourceName;

        };

    } // namespace Resources

} // namespace Berserk

#endif //BERSERK_GLGPUBUFFER_H