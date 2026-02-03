#include "Log.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

using namespace App;
using namespace std;
using namespace spdlog;

Log::Log ()
{
    vector<sink_ptr> log_sinks;

    const level::level_enum level{level::debug};

    log_sinks.emplace_back (make_shared<sinks::stdout_color_sink_mt>());
    log_sinks.emplace_back (make_shared<sinks::msvc_sink_mt>());
    log_sinks.emplace_back (make_shared<sinks::basic_file_sink_mt> ("app.log", true));

    log_sinks [0]->set_pattern ("%^[%T] %n(%l): %v%$");
    log_sinks [1]->set_pattern ("%^[%T] %n(%l): %v%$");
    log_sinks [2]->set_pattern ("[%T] [%l] %n(%l): %v");

    m_logger = make_shared<spdlog::logger> ("APP", begin (log_sinks), end (log_sinks));
    register_logger (m_logger);
    set_default_logger (m_logger);
    m_logger->set_level (level);
    m_logger->flush_on (level);
}
