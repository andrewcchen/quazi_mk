#!/bin/bash

# Script to deal with merge conflicts when merging in qmk changes
# Please run in project root after running git merge

if [[ ! -e .git ]]; then
	echo "Please run in project root"
	exit 1
fi

# QMK files that we removed
remove_files=(
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
builddefs
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
paths.mk
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

# QMK files we overwrote
overwrite_files=(
.gitignore
readme.md
)

# QMK files we want to keep
restore_files=(
drivers
)

for f in ${remove_files[@]}; do
	git rm -rf $f
done

for f in ${overwrite_files[@]}; do
	git restore --ours $f
	git add $f
done

for f in ${restore_files[@]}; do
	git restore --theirs $f
	git add $f
done

exit 0
