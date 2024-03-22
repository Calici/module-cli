from module_cli.base import \
    ModuleLock, \
    ActionHandler, \
    Container
import logging
import sys

class ContainerValidate(ActionHandler):
    def __init__(self, lock: ModuleLock):
        super().__init__(lock)
        self.container = Container(lock)
    
    def action(self):
        with open(self.container.dockerfile(), 'r') as f:
            lines = f.readlines()
        is_root = True
        for line in lines:
            line = line.strip()
            if line.startswith('#'): continue
            elif line.startswith('FROM'): is_root = True
            elif line.startswith('USER calici'):  
                is_root = False
            elif line.startswith('USER'):
                is_root = True
        if is_root:
            logging.error("Module Container has to be run with user `calici` please fix this by adding the following lines : ")
            logging.error("RUN useradd -u 8879 -m calici")
            logging.error("USER calici")
            sys.exit(1)
        print("[✓] Dockerfile is not run as ROOT")