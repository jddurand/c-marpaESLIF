static const char *MARPAESLIFLUA_CONTEXTSTACK =
  "\n"
  "-- Context stack is an array - it is assumed that the caller takes care of table.pack/table.unpack so that a context is ONE SINGLE DATA\n"
  "\n"
  "local marpaESLIFContextStack = {}\n"
  "marpaESLIFContextStack.new = function()\n"
  "  local stack  = {}                -- Initial context stack\n"
  "  local self = {}                  -- The context stack object\n"
  "\n"
  "  function self:get()\n"
  "    return stack[#stack]\n"
  "  end\n"
  "\n"
  "  function self:set(item)\n"
  "    stack[#stack] = item\n"
  "  end\n"
  "\n"
  "  function self:push(item)\n"
  "    stack[#stack+1] = item\n"
  "  end\n"
  "\n"
  "  function self:pop()\n"
  "    stack[#stack] = nil            -- Returns nothing\n"
  "  end\n"
  "\n"
  "  return self\n"
  "end\n"
  "\n"
  "return marpaESLIFContextStack\n"
  "\n";
