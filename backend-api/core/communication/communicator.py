import asyncio
import grpc
import json
from google.protobuf import empty_pb2

import services.api_to_core_pb2_grpc
import services.script_to_api_pb2_grpc
import services.marketdata_pb2_grpc

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


class DistributorToApiHandler:

    def __init__(self, notif_callback):
        self.channel = grpc.aio.insecure_channel('localhost:50052')
        self.stub = services.marketdata_pb2_grpc.MarketDataServiceStub(self.channel)
        self.notif_callback = notif_callback
        self._stream_task = None

    async def start_streaming(self):
        self._stream_task = asyncio.create_task(self._listen())

    async def _listen(self):
        try:
            stream = self.stub.StreamPrices(empty_pb2.Empty())
            async for price_update in stream:
                data = {}
                data['MessageType'] = 'price_update'
                data['price'] = price_update.price
                data['quantity'] = price_update.quantity
                await self.notif_callback(json.dumps(data))
        except grpc.RpcError as err:
            print(f"StreamPrices error: {err}")

    async def stop(self):
        if self._stream_task:
            self._stream_task.cancel()
        await self.channel.close()


class ScriptToApiServicer(services.script_to_api_pb2_grpc.ScriptToApiServicer):
    """gRPC servicer to receive notifications from scripts"""

    def __init__(self, notif_callback):
        self.notif_callback = notif_callback

    async def ScriptAlert(self, request, context):
        data = {}
        data['MessageType'] = 'script_alert'
        data['script_title'] = request.script_title
        data['user'] = request.user
        data['message'] = request.message
        data['priority'] = 'HIGH' if request.priority == 1 else 'MID'
        print("Received ScriptAlert:")
        print(f"Script Title: {request.script_title}")
        print(f"User: {request.user}")
        print(f"Message: {request.message}")
        print(f"Priority: {data['priority']}")
        str_data = json.dumps(data)

        await self.notif_callback(str_data)

        return empty_pb2.Empty()


class ScriptToApiHandler:
    """Handler to manage the gRPC server lifecycle"""

    def __init__(self, notif_callback):
        self.notif_callback = notif_callback
        self.server = None

    async def start(self):
        """Start the gRPC server"""
        self.server = grpc.aio.server()
        services.script_to_api_pb2_grpc.add_ScriptToApiServicer_to_server(
            ScriptToApiServicer(self.notif_callback), self.server)
        self.server.add_insecure_port('[::]:50053')
        await self.server.start()
        print("gRPC ScriptToApi server started on port 50053")

    async def wait_for_termination(self):
        """Wait for server to terminate"""
        if self.server:
            await self.server.wait_for_termination()

    async def stop(self):
        """Stop the gRPC server gracefully"""
        if self.server:
            await self.server.stop(grace=5)
            print("gRPC server stopped")


class WebSocketManager:

    def __init__(self):
        self.active_connections = set()

    async def connect(self, websocket):
        await websocket.accept()
        self.active_connections.add(websocket)
        print(f"WebSocket connected: {websocket.client}")

    def disconnect(self, websocket):
        self.active_connections.discard(websocket)
        print(f"WebSocket disconnected: {websocket.client}")

    async def send_personal_message(self, message: str, websocket):
        print(f"Sending message to {websocket.client}: {message}")
        await websocket.send_text(message)

    async def broadcast(self, message: str):
        """Send message to ALL connected clients"""
        disconnected = set()
        for connection in self.active_connections:
            try:
                await connection.send_text(message)
            except Exception:
                disconnected.add(connection)

        for conn in disconnected:
            self.active_connections.discard(conn)
