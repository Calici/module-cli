from __future__ import annotations
from module_api.API.runnable import \
    Runnable, \
    create, \
    default_run
from lock import ModuleLock, ModuleData
from module_api.API.error import ErrorBuffer
from module_api.API.backend import \
    TokenAPI, \
    NotificationAPI, \
    ModuleAPI

class ModuleRunnable(Runnable[ModuleLock]):
    lock_type = ModuleLock
    def init(self):
        module_id = self.lock.header.module_id.get()
        self.token_api = TokenAPI(module_id)
        self.notification_api = NotificationAPI(module_id)
        self.module_api = ModuleAPI(module_id, ModuleData)
        self.error_buffer = ErrorBuffer(self.lock)
        # This piece of code is used to initialize the runnable

    def re_init(self):
        module_id = self.lock.header.module_id.get()
        self.token_api = TokenAPI(module_id)
        self.notification_api = NotificationAPI(module_id)
        self.module_api = ModuleAPI(module_id, ModuleData)
        self.error_buffer = ErrorBuffer(self.lock)
        # This piece of code is used to re-initialize the runnable if it has
        # been stopped before.
        
    def run(self):
        self.lock.change_status('RUNNING')
        # Code to run the module

    def stop(self):
        self.lock.change_status('STOP')
        # What to do when the module is stopped

if __name__ == '__main__':
    runnable = create(ModuleRunnable)
    default_run(runnable)