from typing import TypedDict, Union
import asyncio
from websockets.server import serve, WebSocketServerProtocol
from websockets import ConnectionClosed

class Display(TypedDict):
    pass

class JSONSocket(TypedDict):
    pass
class CaliciSocketStateInit(TypedDict):
    wsConnectCount: int
    wsDisplay: Display
    wsData: dict[str, Union[str, int]]
    wsInit: bool
    wsQueue: int
    socket: JSONSocket
    wsState: str

class RunSocketBackend:
    def __init__(self, host="localhost", port=8765) -> None:
        self.host = host
        self.port = port
        self.server:  WebSocketServerProtocol = None

    async def handle_client(self, websocket: WebSocketServerProtocol):
        connected_message="Connected to the socket successfully."
        await websocket.send(connected_message)

        try:
            async for message in websocket:
                print(f"Recieved message: {message}")
                await websocket.send(message)
        except ConnectionClosed:
            print("Client disconnected")

        async for message in websocket:
            print(f"Message: {message}")

        #TODO: add manfiest.json data here

    async def start_server(self) -> None:
        self.server = await serve(self.handle_client, self.host, self.port)
        print(f"Server started at {self.host}:{self.port}")
        await self.server.wait_closed()
        # await asyncio.sleep(500)
        # await self.stop_server()

    async def stop_server(self) -> None:
        if self.server:
            self.server.close()
            await self.server.wait_closed()

if __name__ == "__main__":
    server = RunSocketBackend()
    asyncio.run(server.start_server())
