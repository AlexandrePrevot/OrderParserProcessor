#include "processors/script_submit_processor.h"

#include <iostream>
#include <string>

#include "antlr/FiScriptLexer.h"
#include "antlr/FiScriptParser.h"

bool ScriptSubmitProcessor::Process(
    const internal::ScriptSubmitRequest &script_submit_request) {


  std::string salut = "REACT(180)";
  antlr4::ANTLRInputStream input(salut);
  FiScriptLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  FiScriptParser parser(&tokens);

  FiScriptParser::FirstContext* tree = parser.first();

  std::cout << "processing script submission in the name of "
            << script_submit_request.user() << std::endl;

  return true;
}
