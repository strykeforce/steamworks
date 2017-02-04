#include "log.h"

using namespace steamworks::command;

Log::Log(std::string message)
    : InstantCommand("Log"),
      logger_(spdlog::get("command")),
      message_(message) {}

void Log::Initialize() { logger_->info(message_); }
