static const char *MARPAESLIFLUA_CONTEXTSTACK =
  "\n"
  "local marpaESLIFContextStack = {}\n"
  "marpaESLIFContextStack.new = function()\n"
  "   local stack = { } -- Initial context is empty\n"
  "\n"
  "   local self = {}\n"
  "\n"
  "   function self.get()\n"
  "      print('Context at indice '..#stack..' is :')\n"
  "      print(tableDump(stack[#stack]))\n"
  "      return stack[#stack]\n"
  "   end\n"
  "\n"
  "   function self.push(context)\n"
  "      stack[#stack + 1] = context\n"
  "      return self.get()\n"
  "   end\n"
  "\n"
  "   function self.pop()\n"
  "      stack[#stack] = nil\n"
  "      return self.get()\n"
  "   end\n"
  "\n"
  "   function self.stack()\n"
  "      return stack\n"
  "   end\n"
  "\n"
  "   return self\n"
  "end\n"
  "\n"
  "return marpaESLIFContextStack\n"
  "\n";
