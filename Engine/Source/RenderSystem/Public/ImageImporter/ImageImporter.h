//
// Created by Egor Orachyov on 22.02.2019.
//

#ifndef BERSERK_IMAGEIMPORTER_H
#define BERSERK_IMAGEIMPORTER_H

#include "Misc/Types.h"

namespace Berserk
{

    /**
     * Abstract image importer class, which allows to import and expoort
     * loaded or generated images from RenderSystem to os HHD
     */
    class ImageImporter
    {
    public:

        /**
         * Order of color components in the loaded (exported image)
         */
        enum ImageFormat
        {
            RGB,
            BGR,
            RGBA,
            BGRA
        };

        /**
         * Types of values in the buffer array
         */
        enum PixelType
        {
            UNSIGNED_INT,
            RGBF,
            RGBAF

        };

        /**
         * Image data needed for saving and processing image
         * (textures, maps, etc.)
         */
        struct ImageData
        {
            uint32 width;
            uint32 height;

            void* buffer;

            enum ImageFormat format;
            enum PixelType pixelType;
        };

    public:

        /** Initialize internal importer core */
        virtual void initialize() = 0;

        /** Release import and free resources if needed */
        virtual void release() = 0;

        /** @return True if extension is supported by importer */
        virtual bool isReadingSupported(const char *name) = 0;

        /** @return True if extension is supported by importer */
        virtual bool isWritingSupported(const char *name) = 0;

        /** @return True if image successfully loaded and writes its data in image data structure */
        virtual bool import(const char* name, ImageData& data) = 0;

        /** @return True if successfully save image from data */
        virtual bool save(const char *name, const ImageData &data) = 0;

        /** Unload all the internal data (if it was allocated by importer) */
        virtual void unload() = 0;

    };

} // namespace Berserk

#endif //BERSERK_IMAGEIMPORTER_H