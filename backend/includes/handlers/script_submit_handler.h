#pragma once

#include <memory>

#include "services/script_submit_service.h"
#include "processors/script_submit_processor.h"

class ScriptSubmitHandler {
public:
  /*
      after generating protofiles

      1 - instantiate gRPC ScriptSubmitService when building

      2 -
      make a settable function pointer to call when receiving message
      make a very simple ScriptSubmitProcessor class in /processor

      that will instantiate the ScriptSubmitProcessor that will
      return a SynchronousReply
  */

  ScriptSubmitHandler();
private:

  std::unique_ptr<ScriptSubmitService> BuildService();
  std::unique_ptr<ScriptSubmitProcessor> BuildProcessor();

  std::unique_ptr<ScriptSubmitService> service_;
};
