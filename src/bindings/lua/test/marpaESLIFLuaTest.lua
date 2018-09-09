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

print('marpaESLIF version: '..marpaESLIFLua.marpaESLIF_version())
local marpaESLIFp = marpaESLIFLua.marpaESLIF_new(logger)
local marpaESLIFGrammarp = marpaESLIFLua.marpaESLIFGrammar_new(marpaESLIFp, "X ::= x\nx ~ 'lexeme'")
local ngrammar = marpaESLIFLua.marpaESLIFGrammar_ngrammar(marpaESLIFGrammarp)
print('... Number of grammars : '..ngrammar)
print('... Current level      : '..marpaESLIFLua.marpaESLIFGrammar_currentLevel(marpaESLIFGrammarp))
print('... Current description: '..marpaESLIFLua.marpaESLIFGrammar_currentDescription(marpaESLIFGrammarp))
for level = 0,ngrammar-1 do
   print('... Description level '..level..': '..marpaESLIFLua.marpaESLIFGrammar_descriptionByLevel(marpaESLIFGrammarp, level))
end
local ruleIds = marpaESLIFLua.marpaESLIFGrammar_currentRuleIds(marpaESLIFGrammarp)
print('... Current rule Ids   : '..tableDump(ruleIds))
for level = 0,ngrammar-1 do
   print('... Rule Ids level '..level..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_ruleIdsByLevel(marpaESLIFGrammarp, level)))
end
local symbolIds = marpaESLIFLua.marpaESLIFGrammar_currentSymbolIds(marpaESLIFGrammarp)
print('... Current symbol Ids : '..tableDump(symbolIds))
for level = 0,ngrammar-1 do
   print('... Symbol Ids level '..level..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_symbolIdsByLevel(marpaESLIFGrammarp, level)))
end
print('... Current properties: '..tableDump(marpaESLIFLua.marpaESLIFGrammar_currentProperties(marpaESLIFGrammarp)))
for level = 0,ngrammar-1 do
   print('... Properties level '..level..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_propertiesByLevel(marpaESLIFGrammarp, level)))
end
for k, ruleId in pairsByKeys(ruleIds) do
   print('... Current rule No '..ruleId..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_currentRuleProperties(marpaESLIFGrammarp, ruleId)))
end
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFLua.marpaESLIFGrammar_ruleIdsByLevel(marpaESLIFGrammarp, level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      print('... Level '..level..' rule No '..ruleId..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_rulePropertiesByLevel(marpaESLIFGrammarp, level, ruleId)))
   end
end
for k, symbolId in pairsByKeys(symbolIds) do
   print('... Current symbol No '..symbolId..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_currentSymbolProperties(marpaESLIFGrammarp, symbolId)))
end
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFLua.marpaESLIFGrammar_symbolIdsByLevel(marpaESLIFGrammarp, level)
   for k, symbolId in pairsByKeys(symbolIdsByLevel) do
      print('... Level '..level..' symbol No '..symbolId..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_symbolPropertiesByLevel(marpaESLIFGrammarp, level, symbolId)))
   end
end
for k, ruleId in pairsByKeys(ruleIds) do
   print('... Rule No '..ruleId..' display: '..marpaESLIFLua.marpaESLIFGrammar_ruleDisplay(marpaESLIFGrammarp, ruleId))
end
for k, symbolId in pairsByKeys(symbolIds) do
   print('... Symbol No '..symbolId..' display: '..marpaESLIFLua.marpaESLIFGrammar_symbolDisplay(marpaESLIFGrammarp, symbolId))
end
for k, ruleId in pairsByKeys(ruleIds) do
   print('... Rule No '..ruleId..' show: '..marpaESLIFLua.marpaESLIFGrammar_ruleShow(marpaESLIFGrammarp, ruleId))
end
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFLua.marpaESLIFGrammar_ruleIdsByLevel(marpaESLIFGrammarp, level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      print('... Level '..level..' rule No '..ruleId..' display: '..marpaESLIFLua.marpaESLIFGrammar_ruleDisplayByLevel(marpaESLIFGrammarp, level, ruleId))
   end
end
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFLua.marpaESLIFGrammar_symbolIdsByLevel(marpaESLIFGrammarp, level)
   for k, symbolId in pairsByKeys(symbolIdsByLevel) do
      print('... Level '..level..' symbol No '..symbolId..' display: '..marpaESLIFLua.marpaESLIFGrammar_symbolDisplayByLevel(marpaESLIFGrammarp, level, symbolId))
   end
end
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFLua.marpaESLIFGrammar_ruleIdsByLevel(marpaESLIFGrammarp, level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      print('... Level '..level..' rule No '..ruleId..' show: '..marpaESLIFLua.marpaESLIFGrammar_ruleShowByLevel(marpaESLIFGrammarp, level, ruleId))
   end
end
print('... Grammar show: '..marpaESLIFLua.marpaESLIFGrammar_show(marpaESLIFGrammarp))
for level = 0,ngrammar-1 do
   print('... Level '..level..' grammar show: '..marpaESLIFLua.marpaESLIFGrammar_showByLevel(marpaESLIFGrammarp, level))
end
