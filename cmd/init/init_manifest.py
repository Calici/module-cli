from module_api.cmd.base import ActionHandler, TEMPLATE_DIR
import shutil

MANIFEST_TEMPLATE = TEMPLATE_DIR / 'manifest_template.json'
MANIFEST_VALIDATE = TEMPLATE_DIR / 'manifest-validate.json'

class InitManifestHandler(ActionHandler):
    def action(self):
        shutil.copyfile(MANIFEST_TEMPLATE, self.manifest.path)
        shutil.copyfile(
            MANIFEST_VALIDATE, 
            self.manifest.path.parent / 'manifest-validate.json'
        )