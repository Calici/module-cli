from module_api.cmd.base import ActionHandler, TEMPLATE_DIR
from module_api.cmd.base import ModuleLock
from slugify import slugify
import shutil
import subprocess

GITIGNORE_TEMPLATE = TEMPLATE_DIR / '.gitignore.tmp'
DOCKERIGNORE_TEMPLATE = TEMPLATE_DIR / '.dockerignore.tmp'

class InitDir(ActionHandler):
    def __init__(self, lock : ModuleLock):
        super().__init__(lock)
        root_dir = self.lock.module.root_dir.get()
        self.gitignore_path = root_dir / '.gitignore'
        self.dockerignore_path = root_dir / '.dockerignore'
    
    def get_template(self, name : str, internal_name : str, version : str):
        return {
            'module' : {
                'name' : name, 
                'internal_name' : internal_name, 
                'version' : version
            }, 
            'docker' : {
                'container_name' : '{0}:{1}'.format(internal_name, version)
            }
        }

    def validate_version(self, version : str) -> bool:
        try:
            x, y, z = version.split('.')
            return all([x.isdigit(), y.isdigit(), z.isdigit()])
        except:
            return False

    def action(self):
        module_name = input('Enter module name : ')
        internal_module_name = slugify(module_name)
        version = input('Enter version (x.x.x) : ')
        while not self.validate_version(version):
            print("Invalid version string")
            version = input('Enter version (x.x.x) : ')
        self.lock.set(
            **self.get_template(module_name, internal_module_name, version)
        )
        subprocess.run(["git", "init"])
        shutil.copyfile(GITIGNORE_TEMPLATE, self.gitignore_path)
        shutil.copyfile(DOCKERIGNORE_TEMPLATE, self.dockerignore_path)
        