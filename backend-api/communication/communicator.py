import grpc

import services.api_to_core_pb2_grpc
import services.api_to_core_pb2
import messages.script_submit_pb2


class ApiToCoreHandler:

    def __init__(self):
        self.channel = grpc.insecure_channel('localhost:50051')
        self.stub = services.api_to_core_pb2_grpc.ApiToCoreStub(self.channel)

    def ScriptSubmit(self, db_script_submit):
        script_submit_req = messages.script_submit_pb2.ScriptSubmitRequest()
        script_submit_req.content = db_script_submit.Content
        script_submit_req.title = db_script_submit.Title
        script_submit_req.summary = db_script_submit.Summary
        script_submit_req.user = db_script_submit.User

        reply = None

        print("trying to submit script")
        try:
            reply = self.stub.ScriptSubmit(script_submit_req)
        except grpc.RpcError as err:
            print("Script submission failed : " + str(err))

        print("communication finished")
