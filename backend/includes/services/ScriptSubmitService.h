

class ScriptSubmitService {

/*
    after generating protofiles
    
    1 - instantiate gRPC ScriptSubmitService when building
    
    2 - make a settable function pointer to call when receiving message
    make a very simple ScriptSubmitProcessor class in /processor
    that will instantiate the ScriptSubmitProcessor that will return a SynchronousReply

*/

private:
  long long call_count_;
  long long failed_call_count_;
  bool ready_to_serve_;
};
