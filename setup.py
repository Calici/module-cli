from setuptools import \
    setup, \
    find_packages, \
    find_namespace_packages

setup(
    name='module-cli',
    version='1.0.0',    
    description='A Python Package to build, test and deploy module.',
    url='https://github.com/calici/module-cli',
    author='Jonathan Willianto',
    author_email='jo.will@calici.co',
    license='MIT',
    packages=[ 
        "module_cli"
    ],
    install_requires=[
      "requests==2.31.0",
      "typing_extensions==4.7.1",
      "python-slugify==8.0.1",
      "module-api @ https://github.com/Calici/module-api/releases/download/dev-1.0.1-2024-02-13/module_api-1.0.1-py3-none-any.whl"
    ],
    include_package_data = True,
    scripts = ['bin/pharmaco']
)