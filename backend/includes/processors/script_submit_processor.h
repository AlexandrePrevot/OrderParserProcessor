#pragma once

#include "messages/script_submit.pb.h"
class ScriptSubmitProcessor {
public:
  bool Process(const internal::ScriptSubmitRequest &script_submit_request);
};