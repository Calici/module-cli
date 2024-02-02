from .base import TestBase

class TestList(TestBase):
    def action(self):
        tests = self.lock.testing.get()
        if len(tests) == 0:
            print("No Tests")
        else:
            print("Listing Tests : ")
            for test in self.lock.testing.get():
                test_name : str = "- " + test.name.get()
                print(test_name)