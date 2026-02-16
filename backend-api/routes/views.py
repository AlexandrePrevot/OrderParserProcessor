from fastapi import APIRouter, WebSocket
from pydantic import BaseModel
import asyncio

from models import AlgoScript
from core.communication.communicator import ApiToCoreHandler, DistributorToApiHandler, ScriptToApiHandler, WebSocketManager
from core.process_manager import ProcessManager


router = APIRouter()

websocket_queue = asyncio.Queue()


async def notif_callback(notification):
    """ Callback function to handle notifications from scripts or other sources, 
        expects a stringified JSON """
    print(f"Received notification: {notification}")
    await websocket_queue.put(notification)

api_to_core_handler = ApiToCoreHandler()
script_to_api_handler = ScriptToApiHandler(notif_callback)
distributor_to_api_handler = DistributorToApiHandler(notif_callback)
websocket_manager = WebSocketManager()
process_manager = ProcessManager()


@router.get('/')
def index():
    return {"data": {'name': 'Alexandre'}}


@router.websocket('/ws')
async def websocket_endpoint(websocket: WebSocket):
    await websocket_manager.connect(websocket)
    try:
        while True:
            # necessary when client disconnects, so that
            # a task is not awaken when filling the queue
            # on a disconnected websocket, which would raise an error
            queue_task = asyncio.create_task(websocket_queue.get())
            recv_task = asyncio.create_task(websocket.receive())

            done, pending = await asyncio.wait(
                {queue_task, recv_task},
                return_when=asyncio.FIRST_COMPLETED,
            )

            for task in pending:
                # if not cancelled, it will keep waiting
                # for the next message and never check the queue again
                # it avoids "leaks"
                task.cancel()

            if recv_task in done:
                msg = await recv_task
                if msg["type"] == "websocket.disconnect":
                    break

            if queue_task in done:
                notification = await queue_task
                await websocket_manager.send_personal_message(
                    notification,
                    websocket
                )
    except Exception as e:
        print(f"WebSocket error: {e}")
    finally:
        websocket_manager.disconnect(websocket)


class AlgoScriptRequest(BaseModel):
    user: str
    title: str
    summary: str
    content: str


@router.post('/ScriptRequest')
def create_script(script_request: AlgoScriptRequest):
    print("trying to create a new algo script")
    algo_script = AlgoScript(User=script_request.user,
                             Title=script_request.title,
                             Summary=script_request.summary,
                             Content=script_request.content)
    print("created a new algo (check db)")
    api_to_core_handler.ScriptSubmit(algo_script)
    return "Script is created from User " + algo_script.User


class ActivateScriptRequest(BaseModel):
    user: str
    title: str


@router.post('/ActivateScript')
def activate_script(request: ActivateScriptRequest):
    try:
        now_active = process_manager.toggle(request.user, request.title)
        return {
            "status": "activated" if now_active else "deactivated",
            "user": request.user,
            "title": request.title,
            "active": now_active,
        }
    except FileNotFoundError as e:
        from fastapi.responses import JSONResponse
        return JSONResponse(status_code=404, content={"error": str(e)})
    except RuntimeError as e:
        from fastapi.responses import JSONResponse
        return JSONResponse(status_code=400, content={"error": str(e)})
