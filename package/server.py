import argparse
import asyncio
from websockets.server import serve, WebSocketServerProtocol
from websockets import ConnectionClosed
import json
import datetime

class RunSocketBackend:
    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port
        self.server:  WebSocketServerProtocol = None
        self.currentTime = f"{datetime.datetime.now()}"
        self.initialData = json.dumps({"type": "auth", "auth": True, "msg": "Authentication Success"})
        self.queue = json.dumps({"type": "ws_queue", "msg": "This module will run shortly", "queue_num": 0})
        self.validateAuthentication = json.dumps({"type": "ok", "msg": "Authentication Successful"})
        self.initBuffer = json.dumps({"type": "display", "msg": {"component": {"version": "1.0", "controls": {"show_stop": True, "show_run": True}, "messages": [], "status": "INIT", "time": {"timeDelta": 1, "startTime": self.currentTime}, "progress": {"value": 0.0, "bouncy": True}}, "dtype": 0}})
        self.runningModule = json.dumps({"type": "display", "msg": {"component": {"status": "RUNNING"}}})
        self.progress = json.dumps({"type": "display", "msg": {"component": {"progress": {"value": 20}}}})
        self.progressComplete = json.dumps({"type": "display", "msg": {"component": {"progress": {"value": 100}}}})
        self.moduleComplete = json.dumps({"type": "display", "msg": {"component": {"status": "COMPLETE"}}})

    async def handle_client(self, websocket: WebSocketServerProtocol) -> None:
        try:
            async for _ in websocket:
                #TODO: add manfiest.json data here
                await websocket.send(self.initialData)
                await asyncio.sleep(0.5)
                await websocket.send(self.queue)
                await asyncio.sleep(0.5)
                await websocket.send(self.validateAuthentication)
                await asyncio.sleep(0.5)
                await websocket.send(self.initBuffer)
                await asyncio.sleep(0.5)
                await websocket.send(self.runningModule)
                await asyncio.sleep(1)
                await websocket.send(self.progress)
                await asyncio.sleep(2)
                await websocket.send(self.progressComplete)
                await asyncio.sleep(4)
                await websocket.send(self.moduleComplete)

        except ConnectionClosed:
            print("Client disconnected")

    async def start_server(self) -> None:
        self.server = await serve(self.handle_client, self.host, self.port)
        print(f"Server started at {self.host}:{self.port}")
        await asyncio.sleep(10)
        await self.stop_server()

    async def stop_server(self) -> None:
        if self.server:
            self.server.close()
            await self.server.wait_closed()
            print("Backend server closed!")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Start WebSocket server.')
    parser.add_argument('--port', type=int, help='Port number to use for the WebSocket server.')
    args = parser.parse_args()
    port = args.port
    server = RunSocketBackend(host="localhost", port=port)
    asyncio.run(server.start_server())
