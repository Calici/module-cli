import asyncio
from websockets.server import serve

class RunSocketBackend:
    def __init__(self, host="localhost", port=8765):
        self.host = host
        self.port = port
        self.server = None

    async def handle_client(self,websocket):
        connected_message="Connected to the socket successfully."
        await websocket.send(connected_message)

        async for message in websocket:
            print(f"Message: {message}")

        #TODO: add manfiest.json data here

    async def start_server(self):
        self.server = await serve(self.handle_client, self.host, self.port)
        await asyncio.sleep(5)
        await self.stop_server()

    async def stop_server(self):
        if self.server:
            self.server.close()
            await self.server.wait_closed()

if __name__ == "__main__":
    server = RunSocketBackend()
    asyncio.run(server.start_server())