from fastapi import APIRouter
from pydantic import BaseModel

from models import AlgoScript
from core.communication.communicator import ApiToCoreHandler


router = APIRouter()

my_handler = ApiToCoreHandler()


@router.get('/')
def index():
    return {"data": {'name': 'Alexandre'}}


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
    my_handler.ScriptSubmit(algo_script)
    return "Script is created from User " + algo_script.User
