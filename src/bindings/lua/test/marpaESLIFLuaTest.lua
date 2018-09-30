local function pairsByKeys(t, f)
   local a = {}
   for n in pairs(t) do table.insert(a, n or tostring(n)) end
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
local logger = {
   ["trace"]     = function(self, msgs) self:tracef("%s", msgs) end,
   ["debug"]     = function(self, msgs) self:debugf("%s", msgs) end,
   ["info"]      = function(self, msgs) self:infof("%s", msgs) end,
   ["notice"]    = function(self, msgs) self:noticef("%s", msgs) end,
   ["warning"]   = function(self, msgs) self:warningf("%s", msgs) end,
   ["error"]     = function(self, msgs) self:errorf("%s", msgs) end,
   ["critical"]  = function(self, msgs) self:criticalf("%s", msgs) end,
   ["emergency"] = function(self, msgs) self:emergencyf("%s", msgs) end,
   --
   -- Used by us
   --
   ["tracef"]     = function(self, fmts, ...) print('LUA_TRACE '..string.format(fmts,...)) end,
   ["debugf"]     = function(self, fmts, ...) print('LUA_DEBUG '..string.format(fmts,...)) end,
   ["infof"]      = function(self, fmts, ...) print('LUA_INFO '..string.format(fmts,...)) end,
   ["noticef"]    = function(self, fmts, ...) print('LUA_NOTICE '..string.format(fmts,...)) end,
   ["warning"]    = function(self, fmts, ...) print('LUA_WARNING '..string.format(fmts,...)) end,
   ["errorf"]     = function(self, fmts, ...) print('LUA_ERROR '..string.format(fmts,...)) end,
   ["criticalf"]  = function(self, fmts, ...) print('LUA_CRITICAL '..string.format(fmts,...)) end,
   ["emergencyf"] = function(self, fmts, ...) print('LUA_EMERGENCY '..string.format(fmts,...)) end
}

------------------------------------------------------------------------------
logger:noticef('marpaESLIF version: %s', marpaESLIFLua.version())
------------------------------------------------------------------------------
local marpaESLIFp = marpaESLIFLua.marpaESLIF_new(logger)
logger:noticef('marpaESLIFp meta dump: %s', tableDump(getmetatable(marpaESLIFp)))
------------------------------------------------------------------------------
local marpaESLIFGrammarp = marpaESLIFp:marpaESLIFGrammar_new(
   [[
:start   ::= Expression
:default ::=             action        => do_op
                         symbol-action => do_symbol
                         free-action   => do_free
:discard ::= whitespaces event  => discard_whitespaces$
:discard ::= comment     event  => discard_comment$

event ^Number = predicted Number
event Number$ = completed Number
Number   ::= NUMBER   action => ::shift

event Expression$ = completed Expression
event ^Expression = predicted Expression
Expression ::=
    Number                                           action => do_int            name => 'Expression is Number'
    | '(' Expression ')'              assoc => group action => ::copy[1]         name => 'Expression is ()'
   ||     Expression '**' Expression  assoc => right                             name => 'Expression is **'
   ||     Expression  '*' Expression                                             name => 'Expression is *'
    |     Expression  '/' Expression                                             name => 'Expression is /'
   ||     Expression  '+' Expression                                             name => 'Expression is +'
    |     Expression  '-' Expression                                             name => 'Expression is -'

:lexeme ::= NUMBER pause => before event => ^NUMBER
:lexeme ::= NUMBER pause => after  event => NUMBER$
NUMBER     ~ /[\d]+/
whitespaces ::= WHITESPACES
WHITESPACES ~ [\s]+
comment ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/u
]]
)

logger:noticef('marpaESLIFGrammarp      dump: %s', tableDump(marpaESLIFGrammarp))
logger:noticef('marpaESLIFGrammarp meta dump: %s', tableDump(getmetatable(marpaESLIFGrammarp)))
------------------------------------------------------------------------------
local ngrammar = marpaESLIFGrammarp:ngrammar()
logger:noticef('... Number of grammars : %d', ngrammar)
------------------------------------------------------------------------------
local currentLevel = marpaESLIFGrammarp:currentLevel()
logger:noticef('... Current level      : %d', currentLevel)
------------------------------------------------------------------------------
local currentDescription = marpaESLIFGrammarp:currentDescription()
logger:noticef('... Current description: %s', currentDescription)
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local descriptionByLevel = marpaESLIFGrammarp:descriptionByLevel(level)
   logger:noticef('... Description level %d: %s', level, descriptionByLevel)
