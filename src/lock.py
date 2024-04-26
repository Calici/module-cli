import module_api.API.lock as Lock
from module_api.API.backend.module import \
    ModuleSection, \
    ModuleResult
import pathlib

class Params(Lock.LockSection):
    example_field = Lock.LockField(str, default = "example value")

class Depends(Lock.LockSection):
    example_result = Lock.LockField(pathlib.Path, default = pathlib.Path('.'))

class Status(Lock.LockStatus):
    # You don't have to fill this in.
    progress = Lock.LockField(float, default = 0)

class Results(ModuleResult):
    example_result = Lock.LockField(pathlib.Path, default = pathlib.Path('.'))

class ModuleData(ModuleSection):
    results = Results()

class ModuleLock(Lock.CaliciLock):
    params = Params()
    status = Status()
    depends = Depends()