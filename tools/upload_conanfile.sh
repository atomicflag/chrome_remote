#!/bin/sh -ue

exec sed -i 's/\t/    /g' conanfile.py

~/.local/bin/conan export . signal9/stable
~/.local/bin/conan upload 'chrome_remote/*' -r signal9 -c --retry 5
