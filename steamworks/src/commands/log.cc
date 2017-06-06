#include "log.h"

using namespace steamworks::command;

LogCommand::LogCommand(std::string message)
    : frc::InstantCommand("LogCommand"),
      logger_(spdlog::get("command")),
      message_(message) {}

void LogCommand::Initialize() { logger_->info(message_); }
