from .base import TestBase
from module_api.API.lock import CaliciLock
from slugify import slugify
from typing_extensions import Tuple
import pathlib

class MakeTest(TestBase):
    def create_test_dir(self) -> Tuple[str, pathlib.Path]:
        test_workdir = self.get_test_root()
        name = input("Test Name : ")
        test_dir = test_workdir / slugify(name)
        while test_dir.exists():
            print("Test Folder exist")
            name = input("Test Name : ")
            test_dir = test_workdir / slugify(name)
        return name, test_dir

    def initialize_test_dir(self, test_dir : pathlib.Path):
        manifest_content = self.manifest.get_json()
        dir_list = [
            test_dir / 'workdir',
            test_dir / 'sharedir',
            test_dir / 'depends',
            test_dir / 'workdir' / '.reserved',
            test_dir / 'workdir' / '.reserved' / 'display',
        ]
        file_list = [
            test_dir / 'depends' / name
            for name in manifest_content['in_nodes'].keys()
        ]
        for dir in dir_list: dir.mkdir(parents = True, exist_ok = True)
        for file in file_list: open(file, 'w')

    def create_module_lock(self, test_dir : pathlib.Path):
        lock_path = test_dir / 'workdir' / '.reserved' / 'default.lock'
        manifest = self.manifest.get_json()
        params = {
            param_name : param['default']
            for param_name, param in manifest['params'].items()
        }
        container_test_folder = self.container_root() / test_dir.name
        lock = CaliciLock(
            lock_path, 
            header = {
                'process' : self.lock.module.internal_name.get(),
                'workdir' : container_test_folder / 'workdir', 
                'sharedir' : container_test_folder / 'sharedir',
                'module_id' : 1, 
                'log_path' : CaliciLock.default_log_path(
                    container_test_folder / 'workdir'
                ),
                'gpu_blocks' : []
            },
            depends = {
                name : str(container_test_folder / 'depends' / name)
                for name in manifest['in_nodes'].keys()
            }
        )
        lock.set(params = params)

    def add_to_lock(self, name : str, path : pathlib.Path):
        self.lock.testing.append({ 'name' : name, 'path' : path})
        self.lock.save()

    def action(self):
        name, test_path = self.create_test_dir()
        self.initialize_test_dir(test_path)
        self.add_to_lock(name, test_path)
        self.create_module_lock(test_path)