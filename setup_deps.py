#!/usr/bin/env python3

import sys

assert sys.version_info >= (3, 10)

import sh
import os

repo_dir = os.path.abspath(os.path.dirname(__file__))

def build_re2c():
    re2c_dir = os.path.join(repo_dir, "external", "re2c")
    sh.git("clean", "-fdx", _cwd=re2c_dir, _out=sys.stdout)
    sh.autoreconf("-i", "-W", "all", _cwd=re2c_dir, _out=sys.stdout)
    
    sh.mkdir(".build", _cwd=re2c_dir)
    build_dir = os.path.join(re2c_dir, ".build")
    
    sh.bash("../configure", _cwd=build_dir, _out=sys.stdout)
    sh.make(_cwd=build_dir, _out=sys.stdout)
    sh.cp("re2c", f"{repo_dir}/external/bin", _cwd=build_dir)


def main():
    build_re2c()

if __name__ == "__main__":
    main()

