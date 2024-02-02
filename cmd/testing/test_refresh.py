from .base import TestBase
import logging

class TestRefresh(TestBase):
    def action(self):
        root_dir = self.lock.module.root_dir.get()
        logging.info("Scanning Existing Tests")
        to_remove = [
            id 
            for id, entry in enumerate(self.lock.testing.get())
            if not entry.path.get().exists()
        ]
        logging.info("Remove Non-Existing Tests")
        for id in reversed(to_remove): 
            self.lock.testing.remove(id)
        
        convert_path = [
            id for id, entry in enumerate(self.lock.testing.get())
            if entry.path.get().is_absolute()
        ]
        if len(convert_path) != 0:
            logging.info("Updating Test Paths to Relative Paths")
        tests = self.lock.testing.get()
        for id in convert_path:
            self.lock.testing.modify(
                id, { 
                    'path' :  tests[id].path.get().relative_to(root_dir)
                }
            )
        self.lock.save()
                