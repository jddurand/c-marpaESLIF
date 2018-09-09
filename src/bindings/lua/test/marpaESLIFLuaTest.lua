local function pairsByKeys(t, f)
   local a = {}
   for n in pairs(t) do table.insert(a, n) end
   table.sort(a, f)
   local i = 0      -- iterator variable
   local iter = function ()   -- iterator function
      i = i + 1
      if a[i] == nil then return nil
      else return a[i], t[a[i]]
      end
   end
   return iter
end

local function _table2str(lua_table, raw_table, table_map, n, fold, indent)
    indent = indent or 1
    for k, v in pairsByKeys(lua_table) do
        if type(k) == 'string' then
            k = string.format('%q', k)
        else
            k = tostring(k)
        end
        n = n + 1; raw_table[n] = string.rep('    ', indent)
        n = n + 1; raw_table[n] = '['
        n = n + 1; raw_table[n] = k
        n = n + 1; raw_table[n] = ']'
        n = n + 1; raw_table[n] = ' = '
        if type(v) == 'table' then
            if fold and table_map[tostring(v)] then
                n = n + 1; raw_table[n] = tostring(v)
                n = n + 1; raw_table[n] = ',\n'
            else
                table_map[tostring(v)] = true
                n = n + 1; raw_table[n] = '{\n'
                n = _table2str(v, raw_table, table_map, n, fold, indent + 1)
                n = n + 1; raw_table[n] = string.rep('    ', indent)
                n = n + 1; raw_table[n] = '},\n'
            end
        else
            if type(v) == 'string' then
                v = string.format('%q', v)
            else
                v = tostring(v)
            end
            n = n + 1; raw_table[n] = v
            n = n + 1; raw_table[n] = ',\n'
        end
    end
    return n
end

local function tableDump(lua_table, fold)
    if type(lua_table) == 'table' then
        local raw_table = {}
        local table_map = {}
        table_map[tostring(lua_table)] = true
        local n = 0
        n = n + 1; raw_table[n] = '{\n'
        n = _table2str(lua_table, raw_table, table_map, n, fold)
        n = n + 1; raw_table[n] = '}'
        return table.concat(raw_table, '')
    else
        return lua_table
    end
end

local marpaESLIFLua = require 'marpaESLIFLua'
logger = {
   ['trace']     = function(msgs) print('LUA_TRACE '..msgs) end,
   ['debug']     = function(msgs) print('LUA_DEBUG '..msgs) end,
   ['info']      = function(msgs) print('LUA_INFO '..msgs) end,
   ['notice']    = function(msgs) print('LUA_NOTICE '..msgs) end,
   ['warning']   = function(msgs) print('LUA_WARNING '..msgs) end,
   ['error']     = function(msgs) print('LUA_ERROR '..msgs) end,
   ['critical']  = function(msgs) print('LUA_CRITICAL '..msgs) end,
   ['emergency'] = function(msgs) print('LUA_EMERGENCY '..msgs) end
}

------------------------------------------------------------------------------
print('marpaESLIF version: '..marpaESLIFLua.version())
------------------------------------------------------------------------------
local marpaESLIFp = marpaESLIFLua.marpaESLIF_new(logger)
print('marpaESLIFp meta dump:'..tableDump(getmetatable(marpaESLIFp)))
------------------------------------------------------------------------------
local marpaESLIFGrammarp = marpaESLIFLua.marpaESLIFGrammar_new(marpaESLIFp, "X ::= x\nx ~ 'lexeme'")
print('marpaESLIFGrammarp      dump:'..tableDump(marpaESLIFGrammarp))
print('marpaESLIFGrammarp meta dump:'..tableDump(getmetatable(marpaESLIFGrammarp)))
------------------------------------------------------------------------------
local ngrammar = marpaESLIFGrammarp:ngrammar()
print('... Number of grammars : '..ngrammar)
------------------------------------------------------------------------------
local currentLevel = marpaESLIFGrammarp:currentLevel()
print('... Current level      : '..currentLevel)
------------------------------------------------------------------------------
local currentDescription = marpaESLIFGrammarp:currentDescription()
print('... Current description: '..currentDescription)
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local descriptionByLevel = marpaESLIFGrammarp:descriptionByLevel(level)
   print('... Description level '..level..': '..descriptionByLevel)
