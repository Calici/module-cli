from setuptools import setup, find_packages

setup(
    name="module-cli",
    version="1.0.1",
    description="A Python Package to build, test and deploy module.",
    url="https://github.com/calici/module-cli",
    author="Jonathan Willianto",
    author_email="jo.will@calici.co",
    license="MIT",
    packages=find_packages(),
    install_requires=[
        "requests>=2.31.0",
        "typing_extensions>=4.7.1",
        "python-slugify>=8.0.1",
        "module-api>=1.2.1"
    ],
    include_package_data=True,
    scripts=["bin/pharmaco"]
)
