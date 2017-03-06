#!zsh -x

git fetch origin
foreach this (cmake-utils genericHash genericLogger) {
  git fetch $this-remote master
}

git reset --hard origin/master
git clean -ffdx
foreach this (cmake-utils genericHash genericLogger) {
  git subtree pull --prefix 3rdparty/github/$this-remote $this-remote master --squash
}

exit 0
