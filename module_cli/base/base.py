from __future__ import annotations
import pathlib
import json
from abc import ABC, abstractmethod
from typing_extensions import \
    TypedDict, \
    Any, \
    Dict, \
    Union, \
    List, \
    Callable
from .lock import ModuleLock

TEMPLATE_DIR = pathlib.Path(__file__).parent.parent / 'templates'

ManifestT = TypedDict("ManifestT", {
    "params" : Dict[str, Any],
    "results" : Dict[str, Any],
    "in_nodes" : Dict[str, Any], 
    'params_render' : Dict[str, Any],
    'results_render' : Dict[str, Any],
    'out_nodes' : Dict[str, Any],
    'download_struct' : Dict[str, Any],
    'run_count' : Dict[str, Any],
    'token_prices' : Dict[str, Any],
    'version' : str,
    '$schema' : str
})

class Manifest:
    __slots__ = ('path', 'content')
    def __init__(self, manifest : pathlib.Path):
        self.path = manifest
        self.content = None
    
    def get_json(self) -> ManifestT:
        if self.content is None:
            with open(self.path, 'r') as f:
                self.content = json.load(f)
        return self.content
    
    def write_json(self, content : ManifestT):
        with open(self.path, 'w') as f:
            self.content = content
            json.dump(content, f)
    
    def validate_exist(self):
        if not self.path.exists():
            raise RuntimeError(f"Manifest does not exist at {self.path}")

class ActionHandler(ABC):
    def __init__(self, lock : ModuleLock):
        self.lock = lock
        self.manifest = Manifest(lock.module.root_dir.get() / 'manifest.json')
        self.workdir = lock.module.root_dir.get() / 'src'

    @abstractmethod
    def action(self):
        ...

ActionHandlerConstructorT = \
    Callable[[ ModuleLock ], ActionHandler]
HandlerMapT = Dict[
    str, 
    Union[
        ActionHandlerConstructorT, 
        List[Union[str, ActionHandlerConstructorT]]
    ]
]

def exec_handlers(
    handler_map : HandlerMapT, action : str, lock : ModuleLock
):
    Handler = handler_map[action]
    if isinstance(Handler, list):
        for handler in Handler:
            if isinstance(handler, str):
                exec_handlers(handler_map, handler, lock)
            else:
                handler(lock).action()
    else:
        Handler(lock).action()