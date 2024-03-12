from .base import ActionHandler, confirm_or_exit

class Rename(ActionHandler):
    def action(self):
        new_name = input('New name : ')
        print(
            f"Module will be renamed from {self.lock.module.name.get()} to {new_name}"
        )
        confirm_or_exit("Confirm renaming")
        self.lock.set_name(new_name)

class Reversion(ActionHandler):
    def validate_version(self, version: str) -> bool:
        try:
            x, y, z = version.split('.')
            return all([x.isdigit(), y.isdigit(), z.isdigit()])
        except:
            return False

    def action(self):
        version = input('New Version String  (x.x.x) : ')
        while not self.validate_version(version):
            print("Invalid version string")
            version = input('Enter version (x.x.x) : ')
        print(
            f"Module will be reversioned from {self.lock.module.version.get()} to {version}")
        confirm_or_exit("Confirm reversioning")
        self.lock.set_version(version)
