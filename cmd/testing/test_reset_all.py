from .base import TestBase
from module_api.API.lock import CaliciLock
import pathlib
import shutil

class ResetAllTest(TestBase):
    def reset_one_dir(self, test_dir : pathlib.Path):
        lock_path = test_dir / 'workdir' / '.reserved' / 'default.lock'
        lock = CaliciLock( lock_path )
        lock.set(
            status = {
                'initialized' : False, 'status' : 'PRE_INIT'
            }
        )
        # Delete Errors.json file and Display File
        if lock.display_path().exists():
            shutil.rmtree(lock.display_path())
        lock.error_path().unlink(missing_ok = True)

        # Clean work directory
        for entry in (test_dir / 'workdir').iterdir():
            if not (entry.is_dir() and entry.name == '.reserved'):
                if entry.is_dir():
                    shutil.rmtree(entry)
                else:
                    entry.unlink(missing_ok = True)
        # Clean Share Directory
        shutil.rmtree(test_dir / 'sharedir')
        (test_dir / 'sharedir').mkdir(parents = True, exist_ok = True)
    def action(self):
        for test in self.lock.testing.get():
            self.reset_one_dir(test.path.get())
