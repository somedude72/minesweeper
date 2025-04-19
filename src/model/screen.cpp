#include "model/screen.h"

#include <algorithm>
#include <cstdlib>

namespace model {

int32_t Screen::getMinSize() {
    const QRect screen_size = QGuiApplication::primaryScreen()->geometry();
    return std::min(screen_size.height(), screen_size.width());
}

Screen Screen::m_instance = Screen();

} // namespace model