end
------------------------------------------------------------------------------
local ruleIds = marpaESLIFGrammarp:currentRuleIds()
logger:noticef('... Current rule Ids   : %s', tableDump(ruleIds))
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   logger:noticef('... Rule Ids level %d: %s', level, tableDump(ruleIdsByLevel))
end
------------------------------------------------------------------------------
local symbolIds = marpaESLIFLua.marpaESLIFGrammar_currentSymbolIds(marpaESLIFGrammarp)
logger:noticef('... Current symbol Ids : %s', tableDump(symbolIds))
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   logger:noticef('... Symbol Ids level %d: %s', level, tableDump(symbolIdsByLevel))
end
------------------------------------------------------------------------------
local currentProperties = marpaESLIFGrammarp:currentProperties()
logger:noticef('... Current properties: %s', tableDump(currentProperties))
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local propertiesByLevel = marpaESLIFGrammarp:propertiesByLevel(level)
   logger:noticef('... Properties level %d: %s', level, tableDump(propertiesByLevel))
end
------------------------------------------------------------------------------
for k, ruleId in pairsByKeys(ruleIds) do
   local currentRuleProperties = marpaESLIFGrammarp:currentRuleProperties(ruleId)
   logger:noticef('... Current rule No %d: %s', ruleId, tableDump(currentRuleProperties))
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      local rulePropertiesByLevel = marpaESLIFGrammarp:rulePropertiesByLevel(level, ruleId)
      logger:noticef('... Level %d rule No %d: %s', level, ruleId, tableDump(rulePropertiesByLevel))
   end
end
------------------------------------------------------------------------------
for k, symbolId in pairsByKeys(symbolIds) do
   local currentSymbolProperties = marpaESLIFGrammarp:currentSymbolProperties(symbolId)
   logger:noticef('... Current symbol No %d: %s', symbolId, tableDump(currentSymbolProperties))
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   for k, symbolId in pairsByKeys(symbolIdsByLevel) do
      local symbolPropertiesByLevel = marpaESLIFGrammarp:symbolPropertiesByLevel(level, symbolId)
      logger:noticef('... Level %d symbol No %d: %s', level, symbolId, tableDump(symbolPropertiesByLevel))
   end
end
------------------------------------------------------------------------------
for k, ruleId in pairsByKeys(ruleIds) do
   local ruleDisplay = marpaESLIFGrammarp:ruleDisplay(ruleId)
   logger:noticef('... Rule No %d display: %s', ruleId, ruleDisplay)
end
------------------------------------------------------------------------------
for k, symbolId in pairsByKeys(symbolIds) do
   local symbolDisplay = marpaESLIFGrammarp:symbolDisplay(symbolId)
   logger:noticef('... Symbol No %d display: %s', symbolId, symbolDisplay)
end
------------------------------------------------------------------------------
for k, ruleId in pairsByKeys(ruleIds) do
   local ruleShow = marpaESLIFGrammarp:ruleShow(ruleId)
   logger:noticef('... Rule No %d show: %s', ruleId, ruleShow)
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      local ruleDisplayByLevel = marpaESLIFGrammarp:ruleDisplayByLevel(level, ruleId)
      logger:noticef('... Level %d rule No %d display: %s', level, ruleId, ruleDisplayByLevel)
   end
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   for k, symbolId in pairsByKeys(symbolIdsByLevel) do
      local symbolDisplayByLevel = marpaESLIFGrammarp:symbolDisplayByLevel(level, symbolId)
      logger:noticef('... Level %d symbol No %d display: %s', level, symbolId, symbolDisplayByLevel)
   end
end
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   for k, ruleId in pairsByKeys(ruleIdsByLevel) do
      local ruleShowByLevel = marpaESLIFGrammarp:ruleShowByLevel(level, ruleId)
      logger:noticef('... Level %d rule No %d show: %s', level, ruleId, ruleShowByLevel)
   end
end
------------------------------------------------------------------------------
local show = marpaESLIFGrammarp:show()
logger:noticef('... Grammar show: %s', show)
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local showByLevel = marpaESLIFGrammarp:showByLevel(level)
   logger:noticef('... Level %d grammar show: %s', level, showByLevel)
end
------------------------------------------------------------------------------
local strings = {
   "(((3 * 4) + 2 * 7) / 2 - 1)/* This is a\n comment \n */** 3",
   "5 / (2 * 3)",
   "5 / 2 * 3",
   "(5 ** 2) ** 3",
   "5 * (2 * 3)",
   "5 ** (2 ** 3)",
   "5 ** (2 / 3)",
   "1 + ( 2 + ( 3 + ( 4 + 5) )",
   "1 + ( 2 + ( 3 + ( 4 + 50) ) )   /* comment after */",
   " 100"
}

--
-- C.f. https://stackoverflow.com/questions/19326368/iterate-over-lines-including-blank-lines
--
local function magiclines( str )
   local pos = 1;
    return function()
        if not pos then return nil end
        local  p1, p2 = string.find( str, "\r?\n", pos )
        local line
        if p1 then
            line = str:sub( pos, p1 - 1 )
            pos = p2 + 1
        else
            line = str:sub( pos )
            pos = nil
        end
        return line
    end
end

