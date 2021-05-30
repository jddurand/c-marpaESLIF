static const char *MARPAESLIFLUA_CONTEXTSTACK =
  "\n"
  "-- At every Earley Set Id, there is a context attached to any parameterized symbol\n"
  "\n"
  "local marpaESLIFContextStack = {}\n"
  "marpaESLIFContextStack.new = function()\n"
  "   local data = {} -- Initial context stack is empty\n"
  "\n"
  "   local self = {}\n"
  "\n"
  "   function self.get(earleySetId, symbolId)\n"
  "      local earleySetIdContext = data[earleySetId]\n"
  "      if (earleySetIdContext == nil) then\n"
  "        return {}\n"
  "      end\n"
  "      return earleySetIdContext[symbolId]\n"
  "   end\n"
  "\n"
  "   function self.set(earleySetId, symbolId, context)\n"
  "      local earleySetIdContext = data[earleySetId]\n"
  "      if (earleySetIdContext == nil) then\n"
  "        data[earleySetId] = {}\n"
  "        local earleySetIdContext = data[earleySetId]\n"
  "      end\n"
  "      earleySetIdContext[symbolId] = context\n"
  "   end\n"
  "\n"
  "   return self\n"
  "end\n"
  "\n"
  "return marpaESLIFContextStack\n"
  "\n";
