from module_api.cmd.base import \
    ActionHandler, \
    TEMPLATE_DIR
import pathlib

SERVER_FILE = TEMPLATE_DIR / 'server.py'

class TestBase(ActionHandler):
    def get_test_root(self) -> pathlib.Path:
        return self.lock.module.root_dir.get() / '.test'
    def container_root(self) -> pathlib.Path:
        return pathlib.Path('/data')