from __future__ import absolute_import, with_statement, print_function, division
from setuptools import setup, Extension, find_packages
import os
import sys
import numpy as np


connectFourModule = Extension(
    name="connectFour",
    sources=["pyTest/_pytest.cpp"],
    include_dirs=[sys.path, 'include'],
    depends=["pyTest/_pytest.hpp"],
    extra_compile_args=['-std=c++17','-Wall']
    )

setup(
    name="connectFour",
    author="Yang QIAN",
    ext_modules=[connectFourModule],
    headers=["pyTest/_pytest.hpp"],
    include_dirs=[sys.path],
)
