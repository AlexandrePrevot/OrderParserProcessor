#include "handlers/script_submit_handler.h"


ScriptSubmitHandler::ScriptSubmitHandler() { BuildService(); }

void ScriptSubmitHandler::BuildService() {
  service_ = std::make_unique<ScriptSubmitService>();
}
