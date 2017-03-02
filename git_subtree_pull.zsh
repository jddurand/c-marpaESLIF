#!zsh -x

git fetch origin
foreach this (cmake-utils) {
  git fetch $this-remote master
}

git clean -ffdx
foreach this (cmake-utils) {
  git subtree pull --prefix 3rdparty/github/$this-remote $this-remote master --squash
}

exit 0
