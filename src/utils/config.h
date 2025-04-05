#pragma once

#ifndef NO_SPDLOG
  #include "spdlog/spdlog.h"

  #define DEBUG_LEVEL spdlog::level::debug
  #define INFO_LEVEL spdlog::level::info
  #define WARN_LEVEL spdlog::level::warn
  #define ERR_LEVEL spdlog::level::err
  #define LOGGING_OFF spdlog::level::off

  #define SET_LOG_PRIORITY(x) spdlog::set_level(x);
  #define SET_LOG_PATTERN(x) spdlog::set_pattern(x);

  #define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
  #define LOG_INFO(...) spdlog::info(__VA_ARGS__)
  #define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
  #define LOG_ERR(...) spdlog::error(__VA_ARGS__)

#else
  
  #define DEBUG_LEVEL
  #define INFO_LEVEL
  #define WARN_LEVEL
  #define ERR_LEVEL
  #define LOGGING_OFF

  #define SET_LOG_PRIORITY(x)
  #define SET_LOG_PATTERN(x)

  #define LOG_DEBUG(...)
  #define LOG_INFO(...)
  #define LOG_WARN(...)
  #define LOG_ERR(...)

#endif// DEBUG