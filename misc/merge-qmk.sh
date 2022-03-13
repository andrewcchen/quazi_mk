#!/bin/bash

# Script to deal with merge conflicts when merging in qmk changes
# Please run in project root after running git merge

if [[ ! -e .git ]]; then
	echo "Please run in project root"
	exit 1
fi

# QMK files that are removed
removed_files=(
.clang-format
.editorconfig
.gitattributes
.github
.travis.yml
.vscode
CODE_OF_CONDUCT.md
Dockerfile
Makefile
Vagrantfile
api_data
bin
book.json
bootloader.mk
build_full_test.mk
build_json.mk
build_keyboard.mk
build_layout.mk
build_test.mk
common.mk
common_features.mk
data
disable_features.mk
docs
doxygen-todo
keyboards
layouts/community
lib/arm_atsam
lib/chibios
lib/chibios-contrib
lib/lufa
lib/vusb
message.mk
nix
nose2.cfg
platforms
requirements-dev.txt
requirements.txt
secrets.tar.enc
setup.cfg
shell.nix
show_options.mk
testlist.mk
tests
users
util
)

# QMK files that are overriden
overriden_files=(
.gitignore
)

for f in ${removed_files[@]}; do
	git rm -rf $f
done

for f in ${overriden_files[@]}; do
	git restore --ours $f
	git add $f
done

exit 0
