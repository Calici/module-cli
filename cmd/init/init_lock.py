from module_api.cmd.base import ActionHandler

class InitLock(ActionHandler):
    def action(self):
        lock_path = self.lock.file_path.resolve()
        self.lock.set( module = {'root_dir' : lock_path.parent} )