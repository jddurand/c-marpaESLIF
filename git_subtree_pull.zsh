#!env zsh

foreach this (cmake-utils genericHash genericLogger genericSparseArray genericStack) {
  git fetch $this-remote master
  git subtree pull --prefix 3rdparty/github/$this-remote $this-remote master --squash
}

exit 0
