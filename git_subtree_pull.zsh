#!zsh -x

havemodif=0
git diff-index --quiet HEAD
if [ $? -ne 0 ]; then
    havemodif=1
fi

if [ $havemodif -ne 0 ]; then
    git stash
fi

git fetch origin
foreach this (cmake-utils genericLogger genericStack genericHash tconv marpaWrapper luaunpanic lua-template-engine) {
  git fetch $this master
}

current_branch=`git branch --show-current`
git reset --hard origin/$current_branch
git clean -ffdx
foreach this (cmake-utils genericLogger genericStack genericHash tconv marpaWrapper luaunpanic lua-template-engine) {
  git subtree pull --prefix 3rdparty/github/$this $this master --squash
}

if [ $havemodif -ne 0 ]; then
    git stash apply
fi

exit 0
