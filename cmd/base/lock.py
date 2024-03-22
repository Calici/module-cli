import module_api.API.lock as Lock
import pathlib
from slugify import slugify

class ModuleSection(Lock.LockSection):
    name = Lock.LockField(str, default = '')
    internal_name = Lock.LockField(str, default = '')
    version = Lock.LockField(str, default = '0.0.0')
    root_dir = Lock.LockField(pathlib.Path, default = pathlib.Path())

class DockerSection(Lock.LockSection):
    volumes = Lock.ListField(
        Lock.TypeField(Lock.LockField, str), default = ["./src:/app"]
    )
    environments = Lock.ListField(
        Lock.TypeField(Lock.LockField, str), default = []
    )
    container_name = Lock.LockField(str, default = '')
    run_argument = Lock.LockField(str, default = '--rm -it')
    run_command = Lock.LockField(str, default = 'bash')

class BackendSection(Lock.LockSection):
    endpoint = Lock.LockField(str, default = '')
    backend_fd = Lock.LockField(pathlib.Path, default = pathlib.Path())

class TestSection(Lock.LockSection):
    name = Lock.LockField(str, '')
    path = Lock.LockField(pathlib.Path, pathlib.Path())
class ModuleLock(Lock.LockIO):
    module = ModuleSection()
    docker = DockerSection()
    testing = Lock.ListField(Lock.SpreadKwargs(TestSection), [])

    def set_name(self, name : str):
        internal_name = slugify(name)
        container_name = f'{internal_name}:{self.module.version.get()}'
        self.set(
            module = {'internal_name' : slugify(internal_name), 'name' : name},
            docker = {'container_name' : container_name}
        )

    def set_version(self, version : str):
        internal_name = self.module.internal_name.get()
        container_name = f'{internal_name}:{version}'
        self.set(
            docker = {'container_name' : container_name}, 
            module = {'version' : version}
        )