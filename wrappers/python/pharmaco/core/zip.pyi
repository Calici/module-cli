import pathlib
from typing import overload

class ZipError(Exception):
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...

class ZipBuilder:
    @overload
    def add_file(self, p: pathlib.Path) -> ZipBuilder:
        """
        Adds a file specified by p into the zip file. This file will be checked for relativitiy
        to the current work directory and added to the zip file with the correct relative path.
        """
        ...
    @overload
    def add_file(self, p: str, content: str) -> ZipBuilder:
        """
        Creates a file with content `content` and place it at path p in the zip file. No
        relativity check is conducted, the path p is taken as is.
        """
        ...
    def add_directory(self, p: pathlib.Path) -> ZipBuilder:
        """
        Adds a directory specified by p into the zip file.
        """
        ...
    def add_to_zip(self, p: pathlib.Path) -> ZipBuilder:
        """
        Adds a path into the zip file.
        """
        ...
