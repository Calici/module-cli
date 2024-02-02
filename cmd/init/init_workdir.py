from module_api.cmd.base import \
    ActionHandler, \
    TEMPLATE_DIR, \
    ModuleLock
from typing_extensions import \
    Literal
import shutil

PARAM_TYPE = Literal['str', 'int', 'float', 'path', 'dict', 'list', 'bool']
LOCK_TEMPLATE = TEMPLATE_DIR / 'lock_template.txt'
RUN_TEMPLATE = TEMPLATE_DIR / 'run_template.txt'

class ParamField:
    def __init__(self, param_name : str, param_config : dict):
        self.param_name = param_name
        self.param_type = param_config['type']
        self.param_default = param_config['default']
        self.param_config = param_config
    
    def get_type(self, param_type : PARAM_TYPE):
        if param_type == 'path':
            return "pathlib.Path"
        return param_type

    def get_default(self):
        if self.param_type == 'path':
            return "pathlib.Path('{0}')".format(self.param_default)
        elif self.param_type == 'str':
            return '"{0}"'.format(self.param_default)
        elif self.param_type == 'list' or self.param_type == 'dict':
            return str(self.param_default)
        elif self.param_type == 'bool':
            return 'True' if self.param_default else 'False'
        return self.param_default
    
    def render(self) -> str:
        if self.param_type == 'list':
            return \
"{0} = Lock.ListField(Lock.TypeField(Lock.LockField, {1}), default = {2})"\
                .format(
                    self.param_name, 
                    self.get_type(self.param_config['child']['type']),
                    self.get_default()
                )
        else:
            return "{0} = Lock.LockField({1}, default = {2})"\
                .format(
                    self.param_name,
                    self.get_type(self.param_type),
                    self.get_default()
                )

class InitWorkdirHandler(ActionHandler):
    def __init__(self, lock : ModuleLock):
        super().__init__(lock)
        workdir = self.lock.module.root_dir.get() / 'src'
        self.lock_py_path = workdir / 'lock.py'
        self.run_py_path = workdir / 'run.py'
        
        if self.lock_py_path.exists():
            raise RuntimeError(
                f"Refusing to overwrite existing lock file {self.lock_py_path}"
            )
        if self.run_py_path.exists():
            raise RuntimeError(
                f"Refusing to overwrite existing run file {self.run_py_path}"
            )
        self.manifest.validate_exist()
        self.workdir.mkdir(parents = True, exist_ok = True)
    
    def write_lock_file(self):
        manifest = self.manifest.get_json()
        params = "\n    ".join([
            ParamField(param_name, param_config).render()
            for param_name, param_config in manifest['params'].items()
        ])
        results = "\n    ".join([
            ParamField(result_name, result_config).render()
            for result_name, result_config in manifest['results'].items()
        ])
        depends = "\n    ".join([
            ParamField(node_name, {'type' : 'path', 'default' : '.'}).render()
            for node_name, _ in manifest['in_nodes'].items()
        ])
        with open(LOCK_TEMPLATE, 'r') as tmp_f:
            content = tmp_f.read()\
                .replace('$PARAMS', params)\
                .replace('$DEPENDS', depends)\
                .replace('$RESULTS', results)
        with open(self.lock_py_path, 'w') as f:
            f.write(content)
    
    def action(self):
        self.write_lock_file()
        shutil.copy(RUN_TEMPLATE, self.run_py_path)
