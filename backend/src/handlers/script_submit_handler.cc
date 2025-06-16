#include "handlers/script_submit_handler.h"

ScriptSubmitHandler::ScriptSubmitHandler() { service_ = BuildService(); }

std::unique_ptr<ScriptSubmitService> ScriptSubmitHandler::BuildService() {
  std::unique_ptr<ScriptSubmitService> service =
      std::make_unique<ScriptSubmitService>();

  service->SetProcessorFunction(
      [this](const internal::ScriptSubmitRequest &request) {
        const auto processor = BuildProcessor();
        processor->Process(request);

        return processor->GetSynchronousReply();
      });

  return service;
}

std::unique_ptr<ScriptSubmitProcessor> ScriptSubmitHandler::BuildProcessor() {
  return std::make_unique<ScriptSubmitProcessor>();
}
