import sys
import os
from contextlib import asynccontextmanager

root_dir = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
proto_gen_dir = os.path.abspath(os.path.join(root_dir, "generated/python/"))
sys.path.insert(0, proto_gen_dir)


from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from routes.views import router, script_to_api_handler, distributor_to_api_handler


@asynccontextmanager
async def lifespan(app: FastAPI):
    await script_to_api_handler.start()
    await distributor_to_api_handler.start_streaming()
    print("FastAPI app started with gRPC server and client")

    yield

    await script_to_api_handler.stop()
    await distributor_to_api_handler.stop()
    print("FastAPI app shutdown complete")


def create_app() -> FastAPI:
    app = FastAPI(lifespan=lifespan)

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
