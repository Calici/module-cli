import subprocess
import webbrowser
import time

if __name__ == "__main__":
    frontend_port = 3000
    server_port = 8765

    with open(".env", "w") as env_file:
        env_file.write(f"REACT_APP_WS_ENDPOINT=ws://[::1]:{server_port}")

    commands = [
        f"python server.py --port {server_port}",
        f"python -m http.server {frontend_port} --directory Frontend"
    ]

    processes = []
    for cmd in commands:
        processes.append(subprocess.Popen(cmd, shell=True))

    webbrowser.open(f"http://localhost:{frontend_port}", new=1)

    FLAG = True

    while FLAG:
        time.sleep(3)
        for process in processes:
            return_code = process.poll()
            if return_code is not None:
                for proc in processes:
                    proc.terminate()
                    time.sleep(1)
                FLAG = False
                break
    print("Testing Finished!")
