#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short symbolImportb(marpaESLIFSymbol_t *marpaESLIFSymbolp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

#define UTF_8_STRING "UTF-8"
#define SUBJECT "::anything"
#define STRING "# Self grammar\n"               \
  "/*\n"                                        \
  " * **********************\n"                 \
  " * Meta-grammar settings:\n"                 \
  " * **********************\n"                 \
  " *"
#define STRING2 ":start"
#define REGEX "(*MARK:MarkName):+\\w+"

typedef struct marpaESLIFTester_context {
  short            firstb;
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *selfs = "# Self grammar\n"
  "/*\n"
  " * **********************\n"
  " * Meta-grammar settings:\n"
  " * **********************\n"
  " */\n"
  ":start                         ::= statements\n"
  ":default ::= \n"
  "  action => ::ast\n"
  "  symbol-action => ::convert[UTF-8]\n"
  "  event-action => ::lua->event_action\n"
  "  default-encoding => ASCII\n"
  "  fallback-encoding => UTF-8\n"
  "  regex-action => ::lua->LuaRegexAction\n"
  ":desc                          ::= 'G1'\n"
  ":desc                            ~ \xE2\x80\x9CL0\xE2\x80\x9D\n"
  ":desc                          :[2]:= 'Sub Grammar 2'\n"
  ":discard                       ::= /[\\s]+/ event => whitespace$\n"
  ":discard                       ::= /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u event => perl_comment$\n"
  ":discard                       ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u event => cplusplus_comment$\n"
  "\n"
  "/*\n"
  " * ***************\n"
  " * Event settings:\n"
  " * ***************\n"
  " */\n"
  "event :discard[off]=on = nulled <discard off>\n"
  "event :discard[on]=on  = nulled <discard on>\n"
  "\n"
  "/*\n"
  " * ******\n"
  " * Rules:\n"
  " * ******\n"
  " */\n"
  "<statements>                    ::= &<statement>('x', 1 + $&<statement>('y', 0))*\n"
  "<statement>(a, b)               ::= <start rule>\n"
  "                                  | <desc rule>\n"
  "                                  | <empty rule>\n"
  "                                  | <null statement>\n"
  "                                  | <statement group>\n"
  "                                  | <priority rule>\n"
  "                                  | <quantified rule>\n"
  "                                  | <discard rule>\n"
  "                                  | <default rule>\n"
  "                                  | <lexeme rule>\n"
  "                                  | <completion event declaration>\n"
  "                                  | <nulled event declaration>\n"
  "                                  | <prediction event declaration>\n"
  "                                  | <inaccessible statement>\n"
  "                                  | <exception statement>\n"
  "                                  | <autorank statement>\n"
  "                                  | <lua script statement>\n"
  "                                  | <terminal rule>\n"
  "<start rule>                    ::= ':start' <op declare> <rhs primary>\n"
  "<desc rule>                     ::= ':desc' <op declare> <quoted string literal>\n"
  "<empty rule>                    ::= <lhs> <op declare> <adverb list>\n"
  "<null statement>                ::= ';'\n"
  "<statement group>               ::= '{' <statements> '}'\n"
  "<priority rule>                 ::= <lhs> <op declare> <priorities>\n"
  "<quantified rule>               ::= <lhs> <op declare> <rhs primary> <quantifier> <adverb list>\n"
  "<discard rule>                  ::= ':discard' <op declare> <rhs primary> <adverb list>\n"
  "<default rule>                  ::= ':default' <op declare> <adverb list>\n"
  "<lexeme rule>                   ::= ':lexeme' <op declare> <lhs> <adverb list>\n"
  "<terminal rule>                 ::= ':terminal' <op declare> <terminal> <adverb list>\n"
  "<completion event declaration>  ::= 'event' <event initialization> '=' 'completed' <lhs>\n"
  "                                  | 'event' <event initialization> <op declare> 'completed' <lhs>\n"
  "<nulled event declaration>      ::= 'event' <event initialization> '=' 'nulled' <lhs>\n"
  "                                  | 'event' <event initialization> <op declare> 'nulled' <lhs>\n"
  "<prediction event declaration>  ::= 'event' <event initialization> '=' 'predicted' <lhs>\n"
  "                                  | 'event' <event initialization> <op declare> 'predicted' <lhs>\n"
  "<inaccessible statement>        ::= 'inaccessible' 'is' <inaccessible treatment> 'by' 'default'\n"
  "<inaccessible treatment>        ::= 'warn'\n"
  "                                  | 'ok'\n"
  "                                  | 'fatal'\n"
  "<exception statement>           ::= <lhs> <op declare> <rhs primary> '-' <rhs primary> <adverb list>\n"
  "<autorank statement>            ::= 'autorank' 'is' <on or off> 'by' 'default'\n"
  "<op declare>                    ::= <op declare top grammar>\n"
  "                                  | <op declare lex grammar>\n"
  "                                  | <op declare any grammar>\n"
  "<priorities>                    ::= <alternatives>+ separator => <op loosen> proper => 1 hide-separator => 1\n"
  "<alternatives>                  ::= <alternative>+ separator => <op equal priority> proper => 1 hide-separator => 1\n"
  "<alternative>                   ::= <rhs> <adverb list>\n"
  "<adverb list>                   ::= <adverb list items>\n"
  "<adverb list items>             ::= <adverb item>*\n"
  "<adverb item>                   ::= <action>\n"
  "                                  | <left association>\n"
  "                                  | <right association>\n"
  "                                  | <group association>\n"
  "                                  | <separator specification>\n"
  "                                  | <proper specification>\n"
  "                                  | <rank specification>\n"
  "                                  | <null ranking specification>\n"
  "                                  | <priority specification>\n"
  "                                  | <pause specification>\n"
  "                                  | <latm specification>\n"
  "                                  | <naming>\n"
  "                                  | <null adverb>\n"
  "                                  | <symbol action>\n"
  "                                  | <free action>\n"
  "                                  | <event specification>\n"
  "                                  | <hide separator specification>\n"
  "                                  | <if action>\n"
  "                                  | <regex action>\n"
  "                                  | <event action>\n"
  "                                  | <default encoding>\n"
  "                                  | <fallback encoding>\n"
  "<action>                        ::= 'action' '=>' <action name>\n"
  "                                  | 'action' '=>' <string literal>\n"
  "                                  | 'action' '=>' <quoted string literal>\n"
  "<left association>              ::= 'assoc' '=>' 'left'\n"
  "<right association>             ::= 'assoc' '=>' 'right'\n"
  "<group association>             ::= 'assoc' '=>' 'group'\n"
  "<separator specification>       ::= 'separator' '=>' <rhs primary>\n"
  "<proper specification>          ::= 'proper' '=>' <false>\n"
  "                                  | 'proper' '=>' <true>\n"
  "<hide separator specification>  ::= 'hide-separator' '=>' <false>\n"
  "                                  | 'hide-separator' '=>' <true>\n"
  "<rank specification>            ::= 'rank' '=>' <signed integer>\n"
  "<null ranking specification>    ::= 'null-ranking' '=>' <null ranking constant>\n"
  "                                  | 'null' 'rank' '=>' <null ranking constant>\n"
  "<null ranking constant>         ::= 'low'\n"
  "                                  | 'high'\n"
  "<priority specification>        ::= 'priority' '=>' <signed integer>\n"
  "<pause specification>           ::= 'pause' '=>' 'before'\n"
  "                                  | 'pause' '=>' 'after'\n"
  "<event specification>           ::= 'event' '=>' <event initialization>\n"
  "<event initialization>          ::= <event name> <event initializer>\n"
  "<event initializer>             ::= '=' <on or off>\n"
  "<on or off>                     ::= 'on'\n"
  "                                  | 'off'\n"
  "<event initializer>             ::=\n"
  "<latm specification>            ::= 'latm' '=>' <false>\n"
  "                                  | 'latm' '=>' <true>\n"
  "<naming>                        ::= 'name' '=>' <alternative name>\n"
  "<null adverb>                   ::= ','\n"
  "<symbol action>                 ::= 'symbol-action' '=>' <symbol action name>\n"
  "                                  | 'symbol-action' '=>' <string literal>\n"
  "                                  | 'symbol-action' '=>' <quoted string literal>\n"
  "<if action>                     ::= 'if-action' '=>' <if action name>\n"
  "<regex action>                  ::= 'regex-action' '=>' <regex action name>\n"
  "<event action>                  ::= 'event-action' '=>' <event action name>\n"
  "<default encoding>              ::= 'default-encoding' '=>' <default encoding name>\n"
  "<fallback encoding>             ::= 'fallback-encoding' '=>' <fallback encoding name>\n"
  "<free action>                   ::= 'free-action' '=>' <free name>\n"
  "<alternative name>              ::= <standard name>\n"
  "                                  | <quoted string literal>\n"
  "<event name>                    ::= <restricted ascii graph name>\n"
  "                                  | ':symbol'\n"
  "                                  | ':discard[on]'\n"
  "                                  | ':discard[off]'\n"
  "                                  | ':discard[switch]'\n"
  "<lhs>                           ::= <symbol name>\n"
  "                                  | <symbol name> '(' <identifiers> ')'\n"
  "<rhs>                           ::= <rhs alternative>+\n"
  "<rhs alternative>               ::= <single symbol>\n"
  "                                  | <symbol name> '@' <grammar reference>\n"
  "                                  | '(-' <priorities> '-)'\n"
  "                                  | '(' <priorities> ')'\n"
  "                                  | '(-' <rhs primary> '-' <rhs primary> <adverb list> '-)'\n"
  "                                  | '(' <rhs primary> '-' <rhs primary> <adverb list> ')'\n"
  "                                  | '(-' <rhs primary> <quantifier> <adverb list> '-)'\n"
  "                                  | '(' <rhs primary> <quantifier> <adverb list> ')'\n"
  "<rhs primary>                   ::= <single symbol>\n"
  "                                  | <symbol> '@' <grammar reference>\n"
  "                                  | <parameterized rhs primary>\n"
  "<parameterized rhs primary>     ::= '$' <symbol> <arguments>\n"
  "                                  | '$' <symbol> <arguments> '@' <grammar reference>\n"
  "<single symbol>                 ::= <symbol>\n"
  "                                  | <terminal>\n"
  "<terminal>                      ::= <character class>\n"
  "                                  | <regular expression>\n"
  "                                  | <quoted string>\n"
  "                                  | ':eof'\n"
  "<symbol>                        ::= <symbol name>\n"
  "<symbol name>                   ::= <bare name>\n"
  "                                  | <bracketed name>\n"
  "<action name>                   ::= <restricted ascii graph name>\n"
  "                                  | '::shift'\n"
  "                                  | '::undef'\n"
  "                                  | '::ascii'\n"
  "                                  | /::convert\\[[^\\]]+\\]/\n"
  "                                  | '::concat'\n"
  "                                  | /::copy\\[\\d+\\]/\n"
  "                                  | <lua action name>\n"
  "                                  | '::true'\n"
  "                                  | '::false'\n"
  "                                  | '::json'\n"
  "                                  | '::row'\n"
  "                                  | '::table'\n"
  "                                  | '::ast'\n"
  "<free name>                     ::= <restricted ascii graph name>\n"
  "<symbol action name>            ::= <restricted ascii graph name>\n"
  "                                  | '::transfer'\n"
  "                                  | '::undef'\n"
  "                                  | '::ascii'\n"
  "                                  | /::convert\\[[^\\]]+\\]/\n"
  "                                  | '::concat'\n"
  "                                  | <lua action name>\n"
  "                                  | '::true'\n"
  "                                  | '::false'\n"
  "<if action name>                ::= <restricted ascii graph name>\n"
  "                                  | <lua action name>\n"
  "<regex action name>             ::= <restricted ascii graph name>\n"
  "                                  | <lua action name>\n"
  "<event action name>             ::= <restricted ascii graph name>\n"
  "                                  | <lua action name>\n"
  "<default encoding name>         ::= <graph ascii name>\n"
  "<fallback encoding name>        ::= <graph ascii name>\n"
  "<quantifier>                    ::= '*'\n"
  "                                  | '+'\n"
  "<signed integer>                ::= /[+-]?\\d+/\n"
  "<unsigned integer>              ::= /\\d+/\n"
  "<grammar reference>             ::= <quoted string>\n"
  "                                  | <signed integer>\n"
  "                                  | '=' <unsigned integer>\n"
  "<string literal>                ::= <string literal unit>+ proper => 1\n"
  "<string literal unit>           ::= '::u8\"' <discard off> <string literal inside any> '\"' <discard on>\n"
  "<discard off>                   ::=\n"
  "<discard on>                    ::=\n"
  "<string literal inside any>     ::= <string literal inside>* proper => 1\n"
  "<string literal inside>         ::= /[^\"\\\\\\n]/\n"
  "                                  | '\\\\' /[\"'?\\\\abfnrtve]/\n"
  "                                  | '\\\\' /x\\{[a-fA-F0-9]{2}\\}/\n"
  "                                  | '\\\\' /u\\{[a-fA-F0-9]{4}\\}/\n"
  "                                  | '\\\\' /U\\{[a-fA-F0-9]{8}\\}/\n"
  "<lua script statement>          ::= '<luascript>' <discard off> <lua script source> '</luascript>' <discard on>\n"
  "<lua script source>             ::= /[\\s\\S]/*\n"
  "<identifiers>                   ::= <identifier>* separator => ',' proper => 1 hide-separator => 1\n"
  "<identifier>                    ::= /[a-zA-Z_][a-zA-Z0-9_]*/\n"
  "<arguments>                     ::= '(' <lua explist maybe> ')'\n"
  "<lua chunk>                     ::= <lua block>\n"
  "<lua block>                     ::= <lua stat any> <lua retstat maybe>\n"
  "<lua stat>                      ::= ';'\n"
  "                                  | <lua varlist> '=' <lua explist>\n"
  "                                  | <lua function call>\n"
  "                                  | <lua label>\n"
  "                                  | 'break'\n"
  "                                  | 'goto' <lua name>\n"
  "                                  | 'do' <lua block> 'end'\n"
  "                                  | 'while' <lua exp> 'do' <lua block> 'end'\n"
  "                                  | 'repeat' <lua block> 'until' <lua exp>\n"
  "                                  | 'if' <lua exp> 'then' <lua block> <lua elseif exp then block any> <lua else block maybe> 'end'\n"
  "                                  | 'for' <lua name> '=' <lua exp> ',' <lua exp> <lua comma exp maybe> 'do' <lua block> 'end'\n"
  "                                  | 'for' <lua namelist> 'in' <lua explist> 'do' <lua block> 'end'\n"
  "                                  | 'function' <lua funcname> <lua funcbody>\n"
  "                                  | 'local' 'function' <lua name> <lua funcbody>\n"
  "                                  | 'local' <lua namelist> '=' <lua explist>\n"
  "                                  | 'local' <lua namelist> ':start' ':start'\n"
  "<lua retstat>                   ::= 'return' <lua explist maybe> <lua semicolon maybe>\n"
  "<lua label>                     ::= '::' <lua name> '::'\n"
  "<lua funcname>                  ::= <lua name> <lua dot name any> <lua colon name maybe>\n"
  "<lua varlist>                   ::= <lua var>+ separator => ',' proper => 1\n"
  "<lua var>                       ::= <lua name>\n"
  "                                  | <lua prefixexp> '[' <lua exp> ']'\n"
  "                                  | <lua prefixexp> '.' <lua name>\n"
  "<lua namelist>                  ::= <lua name>+ separator => ',' proper => 1\n"
  "<lua explist>                   ::= <lua exp>+ separator => ',' proper => 1\n"
  "<lua exp>                       ::= 'nil'\n"
  "                                  | 'false'\n"
  "                                  | 'true'\n"
  "                                  | <lua numeral>\n"
  "                                  | <lua literalstring>\n"
  "                                  | '...'\n"
  "                                  | <lua functiondef>\n"
  "                                  | <lua prefixexp>\n"
  "                                  | <lua tableconstructor>\n"
  "                                  | <lua exp> <lua binop> <lua exp>\n"
  "                                  | <lua unop> <lua exp>\n"
  "<lua prefixexp>                 ::= <lua var>\n"
  "                                  | <lua function call>\n"
  "                                  | '(' <lua exp> ')'\n"
  "<lua function call>             ::= <lua prefixexp> <lua args>\n"
  "                                  | <lua prefixexp> ':' <lua name> <lua args>\n"
  "                                  | '$' <rhs primary>\n"
  "<lua args>                      ::= '(' <lua explist maybe> ')'\n"
  "                                  | <lua tableconstructor>\n"
  "                                  | <lua literalstring>\n"
  "<lua functiondef>               ::= 'function' <lua funcbody>\n"
  "<lua funcbody>                  ::= '(' <lua parlist maybe> ')' <lua block> 'end'\n"
  "<lua parlist>                   ::= <lua namelist> <lua comma varargs maybe>\n"
  "                                  | '...'\n"
  "<lua tableconstructor>          ::= '{' <lua fieldlist maybe> '}'\n"
  "<lua fieldlist>                 ::= <lua field>+ separator => <lua fieldsep>\n"
  "<lua field>                     ::= '[' <lua exp> ']' '=' <lua exp>\n"
  "                                  | <lua name> '=' <lua exp>\n"
  "                                  | <lua exp>\n"
  "<lua fieldsep>                  ::= ','\n"
  "                                  | ';'\n"
  "<lua binop>                     ::= '+'\n"
  "                                  | '-'\n"
  "                                  | '*'\n"
  "                                  | '/'\n"
  "                                  | '//'\n"
  "                                  | '^'\n"
  "                                  | '%'\n"
  "                                  | '&'\n"
  "                                  | '~'\n"
  "                                  | '|'\n"
  "                                  | '>>'\n"
  "                                  | '<<'\n"
  "                                  | '..'\n"
  "                                  | '<'\n"
  "                                  | '<='\n"
  "                                  | '>'\n"
  "                                  | '>='\n"
  "                                  | '=='\n"
  "                                  | '~='\n"
  "                                  | 'and'\n"
  "                                  | 'or'\n"
  "<lua unop>                      ::= '-'\n"
  "                                  | 'not'\n"
  "                                  | '#'\n"
  "                                  | '~'\n"
  "<lua stat any>                  ::= <lua stat>* proper => 1\n"
  "<lua comma exp maybe>           ::= ',' <lua exp>\n"
  "<lua comma exp maybe>           ::=\n"
  "<lua retstat maybe>             ::= <lua retstat>\n"
  "<lua retstat maybe>             ::=\n"
  "<lua elseif exp then block any> ::= <lua elseif exp then block>* proper => 1\n"
  "<lua elseif exp then block>     ::= 'elseif' <lua exp> 'then' <lua block>\n"
  "<lua else block maybe>          ::= 'else' <lua block>\n"
  "<lua else block maybe>          ::=\n"
  "<lua explist maybe>             ::= <lua explist>\n"
  "<lua explist maybe>             ::=\n"
  "<lua semicolon maybe>           ::= ';'\n"
  "<lua semicolon maybe>           ::=\n"
  "<lua dot name any>              ::= <lua dot name>* proper => 1\n"
  "<lua dot name>                  ::= '.' <lua name>\n"
  "<lua colon name maybe>          ::= <lua colon name>\n"
  "<lua colon name maybe>          ::=\n"
  "<lua colon name>                ::= ':' <lua name>\n"
  "<lua parlist maybe>             ::= <lua parlist>\n"
  "<lua parlist maybe>             ::=\n"
  "<lua comma varargs maybe>       ::= ',' '...'\n"
  "<lua comma varargs maybe>       ::=\n"
  "<lua fieldlist maybe>           ::= <lua fieldlist>\n"
  "<lua fieldlist maybe>           ::=\n"
  "<lua name>                      ::= <identifier>\n"
  "<lua literalstring>             ::= <quoted string literal>\n"
  "<lua numeral>                   ::= /(?:0[xX][a-fA-F0-9]+(?:\\.[a-fA-F0-9]+)?(?:[pP][+-]?[0-9]+)?)|(?:[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)/\n"
  "\n"
  ":default ~ \n"
  "  regex-action => ::lua->LuaRegexAction\n"
  "<op declare any grammar>         ~ /:\\[[\\d]+\\]:=/\n"
  "<op declare top grammar>         ~ '::='\n"
  "<op declare lex grammar>         ~ '~'\n"
  "<op loosen>                      ~ '||'\n"
  "<op equal priority>              ~ '|'\n"
  "<true>                           ~ '1'\n"
  "<false>                          ~ '0'\n"
  "<word character>                 ~ /[\\w]/\n"
  "<one or more word characters>    ~ <word character>+ proper => 1 name => 'Alternative name using single quotes'\n"
  "<zero or more word characters>   ~ <word character>* proper => 1 name => \"Alternative name using double quotes\"\n"
  "<restricted ascii graph name>    ~ /[!#$%&*+.\\/;?\\[\\\\\\]^_`~A-Za-z0-9]+/ name => \xE2\x80\x9C" "Alternative name using UTF-8 quotes\xE2\x80\x9D\n"
  "<graph ascii name>               ~ /[[:graph:]]+/\n"
  "<lua action name>                ~ /::lua->[a-zA-Z_][a-zA-Z0-9_]*/\n"
  "<bare name>                      ~ <word character>+ proper => 1\n"
  "<standard name>                  ~ /[a-zA-Z]/ <zero or more word characters>\n"
  "<bracketed name>                 ~ '<' <bracketed name string> '>'\n"
  "<bracketed name string>          ~ /[\\s\\w]+/\n"
  "<quoted string>                  ~ /(?:(?|(?:')(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)(?:')|(?:\")(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)(?:\")|(?:\\x{201C})(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)(?:\\x{201D})))/su\n"
  "                                 | /(?:(?|(?:')(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)(?:')|(?:\")(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)(?:\")|(?:\\x{201C})(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)(?:\\x{201D})))/su ':' /ic?/\n"
  "<quoted string literal>          ~ /(?:(?|(?:')(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)(?:')|(?:\")(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)(?:\")|(?:\\x{201C})(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)(?:\\x{201D})))/su\n"
  "<character class>                ~ /((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))/\n"
  "                                 | /((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))/ ':' /[eijmnsxDJUuaNbcA]+/\n"
  "<regular expression>             ~ /(*NO_JIT)((?|(?:\\/(?![*\\/]))(?:[^\\\\\\/]*(?:\\\\.[^\\\\\\/]*)*)(?:\\/)))(?C\"some \"\"arbitrary\"\" text 1\")/su\n"
  "                                 | /(*NO_JIT)((?|(?:\\/(?![*\\/]))(?:[^\\\\\\/]*(?:\\\\.[^\\\\\\/]*)*)(?:\\/)))(?C\"some \"\"arbitrary\"\" text 2\")/su /[eijmnsxDJUuaNbcA]+/\n"
  "\n"
  "test_group(a,b )               ::= 'X' (  'Y' action => ::convert[UTF-8]\n"
  "                                       |  'Z'\n"
  "                                       || (-'yy'-)\n"
  "                                       |  'zz'\n"
  "                                       |  ( &TEST_GROUP_FOR_EXCEPTION_0(x+1) - TEST_GROUP_FOR_EXCEPTION_1 action => exceptionAction)\n"
  "                                       |  ( 'A'* action => quantifiedAction )\n"
  "                                       )\n"
  "TEST_GROUP(x, y)                 ~ 'X' ( 'Y' action => ::convert[UTF32] | 'Z' || (-'yy'-) | 'zz' | ('B'* action => quantifiedAction ) )\n"
  "TEST_GROUP_FOR_EXCEPTION_0( )    ~ 'X'\n"
  "TEST_GROUP_FOR_EXCEPTION_1       ~ 'Y'\n"
  "TEST_GROUP_FOR_ACTION_0          ~ 'X' action => '\xE2\x99\xA5\x21" "X!'\n"
  "TEST_GROUP_FOR_ACTION_1          ~ 'X' action => \"\xE2\x99\xA5\x21" "Y!\"\n"
  "TEST_GROUP_FOR_ACTION_2          ~ 'X' action => \xE2\x80\x9C\x21Z\x21\xE2\x80\x9D\n"
  "\n"
  "DUMMY_RULE_FOR_LEVEL_2      :[2]:= 'dummy'\n"
  "EOF_TEST_RULE               ::= :eof\n"
  "<luascript>\n"
  "function LuaRegexAction()\n"
  "  return 0\n"
  "end\n"
  "</luascript>\n"
  "\n";

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFOption_t          *marpaESLIFOptionp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFGrammarOption_t   *marpaESLIFGrammarOptionp;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFGrammarDefaults_t  marpaESLIFGrammarDefaults;
  char                        *grammarscripts;
  marpaESLIFSymbol_t          *stringSymbolp = NULL;
  marpaESLIFSymbol_t          *stringSymbol2p = NULL;
  marpaESLIFSymbol_t          *regexSymbolp = NULL;
  marpaESLIFSymbol_t          *metaSymbolp = NULL;
  marpaESLIFString_t           string;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  short                        matchb;
  marpaESLIFSymbolOption_t     marpaESLIFSymbolOption;
  size_t                       discardl;
  char                        *discardLasts;
  size_t                       discardLastl;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }
  marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
  if (marpaESLIFOptionp == NULL) {
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "ESLIF was generated with these options:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... genericLoggerp: %p", marpaESLIFOptionp->genericLoggerp);
  if (marpaESLIFOptionp->genericLoggerp != marpaESLIFOption.genericLoggerp) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "... genericLoggerp != %p", marpaESLIFOption.genericLoggerp);
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------\n");

  /* Inspect options used to create ESLIF's grammar */
  marpaESLIFGrammarOptionp = marpaESLIFGrammar_optionp(marpaESLIF_grammarp(marpaESLIFp));
  if (marpaESLIFGrammarOptionp == NULL) {
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "ESLIF's grammar was generated with these options:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... bytep    : %p", marpaESLIFGrammarOptionp->bytep);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... bytel    : %ld", (unsigned long) marpaESLIFGrammarOptionp->bytel);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... encodings: %p", marpaESLIFGrammarOptionp->encodings);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... encodingl: %ld", (unsigned long) marpaESLIFGrammarOptionp->encodingl);
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------\n");

  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIF_grammarp(marpaESLIFp), &ngrammari)) {
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ESLIF's ngrammari is %d", ngrammari);
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIF_grammarp(marpaESLIFp), &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ESLIF grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
      } else {
        GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIF_grammarp(marpaESLIFp), &grammarshows, leveli, NULL) failure, %s", strerror(errno));
        goto err;
      }
    }
  } else {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_ngrammarib(marpaESLIF_grammarp(marpaESLIFp), &ngrammari) failure, %s", strerror(errno));
    goto err;
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIF_grammarp(marpaESLIFp), &grammarscripts)) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "ESLIF grammar script:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarscripts);

  marpaESLIFGrammarOption.bytep               = (void *) selfs;
  marpaESLIFGrammarOption.bytel               = strlen(selfs);
  marpaESLIFGrammarOption.encodings           = UTF_8_STRING;
  marpaESLIFGrammarOption.encodingl           = strlen(UTF_8_STRING);
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    goto err;
  }
  for (leveli = 0; leveli < ngrammari; leveli++) {
    if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
    }
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammarp, &grammarscripts)) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "ESLIF grammar script:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarscripts);

  /* So in theory we must be able to reparse ESLIF using itself -; */
  marpaESLIFTester_context.firstb         = 1;
  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = (char *) selfs;
  marpaESLIFTester_context.inputl         = strlen(selfs);

  marpaESLIFRecognizerOption.userDatavp        = &marpaESLIFTester_context; /* User specific context */
  marpaESLIFRecognizerOption.readerCallbackp   = inputReaderb; /* Reader */
  marpaESLIFRecognizerOption.disableThresholdb = 0; /* Default: 0 */
  marpaESLIFRecognizerOption.exhaustedb        = 0; /* Exhaustion event. Default: 0 */
  marpaESLIFRecognizerOption.newlineb          = 1; /* Count line/column numbers. Default: 0 */
  marpaESLIFRecognizerOption.trackb            = 1; /* Absolute position tracking. Default: 0 */
  marpaESLIFRecognizerOption.bufsizl           = 10; /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
  marpaESLIFRecognizerOption.ifActionResolverp = NULL;
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */)) {
    goto err;
  }

  /* Play with defaults */
  GENERICLOGGER_LEVEL_SET(marpaESLIFOption.genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);
  for (leveli = 0; leveli < ngrammari; leveli++) {
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Getting defaults of grammar at level %d", leveli);
    if (! marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, leveli, NULL /* descp */)) {
      goto err;
    }
  }

  /* Play with terminal outside of any grammar */
  marpaESLIFSymbolOption.userDatavp = &marpaESLIFTester_context;
  marpaESLIFSymbolOption.importerp  = symbolImportb;

  string.bytep          = "'" STRING "'";
  string.bytel          = strlen("'" STRING "'");
  string.encodingasciis = "ASCII";
  string.asciis         = NULL;

  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Creating string symbol for: %s, no modifier", STRING);
  stringSymbolp = marpaESLIFSymbol_string_newp(marpaESLIFp, &string, NULL /* modifiers */, &marpaESLIFSymbolOption);
  if (stringSymbolp == NULL) {
    goto err;
  }

  string.bytep          = "'" STRING2 "'";
  string.bytel          = strlen("'" STRING2 "'");
  string.encodingasciis = "ASCII";
  string.asciis         = NULL;

  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Creating string symbol for: %s, no modifier", STRING2);
  stringSymbol2p = marpaESLIFSymbol_string_newp(marpaESLIFp, &string, NULL /* modifiers */, &marpaESLIFSymbolOption);
  if (stringSymbol2p == NULL) {
    goto err;
  }

  string.bytep          = REGEX;
  string.bytel          = strlen(REGEX);
  string.encodingasciis = "ASCII";
  string.asciis         = NULL;

  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Creating regex symbol for: %s, no modifier", REGEX);
  regexSymbolp = marpaESLIFSymbol_regex_newp(marpaESLIFp, &string, NULL, &marpaESLIFSymbolOption);
  if (regexSymbolp == NULL) {
    goto err;
  }

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Creating meta symbol at \"restricted ascii graph name\" in our grammar");
  metaSymbolp = marpaESLIFSymbol_meta_newp(marpaESLIFp, marpaESLIFGrammarp, "restricted ascii graph name", &marpaESLIFSymbolOption);
  if (metaSymbolp == NULL) {
    goto err;
  }

  /* Reset the context for the new recognizer */
  marpaESLIFTester_context.firstb         = 1;
  marpaESLIFTester_context.inputs         = (char *) selfs;
  marpaESLIFTester_context.inputl         = strlen(selfs);

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }

  /* Discard ourself the noise */
  do {
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Trying to discard data - inputs=%p, inputl=%04ld", marpaESLIFTester_context.inputs, (unsigned long) marpaESLIFTester_context.inputl);
    if (! marpaESLIFRecognizer_discardb(marpaESLIFRecognizerp, &discardl)) {
      goto err;
    }
    if (discardl) {
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, ":discard was successful on %ld bytes as per marpaESLIFRecognizer_discardb", (unsigned long) discardl);
      if (! marpaESLIFRecognizer_discard_lastb(marpaESLIFRecognizerp, &discardLasts, &discardLastl)) {
        goto err;
      }
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, ":discard was successful on %ld bytes as per marpaESLIFRecognizer_discard_lastb: %s", (unsigned long) discardLastl, discardLasts);
    }
  } while (discardl > 0);

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external string2 symbol match on recognizer");
  if (! marpaESLIFRecognizer_symbol_tryb(marpaESLIFRecognizerp, stringSymbol2p, &matchb)) {
    goto err;
  }

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external regex symbol match on recognizer");
  if (! marpaESLIFRecognizer_symbol_tryb(marpaESLIFRecognizerp, regexSymbolp, &matchb)) {
    goto err;
  }

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external meta symbol match on recognizer");
  if (! marpaESLIFRecognizer_symbol_tryb(marpaESLIFRecognizerp, metaSymbolp, &matchb)) {
    goto err;
  }

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external string symbol match on itself");
  if (! marpaESLIFSymbol_tryb(stringSymbolp, STRING, strlen(STRING), &matchb)) {
    goto err;
  }

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external regex symbol match on itself");
  if (! marpaESLIFSymbol_tryb(regexSymbolp, SUBJECT, strlen(SUBJECT), &matchb)) {
    goto err;
  }

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external meta symbol match on our grammar");
  if (! marpaESLIFSymbol_tryb(metaSymbolp, (char *) selfs, strlen(selfs), &matchb)) {
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  }
  if (stringSymbolp != NULL) {
    marpaESLIFSymbol_freev(stringSymbolp);
  }
  if (stringSymbol2p != NULL) {
    marpaESLIFSymbol_freev(stringSymbol2p);
  }
  if (regexSymbolp != NULL) {
    marpaESLIFSymbol_freev(regexSymbolp);
  }
  if (metaSymbolp != NULL) {
    marpaESLIFSymbol_freev(metaSymbolp);
  }
  GENERICLOGGER_LEVEL_SET(marpaESLIFOption.genericLoggerp, GENERICLOGGER_LOGLEVEL_INFO);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Exiting with status %d", exiti);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  size_t                      sendl;

  GENERICLOGGER_INFOF(marpaESLIFTester_contextp->genericLoggerp, "inputReaderb: before: inputs=%p, inputl=%04ld", marpaESLIFTester_contextp->inputs, (unsigned long) marpaESLIFTester_contextp->inputl);

  if (marpaESLIFTester_contextp->firstb) {
    /* For a correct BOM check we always want to send at least 4 bytes at the very beginning */
    sendl = 4;
    marpaESLIFTester_contextp->firstb = 0;
  } else {
    /* Else send 100 bytes if possible */
    sendl = 100;
  }

  if (sendl > marpaESLIFTester_contextp->inputl) {
    sendl = marpaESLIFTester_contextp->inputl;
  }

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = sendl;
  *characterStreambp    = 0; /* We say this is not a stream of characters - regexp will adapt and to UTF correctness if needed */
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;

  marpaESLIFTester_contextp->inputs += sendl;
  marpaESLIFTester_contextp->inputl -= sendl;

  *eofbp = marpaESLIFTester_contextp->inputl <= 0 ? 1 : 0;

  GENERICLOGGER_INFOF(marpaESLIFTester_contextp->genericLoggerp, "inputReaderb: after : inputs=%p, inputl=%04ld, *eofbp=%d", marpaESLIFTester_contextp->inputs, (unsigned long) marpaESLIFTester_contextp->inputl, (int) *eofbp);

  return 1;
}

/*****************************************************************************/
static short symbolImportb(marpaESLIFSymbol_t *marpaESLIFSymbolp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  short                       rcb;
  char                       *tmps;

  if (marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_ARRAY) {
    if (marpaESLIFValueResultp->u.a.p == NULL) {
      GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "Match of type ARRAY but p is NULL");
      goto err;
    }

    GENERICLOGGER_INFOF(marpaESLIFTester_contextp->genericLoggerp, "Match of type ARRAY on %ld bytes", (unsigned long) marpaESLIFValueResultp->u.a.sizel);
    tmps = (char *) malloc(marpaESLIFValueResultp->u.a.sizel + 1);
    if (tmps == NULL) {
      GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    if (marpaESLIFValueResultp->u.a.sizel > 0) {
      memcpy(tmps, marpaESLIFValueResultp->u.a.p, marpaESLIFValueResultp->u.a.sizel);
    }
    tmps[marpaESLIFValueResultp->u.a.sizel] = '\0';
    GENERICLOGGER_INFOF(marpaESLIFTester_contextp->genericLoggerp, "Match is: %s", tmps);
    free(tmps);
  } else {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "Match of type %d ?", marpaESLIFValueResultp->type);
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
