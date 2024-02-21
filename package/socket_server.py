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
        self.initBuffer = json.dumps({"type": "display", "msg": {"dtype": 1, "component": {"table": {"types": [{"zoomable": "none", "type": "string", "sortable": False}, {"zoomable": "none", "type": "string", "sortable": False}, {"zoomable": "none", "type": "string", "sortable": True}, {"zoomable": "normal", "type": "string", "sortable": False}], "headers": ["Rank", "Name", "Binding Energy (kcal/mol)", "SMILES"], "rows": []}, "controls": {"show_run": True, "show_stop": True}, "start_time": f"{self.currentTime}", "status": "RUNNING", "version": "0.0", "progress": 1.0, "messages": ["Generating ligand library..."]}}})
        self.runningModule = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom..."]}}})
        self.progress = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom...", "Converting the receptor protein..."]}}})
        self.progressComplete = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom...", "Converting the receptor protein...", "Generating grid parameter files (GPF)..."]}}})
        self.moduleComplete = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom...", "Converting the receptor protein...", "Generating grid parameter files (GPF)...", "Generating map files for the pocket..."]}}})
        self.test1 = json.dumps({"type": "display", "msg": {"component": {"progress": 1}}})
        self.test2 = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom...", "Converting the receptor protein...", "Generating grid parameter files (GPF)...", "Generating map files for the pocket...", "Docking ligand FDA1802..."]}}})
        self.test3 = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom...", "Converting the receptor protein...", "Generating grid parameter files (GPF)...", "Generating map files for the pocket...", "Docking ligand FDA1802...", "Docking ligand FDA560..."]}}})
        self.test4 = json.dumps({"type": "display", "msg": {"component": {"messages": ["Generating ligand library...", "Remove heterogeneous atom...", "Converting the receptor protein...", "Generating grid parameter files (GPF)...", "Generating map files for the pocket...", "Docking ligand FDA1802...", "Docking ligand FDA560..."]}}})
    async def handle_client(self, websocket: WebSocketServerProtocol) -> None:
        try:
            async for _ in websocket:
                #TODO: add display.json data here
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
                await asyncio.sleep(4)
                await websocket.send(self.test1)
                await asyncio.sleep(4)
                await websocket.send(self.test2)
                await asyncio.sleep(4)
                await websocket.send(self.test3)
                await asyncio.sleep(4)
                await websocket.send(self.test4)

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
    server = RunSocketBackend(host="0.0.0.0", port=port)
    asyncio.run(server.start_server())
