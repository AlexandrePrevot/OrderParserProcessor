#include "processors/script_submit_processor.h"
#include "messages/script_submit.pb.h"
#include <iostream>

int main() {
    ScriptSubmitProcessor processor;
    script_submit::ScriptSubmitRequest req;

    req.set_content("myFloat = 18.5\nPrint(\"Done\")");
    req.set_title("test");
    req.set_summary("test");
    req.set_user("test");

    if (processor.Process(req)) {
        std::cout << "Compilation successful!" << std::endl;
    } else {
        std::cout << "Compilation failed!" << std::endl;
    }

    return 0;
}
