from module_api.cmd.base import \
    ModuleLock, \
    Container
from module_api.API.patterns import WithPattern as Cleaner
from .base import \
    TestBase, \
    SERVER_FILE
from module_api.API.test import random_string
from module_api.API.lock import CaliciLock
from typing import List
import subprocess
import pathlib
class TestAll(TestBase):
    def __init__(self, lock : ModuleLock):
        super().__init__(lock)
        self.container = Container(lock)
        self.prepare_run()

    def get_api_token(self) -> str:
        try:
            return self.api_token
        except AttributeError:
            self.api_token = random_string(20)
            return self.api_token
    
    def prep_test(self) -> subprocess.Popen:
        subprocess.run(["docker", "network", "create", self.network_name()])
        process = subprocess.Popen([
            "docker", "run",
            "--rm", "--name", self.server_name(),
            "--network", self.network_name(),
            "-v", "{0}:{1}".format(str(SERVER_FILE),'/app/server.py'),
            "-e", f"PORT={self.port()}",
            "python:3.11-slim", 
            "python3",
            "/app/server.py"
        ])
        return process

    def cleanup_test(self, process : subprocess.Popen):
        process.kill()
        process.wait()
        subprocess.run(["docker", "kill", self.server_name()])
        subprocess.run(["docker", "network", "rm", self.network_name()])
    
    def server_name(self):
        try:
            return self.__server_name
        except AttributeError:
            self.__server_name = random_string(10)
            return self.__server_name

    def network_name(self):
        try:
            return self.__network_name
        except AttributeError:
            self.__network_name = random_string(10)
            return self.__network_name

    def port(self) -> int:
        return 65000
    
    def nvidia_arg_or_nothing(self, test_path : pathlib.Path) -> List[str]:
        lock_path = test_path / 'workdir' / '.reserved' / 'default.lock'
        lock = CaliciLock(lock_path)
        gpus = lock.header.gpu_blocks.get()
        if len(gpus) == 0:
            return []
        else:
            gpu_devices = ",".join([
                str(gpu.gpu_id.get()) for gpu in gpus
            ])
            return [
                "-e", f"CUDA_VISIBLE_DEVICES={gpu_devices}",
                "--runtime=nvidia",
                "--privileged"
            ]

    def prepare_run(self):
        port = self.port()
        server_name = self.server_name()
        root_dir = self.lock.module.root_dir.get()
        self.run_args = {
            test.name.get() : [
                *self.container.get_env_args({
                    'DJANGO_API_ENDPOINT' : f'http://{server_name}:{port}',
                    'DJANGO_API_TOKEN' : self.get_api_token()
                }), 
                *self.container.get_volume_args([
                    '{0}:{1}'.format(
                        str(root_dir / test.path.get()), 
                        str(self.container_root() / test.path.get().name)
                    )
                ]),
                "--network={0}".format(self.network_name()),
                "--rm", "-it", *self.nvidia_arg_or_nothing(test.path.get())
            ]
            for test in self.lock.testing.get()
        }

    def run_process(self, name : str, test_path : pathlib.Path):
        process = self.container.run(
            self.run_args[name], 
            [
                "--lock", 
                str(
                    self.container_root() /\
                    test_path.name /\
                    'workdir' /\
                     '.reserved' /\
                    'default.lock'
                )
            ]
        )
        process.wait()

    def action(self):
        tests = self.lock.testing.get()
        if len(tests) == 0:
            print("No Tests")
            return
        with Cleaner(self.prep_test, self.cleanup_test):
            for test in self.lock.testing.get():
                name = test.name.get()
                print(f"Running test : {name}")
                self.run_process(name, test.path.get())