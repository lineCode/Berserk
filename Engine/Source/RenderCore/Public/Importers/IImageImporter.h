//
// Created by Egor Orachyov on 22.02.2019.
//

#ifndef BERSERK_IIMAGEIMPORTER_H
#define BERSERK_IIMAGEIMPORTER_H

#include "Misc/Types.h"
#include "Platform/IRenderDriver.h"

namespace Berserk::Importers
{

#ifndef PROFILE_IMAGE_IMPORTER
    #define PROFILE_IMAGE_IMPORTER 0
#endif

    /**
     * Abstract image importer class, which allows to import and expoort
     * loaded or generated images from RenderSystem to os HHD
     */
    class IImageImporter
    {
    public:

        /**
         * Image data needed for loading and processing image
         * (textures, maps, etc.)
         */
        struct ImageData
        {
            uint32 width = 0;
            uint32 height = 0;

            void* buffer = nullptr;

            Resources::IRenderDriver::PixelFormat   pixelFormat;
            Resources::IRenderDriver::DataType      pixelType;
            Resources::IRenderDriver::StorageFormat storageFormat;
        };

        struct ImageSave
        {
            uint32 width = 0;
            uint32 height = 0;

            void* buffer = nullptr;
        };

    public:

        /** Release import and free resources if needed */
        virtual ~IImageImporter() = default;

        /** @return True if extension is supported by importer */
        virtual bool isReadingSupported(const char *name) = 0;

        /** @return True if extension is supported by importer */
        virtual bool isWritingSupported(const char *name) = 0;

        /** @return True if image successfully loaded and writes its data in image data structure */
        virtual bool import(const char* name, ImageData &data) = 0;

        /** @return True if successfully save image from data */
        virtual bool save(const char *name, const ImageSave &data) = 0;

        /** Unload all the internal data (if it was allocated by importer) */
        virtual void unload() = 0;

        /** @return Memory cost of this resource (on CPU side only) */
        virtual uint32 getMemoryUsage() = 0;

    };

} // namespace Berserk::Importers

#endif //BERSERK_IIMAGEIMPORTER_H