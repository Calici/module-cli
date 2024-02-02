from module_api.cmd.base import \
    ModuleLock, \
    ActionHandler, \
    Container

class RunDocker(ActionHandler):
    def __init__(self, lock: ModuleLock):
        super().__init__(lock)
        self.container = Container(lock)
    
    def action(self):
        run_args = self.lock.docker.run_argument.get().split() + \
            self.container.get_volume_args() + \
            self.container.get_env_args()
        command = self.lock.docker.run_command.get().split()
        self.container.run(run_args, command).wait()