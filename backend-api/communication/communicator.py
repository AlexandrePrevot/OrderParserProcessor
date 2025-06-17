import grpc

import services.api_to_core_pb2_grpc
import services.api_to_core_pb2
import messages.script_submit_pb2


class ApiToCoreHandler:

    def __init__(self):
        self.channel = grpc.insecure_channel('localhost:50051')
        self.stub = services.api_to_core_pb2_grpc.ApiToCoreStub(self.channel)

    def ScriptSubmit(self):
        script_submit_req = messages.script_submit_pb2.ScriptSubmitRequest()
        script_submit_req.content = "content"
        script_submit_req.title = "title"
        script_submit_req.summary = "summary"
        script_submit_req.user = "user"

        reply = None

        try:
            reply = self.stub.ScriptSubmit(script_submit_req)
        except grpc.RpcError as err:
            print("communication with B.E. failed : " + str(err))

        print("communication finished")
