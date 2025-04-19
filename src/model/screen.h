#pragma once 

#include "QGuiApplication"
#include "QScreen"
#include "QRect"

#include <cstdint>

namespace model {

class Screen {
public:
    Screen(const Screen& other) = delete;
    static int32_t getMinSize();
private:
    Screen() = default;
    static Screen m_instance;
};

} // namespace model