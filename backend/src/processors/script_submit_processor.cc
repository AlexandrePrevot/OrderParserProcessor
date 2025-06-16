#include "processors/script_submit_processor.h"

#include <iostream>

bool ScriptSubmitProcessor::Process(
    const internal::ScriptSubmitRequest &script_submit_request) {

  std::cout << "processing script submission in the name of "
            << script_submit_request.user() << std::endl;
  return true;
}
