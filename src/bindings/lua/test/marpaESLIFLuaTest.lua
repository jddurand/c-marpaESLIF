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

local logTimeStamp = function()
   return os.date("%Y/%m/%d %H:%M:%S")
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
   ["tracef"]     = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'TRACE', ...)) end,
   ["debugf"]     = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'DEBUG', ...)) end,
   ["infof"]      = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'INFO', ...)) end,
   ["noticef"]    = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'NOTICE', ...)) end,
   ["warningf"]   = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'WARN', ...)) end,
   ["errorf"]     = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'ERROR', ...)) end,
   ["criticalf"]  = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'CRITICAL', ...)) end,
   ["emergencyf"] = function(self, fmts, ...) print(string.format("%s %-9s "..fmts, logTimeStamp(), 'EMERGENCY', ...)) end
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
            line = str:sub( pos, p2 )
            pos = p2 + 1
        else
            line = str:sub( pos )
            pos = nil
        end
        return line
    end
end

--
-- C.f. https://gist.github.com/cwarden/1207556
--
function catch(what)
   return what[1]
end

function try(what)
   status, result = pcall(what[1])
   if not status then
      what[2](result)
   end
   return result
end

------------------------------------------------------------------------------
logger:noticef('marpaESLIFLua version: %s', tostring(marpaESLIFLua:version()))
logger:noticef('marpaESLIFLua version major: %s', tostring(marpaESLIFLua:versionMajor()))
logger:noticef('marpaESLIFLua version minor: %s', tostring(marpaESLIFLua:versionMinor()))
logger:noticef('marpaESLIFLua version patch: %s', tostring(marpaESLIFLua:versionPatch()))
------------------------------------------------------------------------------
local marpaESLIFp = marpaESLIFLua.marpaESLIF_new(logger)
logger:noticef('marpaESLIF version: %s', tostring(marpaESLIFp:version()))
logger:noticef('marpaESLIF version major: %s', tostring(marpaESLIFp:versionMajor()))
logger:noticef('marpaESLIF version minor: %s', tostring(marpaESLIFp:versionMinor()))
logger:noticef('marpaESLIF version patch: %s', tostring(marpaESLIFp:versionPatch()))
logger:noticef('marpaESLIFp dump: %s', tableDump(marpaESLIFp))
logger:noticef('marpaESLIFp meta dump: %s', tableDump(getmetatable(marpaESLIFp)))
------------------------------------------------------------------------------
marpaESLIFGrammarp = marpaESLIFp:marpaESLIFGrammar_new(
   [[
:start   ::= Expression
:default ::=             action        => do_op
                         symbol-action => do_symbol
                         event-action  => do_event
                         regex-action  => do_regex
:discard ::= whitespaces event  => discard_whitespaces$
:discard ::= comment     event  => discard_comment$

event ^Number = predicted Number
event Number$ = completed Number
Number   ::= NUMBER   action => ::shift

event Expression$ = completed Expression
event ^Expression = predicted Expression
Expression ::=
    Number                                           action => do_int            name => 'Expression is Number'
    | /\((?C12)/ Expression ')'              assoc => group action => ::copy[1]         name => 'Expression is ()'
   ||     Expression '**' Expression  assoc => right                             name => 'Expression is **'
   ||     Expression  '*' Expression                                             name => 'Expression is *'
    |     Expression  '/' Expression                                             name => 'Expression is /'
   ||     Expression  '+' Expression                                             name => 'Expression is +'
    |     Expression  '-' Expression                                             name => 'Expression is -'

:symbol ::= NUMBER pause => before event => ^NUMBER
:symbol ::= NUMBER pause => after  event => NUMBER$
:symbol ::= NUMBER if-action => if_number

NUMBER     ~ /[\d]+/
whitespaces ::= WHITESPACES
WHITESPACES ~ [\s]+
comment ::= /(((\/\/)([^\n]*)(\n|\z))|((\/\*)(([^\*]+|\*(?!\/))*)(\*\/)))(?C"Comment Regex Callout")/u
]]
)
-- marpaESLIFJSONp = nil
-- collectgarbage()
-- marpaESLIFGrammarp = nil
-- collectgarbage()
-- marpaESLIFp = nil
-- collectgarbage()
-- os.exit()

logger:noticef('marpaESLIFGrammarp      dump: %s', tableDump(marpaESLIFGrammarp))
logger:noticef('marpaESLIFGrammarp meta dump: %s', tableDump(getmetatable(marpaESLIFGrammarp)))
------------------------------------------------------------------------------
ngrammar = marpaESLIFGrammarp:ngrammar()
logger:noticef('... Number of grammars : %d', ngrammar)
------------------------------------------------------------------------------
currentLevel = marpaESLIFGrammarp:currentLevel()
logger:noticef('... Current level      : %d', currentLevel)
------------------------------------------------------------------------------
currentDescription = marpaESLIFGrammarp:currentDescription()
logger:noticef('... Current description: %s', currentDescription)
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local descriptionByLevel = marpaESLIFGrammarp:descriptionByLevel(level)
   logger:noticef('... Description level %d: %s', level, descriptionByLevel)
end
------------------------------------------------------------------------------
ruleIds = marpaESLIFGrammarp:currentRuleIds()
logger:noticef('... Current rule Ids   : %s', tableDump(ruleIds))
for level = 0,ngrammar-1 do
   local ruleIdsByLevel = marpaESLIFGrammarp:ruleIdsByLevel(level)
   logger:noticef('... Rule Ids level %d: %s', level, tableDump(ruleIdsByLevel))
end
------------------------------------------------------------------------------
symbolIds = marpaESLIFGrammarp:currentSymbolIds()
logger:noticef('... Current symbol Ids : %s', tableDump(symbolIds))
for level = 0,ngrammar-1 do
   local symbolIdsByLevel = marpaESLIFGrammarp:symbolIdsByLevel(level)
   logger:noticef('... Symbol Ids level %d: %s', level, tableDump(symbolIdsByLevel))
end
------------------------------------------------------------------------------
currentProperties = marpaESLIFGrammarp:currentProperties()
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
show = marpaESLIFGrammarp:show()
logger:noticef('... Grammar show: %s', show)
------------------------------------------------------------------------------
for level = 0,ngrammar-1 do
   local showByLevel = marpaESLIFGrammarp:showByLevel(level)
   logger:noticef('... Level %d grammar show: %s', level, showByLevel)
end
------------------------------------------------------------------------------
strings = {
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

------------------------------------------------------------------------------
valueInterface = {
   ["isWithHighRankOnly"]     = function(self)
      local isWithHighRankOnly = true
      logger:tracef("isWithHighRankOnly => %s", tostring(isWithHighRankOnly))
      return isWithHighRankOnly
   end,
   ["isWithOrderByRank"]      = function(self)
      local isWithOrderByRank = true
      logger:tracef("isWithOrderByRank => %s", tostring(isWithOrderByRank))
      return isWithOrderByRank
   end,
   ["isWithAmbiguous"]        = function(self)
      local isWithAmbiguous = false
      logger:tracef("isWithAmbiguous => %s", tostring(isWithAmbiguous))
      return isWithAmbiguous
   end,
   ["isWithNull"]             = function(self)
      local isWithNull = false
      logger:tracef("isWithNull => %s", tostring(isWithNull))
      return isWithNull
   end,
   ["maxParses"]              = function(self)
      local maxParses = 0
      logger:tracef("maxParses => %s", tostring(maxParses))
      return maxParses
   end,
   ["getResult"]              = function(self)
      logger:tracef("getResult => %s", tostring(self._result))
      return self._result
   end,
   ["setResult"]              = function(self, result)
      logger:tracef("setResult('%s')", tostring(result))
      self._result = result
   end,
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
recognizerInterface = {
   ["init"] = function(self, input)
      self._magiclinesFunction = magiclines(input)
      self._nbParameterizedRhsCalls = 0
   end,
   ["read"] = function(self)
      self._data = self._magiclinesFunction()
      if (self._data == nil) then
         logger:warningf("read failure")
      else
         logger:tracef("read => %s", self._data)
      end
      return true
   end,
   ["isEof"]                  = function(self)
      local isEof = (self._data == nil)
      logger:tracef("isEof => %s", tostring(isEof))
      return isEof
   end,
   ["isCharacterStream"]      = function(self)
      local isCharacterStream = true
      logger:tracef("isCharacterStream => %s", tostring(isCharacterStream))
      return isCharacterStream
   end,
   ["encoding"]               = function(self)
      local encoding = nil
      logger:tracef("encoding => %s", tostring(encoding))
      return nil
   end,
   ["data"]                   = function(self)
      local data = self._data
      logger:tracef("data => %s", tostring(data))
      return self._data
   end,
   ["isWithDisableThreshold"] = function(self)
      local isWithDisableThreshold = false
      logger:tracef("isWithDisableThreshold => %s", tostring(isWithDisableThreshold))
      return isWithDisableThreshold
   end,
   ["isWithExhaustion"]       = function(self)
      local isWithExhaustion = false
      logger:tracef("isWithExhaustion => %s", tostring(isWithExhaustion))
      return isWithExhaustion
   end,
   ["isWithNewline"]          = function(self)
      local isWithNewline = true
      logger:tracef("isWithNewline => %s", tostring(isWithNewline))
      return isWithNewline
   end,
   ["isWithTrack"]            = function(self)
      local isWithTrack = true
      logger:tracef("isWithTrack => %s", tostring(isWithTrack))
      return isWithTrack
   end,
   ["setRecognizer"]            = function(self, recognizer)
      logger:tracef("setRecognizer <= %s", tostring(recognizer))
      self._recognizer = recognizer
   end,
   ["getRecognizer"]            = function(self)
      logger:tracef("getRecognizer => %s", tostring(self._recognizer))
      return self._recognizer
   end,
   ["if_number"]            = function(self, lexeme)
      local if_number = true
      logger:tracef("if_number('%s') => %s", tostring(lexeme), tostring(if_number))
      logger:tracef('if_number: recognizer             = %s', tostring(self:getRecognizer()))
      logger:tracef('if_number: recognizer.input(0, 3) = %s', tostring(self:getRecognizer():input(0, 3)))
      return if_number
   end,
   ["do_event"]            = function(self, events)
      local do_event = true
      logger:tracef('%s', tableDump(events))
      logger:tracef("do_event => true", tostring(do_event))
      logger:tracef('do_event: recognizer             = %s', tostring(self:getRecognizer()))
      logger:tracef('do_event: recognizer.input(0, 3) = %s', tostring(self:getRecognizer():input(0, 3)))
      return do_event
   end,
   ["do_regex"]             = function(self, block)
      logger:tracef("do_regex(%s)", block)
      logger:tracef('do_regex: calloutNumber          = %s, type %s', tostring(block:getCalloutNumber()), type(block:getCalloutNumber()))
      logger:tracef('do_regex: calloutString          = %s, type %s', tostring(block:getCalloutString()), type(block:getCalloutString()))
      logger:tracef('do_regex: subject                = %s, type %s', tostring(block:getSubject()), type(block:getSubject()))
      logger:tracef('do_regex: pattern                = %s, type %s', tostring(block:getPattern()), type(block:getPattern()))
      logger:tracef('do_regex: captureTop             = %s, type %s', tostring(block:getCaptureTop()), type(block:getCaptureTop()))
      logger:tracef('do_regex: captureLast            = %s, type %s', tostring(block:getCaptureLast()), type(block:getCaptureLast()))
      logger:tracef('do_regex: offsetVector           = %s', tableDump(block:getOffsetVector()))
      logger:tracef('do_regex: mark                   = %s, type %s', tostring(block:getMark()), type(block:getMark()))
      logger:tracef('do_regex: currentPosition        = %s, type %s', tostring(block:getCurrentPosition()), type(block:getCurrentPosition()))
      logger:tracef('do_regex: nextItem               = %s, type %s', tostring(block:getNextItem()), type(block:getNextItem()))
      logger:tracef('do_regex: grammarLevel           = %s, type %s', tostring(block:getGrammarLevel()), type(block:getGrammarLevel()))
      logger:tracef('do_regex: symbolId               = %s, type %s', tostring(block:getSymbolId()), type(block:getSymbolId()))
      logger:tracef('do_regex: recognizer             = %s, type %s', tostring(self:getRecognizer()), type(self:getRecognizer()))
      logger:tracef('do_regex: recognizer.input(0, 3) = %s, type %s', tostring(self:getRecognizer():input(0, 3)), type(self:getRecognizer():input(0, 3)))
      return 0
   end,
   ["parameterizedRhs"]             = function(self, parameter, undef, explanation)
      self._nbParameterizedRhsCalls = self._nbParameterizedRhsCalls + 1

      local output
      if (self._nbParameterizedRhsCalls == 5) then
        output = "start ::= '5'"
      elseif (self._nbParameterizedRhsCalls > 5) then
        output = "start ::= 'no match'"
      else
        parameter = parameter + 1
        output = "start ::= . => parameterizedRhs->("..parameter..", { x = 'Value of x', y = 'Value of y' }, 'Input should be \""..parameter.."\"')"
      end
      logger:tracef("parameterizedRhs([%s] %s, [%s] %s, [%s] %s) => %s", type(parameter), tostring(parameter), type(undef), tostring(undef), type(explanation), tostring(explanation), output)
      logger:tracef('parameterizedRhs: recognizer             = %s', tostring(self:getRecognizer()))
      logger:tracef('parameterizedRhs: recognizer.input(0, 3) = %s', tostring(self:getRecognizer():input(0, 3)))
      return output
   end
}

--
-- Test the parse interface
--
local regexPattern = "[\\d]+"
local stringPattern = "'('"
local metaGrammarp = marpaESLIFp:marpaESLIFGrammar_new([[
<anything up to newline>        ::= <ANYTHING UP TO NEWLINE>
<ANYTHING UP TO NEWLINE>          ~ /[^\n]*/
]]
)

show = metaGrammarp:show()
logger:noticef('... External Meta Grammar show: %s', show)

marpaESLIFSymbolRegexp = marpaESLIFp:marpaESLIFSymbol_new('regex', regexPattern)
marpaESLIFSymbolStringp = marpaESLIFp:marpaESLIFSymbol_new('string', stringPattern)
metaSymbolName = 'ANYTHING UP TO NEWLINE'
marpaESLIFSymbolMetap = marpaESLIFp:marpaESLIFSymbol_new('meta', metaGrammarp, metaSymbolName)

for _, localstring in pairs(strings) do
   logger:noticef('Testing external symbols on %s', localstring)

   logger:noticef('... Testing regex symbol %s', regexPattern)
   local regexMatch = marpaESLIFSymbolRegexp:try(localstring)
   logger:noticef('... ... Regex symbol %s result: %s', regexPattern, regexMatch)

   logger:noticef('... Testing string symbol %s', stringPattern)
   local stringMatch = marpaESLIFSymbolStringp:try(localstring)
   logger:noticef('... ... String symbol %s result: %s', stringPattern, stringMatch)

   logger:noticef('... Testing meta symbol %s', metaSymbolName)
   local stringMatch = marpaESLIFSymbolMetap:try(localstring)
   logger:noticef('... ... Meta symbol %s result: %s', metaSymbolName, stringMatch)

   logger:noticef('Testing parse on %s', localstring)
   recognizerInterface:init(localstring)
   local parseb = marpaESLIFGrammarp:parse(recognizerInterface, valueInterface)
   logger:noticef('... Grammar parse status: %s', tostring(parseb))
   if (parseb) then
      local result = valueInterface:getResult()
      logger:noticef('... Grammar parse result: %s', tostring(result))
   end
end

--
-- Test the scan/resume interface
--
showEvents = function(context, eslifRecognizer)
   logger:debugf("[%s] Events: %s", context, tableDump(eslifRecognizer:events()))
end

showRecognizerInput = function(context, eslifRecognizer)
   local input = eslifRecognizer:input(0)
   logger:debugf("[%s] Recognizer buffer:\n%s", context, tostring(input))
   for offset=-5,5 do
     input = eslifRecognizer:input(offset)
     logger:debugf("[%s] ... ... input(%d) returns: %s", context, offset, tostring(input))
     for length=-5,5 do
       input = eslifRecognizer:input(offset, length)
       logger:debugf("[%s] ... ... ... input(%d,%d) returns: %s", context, offset, length, tostring(input))
     end
   end
end

showNameExpected = function(context, eslifRecognizer)
   logger:debugf("[%s] Expected lexemes: %s", context, tableDump(eslifRecognizer:nameExpected()))
end

showLocation = function(context, eslifRecognizer)
   local line = eslifRecognizer:line()
   local column = eslifRecognizer:column()
   local location = eslifRecognizer:location()
   if ((line ~= location.line) or (column ~= location.column)) then
      error(string.format("eslifRecognizer->location() is not equivalent to (eslifRecognizer:line(), eslifRecognizer:column())"))
   end
   logger:debugf("[%s] Location is %d:%d", context, line, column)
end

showInputLength = function(context, eslifRecognizer)
   local inputLength = eslifRecognizer:inputLength()
   logger:debugf("[%s] Input length is %d", context, inputLength)
end

showInput = function(context, eslifRecognizer)
   local input = eslifRecognizer:input(0)
   logger:debugf("[%s] Input is %s", context, tostring(input))
end

showError = function(context, eslifRecognizer)
   logger:debugf("[%s] Simulating error report:", context)
   eslifRecognizer:error()
end

doScan = function(eslifRecognizer, initialEvents)
   logger:debugf(" =============> scan(initialEvents=%s)", tostring(initialEvents))
    if (not eslifRecognizer:scan(initialEvents)) then
       return false
    end
    local context = "after scan"
    showRecognizerInput(context, eslifRecognizer)
    showEvents(context, eslifRecognizer)
    showNameExpected(context, eslifRecognizer)
		
    return true
end

doResume = function(eslifRecognizer, deltaLength)
   local context
		
   logger:debugf(" =============> resume(deltaLength=%d)", deltaLength)
   if (not eslifRecognizer:resume(deltaLength)) then
      return false
   end

   context = "after resume"
   showRecognizerInput(context, eslifRecognizer)
   showEvents(context, eslifRecognizer)
   showNameExpected(context, eslifRecognizer)
		
   return true
end

doAlternativeRead = function(eslifRecognizer, symbol, value, pause)
   --
   -- "pause" is a "lua string", i.e. nothing else but a sequence of bytes
   -- returned by marpaESLIF, guaranteed to be in UTF-8 encoding
   --
   -- Remember that lua's length() returns the number of BYTES indeed.
   --
   local length = #pause
   local context
   logger:debugf("... Forcing Integer %s spanned on %d bytes instead of \"%s\"", tostring(value), length, tostring(pause))
   if (not eslifRecognizer:alternativeRead(symbol, value, length, 1)) then
      return false
   end

   context = "after alternativeRead"
   showRecognizerInput(context, eslifRecognizer)
   showEvents(context, eslifRecognizer)
   showNameExpected(context, eslifRecognizer)

   return true
end

function catch(what)
   return what[1]
end

function try(what)
   status, result = pcall(what[1])
   if not status then
      what[2](result)
   end
   return result
end

doDiscardTry = function(eslifRecognizer)

   local test
   try {
      function()
         test = eslifRecognizer:discardTry()
         logger:debugf("... Testing discard at current position returns %s", tostring(test))
         if (test) then
            local todiscard = eslifRecognizer:discardLastTry()
            logger:debugf("... Testing discard at current position gave \"%s\" (%d bytes)", tostring(todiscard), #todiscard)
            local discardedbytes = eslifRecognizer:discard()
            logger:debugf("... Applying discard at current position removed %d bytes, prediction gave '%s' (%d bytes)", discardedbytes, tostring(todiscard), #todiscard);
         end
      end,
      catch {
         function(message)
            logger:debug(message)
         end
      }
   }
end

doTry = function(eslifRecognizer, symbol)

   local test
   try {
      function()
         test = eslifRecognizer:nameTry(symbol)
         logger:debugf("... Testing %s lexeme at current position returns %s", symbol, tostring(test))
         if (test) then
            local lastTry = eslifRecognizer:nameLastTry()
            logger:debugf("... Testing symbol %s at current position gave \"%s\"", symbol, lastTry)
         end
      end,
      catch {
         function(message)
            logger:debug(message)
         end
      }
   }
end

showLastCompletion = function(context, eslifRecognizer, symbol, origin)
   try {
      function()
         --
         -- Take care, offsets returned by marpaESLIFLua are 0 based
         --
         local lastExpressionOffset = eslifRecognizer:lastCompletedOffset(symbol)
         local lastExpressionLength = eslifRecognizer:lastCompletedLength(symbol)
         local lastExpressionLocation = eslifRecognizer:lastCompletedLocation(symbol)
         if ((lastExpressionOffset ~= lastExpressionLocation.offset) or (lastExpressionLength ~= lastExpressionLocation.length)) then
            error("eslifRecognizer:lastCompletedLocation() is not equivalent to (eslifRecognizer:lastCompletedOffset, eslifRecognizer:lastCompletedLength)")
         end
         local lastExpressionStart = lastExpressionOffset + 1
         local lastExpressionEnd = lastExpressionStart + lastExpressionLength
         local matchedString = string.sub(origin, lastExpressionStart, lastExpressionEnd)
         logger:debugf("[%s] Last %s completion is %s", context, symbol, matchedString)
      end,
      catch {
         function(message)
            logger:warningf("[%s] Last %s completion raised an exception, %s", context, symbol, message)
         end
      }
   }
end

changeEventState = function(context, eslifRecognizer, eventType, symbol, eventTypes, state)
   logger:debugf("[%s] Changing event state %d of symbol %s to %s", context, eventType, symbol, state)
   eslifRecognizer:eventOnOff(symbol, eventTypes, state)
end

local symbolRegexPattern = '[\\d]+'
local symbolRegex  = marpaESLIFp:marpaESLIFSymbol_new('regex', symbolRegexPattern);

local symbolStringPattern = '"("'
local symbolString = marpaESLIFp:marpaESLIFSymbol_new('string', symbolStringPattern);

i = 0
for _, localstring in pairs(strings) do
   local context = "main loop"
   logger:noticef('Testing scan/resume on %s', localstring)
   recognizerInterface:init(localstring)
   local eslifRecognizer = marpaESLIFGrammarp:marpaESLIFRecognizer_new(recognizerInterface)
   local matchRegex = eslifRecognizer:symbolTry(symbolRegex)
   if (matchRegex == nil) then
      logger:infof("%s: no match", symbolRegexPattern)
   else
      logger:infof("%s: match: %s", symbolRegexPattern, matchRegex)
   end
   local matchString = eslifRecognizer:symbolTry(symbolString)
   if (matchString == nil) then
      logger:infof("%s: no match", symbolStringPattern)
   else
      logger:infof("%s: match: %s", symbolStringPattern, matchString)
   end
   if (doScan(eslifRecognizer, true)) then
      showLocation("After doScan", eslifRecognizer)
      showInputLength("After doScan", eslifRecognizer)
      showInput("After doScan", eslifRecognizer)
      showError("After doScan", eslifRecognizer)
      logger:infof('isStartComplete: %s', eslifRecognizer:isStartComplete())
      if (not eslifRecognizer:isEof()) then
         if (not eslifRecognizer:read()) then
            break
         end
         showRecognizerInput(context, eslifRecognizer)
      end
      local j = 0
      while (eslifRecognizer:isCanContinue()) do
         logger:infof('isStartComplete: %s', eslifRecognizer:isStartComplete())
         logger:debugf("[%s] Progress log:", context)
         eslifRecognizer:progressLog(-1, -1, GENERICLOGGER_LOGLEVEL_NOTICE)
         local progress = eslifRecognizer:progress(-1, -1)
         logger:infof('Progress: %s', tableDump(progress))
         if (not doResume(eslifRecognizer, 0)) then
            break
         end
         showLocation(string.format("Loop No %d", j), eslifRecognizer)
         local events = eslifRecognizer:events()
         for k, v in pairsByKeys(events) do
            if (v.event == "^NUMBER") then
               --
               -- Take opportunity of this event to test the hooks
               --
               eslifRecognizer:hookDiscard(false)
               eslifRecognizer:hookDiscard(true)
               --
               -- Recognizer will wait forever if we do not feed the number
               --
               local pause = eslifRecognizer:nameLastPause("NUMBER")
               if (pause == nil) then
                  error("Pause before on NUMBER but no pause information!")
               end
               if (not doAlternativeRead(eslifRecognizer, "NUMBER", j, pause)) then
                  error("NUMBER expected but reading such lexeme fails!")
               end
               doTry(eslifRecognizer, "WHITESPACES")
               doTry(eslifRecognizer, "whitespaces")
               doDiscardTry(eslifRecognizer)
            end
         end
         if (j == 0) then
            changeEventState(string.format("Loop No %d", j), eslifRecognizer, MARPAESLIF_EVENTTYPE_PREDICTED, "Expression", { MARPAESLIF_EVENTTYPE_PREDICTED }, false)
            changeEventState(string.format("Loop No %d", j), eslifRecognizer, MARPAESLIF_EVENTTYPE_DISCARD, "whitespaces", { MARPAESLIF_EVENTTYPE_DISCARD }, false)
            changeEventState(string.format("Loop No %d", j), eslifRecognizer, MARPAESLIF_EVENTTYPE_AFTER, "NUMBER", { MARPAESLIF_EVENTTYPE_AFTER }, false)
         end
         showLastCompletion(string.format("Loop No %d", j), eslifRecognizer, "Expression", localstring)
         showLastCompletion(string.format("Loop No %d", j), eslifRecognizer, "Number", localstring)
         j = j + 1
      end
      local discardLast = eslifRecognizer:discardLast()
      logger:infof("Last discard: %s", discardLast)
      try {
         function()
            logger:infof("Testing value() on %s", localstring)
            local value = eslifRecognizer:marpaESLIFValue_new(valueInterface)
            while (value:value()) do
               logger:infof("Result: %s", valueInterface:getResult())
            end
         end,
         catch {
            function(message)
               logger:errorf("Cannot value the input: %s", message)
            end
         }
      }
   end
   i = i + 1
end

------------------------------------------------------------------------------
marpaESLIFJSONDecoderp = marpaESLIFp:marpaESLIFJSONDecoder_new()
strings = {
   "null",
   "{\"key1\": [1, \"string inside array\", 3]}",
   "{\"key1\": {\"one\": 1, \"two\": \"string inside array\", \"three\": 3}}",
   "{\"@context\":\"https://raw.githubusercontent.com/codemeta/codemeta/master/codemeta.jsonld\",\"_not_in_schema\": \"This invalid JSON name should cause an error when using JSON-LD expand/compact\",\"_description\": \"This JSON file is used in the Travis CI tests to ensure invalid JSON names are detected during JSON-LD expand / compact operations\"}",
   "[{\"description\": \"ECMA 262 regex non-compliance\",\"schema\": { \"format\": \"regex\" },\"tests\": [{\"description\": \"ECMA 262 has no support for \\\\Z anchor from .NET\",\"data\": \"^\\\\S(|(.|\\\\n)*\\\\S)\\\\Z\",\"valid\": false}]}]",
   "{\"+inf\": +Infinity, \"-inf\": -Infinity, \"+nan\": +NaN, \"-nan\": -NaN, \"number\": 1.234e+5, \"string\": \"the string\", \"array\": [\"indice1\", \"indice2\"], \"object\": {\"key1\": null, \"key1\": null, \"key2\": [1,2,3]}}",
}
for _, localstring in pairs(strings) do
   logger:infof('Testing decode on %s', localstring)
   try {
      function()
         decoded = marpaESLIFJSONDecoderp:decode(localstring, nil)
         logger:infof('%s', tableDump(decoded))
      end,

      catch {
         function(error)
            print('caught error: ' .. error)
         end
      }
   }
end

------------------------------------------------------------------------------
::json::
logger:infof('---------------------------------------------------------------------------------------')
logger:infof('Testing marpaESLIFp:marpaESLIFJSONEncoder_new and marpaESLIFp:marpaESLIFJSONDecoder_new')
logger:infof('---------------------------------------------------------------------------------------')
marpaESLIFJSONEncoderp = marpaESLIFp:marpaESLIFJSONEncoder_new()
marpaESLIFJSONDecoderp = marpaESLIFp:marpaESLIFJSONDecoder_new()

logger:infof('marpaESLIFp meta table')
stringencoded = marpaESLIFJSONEncoderp:encode(getmetatable(marpaESLIFp))
logger:infof('... encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONDecoderp:decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('marpaESLIFGrammarp meta table')
stringencoded = marpaESLIFJSONEncoderp:encode(getmetatable(marpaESLIFGrammarp))
logger:infof('... encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONDecoderp:decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof("table { keya=1, keyb=2, keyc='x', keyy='y', {7,8,9} }")
value = { keya=1, keyb=2, keyc='x', keyy='y', {7,8,9} }
stringencoded = marpaESLIFJSONEncoderp:encode(value)
logger:infof('table encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONDecoderp:decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('niled table { 1=nil, 2=nil }')
tniledtablekv = niledtablekv(1,nil, 2,nil)
stringencoded = marpaESLIFJSONEncoderp:encode(tniledtablekv)
logger:infof('niledtablekv encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONDecoderp:decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('niled array { 1, nil, nil }')
tniledarray = niledarray(1,nil,nil)
stringencoded = marpaESLIFJSONEncoderp:encode(tniledarray)
logger:infof('niledarray encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONDecoderp:decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('----------------------------------')
logger:infof('Testing marpaESLIFJSON handy class')
logger:infof('----------------------------------')
marpaESLIFJSONp = marpaESLIFJSON.new(marpaESLIFp)
logger:infof('marpaESLIFJSONp dump: %s', tableDump(marpaESLIFJSONp))
logger:infof('marpaESLIFJSONp meta dump: %s', tableDump(getmetatable(marpaESLIFJSONp)))

logger:infof('marpaESLIFp meta table')
stringencoded = marpaESLIFJSONp.encode(getmetatable(marpaESLIFp))
logger:infof('... encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONp.decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('marpaESLIFGrammarp meta table')
stringencoded = marpaESLIFJSONp.encode(getmetatable(marpaESLIFGrammarp))
logger:infof('... encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONp.decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof("table { keya=1, keyb=2, keyc='x', keyy='y', {7,8,9} }")
value = { keya=1, keyb=2, keyc='x', keyy='y', {7,8,9} }
stringencoded = marpaESLIFJSONp.encode(value)
logger:infof('table encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONp.decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('niled table { 1=nil, 2=nil }')
tniledtablekv = niledtablekv(1,nil, 2,nil)
stringencoded = marpaESLIFJSONp.encode(tniledtablekv)
logger:infof('niledtablekv encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONp.decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

logger:infof('niled array { 1, nil, nil }')
tniledarray = niledarray(1,nil,nil)
stringencoded = marpaESLIFJSONp.encode(tniledarray)
logger:infof('niledarray encoded: %s, encoding=%s', stringencoded, stringencoded:encoding())
stringdecoded = marpaESLIFJSONp.decode(stringencoded)
logger:infof('... decoded: %s', tableDump(stringdecoded))

------------------------------------------------------------------------------
::parameterizedRules::
logger:infof('---------------------------------------------------------------------------------------')
logger:infof('Testing parameterizedRules')
logger:infof('---------------------------------------------------------------------------------------')

marpaESLIFGrammarp = marpaESLIFp:marpaESLIFGrammar_new([[
top  ::= rhs1
rhs1 ::= . => parameterizedRhs->(1, nil, 'Input should be "1"')
       | . => parameterizedRhs->(2, nil, 'Input should be "2"')
       | . => parameterizedRhs->(3, nil, 'Input should be "3"')
       | . => parameterizedRhs->(4, nil, 'Input should be "4"')
       | . => ::luac->function(x,y,z) return x^y end ->(10,11,12)
       | . => ::lua->lua_action->(10,12) /* Voluntarily undefined */
rhs2 ::= . => parameterizedRhs->(1, 'Input should be "1"')
       | . => parameterizedRhs->(2, 'Input should be "2"')
       | . => parameterizedRhs->(3, 'Input should be "3"')
       | . => parameterizedRhs->(4, 'Input should be "4"')
       | . => ::lua->function(x,z) return x^y end ->(10,12)
]])
logger:noticef('... Grammar show: %s', marpaESLIFGrammarp:show())
recognizerInterface:init('5')
marpaESLIFGrammarp:parse(recognizerInterface, valueInterface)
logger:noticef('... Grammar parse result: %s', valueInterface:getResult())
