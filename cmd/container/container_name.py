from module_cli.base import \
    ModuleLock, \
    ActionHandler, \
    Container

class ContainerName(ActionHandler):
    def __init__(self, lock : ModuleLock):
        super().__init__(lock)
        self.container = Container(lock)
    def action(self):
        print(self.container.name())