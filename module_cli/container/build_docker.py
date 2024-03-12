from module_cli.base import \
    ModuleLock, \
    ActionHandler, \
    Container

class BuildDocker(ActionHandler):
    def __init__(self, lock: ModuleLock):
        self.container = Container(lock)
    
    def action(self):
        self.container.build().wait()