end
------------------------------------------------------------------------------
local ruleIds = marpaESLIFGrammarp:currentRuleIds()
print('... Current rule Ids   : '..tableDump(ruleIds))
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   print('... Rule Ids level '..level..': '..tableDump(ruleIdsByLevel))
end
------------------------------------------------------------------------------
local symbolIds = marpaESLIFLua.marpaESLIFGrammar_currentSymbolIds(marpaESLIFGrammarp)
print('... Current symbol Ids : '..tableDump(symbolIds))
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   print('... Symbol Ids level '..level..': '..tableDump(symbolIdsByLevel))
end
------------------------------------------------------------------------------
local currentProperties = marpaESLIFGrammarp:currentProperties()
print('... Current properties: '..tableDump(currentProperties))
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local propertiesByLevel = marpaESLIFGrammarp:propertiesByLevel(level)
   print('... Properties level '..level..': '..tableDump(propertiesByLevel))
end
------------------------------------------------------------------------------
for k, ruleId in pairsByKeys(ruleIds) do
   local currentRuleProperties = marpaESLIFGrammarp:currentRuleProperties(ruleId)
   print('... Current rule No '..ruleId..': '..tableDump(currentRuleProperties))
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      local rulePropertiesByLevel = marpaESLIFGrammarp:rulePropertiesByLevel(level, ruleId)
      print('... Level '..level..' rule No '..ruleId..': '..tableDump(rulePropertiesByLevel))
   end
end
------------------------------------------------------------------------------
for k, symbolId in pairsByKeys(symbolIds) do
   local currentSymbolProperties = marpaESLIFGrammarp:currentSymbolProperties(symbolId)
   print('... Current symbol No '..symbolId..': '..tableDump(currentSymbolProperties))
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   for k, symbolId in pairsByKeys(symbolIdsByLevel) do
      local symbolPropertiesByLevel = marpaESLIFGrammarp:symbolPropertiesByLevel(level, symbolId)
      print('... Level '..level..' symbol No '..symbolId..': '..tableDump(symbolPropertiesByLevel))
   end
end
------------------------------------------------------------------------------
for k, ruleId in pairsByKeys(ruleIds) do
   local ruleDisplay = marpaESLIFGrammarp:ruleDisplay(ruleId)
   print('... Rule No '..ruleId..' display: '..ruleDisplay)
end
------------------------------------------------------------------------------
for k, symbolId in pairsByKeys(symbolIds) do
   local symbolDisplay = marpaESLIFGrammarp:symbolDisplay(symbolId)
   print('... Symbol No '..symbolId..' display: '..symbolDisplay)
end
------------------------------------------------------------------------------
for k, ruleId in pairsByKeys(ruleIds) do
   local ruleShow = marpaESLIFGrammarp:ruleShow(ruleId)
   print('... Rule No '..ruleId..' show: '..ruleShow)
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      local ruleDisplayByLevel = marpaESLIFGrammarp:ruleDisplayByLevel(level, ruleId)
      print('... Level '..level..' rule No '..ruleId..' display: '..ruleDisplayByLevel)
   end
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   for k, symbolId in pairsByKeys(symbolIdsByLevel) do
      local symbolDisplayByLevel = marpaESLIFGrammarp:symbolDisplayByLevel(level, symbolId)
      print('... Level '..level..' symbol No '..symbolId..' display: '..symbolDisplayByLevel)
   end
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      local ruleShowByLevel = marpaESLIFGrammarp:ruleShowByLevel(level, ruleId)
      print('... Level '..level..' rule No '..ruleId..' show: '..ruleShowByLevel)
   end
end
------------------------------------------------------------------------------
local show = marpaESLIFGrammarp:show()
print('... Grammar show: '..show)
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local showByLevel = marpaESLIFGrammarp:showByLevel(level)
   print('... Level '..level..' grammar show: '..showByLevel)
end
------------------------------------------------------------------------------
