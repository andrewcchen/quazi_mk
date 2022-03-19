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
message.mk
nix
nose2.cfg
paths.mk
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
lib
readme.md
)

for f in ${remove_files[@]}; do
	git rm -rf $f
done

for f in ${overwrite_files[@]}; do
	git restore --staged --ignore-unmerged $f
	git restore --ours $f
	git clean -f -- $f
done

exit 0
