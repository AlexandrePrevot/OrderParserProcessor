#pragma once

#include <memory>

#include "services/script_submit_service.h"
#include "processors/script_submit_processor.h"

class ScriptSubmitHandler {
public:
  ScriptSubmitHandler();

  ScriptSubmitService* GetScriptSubmitServicePtr() {
    return service_.get();
  }
private:

  std::unique_ptr<ScriptSubmitService> BuildService();
  std::unique_ptr<ScriptSubmitProcessor> BuildProcessor();

  std::unique_ptr<ScriptSubmitService> service_;
};
