#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short symbolImportb(marpaESLIFSymbol_t *marpaESLIFSymbolp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static marpaESLIFRecognizerEventCallback_t eventActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);

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
  ":start ::= <statements>\n"
  ":desc    ::= 'G1'\n"
  ":default ::=\n"
  "  action => ::concat\n"
  "  symbol-action => ::transfer\n"
  "  latm => 1\n"
  "  default-encoding => ASCII\n"
  "\n"
  "/*\n"
  " * ****************\n"
  " * Symbol settings:\n"
  " * ****************\n"
  " */\n"
  "event :discard[on]=on = nulled <discard on>\n"
  "event :discard[off]=on = nulled <discard off>\n"
  "\n"
  "/*\n"
  " * ******\n"
  " * Rules:\n"
  " * ******\n"
  " */\n"
  "<statements>                   ::= <statement>*\n"
  "<statement>                    ::= <start rule>\n"
  "                                 | <desc rule>\n"
  "                                 | <empty rule>\n"
  "                                 | <null statement>\n"
  "                                 | <statement group>\n"
  "                                 | <priority rule>\n"
  "                                 | <quantified rule>\n"
  "                                 | <discard rule>\n"
  "                                 | <default rule>\n"
  "                                 | <lexeme rule>\n"
  "                                 | <completion event declaration>\n"
  "                                 | <nulled event declaration>\n"
  "                                 | <prediction event declaration>\n"
  "                                 | <inaccessible statement>\n"
  "                                 | <exception statement>\n"
  "                                 | <autorank statement>\n"
  "                                 | <lua script statement>\n"
  "                                 | <terminal rule>\n"
  "<start rule>                   ::= ':start' <op declare> <symbol>\n"
  "<desc rule>                    ::= ':desc' <op declare> <quoted string literal>\n"
  "<empty rule>                   ::= <lhs> <op declare> <adverb list>\n"
  "<null statement>               ::= ';'\n"
  "<statement group>              ::= '{' <statements> '}'\n"
  "<priority rule>                ::= <lhs> <op declare> <priorities>\n"
  "<quantified rule>              ::= <lhs> <op declare> <rhs primary> <quantifier> <adverb list>\n"
  "<discard rule>                 ::= ':discard' <op declare> <rhs primary> <adverb list>\n"
  "<default rule>                 ::= ':default' <op declare> <adverb list>\n"
  "<lexeme rule>                  ::= ':lexeme' <op declare> <symbol> <adverb list>\n"
  "<terminal rule>                ::= ':terminal' <op declare> <terminal> <adverb list>\n"
  "<completion event declaration> ::= 'event' <event initialization> '=' 'completed' <symbol>\n"
  "                                 | 'event' <event initialization> <op declare> 'completed' <symbol>\n"
  "<nulled event declaration>     ::= 'event' <event initialization> '=' 'nulled' <symbol>\n"
  "                                 | 'event' <event initialization> <op declare> 'nulled' <symbol>\n"
  "<prediction event declaration> ::= 'event' <event initialization> '=' 'predicted' <symbol>\n"
  "                                 | 'event' <event initialization> <op declare> 'predicted' <symbol>\n"
  "<inaccessible statement>       ::= 'inaccessible' 'is' <inaccessible treatment> 'by' 'default'\n"
  "<inaccessible treatment>       ::= 'warn'\n"
  "                                 | 'ok'\n"
  "                                 | 'fatal'\n"
  "<exception statement>          ::= <lhs> <op declare> <rhs primary> '-' <rhs primary> <adverb list>\n"
  "<autorank statement>           ::= 'autorank' 'is' <on or off> 'by' 'default'\n"
  "<op declare>                   ::= <op declare top grammar>\n"
  "                                 | <op declare lex grammar>\n"
  "                                 | <op declare any grammar>\n"
  "<priorities>                   ::= <alternatives>+ separator => <op loosen> proper => 1 hide-separator => 1\n"
  "<alternatives>                 ::= <alternative>+ separator => <op equal priority> proper => 1 hide-separator => 1\n"
  "<alternative>                  ::= <rhs> <adverb list>\n"
  "<adverb list>                  ::= <adverb list items>\n"
  "<adverb list items>            ::= <adverb item>*\n"
  "<adverb item>                  ::= <action>\n"
  "                                 | <left association>\n"
  "                                 | <right association>\n"
  "                                 | <group association>\n"
  "                                 | <separator specification>\n"
  "                                 | <proper specification>\n"
  "                                 | <rank specification>\n"
  "                                 | <null ranking specification>\n"
  "                                 | <priority specification>\n"
  "                                 | <pause specification>\n"
  "                                 | <latm specification>\n"
  "                                 | <naming>\n"
  "                                 | <null adverb>\n"
  "                                 | <symbol action>\n"
  "                                 | <event specification>\n"
  "                                 | <hide separator specification>\n"
  "                                 | <if action>\n"
  "                                 | <event action>\n"
  "                                 | <default encoding>\n"
  "                                 | <fallback encoding>\n"
  "                                 | <regex action>\n"
  "<action>                       ::= 'action' /=>|\\x{21D2}/u <action name>\n"
  "                                 | 'action' /=>|\\x{21D2}/u <string literal>\n"
  "                                 | 'action' /=>|\\x{21D2}/u <quoted string literal>\n"
  "<left association>             ::= 'assoc' /=>|\\x{21D2}/u 'left'\n"
  "<right association>            ::= 'assoc' /=>|\\x{21D2}/u 'right'\n"
  "<group association>            ::= 'assoc' /=>|\\x{21D2}/u 'group'\n"
  "<separator specification>      ::= 'separator' /=>|\\x{21D2}/u <rhs primary>\n"
  "<proper specification>         ::= 'proper' /=>|\\x{21D2}/u <false>\n"
  "                                 | 'proper' /=>|\\x{21D2}/u <true>\n"
  "<hide separator specification> ::= 'hide-separator' /=>|\\x{21D2}/u <false>\n"
  "                                 | 'hide-separator' /=>|\\x{21D2}/u <true>\n"
  "<rank specification>           ::= 'rank' /=>|\\x{21D2}/u <signed integer>\n"
  "<null ranking specification>   ::= 'null-ranking' /=>|\\x{21D2}/u <null ranking constant>\n"
  "                                 | 'null' 'rank' /=>|\\x{21D2}/u <null ranking constant>\n"
  "<null ranking constant>        ::= 'low'\n"
  "                                 | 'high'\n"
  "<priority specification>       ::= 'priority' /=>|\\x{21D2}/u <signed integer>\n"
  "<pause specification>          ::= 'pause' /=>|\\x{21D2}/u 'before'\n"
  "                                 | 'pause' /=>|\\x{21D2}/u 'after'\n"
  "<event specification>          ::= 'event' /=>|\\x{21D2}/u <event initialization>\n"
  "<event initialization>         ::= <event name> <event initializer>\n"
  "<event initializer>            ::= '=' <on or off>\n"
  "<on or off>                    ::= 'on'\n"
  "                                 | 'off'\n"
  "<event initializer>            ::=\n"
  "<latm specification>           ::= 'latm' /=>|\\x{21D2}/u <false>\n"
  "                                 | 'latm' /=>|\\x{21D2}/u <true>\n"
  "<naming>                       ::= 'name' /=>|\\x{21D2}/u <alternative name>\n"
  "<null adverb>                  ::= ','\n"
  "<symbol action>                ::= 'symbol-action' /=>|\\x{21D2}/u <symbol action name>\n"
  "                                 | 'symbol-action' /=>|\\x{21D2}/u <string literal>\n"
  "                                 | 'symbol-action' /=>|\\x{21D2}/u <quoted string literal>\n"
  "<if action>                    ::= 'if-action' /=>|\\x{21D2}/u <if action name>\n"
  "<regex action>                 ::= 'regex-action' /=>|\\x{21D2}/u <regex action name>\n"
  "<alternative name>             ::= <standard name>\n"
  "                                 | <quoted string literal>\n"
  "<event name>                   ::= <restricted ascii graph name>\n"
  "                                 | ':symbol'\n"
  "                                 | ':discard[on]'\n"
  "                                 | ':discard[off]'\n"
  "                                 | ':discard[switch]'\n"
  "<lhs>                          ::= <symbol name>\n"
  "<rhs>                          ::= <rhs alternative>+\n"
  "<rhs alternative>              ::= <rhs primary>\n"
  "                                 | '(-' <priorities> '-)'\n"
  "                                 | '(' <priorities> ')'\n"
  "                                 | '(-' <rhs primary> '-' <rhs primary> <adverb list> '-)'\n"
  "                                 | '(' <rhs primary> '-' <rhs primary> <adverb list> ')'\n"
  "                                 | '(-' <rhs primary> <quantifier> <adverb list> '-)'\n"
  "                                 | '(' <rhs primary> <quantifier> <adverb list> ')'\n"
  "<rhs primary>                  ::= <single symbol>\n"
  "                                 | <symbol> '@' <grammar reference>\n"
  "<single symbol>                ::= <symbol>\n"
  "                                 | <terminal>\n"
  "<terminal>                     ::= <character class>\n"
  "                                 | <regular expression>\n"
  "                                 | <quoted string>\n"
  "                                 | ':eof'\n"
  "                                 | ':eol'\n"
  "                                 | ':sol'\n"
  "<symbol>                       ::= <symbol name>\n"
  "<symbol name>                  ::= <bare name>\n"
  "                                 | <bracketed name>\n"
  "<action name>                  ::= <restricted ascii graph name>\n"
  "                                 | '::shift'\n"
  "                                 | '::undef'\n"
  "                                 | '::ascii'\n"
  "                                 | /::convert\\[[^\\]]+\\]/\n"
  "                                 | '::concat'\n"
  "                                 | /::copy\\[\\d+\\]/\n"
  "                                 | <lua action name>\n"
  "                                 | '::true'\n"
  "                                 | '::false'\n"
  "                                 | '::json'\n"
  "                                 | '::jsonf'\n"
  "                                 | '::row'\n"
  "                                 | '::table'\n"
  "                                 | '::ast'\n"
  "<symbol action name>           ::= <restricted ascii graph name>\n"
  "                                 | '::transfer'\n"
  "                                 | '::undef'\n"
  "                                 | '::ascii'\n"
  "                                 | /::convert\\[[^\\]]+\\]/\n"
  "                                 | '::concat'\n"
  "                                 | <lua action name>\n"
  "                                 | '::true'\n"
  "                                 | '::false'\n"
  "                                 | '::json'\n"
  "                                 | '::jsonf'\n"
  "<if action name>               ::= <restricted ascii graph name>\n"
  "                                 | <lua action name>\n"
  "<regex action name>            ::= <restricted ascii graph name>\n"
  "                                 | <lua action name>\n"
  "<quantifier>                   ::= '*'\n"
  "                                 | '+'\n"
  "<signed integer>               ::= /[+-]?\\d+/\n"
  "<unsigned integer>             ::= /\\d+/\n"
  "<grammar reference>            ::= <quoted string>\n"
  "                                 | <signed integer>\n"
  "                                 | '=' <unsigned integer>\n"
  ":discard                       ::= /[\\s]+/\n"
  ":discard                       ::= /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u\n"
  ":discard                       ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u\n"
  "<string literal>               ::= <string literal unit>+ proper => 1\n"
  "<string literal unit>          ::= '::u8\"' <discard off> <string literal inside any> '\"' <discard on>\n"
  "<discard on>                   ::=\n"
  "<discard off>                  ::=\n"
  "<string literal inside any>    ::= <string literal inside>* proper => 1\n"
  "<string literal inside>        ::= /[^\"\\\\\\n]/\n"
  "                                 | '\\\\' /[\"'?\\\\abfnrtve]/\n"
  "                                 | '\\\\' /x\\{[a-fA-F0-9]{2}\\}/\n"
  "                                 | '\\\\' /u\\{[a-fA-F0-9]{4}\\}/\n"
  "                                 | '\\\\' /U\\{[a-fA-F0-9]{8}\\}/\n"
  "<lua script statement>         ::= '<luascript>' <discard off> <lua script source> '</luascript>' <discard on>\n"
  "<lua script source>            ::= /[\\s\\S]/*\n"
  "<event action>                 ::= 'event-action' /=>|\\x{21D2}/u <event action name>\n"
  "<event action name>            ::= <restricted ascii graph name>\n"
  "                                 | <lua action name>\n"
  "<default encoding>             ::= 'default-encoding' /=>|\\x{21D2}/u <default encoding name>\n"
  "<default encoding name>        ::= <graph ascii name>\n"
  "<fallback encoding>            ::= 'fallback-encoding' /=>|\\x{21D2}/u <fallback encoding name>\n"
  "<fallback encoding name>       ::= <graph ascii name>\n"
  "\n"
  "/*\n"
  " * **********************\n"
  " * Meta-grammar settings:\n"
  " * **********************\n"
  " */\n"
  ":start ~ <op declare any grammar>\n"
  ":desc ~ 'L0'\n"
  ":default ~ action => ::concat symbol-action => ::transfer latm => 1 default-encoding => ASCII\n"
  "\n"
  "/*\n"
  " * ****************\n"
  " * Symbol settings:\n"
  " * ****************\n"
  " */\n"
  "\n"
  "/*\n"
  " * ******\n"
  " * Rules:\n"
  " * ******\n"
  " */\n"
  "<op declare any grammar> ~ /:\\[[\\d]+\\]:=/\n"
  "<op declare top grammar> ~ '::='\n"
  "<op declare lex grammar> ~ '~'\n"
  "<op loosen> ~ '||'\n"
  "<op equal priority> ~ '|'\n"
  "<true> ~ '1'\n"
  "<false> ~ '0'\n"
  "<word character> ~ /[\\w]/\n"
  "<one or more word characters> ~ <word character>+ proper => 1\n"
  "<zero or more word characters> ~ <word character>* proper => 1\n"
  "<restricted ascii graph name> ~ /[!#$%&*+.\\/;?\\[\\\\\\]^_`~A-Za-z0-9]+/\n"
  "<lua action name> ~ /::lua->[a-zA-Z_][a-zA-Z0-9_]*/\n"
  "<bare name> ~ <word character>+ proper => 1\n"
  "<standard name> ~ /[a-zA-Z]/ <zero or more word characters>\n"
  "<bracketed name> ~ '<' <bracketed name string> '>'\n"
  "<bracketed name string> ~ /[\\s\\w]+/\n"
  "<quoted string> ~ /'(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)'|\"(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)\"|\\x{201C}(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)\\x{201D}/su\n"
  "<quoted string> ~ /'(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)'|\"(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)\"|\\x{201C}(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)\\x{201D}/su ':' /ic?/\n"
  "<quoted string literal> ~ /'(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)'|\"(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)\"|\\x{201C}(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)\\x{201D}/su\n"
  "<character class> ~ /((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))/\n"
  "<character class> ~ /((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))/ ':' /[eijmnsxDJUuaNbcA]+/\n"
  "<regular expression> ~ /\\/(?![*\\/])(?:[^\\\\\\/]*(?:\\\\.[^\\\\\\/]*)*)\\//su\n"
  "<regular expression> ~ /\\/(?![*\\/])(?:[^\\\\\\/]*(?:\\\\.[^\\\\\\/]*)*)\\//su /[eijmnsxDJUuaNbcA]+/ name => 'regular expression 2'\n"
  "<graph ascii name> ~ /[[:graph:]]+/ name => 'graph ascii name'\n"
  "\n"
  "# ---------\n"
  "# For tests\n"
  "# ---------\n"
  "inaccessible is warn by default\n"
  "<anything up to newline>        ::= <ANYTHING UP TO NEWLINE>\n"
  "<ANYTHING UP TO NEWLINE>          ~ /[^\\n]*/\n"
  "test_group                      ::= 'X' (  'Y' action => ::convert[UTF-8]\n"
  "                                       |  'Z'\n"
  "                                       || (-'yy'-)\n"
  "                                       |  'zz'\n"
  "                                       |  ( TEST_GROUP_FOR_EXCEPTION_0 - TEST_GROUP_FOR_EXCEPTION_1 action => exceptionAction)\n"
  "                                       |  ( 'A'* action => quantifiedAction )\n"
  "                                       )\n"
  "TEST_GROUP                       ~ 'X' ( 'Y' action => ::convert[UTF32] | 'Z' || (-'yy'-) | 'zz' | ('B'* action => quantifiedAction ) )\n"
  "TEST_GROUP_FOR_EXCEPTION_0       ~ 'X'\n"
  "TEST_GROUP_FOR_EXCEPTION_1       ~ 'Y'\n"
  "TEST_GROUP_FOR_ACTION_0          ~ 'X' action => '\xE2\x99\xA5\x21" "X!'\n"
  "TEST_GROUP_FOR_ACTION_1          ~ 'X' action => \"\xE2\x99\xA5\x21" "Y!\"\n"
  "TEST_GROUP_FOR_ACTION_2          ~ 'X' action => \xE2\x80\x9C\x21Z\x21\xE2\x80\x9D\n"
  "\n"
  ;