--
-- Test the parse interface
--
for _, localstring in pairs(strings) do
   logger:noticef('Testing parse on %s', localstring)
   local magiclinesFunction = magiclines(localstring)
   local recognizerInterface = {
      ["read"]  = function(self)
         self._data = magiclinesFunction()
         self._isEof = (self._data == nil)
         return true
      end,
      ["isEof"]                  = function(self) return self._isEof end,
      ["isCharacterStream"]      = function(self) return true end,
      ["encoding"]               = function(self) return nil end,
      ["data"]                   = function(self) return self._data end,
      ["isWithDisableThreshold"] = function(self) return false end,
      ["isWithExhaustion"]       = function(self) return false end,
      ["isWithNewline"]          = function(self) return true end,
      ["isWithTrack"]            = function(self) return true end
   }
   local valueInterface = {
      ["isWithHighRankOnly"]     = function(self) return true end,
      ["isWithOrderByRank"]      = function(self) return true end,
      ["isWithAmbiguous"]        = function(self) return false end,
      ["isWithNull"]             = function(self) return false end,
      ["maxParses"]              = function(self) return 0 end,
      ["getResult"]              = function(self) return self._result end,
      ["setResult"]              = function(self, result) self._result = result end,
      --
      -- Grammar actions
      --
      ["do_symbol"]              = function(self, symbol)
         logger:tracef("do_symbol('%s')", symbol)
         local do_symbol = symbol
         logger:tracef("do_symbol('%s') => '%s'", symbol, do_symbol)
         -- $self->trace_local_variables('do_symbol');
         return do_symbol
      end,
      ["do_int"]                 = function(self, number)
         logger:tracef("do_int('%s')", number)
         local do_int = tonumber(number)
         logger:tracef("do_int('%s') => %s", number, do_int)
         -- $self->trace_local_variables('do_symbol');
         return do_int
      end,
      ["do_free"]                = function(self, result)
         logger:tracef("do_free(%s) called and this should never happen", result)
         error("do_free() called and this should never happen")
      end,
      ["do_op"]                  = function(self, left, op, right)
         logger:tracef("do_op(%s, %s, %s)", left, op, right);
         local result
         if (op == '**') then
            result = math.pow(left, right)
         elseif (op == '*') then
            result = left * right
         elseif (op == '/') then
            result = left / right
         elseif (op == '+') then
            result = left + right
         elseif (op == '-') then
            result = left - right
         else
            error(string.format("Unsupported op %s", op))
         end
         logger:tracef("do_op(%s, %s, %s) => %s", left, op, right, result);
         -- $self->trace_local_variables('do_op');
         -- $self->trace_rule_property('do_op');
         return result
       end
   }

   local parseb = marpaESLIFGrammarp:parse(recognizerInterface, valueInterface);
   logger:noticef('... Grammar parse status: %s', tostring(parseb))
   if (parseb) then
      local result = valueInterface:getResult()
      logger:noticef('... Grammar parse result: %s', tostring(result))
   end
end

local recognizerInterface = {
   ["read"]  = function(self)
      self._data = magiclinesFunction()
      self._isEof = (self._data == nil)
      return true
   end,
   ["isEof"]                  = function(self) return self._isEof end,
   ["isCharacterStream"]      = function(self) return true end,
   ["encoding"]               = function(self) return nil end,
   ["data"]                   = function(self) return self._data end,
   ["isWithDisableThreshold"] = function(self) return false end,
   ["isWithExhaustion"]       = function(self) return false end,
   ["isWithNewline"]          = function(self) return true end,
   ["isWithTrack"]            = function(self) return true end
}
------------------------------------------------------------------------------
local marpaESLIFRecognizerp = marpaESLIFGrammarp:marpaESLIFRecognizer_new(recognizerInterface);
logger:noticef('marpaESLIFRecognizerp dump: %s', tableDump(marpaESLIFRecognizerp))
logger:noticef('marpaESLIFRecognizerp meta dump: %s', tableDump(getmetatable(marpaESLIFRecognizerp)))
marpaESLIFRecognizerp:set_exhausted_flag(true);
marpaESLIFRecognizerp:set_exhausted_flag(false);
------------------------------------------------------------------------------
local marpaESLIFRecognizerFromp = marpaESLIFRecognizerp:newFrom(marpaESLIFGrammarp);
logger:noticef('marpaESLIFRecognizerFromp dump: %s', tableDump(marpaESLIFRecognizerFromp))
logger:noticef('marpaESLIFRecognizerFromp meta dump: %s', tableDump(getmetatable(marpaESLIFRecognizerFromp)))
------------------------------------------------------------------------------
marpaESLIFRecognizerp:share(marpaESLIFRecognizerFromp);
marpaESLIFRecognizerp:share(marpaESLIFRecognizerFromp);
------------------------------------------------------------------------------
logger:noticef('marpaESLIFRecognizer::isCancontinue: %s', tostring(marpaESLIFRecognizerp:isCanContinue()))
logger:noticef('marpaESLIFRecognizer::isExhausted: %s', tostring(marpaESLIFRecognizerp:isExhausted()))
