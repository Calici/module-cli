import module_api.API.lock as Lock
import pathlib

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