#include <QGuiApplication>
#include <QScreen>
#include <QRect>

#include <algorithm>
#include <cstdint>

#include "model/screen.h"

int32_t minScreenSize() {
    const QRect screen_size = QGuiApplication::primaryScreen()->geometry();
    const int32_t ret = std::min(screen_size.height(), screen_size.width());
    return ret;
}
