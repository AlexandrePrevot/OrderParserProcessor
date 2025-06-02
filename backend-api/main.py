from fastapi import FastAPI

app = FastAPI()

@app.get('/')
def index():
    return {"data" : {'name' : 'Alexandre'} }

#should be added before '/fatima/{last_name}' otherwise it will never match
@app.get('/fatima/problems')
def problem():
    return "this is problems"

@app.get('/fatima/{last_name}')
def fatima(last_name):
    return "this is fatima " + last_name

@app.get('/exemple')
def exemple(name : str, age : int):
    return "my name is " + name + " and I am " + str(age)