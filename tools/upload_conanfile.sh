#!/bin/sh -ue

ORIG_DIR=$(pwd)
TMP_DIR=$(mktemp -d)

cd $TMP_DIR
cp -rT $ORIG_DIR .
cd $TMP_DIR

find . -name 'conanfile.py' -exec sed -i 's/\t/    /g' {} \;

~/.local/bin/conan export signal9/stable
~/.local/bin/conan upload 'chrome_remote/*' -r private -c --retry 5

cd $ORIG_DIR
rm -rf $TMP_DIR
