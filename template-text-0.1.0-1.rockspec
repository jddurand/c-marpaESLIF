package = "template-text"
version = "0.1.0-1"
source = {
  url = "git@github.com:mfrigerio17/lua-template-engine.git",
  tag = "v0.1.0"
}

description = {
  summary  = "Template engine for text generation",
  detailed = [[
  ]],
  license = "BSD 2-clause",
  homepage = "https://github.com/mfrigerio17/lua-template-engine"
}

dependencies = {
  "lua > 5.1"
}

build = {
  type = "builtin",
  modules = {

  }
}

build.modules["template-text"] = "src/template-text.lua"
