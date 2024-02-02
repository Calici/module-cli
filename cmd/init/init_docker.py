from module_api.cmd.base import ActionHandler, TEMPLATE_DIR
import shutil

DOCKER_TEMPLATE = TEMPLATE_DIR / 'dockerfile.tmp'

class InitDockerHandler(ActionHandler):
    def action(self):
        docker_path = self.lock.module.root_dir.get() / 'dockerfile'
        shutil.copyfile(DOCKER_TEMPLATE, docker_path)