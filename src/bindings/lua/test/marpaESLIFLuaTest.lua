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
local marpaESLIFGrammarp = marpaESLIFLua.marpaESLIFGrammar_new(marpaESLIFp, "X ::= x\nx ~ 'x'")
local ngrammar = marpaESLIFLua.marpaESLIFGrammar_ngrammar(marpaESLIFGrammarp)
print('... Number of grammars : '..ngrammar)
print('... Current level      : '..marpaESLIFLua.marpaESLIFGrammar_currentLevel(marpaESLIFGrammarp))
print('... Current description: '..marpaESLIFLua.marpaESLIFGrammar_currentDescription(marpaESLIFGrammarp))
for level = 0,ngrammar-1 do
   print('... Description level '..level..': '..marpaESLIFLua.marpaESLIFGrammar_descriptionByLevel(marpaESLIFGrammarp, level))
end
print('... Current rule Ids   : '..tableDump(marpaESLIFLua.marpaESLIFGrammar_currentRuleIds(marpaESLIFGrammarp)))
for level = 0,ngrammar-1 do
   print('... Rule Ids level '..level..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_ruleIdsByLevel(marpaESLIFGrammarp, level)))
end
print('... Current symbol Ids : '..tableDump(marpaESLIFLua.marpaESLIFGrammar_currentSymbolIds(marpaESLIFGrammarp)))
for level = 0,ngrammar-1 do
   print('... Symbol Ids level '..level..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_symbolIdsByLevel(marpaESLIFGrammarp, level)))
end
print('... Current properties: '..tableDump(marpaESLIFLua.marpaESLIFGrammar_currentProperties(marpaESLIFGrammarp)))
for level = 0,ngrammar-1 do
   print('... Properties level '..level..': '..tableDump(marpaESLIFLua.marpaESLIFGrammar_propertiesByLevel(marpaESLIFGrammarp, level)))
end
