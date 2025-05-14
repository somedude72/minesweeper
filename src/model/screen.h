#pragma once 

#include <cstdint>

#include <QGuiApplication>
#include <QScreen>
#include <QRect>

// returns min(screen_pixel_height, screen_pixel_width)
inline int32_t minScreenSize() {
    const QRect screen_size = QGuiApplication::primaryScreen()->geometry();
    const int32_t ret = std::min(screen_size.height(), screen_size.width());
    return ret;
}