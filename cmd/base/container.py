from .lock import ModuleLock
from typing_extensions import \
    List, \
    Dict
import pathlib
import subprocess

class Container:
    def __init__(self, lock : ModuleLock):
        self.lock = lock

    def map_dir(self, host : str, client : str) -> str:
        root_dir = self.lock.module.root_dir.get()
        host_path = pathlib.Path(host)
        if host_path.is_absolute():
            return f'{host}:{client}'
        else:
            return '{0}:{1}'.format(str(root_dir / host), client)
    
    def get_volume_args(self, additional_volumes : List[str] = []) -> List[str]:
        all_volumes = self.lock.docker.volumes.serialize() + additional_volumes
        return [
            entry 
            for volume in all_volumes
            for entry in ["-v", self.map_dir(*volume.split(':'))]
        ]

    def get_env_args(self, env_args : Dict[str, str] = {}) -> List[str]:
        lock_envs = [ 
            entry.split('=') 
            for entry in self.lock.docker.environments.serialize()
        ]
        all_envs = {
            name : value for name, value in lock_envs
        } | env_args
        return [
            entry
            for env_name, env_value in all_envs.items()
            for entry in ["-e", f"{env_name}={env_value}"]
        ]
    def run(self, 
        run_args : List[str] = [], container_args : List[str] = [], 
        *args, **kwargs
    ) -> subprocess.Popen:
        container_name = self.lock.docker.container_name.get()
        return subprocess.Popen([
            "docker", "run", *run_args, container_name, *container_args
        ], *args, **kwargs)

    def build(self) -> subprocess.Popen:
        container_name = self.lock.docker.container_name.get()
        root_dir = self.lock.module.root_dir.get()
        dockerfile = str(root_dir / 'dockerfile')
        return subprocess.Popen([
            'docker', 'build', '-f', 
            dockerfile, '-t', container_name, str(root_dir)
        ])
