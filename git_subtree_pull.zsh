#!zsh -x

git fetch origin
foreach this (cmake-utils dlfcn-win32 genericLogger optparse coveralls-cmake) {
  git fetch $this master
}

git reset --hard origin/master
git clean -ffdx
foreach this (cmake-utils dlfcn-win32 genericLogger optparse coveralls-cmake) {
  git subtree pull --prefix 3rdparty/github/$this $this master --squash
}

exit 0
