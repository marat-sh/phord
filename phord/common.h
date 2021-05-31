#ifndef COMMON_H
#define COMMON_H

#include <cstddef>

// TODO make thumbnails params optional

const int THM_IMG_WIDTH = 33*4; // thumbnail's image width
const int THM_IMG_HEIGHT = 33*3; // thumbnail's image height
const int THM_NAME_HEIGHT = 30; // file name line height
const int THM_FULL_WIDTH = THM_IMG_WIDTH;
const int THM_FULL_HEIGHT = THM_IMG_HEIGHT + THM_NAME_HEIGHT;

constexpr std::size_t imgSize (int width, int height)
{
    return static_cast<std::size_t>(width * height * 4);
}

// This memory is used to store unused items.
const std::size_t MAX_CACHE_MEM_SZ = 64*1024*1024;

#define THM_DRAG_DROP_MIME_TYPE QStringLiteral("application/image.info")

#endif // COMMON_H
