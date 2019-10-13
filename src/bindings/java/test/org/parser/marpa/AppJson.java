package org.parser.marpa;

import java.io.BufferedReader;
import java.io.StringReader;
import java.util.concurrent.TimeUnit;

/**
 * Import/export Application
 */
public class AppJson implements Runnable {
	ESLIFLoggerInterface eslifLogger;
	
	/**
	 * @param eslifLogger logger interface
	 */
	public AppJson(ESLIFLoggerInterface eslifLogger) {
		this.eslifLogger = eslifLogger;
	}

	public void run() {
		ESLIF eslif;
		try {
			eslif = ESLIF.getInstance(eslifLogger);
		} catch (InterruptedException e) {
			e.printStackTrace();
			return;
		}

		if (false) {
			eslifLogger.info(" ATTACH ME");
			try {
				TimeUnit.SECONDS.sleep(10);
			} catch (InterruptedException e) {
				System.err.println(e);
			}
		}

		final String grammar = 
				" \n" +
						"# \n" +
						"# Default action is to propagate the first RHS value \n" +
						"# \n" +
						":default ::= action => ::shift \n" +
						" \n" +
						"                   ####################################################### \n" +
						"                   # >>>>>>>>>>>>>>>> Strict JSON Grammar <<<<<<<<<<<<<<<< \n" +
						"                   ####################################################### \n" +
						" \n" +
						"# ----------------------------------------- \n" +
						"# Start is a value that we want stringified \n" +
						"# ----------------------------------------- \n" +
						":start ::= value2string \n" +
						"value2string ::= value action => ::json \n" +
						" \n" +
						"# ------------------- \n" +
						"# Composite separator \n" +
						"# ------------------- \n" +
						"comma    ::= ','                                  action         => ::undef   # No-op anyway, override ::shift (default action) \n" +
						" \n" +
						"# ---------- \n" +
						"# JSON value \n" +
						"# ---------- \n" +
						"value    ::= string                                                           # ::shift (default action) \n" +
						"           | number                                                           # ::shift (default action) \n" +
						"           | object                                                           # ::shift (default action) \n" +
						"           | array                                                            # ::shift (default action) \n" +
						"           | 'true'                               action => ::true            # built-in true action \n" +
						"           | 'false'                              action => ::false           # built-in false action \n" +
						"           | 'null'                               action => ::lua->lua_null   # built-in undef action \n" +
						" \n" +
						"# ----------- \n" +
						"# JSON object \n" +
						"# ----------- \n" +
						"object   ::= (-'{'-) members (-'}'-)                                          # ::shift (default action) \n" +
						"members  ::= pairs*                               action         => ::lua->lua_members   # Returns { @{pairs1}, ..., @{pair2} } \n" +
						"                                                  separator      => comma     # ... separated by comma \n" +
						"                                                  proper         => 1         # ... with no trailing separator \n" +
						"                                                  hide-separator => 1         # ... and hide separator in the action \n" +
						"                                                   \n" +
						"pairs    ::= string (-':'-) value                 action         => ::lua->lua_pairs     # Returns [ string, value ] \n" +
						" \n" +
						"# ----------- \n" +
						"# JSON Arrays \n" +
						"# ----------- \n" +
						"array    ::= (-'['-) elements (-']'-)                                         # ::shift (default action) \n" +
						"elements ::= value*                               action => ::row             # Returns [ value1, ..., valuen ] \n" +
						"                                                  separator      => comma     # ... separated by comma \n" +
						"                                                  proper         => 1         # ... with no trailing separator \n" +
						"                                                  hide-separator => 1         # ... and hide separator in the action \n" +
						"                                                   \n" +
						" \n" +
						"# ------------ \n" +
						"# JSON Numbers \n" +
						"# ------------ \n" +
						"number ::= NUMBER                                 action => ::lua->lua_number # Prepare for eventual bignum extension \n" +
						" \n" +
						":lexeme ::= NUMBER if-action => if_number\n" +
						"NUMBER   ~ _INT \n" +
						"         | _INT _FRAC \n" +
						"         | _INT _EXP \n" +
						"         | _INT _FRAC _EXP \n" +
						"_INT     ~ _DIGIT \n" +
						"         | _DIGIT19 _DIGITS \n" +
						"         | '-' _DIGIT \n" +
						"         | '-' _DIGIT19 _DIGITS \n" +
						"_DIGIT   ~ [0-9] \n" +
						"_DIGIT19 ~ [1-9] \n" +
						"_FRAC    ~ '.' _DIGITS \n" +
						"_EXP     ~ _E _DIGITS \n" +
						"_DIGITS  ~ /[0-9]+/ \n" +
						"_E       ~ /e[+-]?/i \n" +
						" \n" +
						"# ----------- \n" +
						"# JSON String \n" +
						"# ----------- \n" +
						"string     ::= '\"' discardOff chars '\"' discardOn action => ::copy[2] \n" +
						"discardOff ::=                                                                        # Nullable rule used to disable discard \n" +
						"discardOn  ::=                                                                        # Nullable rule used to enable discard \n" +
						" \n" +
						"event :discard[on]  = nulled discardOn                                                # Implementation of discard disabing using reserved ':discard[on]' keyword \n" +
						"# event :discard[switch]  = nulled discardOn                                            # Implementation of discard disabing using reserved ':discard[switch]' keyword \n" +
						"event :discard[off] = nulled discardOff                                               # Implementation of discard enabling using reserved ':discard[off]' keyword \n" +
						" \n" +
						"chars   ::= filled                                  action => ::lua->lua_chars \n" +
						"filled  ::= char+                                   action => ::concat                # Returns join('', char1, ..., charn) \n" +
						"chars   ::=                                         action => ::lua->lua_empty_string # Prefering empty string instead of undef \n" +
						"char    ::= /[^\"\\\\\\x00-\\x1F]+/                                                   # ::shift (default action) - take care PCRE2 [:cntrl:] includes DEL character \n" +
						"          | '\\\\' '\"'                             action => ::copy[1]               # Returns double quote, already ok in data \n" +
						"          | '\\\\' '\\\\'                           action => ::copy[1]               # Returns backslash, already ok in data \n" +
						"          | '\\\\' '/'                              action => ::copy[1]               # Returns slash, already ok in data \n" +
						"          | '\\\\' 'b'                              action => ::u8\"\\x{08}\" \n" +
						"          | '\\\\' 'f'                              action => ::u8\"\\x{0C}\" \n" +
						"          | '\\\\' 'n'                              action => ::u8\"\\x{0A}\" \n" +
						"          | '\\\\' 'r'                              action => ::u8\"\\x{0D}\" \n" +
						"          | '\\\\' 't'                              action => ::u8\"\\x{09}\" \n" +
						"          | /(?:\\\\u[[:xdigit:]]{4})+/             action => ::lua->lua_unicode \n" +
						" \n" +
						" \n" +
						"# ------------------------- \n" +
						"# Unsignificant whitespaces \n" +
						"# ------------------------- \n" +
						":discard ::= /[\\x{9}\\x{A}\\x{D}\\x{20}]+/ \n" +
						" \n" +
						"                   ####################################################### \n" +
						"                   # >>>>>>>>>>>>>>>>>> JSON Extensions <<<<<<<<<<<<<<<<<< \n" +
						"                   ####################################################### \n" +
						" \n" +
						"# -------------------------- \n" +
						"# Unlimited commas extension \n" +
						"# -------------------------- \n" +
						"# /* Unlimited commas */commas   ::= comma+ \n" +
						" \n" +
						"# -------------------------- \n" +
						"# Perl comment extension \n" +
						"# -------------------------- \n" +
						":discard ::= perl_comment event => perl_comment$ \n" +
						"perl_comment ::= /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u \n" +
						" \n" +
						"# -------------------------- \n" +
						"# C++ comment extension \n" +
						"# -------------------------- \n" +
						"# /* C++ comment */:discard ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/ \n" +
						" \n" +
						"# ---------------- \n" +
						"# Number extension \n" +
						"# ---------------- \n" +
						"# \n" +
						"# number ::= /\\-?(?:(?:[1-9]?[0-9]+)|[0-9])(?:\\.[0-9]+)?(?:[eE](?:[+-])?[0-9]+)?/ # /* bignum */action => ::lua->lua_number \n" +
						" \n" +
						"# /* nan */number   ::= '-NaN':i                               action => ::lua->lua_nan \n" +
						"# /* nan */number   ::=  'NaN':i                               action => ::lua->lua_nan \n" +
						"# /* nan */number   ::= '+NaN':i                               action => ::lua->lua_nan \n" +
						"# /* inf */number   ::= '-Infinity':i                          action => ::lua->lua_negative_infinity \n" +
						"# /* inf */number   ::=  'Infinity':i                          action => ::lua->lua_positive_infinity \n" +
						"# /* inf */number   ::= '+Infinity':i                          action => ::lua->lua_positive_infinity \n" +
						"# /* inf */number   ::= '-Inf':i                               action => ::lua->lua_negative_infinity \n" +
						"# /* inf            ::=  'Inf':i                               action => ::lua->lua_positive_infinity \n" +
						"# /* inf */number   ::= '+Inf':i                               action => ::lua->lua_positive_infinity \n" +
						" \n" +
						"# ----------------- \n" +
						"# Control character \n" +
						"# ----------------- \n" +
						"# /* cntrl */char      ::= /[\\x00-\\x1F]/                                                          # Because [:cntrl:] includes DEL (x7F) \n" +
						" \n" +
						"# ----------------- \n" +
						"# Lua actions       \n" +
						"# ----------------- \n" +
						"<luascript> \n" +
						"  ----------------------------------- \n" +
						"  function lua_null() \n" +
						"    -- Special case to have nil persistency: we will return a table saying we want it to be opaque to marpaESLIF: \n" +
						" \n" +
						"    -- This table's metatable will host: an opaque flag and the representation. \n" +
						"    -- The __marpaESLIF_opaque boolean metafield gives the opaque flag. \n" +
						"    -- The __tostring standard metafield gives the representation, and must be a function that returns a string. \n" +
						"    local _mt = {} \n" +
						"    _mt.opaque = true \n" +
						"    _mt.__tostring = function() return 'null' end \n" +
						" \n" +
						"    local _result = {} \n" +
						"    setmetatable(_result, _mt)  \n" +
						"    return _result \n" +
						"  end \n" +
						"  ----------------------------------- \n" +
						"  function lua_members(...) \n" +
						"    local _result = {} \n" +
						"    for _i=1,select('#', ...) do \n" +
						"      local _pair = select(_i, ...) \n" +
						"      local _key = _pair[1] \n" +
						"      local _value = _pair[2] \n" +
						"      _result[_key] = _value \n" +
						"    end \n" +
						"    local _mt = {} \n" +
						"    _mt.canarray = false -- hint to say that we never want that to appear as a marpaESLIF array \n" +
						"    setmetatable(_result, _mt) \n" +
						"    return _result \n" +
						"  end \n" +
						"  ----------------------------------- \n" +
						"  function lua_pairs(key, value) \n" +
						"    local _pair = {[1] = key, [2] = value} \n" +
						"    return _pair \n" +
						"  end \n" +
						"  ----------------------------------- \n" +
						"  function lua_number(number) \n" +
						"    local _result = tonumber(number) \n" +
						"    return _result \n" +
						"  end \n" +
						"  ----------------------------------- \n" +
						"  function lua_empty_string() \n" +
						"    local _result = '' \n" +
						"    _result:encoding('UTF-8') \n" +
						"    return _result \n" +
						"  end \n" +
						"  ----------------------------------- \n" +
						"  function lua_chars(chars) \n" +
						"    local _result = chars \n" +
						"    _result:encoding('UTF-8') \n" +
						"    return _result \n" +
						"  end \n" +
						"  ----------------------------------- \n" +
						"  function lua_unicode(u) \n" +
						"    local _hex = {} \n" +
						"    local _maxpos = string.len(u) \n" +
						"    local _nextArrayIndice = 1 \n" +
						"    local _pos = 1 \n" +
						" \n" +
						"    -- Per def u is a sequence of [::xdigit::]{4} i.e. 6 'characters', ahem bytes \n" +
						"    while (_pos < _maxpos) do \n" +
						"       -- Extract the [[:xdigit:]]{4} part \n" +
						"      local _codepointAsString = string.sub(u, _pos + 2, _pos + 5) \n" +
						"      local _codepoint = tonumber(_codepointAsString, 16) \n" +
						"      _hex[_nextArrayIndice] = _codepoint \n" +
						"      _nextArrayIndice = _nextArrayIndice + 1 \n" +
						"      _pos = _pos + 6 \n" +
						"    end \n" +
						" \n" +
						"    local _unicode = '' \n" +
						"    local _high \n" +
						"    local _low \n" +
						"    local _codepoint \n" +
						"    while (#_hex > 0) do \n" +
						"      if (#_hex > 1) then \n" +
						"        _high, _low = table.unpack(_hex, 1, 2) \n" +
						"        -- UTF-16 surrogate pair ? \n" +
						"        if ((_high >= 0xD800) and (_high <= 0xDBFF) and (_low >= 0xDC00) and (_low <= 0xDFFF)) then \n" +
						"          _codepoint = ((_high - 0xD800) * 0x400) + (_low - 0xDC00) + 0x10000 \n" +
						"          table.remove(_hex, 1) \n" +
						"          table.remove(_hex, 1) \n" +
						"        else \n" +
						"          _codepoint = _high \n" +
						"          table.remove(_hex, 1) \n" +
						"        end \n" +
						"      else \n" +
						"        _codepoint = table.remove(_hex, 1) \n" +
						"      end \n" +
						"      _unicode = _unicode..utf8.char(_codepoint) \n" +
						"    end \n" +
						" \n" +
						"    _unicode:encoding('UTF-8') \n" +
						"    return _unicode \n" +
						"  end \n" +
						"</luascript> \n" +
						" ";

		String[] strings = {
						"     [ " +
						"       { " +
						"          \"precision\": \"zip\", " +
						"          \"Latitude\":  37.7668, " +
						"          \"Longitude\": -122.3959, " +
						"          \"Address\":   \"\", " +
						"          \"City\":      \"SAN FRANCISCO\", " +
						"          \"State\":     \"CA\", " +
						"          \"Zip\":       \"94107\", " +
						"          \"Country\":   \"US\" " +
						"       }, " +
						"       { " +
						"          \"precision\": \"zip\", " +
						"          \"Latitude\":  37.371991, " +
						"          \"Longitude\": -122.026020, " +
						"          \"Address\":   \"\", " +
						"          \"City\":      \"SUNNYVALE\", " +
						"          \"State\":     \"CA\", " +
						"          \"Zip\":       \"94085\", " +
						"          \"Country\":   \"US\" " +
						"       } " +
						"     ] " +
						" "
						};  

		ESLIFGrammar eslifGrammar = null;
		try {
			eslifGrammar = new ESLIFGrammar(eslif, grammar);
			for (int i = 0; i < strings.length; i++) {
				String string = new String(strings[i]);
	
				BufferedReader reader = new BufferedReader(new StringReader(string));
				AppRecognizer eslifAppRecognizer = new AppRecognizer(reader, eslifLogger);
				AppValue eslifAppValue = new AppValue();
				eslifLogger.info("Testing parse() on " + string);
				try {
					if (eslifGrammar.parse(eslifAppRecognizer, eslifAppValue)) {
						eslifLogger.info("Result: " + eslifAppValue.getResult());
					}
				} catch (Exception e) {
					System.err.println("Failed to parse " + string + ": " + e.getMessage());
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			return;
		} finally {
			if (eslifGrammar != null) {
				try {
					eslifGrammar.free();
				} catch (ESLIFException e) {
					e.printStackTrace();
				}
			}
		}
	}
}
