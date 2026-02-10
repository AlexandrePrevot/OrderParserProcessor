#include "processors/script_submit_processor.h"

#include <iostream>
#include <string>

#include "antlr/FiScriptLexer.h"
#include "antlr/FiScriptParser.h"
#include "processors/visitors/concrete_fiscript_visitor.h"
#include "processors/visitors/file_maker.h"

bool ScriptSubmitProcessor::Process(
    const internal::ScriptSubmitRequest &script_submit_request) {

  const bool parsed = Parse(script_submit_request.content(),
                            script_submit_request.user(),
                            script_submit_request.title());

  if (!parsed) {
    std::cout << "could not compile given code" << std::endl;
  } else {
    std::cout << "successfully compiled given code" << std::endl;
  }

  return parsed;
}

bool ScriptSubmitProcessor::Parse(const std::string &code,
                                   const std::string &username,
                                   const std::string &script_title) {
  ConcreteFiScriptVisitor visitor;
  if (!visitor.Compile(code)) {
    return false;
  }

  const auto &commands = visitor.get_commands_list();

  FileMaker maker(commands, username, script_title);

  if (!maker.Compiled()) {
    return false;
  }

  maker.GenerateScript();

  return true;
}