from .base import ActionHandler
import yaml

class TestFix(ActionHandler):
    """
      Renames params yml to params
    """
    def action(self):
        workdir = self.lock.module.root_dir.get()
        for test in self.lock.testing.get():
            test_path = workdir / test.path.get()
            params_path = test_path / 'workdir' / '.reserved' / 'params.json'
            if params_path.exists():
                with open(params_path, 'r') as f:
                    params = yaml.safe_load(f)
                with open(params_path.parent / 'params.yml', 'w') as f:
                    yaml.dump(params, f)
                params_path.unlink(missing_ok = True)