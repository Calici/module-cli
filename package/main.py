import subprocess
import webbrowser
import socket
from contextlib import closing

def check_port_availability(host: str, port: int) -> bool:
    with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
        return sock.connect_ex((host, port)) == 0

if __name__ == "__main__":
    frontend_ports = [8080, 8000, 3000, 5000, 9000, 8888, 8081, 8090, 8181, 8887]
    server_ports = [8000, 8080, 3000, 5000, 8081, 8888, 9090, 3001, 3030, 4000]
    frontend_port = None
    server_port = None

    for port in frontend_ports:
        print(f"Frontend trying port {port}")
        available = check_port_availability("localhost",port)
        if available:
            frontend_port = port
            break

    for port in server_ports:
        print(f"Server trying port {port}")
        available = check_port_availability("localhost",port)
        if available and port != frontend_port:
            server_port = port
            break

    with open(".env", "w") as env_file:
        env_file.write(f"REACT_APP_WS_ENDPOINT=ws://localhost:{server_port}/")

    commands = [
        f"python server.py --port {server_port}",
        f"python -m http.server {frontend_port} --directory frontend"
    ]

    processes = []
    for cmd in commands:
        processes.append(subprocess.Popen(cmd, shell=True))

    webbrowser.open(f"http://localhost:{frontend_port}", new=1)

    for process in processes:
        process.wait()
