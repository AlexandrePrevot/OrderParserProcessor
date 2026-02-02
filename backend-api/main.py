import sys
import os

root_dir = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
proto_gen_dir = os.path.abspath(os.path.join(root_dir, "generated/python/"))
sys.path.insert(0, proto_gen_dir)


from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from routes.views import router


def create_app() -> FastAPI:
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

    app.include_router(router)

    return app


app = create_app()


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
