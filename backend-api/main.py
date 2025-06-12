from fastapi import FastAPI, Depends
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware


from models import Order
from database import engine, SessionLocal
from sqlalchemy.orm import Session

app = FastAPI()

# CORS setup: allow React frontend (Vite runs on port 5173 by default)
origins = [
    "http://localhost:5173",
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

Order.metadata.create_all(engine)

@app.get('/')
def index():
    return {"data" : {'name' : 'Alexandre'} }

#should be added before '/fatima/{last_name}' otherwise it will never match
@app.get('/fatima/problems')
def problem():
    print("this is problems")
    return "this is problems"


class Request(BaseModel):
    id: str
    qty: float
    price: float

@app.get('/fatima/{last_name}')
def fatima(last_name):
    print("this is fatima " + last_name)
    return "this is fatima " + last_name

@app.get('/exemple')
def exemple(name: str, age: int):
    return "my name is " + name + " and I am " + str(age)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

#this is a post request
# it is a post request because it needs a body in the request to create
# the Request structure
@app.post('/request')
def create_order(request: Request, db: Session = Depends(get_db)):
    print("creating a new order")
    new_order = Order(ID=request.id, Quantity=request.qty, Price=request.price)
    db.add(new_order)
    db.commit()
    db.refresh(new_order)
    print("created and persisted new order done")
    return "request is created from " + request.id