const static char *luas =   "#\n"
  "# Based on perl package MarpaX::Languages::Lua::Parser\n"
  "#\n"
  ":default                               ::= action => ::ast\n"
  "\n"
  ":discard                               ::= /\\s+/                            # event => whitespace$\n"
  ":discard                               ::= /--(?:\\[(?!=*\\[))?[^\\n]*/        # event => short_comment$\n"
  ":discard                               ::= /--\\[(=*)\\[.*?\\]\\1\\]/s           # event => long_comment$\n"
  "\n"
  "<lua chunk>                            ::=\n"
  "<lua chunk>                            ::= <lua stat list>\n"
  "                                         | <lua stat list> <lua laststat>\n"
  "                                         | <lua stat list> <lua laststat> ';'\n"
  "                                         | <lua laststat> ';'\n"
  "                                         | <lua laststat>\n"
  "<lua stat list>                        ::= <lua stat>\n"
  "                                         | <lua stat> ';'\n"
  "                                         | <lua stat list> <lua stat> rank => -1\n"
  "                                         | <lua stat list> <lua stat> ';'\n"
  "<lua block>                            ::= <lua chunk>\n"
  "<lua stat>                             ::= <lua varlist> '=' <lua explist>\n"
  "                                         | <lua functioncall> rank => -1\n"
  "                                         | <lua label>\n"
  "                                         | <lua keyword goto> <lua Name>\n"
  "                                         | <lua keyword do> <lua block> <lua keyword end>\n"
  "                                         | <lua keyword while> <lua exp> <lua keyword do> <lua block> <lua keyword end>\n"
  "                                         | <lua keyword repeat> <lua block> <lua keyword until> <lua exp>\n"
  "                                         | <lua keyword if> <lua exp> <lua keyword then> <lua block> <lua elseif sequence> <lua optional else block> <lua keyword end>\n"
  "                                         | <lua keyword for> <lua Name> '=' <lua exp> ',' <lua exp> ',' <lua exp> <lua keyword do> <lua block> <lua keyword end>\n"
  "                                         | <lua keyword for> <lua Name> '=' <lua exp> ',' <lua exp> <lua keyword do> <lua block> <lua keyword end>\n"
  "                                         | <lua keyword for> <lua namelist> <lua keyword in> <lua explist> <lua keyword do> <lua block> <lua keyword end>\n"
  "                                         | <lua keyword function> <lua funcname> <lua funcbody>\n"
  "                                         | <lua keyword local> <lua keyword function> <lua Name> <lua funcbody>\n"
  "                                         | <lua keyword local> <lua namelist> <lua optional namelist initialization>\n"
  "                                         | ';'\n"
  "<lua elseif sequence>                  ::=\n"
  "<lua elseif sequence>                  ::= <lua elseif sequence> <lua elseif block>\n"
  "<lua elseif block>                     ::= <lua keyword elseif> <lua exp> <lua keyword then> <lua block>\n"
  "<lua optional else block>              ::=\n"
  "<lua optional else block>              ::= <lua keyword else> <lua block>\n"
  "<lua optional namelist initialization> ::=\n"
  "<lua optional namelist initialization> ::= '=' <lua explist>\n"
  "<lua laststat>                         ::= <lua keyword return> <lua optional explist>\n"
  "                                         | <lua keyword break>\n"
  "<lua optional explist>                 ::=\n"
  "<lua optional explist>                 ::= <lua explist>\n"
  "<lua funcname>                         ::= <lua dotted name> <lua optional colon name element>\n"
  "<lua dotted name>                      ::= <lua Name>+ separator => '.' proper => 1\n"
  "<lua optional colon name element>      ::=\n"
  "<lua optional colon name element>      ::= ':' <lua Name>\n"
  "<lua varlist>                          ::= <lua var>+ separator => ',' proper => 1\n"
  "<lua var>                              ::= <lua Name>\n"
  "                                         | <lua prefixexp> '[' <lua exp> ']'\n"
  "                                         | <lua prefixexp> '.' <lua Name>\n"
  "<lua namelist>                         ::= <lua Name>+ separator => ',' proper => 1\n"
  "<lua explist>                          ::= <lua exp>+ separator => ',' proper => 1\n"
  "<lua exp>                              ::= <lua var>\n"
  "                                         | '(' <lua exp> ')' assoc => group\n"
  "                                        || <lua exp> <lua args> assoc => right\n"
  "                                        || <lua exp> ':' <lua Name> <lua args> assoc => right\n"
  "                                         | <lua keyword nil>\n"
  "                                         | <lua keyword false>\n"
  "                                         | <lua keyword true>\n"
  "                                         | <lua Number>\n"
  "                                         | <lua String>\n"
  "                                         | '...'\n"
  "                                         | <lua tableconstructor>\n"
  "                                         | <lua function>\n"
  "                                        || <lua exp> '^' <exponent> assoc => right\n"
  "                                        || '-' <lua exp>\n"
  "                                         | <lua keyword not> <lua exp>\n"
  "                                         | '#' <lua exp>\n"
  "                                         | '~' <lua exp>\n"
  "                                        || <lua exp> '*' <lua exp>\n"
  "                                         | <lua exp> '/' <lua exp>\n"
  "                                         | <lua exp> '//' <lua exp>\n"
  "                                         | <lua exp> '%' <lua exp>\n"
  "                                        || <lua exp> '+' <lua exp>\n"
  "                                         | <lua exp> '-' <lua exp>\n"
  "                                        || <lua exp> '..' <lua exp> assoc => right\n"
  "                                        || <lua exp> '<<' <lua exp>\n"
  "                                         | <lua exp> '>>' <lua exp>\n"
  "                                        || <lua exp> '&' <lua exp>\n"
  "                                        || <lua exp> '~' <lua exp>\n"
  "                                        || <lua exp> '|' <lua exp>\n"
  "                                        || <lua exp> '<' <lua exp>\n"
  "                                         | <lua exp> '<=' <lua exp>\n"
  "                                         | <lua exp> '>' <lua exp>\n"
  "                                         | <lua exp> '>=' <lua exp>\n"
  "                                         | <lua exp> '==' <lua exp> rank => 1\n"
  "                                         | <lua exp> '~=' <lua exp>\n"
  "                                        || <lua exp> <lua keyword and> <lua exp> rank => 1\n"
  "                                        || <lua exp> <lua keyword or> <lua exp>\n"
  "<exponent>                             ::= <lua var>\n"
  "                                         | '(' <lua exp> ')'\n"
  "                                        || <exponent> <lua args>\n"
  "                                        || <exponent> ':' <lua Name> <lua args>\n"
  "                                         | <lua keyword nil>\n"
  "                                         | <lua keyword false>\n"
  "                                         | <lua keyword true>\n"
  "                                         | <lua Number>\n"
  "                                         | <lua String>\n"
  "                                         | '...'\n"
  "                                         | <lua tableconstructor>\n"
  "                                         | <lua function>\n"
  "                                        || <lua keyword not> <exponent>\n"
  "                                         | '#' <exponent>\n"
  "                                         | '-' <exponent>\n"
  "<lua prefixexp>                        ::= <lua var>\n"
  "                                         | <lua functioncall>\n"
  "                                         | '(' <lua exp> ')'\n"
  "<lua functioncall>                     ::= <lua prefixexp> <lua args>\n"
  "                                         | <lua prefixexp> ':' <lua Name> <lua args>\n"
  "<lua args>                             ::= '(' <lua optional explist> ')'\n"
  "                                         | <lua tableconstructor>\n"
  "                                         | <lua String>\n"
  "<lua function>                         ::= <lua keyword function> <lua funcbody>\n"
  "<lua funcbody>                         ::= '(' <lua optional parlist> ')' <lua block> <lua keyword end>\n"
  "<lua optional parlist>                 ::=\n"
  "<lua optional parlist>                 ::= <lua namelist>\n"
  "                                         | <lua namelist> ',' '...'\n"
  "                                         | '...'\n"
  " \n"
  "# A lone comma is not allowed in an empty fieldlist,\n"
  "# apparently. This is why I use a dedicated rule\n"
  "# for an empty table and a '+' sequence,\n"
  "# instead of a '*' sequence.\n"
  " \n"
  "<lua tableconstructor>                 ::= '{' '}'\n"
  "                                         | '{' <lua fieldlist> '}'\n"
  "<lua fieldlist>                        ::= <lua field>+ separator => [,;]\n"
  "<lua field>                            ::= '[' <lua exp> ']' '=' <lua exp>\n"
  "                                         | <lua Name> '=' <lua exp>\n"
  "                                         | <lua exp>\n"
  "<lua label>                            ::= '::' <lua Name> '::'\n"
  "<lua Name>                             ::= <LUA NAME> - <LUA RESERVED KEYWORDS>\n"
  "<lua String>                           ::= /'(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)'|\"(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)\"|\\[(=*)\\[.*?\\]\\1\\]/su\n"
  "\n"
  "# A lua number can start with '.' if the later is followed by at least one (hex) digit\n"
  "<lua Number>                           ::= /(?:\\.[0-9]+|[0-9]+(?:\\.[0-9]*)?)(?:[eE][+-]?[0-9]+)?/ \n"
  "                                         | /0[xX](?:\\.[a-fA-F0-9]+|[a-fA-F0-9]+(?:\\.[a-fA-F0-9]*)?)(?:\\.[a-fA-F0-9]*)?(?:[pP][+-]?[0-9]+)?/ \n"
  "\n"
  "\n"
  "<lua keyword and>                        ~ 'and'\n"
  "<lua keyword break>                      ~ 'break'\n"
  "<lua keyword do>                         ~ 'do'\n"
  "<lua keyword else>                       ~ 'else'\n"
  "<lua keyword elseif>                     ~ 'elseif'\n"
  "<lua keyword end>                        ~ 'end'\n"
  "<lua keyword false>                      ~ 'false'\n"
  "<lua keyword for>                        ~ 'for'\n"
  "<lua keyword function>                   ~ 'function'\n"
  "<lua keyword if>                         ~ 'if'\n"
  "<lua keyword in>                         ~ 'in'\n"
  "<lua keyword local>                      ~ 'local'\n"
  "<lua keyword nil>                        ~ 'nil'\n"
  "<lua keyword not>                        ~ 'not'\n"
  "<lua keyword or>                         ~ 'or'\n"
  "<lua keyword repeat>                     ~ 'repeat'\n"
  "<lua keyword return>                     ~ 'return'\n"
  "<lua keyword then>                       ~ 'then'\n"
  "<lua keyword true>                       ~ 'true'\n"
  "<lua keyword until>                      ~ 'until'\n"
  "<lua keyword while>                      ~ 'while'\n"
  "<lua keyword goto>                       ~ 'goto'\n"
  " \n"
  "<LUA NAME>                               ~ /[a-zA-Z_][a-zA-Z_0-9]*/\n"
  "<LUA RESERVED KEYWORDS>                  ~ 'and'\n"
  "                                         | 'break'\n"
  "                                         | 'do'\n"
  "                                         | 'else'\n"
  "                                         | 'elseif'\n"
  "                                         | 'end'\n"
  "                                         | 'false'\n"
  "                                         | 'for'\n"
  "                                         | 'function'\n"
  "                                         | 'if'\n"
  "                                         | 'in'\n"
  "                                         | 'local'\n"
  "                                         | 'nil'\n"
  "                                         | 'not'\n"
  "                                         | 'or'\n"
  "                                         | 'repeat'\n"
  "                                         | 'return'\n"
  "                                         | 'then'\n"
  "                                         | 'true'\n"
  "                                         | 'until'\n"
  "                                         | 'while'\n"
  "                                         | 'goto'\n"
  "\n";

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarLuap = NULL;
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

  /* Compile lua grammar */
  marpaESLIFGrammarOption.bytep               = (void *) luas;
  marpaESLIFGrammarOption.bytel               = strlen(luas);
  marpaESLIFGrammarOption.encodings           = UTF_8_STRING;
  marpaESLIFGrammarOption.encodingl           = strlen(UTF_8_STRING);
  marpaESLIFGrammarLuap = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarLuap == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarLuap, &ngrammari)) {
    goto err;
  }
  for (leveli = 0; leveli < ngrammari; leveli++) {
    if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarLuap, &grammarshows, leveli, NULL)) {
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "LUA grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "------------------------\n%s", grammarshows);
    }
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammarLuap, &grammarscripts)) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "LUA grammar script:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------\n%s", grammarscripts);

  /* We must be able to reparse ESLIF using itself -; */
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
  marpaESLIFRecognizerOption.ifActionResolverp    = NULL;
  marpaESLIFRecognizerOption.eventActionResolverp = eventActionResolver;
  marpaESLIFRecognizerOption.regexActionResolverp = NULL;
  marpaESLIFRecognizerOption.importerp            = NULL;

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

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Creating meta symbol at \"ANYTHING UP TO NEWLINE\" in our grammar");
  metaSymbolp = marpaESLIFSymbol_meta_newp(marpaESLIFp, marpaESLIFGrammarp, "ANYTHING UP TO NEWLINE", &marpaESLIFSymbolOption);
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

  GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "Trying external string symbol match on recognizer");
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
  marpaESLIFGrammar_freev(marpaESLIFGrammarLuap);
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
  *characterStreambp    = 0;
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

/*****************************************************************************/
static marpaESLIFRecognizerEventCallback_t eventActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  short                       rcb;

  GENERICLOGGER_NOTICEF(marpaESLIFTester_contextp->genericLoggerp, "Event %s", actions);

  return 0;
}

