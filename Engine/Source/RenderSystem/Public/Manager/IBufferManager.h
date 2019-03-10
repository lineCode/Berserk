//
// Created by Egor Orachyov on 26.02.2019.
//

#ifndef BERSERK_IBUFFERMANAGER_H
#define BERSERK_IBUFFERMANAGER_H

#include "Misc/Types.h"
#include "Misc/UsageDescriptors.h"
#include "Platform/IGPUBuffer.h"
#include "Platform/IDepthBuffer.h"
#include "Platform/IFrameBuffer.h"
#include "Platform/IUniformBuffer.h"
#include "XML/XMLNode.h"

namespace Berserk
{

    /**
     * An interface which provides access to the buffer manager implemented in 3D Rendering System.
     * Responsible for creating and finding buffers of different type Engine system.
     * Allows to get gpu buffer for vertex/mesh/model data, frame and depth buffer for rendering,
     * uniform buffers for updating uniform data in the shader program.
     * Handles reference counting and memory allocation for resources.
     */
    class ENGINE_API IBufferManager
    {
    public:

        /**
         * Explicit initialization of manager (must be invoked)
         */
        virtual void initialize() = 0;

        /** De-initialize manager */
        virtual void release() = 0;

        /** Rename chosen buffer with new name */
        virtual void renameGPUBuffer(IGPUBuffer* buffer, const char* name) = 0;

        /** Rename chosen buffer with new name */
        virtual void renameFrameBuffer(IFrameBuffer* buffer, const char* name) = 0;

        /** Rename chosen buffer with new name */
        virtual void renameDepthBuffer(IDepthBuffer* buffer, const char* name) = 0;

        /** Rename chosen buffer with new name */
        virtual void renameUniformBuffer(IUniformBuffer* buffer, const char* name) = 0;

        /** Delete specified resource */
        virtual void deleteGPUBuffer(IGPUBuffer* buffer) = 0;

        /** Delete specified resource */
        virtual void deleteFrameBuffer(IFrameBuffer* buffer) = 0;

        /** Delete specified resource */
        virtual void deleteDepthBuffer(IDepthBuffer* buffer) = 0;

        /** Delete specified resource */
        virtual void deleteUniformBuffer(IUniformBuffer* buffer) = 0;

        /** @return New empty (if it does not exist) buffer with specified name */
        virtual IGPUBuffer* createGPUBuffer(const char* name) = 0;

        /** @return GPUBuffer with specified name */
        virtual IGPUBuffer* findGPUBuffer(const char* name) = 0;

        /** @return Pointer to resource with incrementing reference count */
        virtual IGPUBuffer* getGPUBuffer(const char* name) = 0;

        /** @return New empty (if it does not exist) buffer with specified name */
        virtual IFrameBuffer* createFrameBuffer(const char* name) = 0;

        /** @return FrameBuffer with specified name */
        virtual IFrameBuffer* findFrameBuffer(const char* name) = 0;

        /** @return Pointer to resource with incrementing reference count */
        virtual IFrameBuffer* getFrameBuffer(const char* name) = 0;

        /** @return New empty (if it does not exist) buffer with specified name */
        virtual IDepthBuffer* createDepthBuffer(const char* name) = 0;

        /** @return DepthBuffer with specified name */
        virtual IDepthBuffer* findDepthBuffer(const char* name) = 0;

        /** @return Pointer to resource with incrementing reference count */
        virtual IDepthBuffer* getDepthBuffer(const char* name) = 0;

        /** @return New empty (if it does not exist) buffer with specified name */
        virtual IUniformBuffer* createUniformBuffer(const char* name) = 0;

        /** @return UniformBuffer with specified name */
        virtual IUniformBuffer* findUniformBuffer(const char* name) = 0;

        /** @return Pointer to resource with incrementing reference count */
        virtual IUniformBuffer* getUniformBuffer(const char* name) = 0;

        /** @return Memory usage on CPU (RAM) side */
        virtual uint32 getMemoryUsage() = 0;

    };

} // namespace Berserk

#endif //BERSERK_IBUFFERMANAGER_H