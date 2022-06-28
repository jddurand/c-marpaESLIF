package MyRecognizerInterface;
use strict;
use diagnostics;
use Log::Any qw/$log/;

sub new                    { my ($pkg, $string) = @_; bless { string => $string }, $pkg }
sub read                   { 1 }
sub isEof                  { 1 }
sub isCharacterStream      { 1 }
sub encoding               { }
sub data                   { $_[0]->{string} }
sub isWithDisableThreshold { 0 }
sub isWithExhaustion       { 0 }
sub isWithNewline          { 1 }
sub isWithTrack            { 1 }

package MyValueInterface;
use strict;
use diagnostics;
use Log::Any qw/$log/;

sub new                { my ($pkg, $isWithAmbiguous) = @_; $isWithAmbiguous //= 0; bless { result => undef, isWithAmbiguous => $isWithAmbiguous }, $pkg }
sub isWithHighRankOnly { 1 }
sub isWithOrderByRank  { 1 }
sub isWithAmbiguous    { $_[0]->{isWithAmbiguous} }
sub isWithNull         { 0 }
sub maxParses          { 0 }
sub getResult          { $_[0]->{result} }
sub setResult          { $_[0]->{result} = $_[1] }

package main;
use strict;
use warnings FATAL => 'all';
use Data::Dumper qw/Dumper/;
use Test::More;
use Test::More::UTF8;
use Log::Log4perl qw/:easy/;
use Log::Any::Adapter;
use Log::Any qw/$log/;

#
# Init log
#
our $defaultLog4perlConf = '
log4perl.rootLogger              = INFO, Screen
log4perl.appender.Screen         = Log::Log4perl::Appender::Screen
log4perl.appender.Screen.stderr  = 0
log4perl.appender.Screen.layout  = PatternLayout
log4perl.appender.Screen.layout.ConversionPattern = %d %-5p %6P %m{chomp}%n
';
Log::Log4perl::init(\$defaultLog4perlConf);
Log::Any::Adapter->set('Log4perl');

BEGIN { require_ok('MarpaX::ESLIF') };

my $bnf = do { local $/; <DATA> };

my %inputs = (
    '<whitespace>' => [
	[ 1, 'One blank',                      " "           ],
	[ 1, 'Many blanks',                    "    "        ],
	[ 1, 'Only \\x{0000} character',       "\x{0000}"    ],
	[ 0, '// no text after',               "//"          ],
	[ 0, '// with text but no line break', "// "         ],
	[ 1, '// with text and line break',    "// \n"       ],
	[ 1, 'Multiline comment',              "/* /* */ */" ],
	[ 1, 'Nested multiline comment',       "/* /* */ */" ],
	[ 0, 'Badly nested multiline comment', "/* */ */"    ],
	[ 0, 'Uncomplete discard',             "/* */ /*"    ],
    ],
    '<integer literal>' => [
	[ 1, 'Literal 42',                             "42"          ],
    ],
    '<floating point literal>' => [
	[ 1, 'Literal 3.14159',                        "3.14159"     ],
    ],
    '<string literal>' => [
	[ 1, 'Literal "Hello, World!"',                '"Hello, World!"' ],
    ],
    '<regular expression literal>' => [
	[ 1, 'Literal /Hello, .*/',                    "/Hello, .*/"  ],
    ],
    '<boolean literal>' => [
	[ 1, 'Literal true',                           'true' ],
    ],
    '<top level declaration>' => [
    ],
    );

my $eslif = MarpaX::ESLIF->new($log);
isa_ok($eslif, 'MarpaX::ESLIF');

#
# Collect top-level symbols for tests
#
my @top = keys %inputs;
my %GRAMMARS = ();
foreach my $top (@top) {
    $log->infof('Creating SWIFT grammar at symbol %s', $top);
    my $topbnf = $bnf;
    $topbnf =~ s/\$START/$top/;
    $GRAMMARS{$top} = MarpaX::ESLIF::Grammar->new($eslif, $topbnf);
}

#
# Tests
#
foreach my $top (@top) {
    foreach my $test (@{$inputs{$top}}) {
	my ($expected, $testName, $testData) = @{$test};
	# diag(sprintf('%s: %s: Expecting %s', $top, $testName, $expected ? "success" : "failure"));
	my $g = $GRAMMARS{$top};
	my $recognizerInterface = MyRecognizerInterface->new($testData);
	my $valueInterface = MyValueInterface->new();
        Log::Log4perl->get_logger->level($FATAL); # This will basically shut down logger
	my $got = $g->parse($recognizerInterface, $valueInterface) ? 1 : 0;
        Log::Log4perl->get_logger->level($INFO);
	is($got, $expected, sprintf('%s: %s: Expecting %s', $top, $testName, $expected ? "success" : "failure"));
	if ($got != $expected && $expected) {
	    #
	    # Redo the parse, but with ERROR level and looping on results in case it is ambiguous
	    #
	    $recognizerInterface = MyRecognizerInterface->new($testData);
	    Log::Log4perl->get_logger->level($TRACE);
	    my $r;
	    eval { $r = MarpaX::ESLIF::Recognizer->new($g, $recognizerInterface) };
	    if (defined($r)) {
		if ($r->scan(1)) {
		    while ($r->isCanContinue) {
			last unless $r->resume;
		    }
		}
		$valueInterface = MyValueInterface->new(1);
		my $v;
		eval { $v = MarpaX::ESLIF::Value->new($r, $valueInterface) };
		if (defined($v)) {
		    my $i = 0;
		    while ($v->value) {
			$log->tracef('Value No %d: %s', $i++, Dumper($valueInterface->getResult));
		    }
		}
	    }
	    Log::Log4perl->get_logger->level($INFO);
	}
    }
}


done_testing();

__DATA__
:start ::= $START
:default ::= discard-is-fallback => 1 action => ::ast

# =======
# Discard
# =======
:discard ::= <discard>
<discard> ::= <whitespace>+

# =================
# Lexical Structure
# =================

# Grammar of whitespace
# ---------------------
<whitespace> ::= <whitespace item> <whitespace opt>
<whitespace item> ::= <line break>
<whitespace item> ::= <inline space>
<whitespace item> ::= <comment>
<whitespace item> ::= <multiline comment>
<whitespace item> ::= [\x{0000}\x{000B}\x{000C}]
<line break> ::= [\x{000A}]
<line break> ::= [\x{000D}]
<line break> ::= /\x{000D}\x{000A}/
<inline spaces> ::= <inline space> <inline spaces opt>
<inline space> ::= [\x{0009}\x{0020}]
<comment> ::= '//' <comment text> <line break>
<multiline comment> ::= '/*' <multiline comment text> '*/'
<comment text> ::= <comment text item> <comment text opt>
<comment text item> ::= [^\x{000A}\x{000D}]
<multiline comment text> ::= <multiline comment text item> <multiline comment text opt>
<multiline comment text item> ::= <multiline comment>
<multiline comment text item> ::= <comment text item>
<multiline comment text item> ::= '/' (?!'*') | '*' (?!'/')

# Grammar of an identifier
# ------------------------
<identifier> ::= <identifier head> <identifier characters opt>
<identifier> ::= '`' <identifier head> <identifier characters opt> '`'
<identifier> ::= <implicit parameter name>
<identifier> ::= <property wrapper projection>
<identifier list> ::= <identifier> | <identifier> ',' <identifier list>
<identifier head> ::= [a-zA-Z]
<identifier head> ::= '_'
<identifier head> ::= [\x{00A8}\x{00AA}\x{00AD}\x{00AF}\x{00B2}-\x{00B5}\x{00B7}-\x{00BA}]
<identifier head> ::= [\x{00BC}-\x{00BE}\x{00C0}-\x{00D6}\x{00D8}-\x{00F6}\x{00F8}-\x{00FF}]
<identifier head> ::= [\x{0100}-\x{02FF}\x{0370}-\x{167F}\x{1681}-\x{180D}\x{180F}-\x{1DBF}]:u
<identifier head> ::= [\x{1E00}-\x{1FFF}]:u
<identifier head> ::= [\x{200B}-\x{200D}\x{202A}-\x{202E}\x{203F}-\x{2040}\x{2054}\x{2060}-\x{206F}]:u
<identifier head> ::= [\x{2070}-\x{20CF}\x{2100}-\x{218F}\x{2460}-\x{24FF}\x{2776}-\x{2793}]:u
<identifier head> ::= [\x{2C00}-\x{2DFF}\x{2E80}-\x{2FFF}]:u
<identifier head> ::= [\x{3004}-\x{3007}\x{3021}-\x{302F}\x{3031}-\x{303F}\x{3040}-\x{D7FF}]:u
<identifier head> ::= [\x{F900}-\x{FD3D}\x{FD40}-\x{FDCF}\x{FDF0}-\x{FE1F}\x{FE30}-\x{FE44}]:u
<identifier head> ::= [\x{FE47}-\x{FFFD}]:u
<identifier head> ::= [\x{10000}-\x{1FFFD}\x{20000}-\x{2FFFD}\x{30000}-\x{3FFFD}\x{40000}-\x{4FFFD}]:u
<identifier head> ::= [\x{50000}-\x{5FFFD}\x{60000}-\x{6FFFD}\x{70000}-\x{7FFFD}\x{80000}-\x{8FFFD}]:u
<identifier head> ::= [\x{90000}-\x{9FFFD}\x{A0000}-\x{AFFFD}\x{B0000}-\x{BFFFD}\x{C0000}-\x{CFFFD}]:u
<identifier head> ::= [\x{D0000}-\x{DFFFD}\x{E0000}-\x{EFFFD}]:u
<identifier character> ::= [0-9]
<identifier character> ::= [\x{0300}-\x{036F}\x{1DC0}-\x{1DFF}\x{20D0}-\x{20FF}\x{FE20}-\x{FE2F}]:u
<identifier character> ::= <identifier head>
<identifier characters> ::= <identifier character> <identifier characters opt>
<implicit parameter name> ::= '$' <decimal digits>
<property wrapper projection> ::= '$' <identifier characters>

# Grammar of a literal
# --------------------
<literal> ::= <numeric literal> | <string literal> | <regular expression literal> | <boolean literal> | <nil literal>
<numeric literal> ::= <minus character opt> <integer literal> | <minus character opt> <floating point literal>
<boolean literal> ::= 'true' | 'false'
<nil literal> ::= 'nil'

# Grammar of an integer literal
# -----------------------------
<integer literal> ::= <binary literal>
<integer literal> ::= <octal literal>
<integer literal> ::= <decimal literal>
<integer literal> ::= <hexadecimal literal>
<binary literal> ::= '0b' <binary digit> <binary literal characters opt>
<binary digit> ::= [0-1]
<binary literal character> ::= <binary digit> | '_'
<binary literal characters> ::= <binary literal character> <binary literal characters opt>
<octal literal> ::= '0o' <octal digit> <octal literal characters opt>
<octal digit> ::= [0-7]
<octal literal character> ::= <octal digit> | '_'
<octal literal characters> ::= <octal literal character> <octal literal characters opt>
<decimal literal> ::= <decimal digit> <decimal literal characters opt>
<decimal digit> ::= [0-9]
<decimal digits> ::= <decimal digit> <decimal digits opt>
<decimal literal character> ::= <decimal digit> | '_'
<decimal literal characters> ::= <decimal literal character> <decimal literal characters opt>
<hexadecimal literal> ::= '0x' <hexadecimal digit> <hexadecimal literal characters opt>
<hexadecimal digit> ::= [0-9a-fA-F]
<hexadecimal literal character> ::= <hexadecimal digit> | '_'
<hexadecimal literal characters> ::= <hexadecimal literal character> <hexadecimal literal characters opt>

# Grammar of a floating-point literal
# -----------------------------------
<floating point literal> ::= <decimal literal> <decimal fraction opt> <decimal exponent opt>
<floating point literal> ::= <hexadecimal literal> <hexadecimal fraction opt> <hexadecimal exponent>
<decimal fraction> ::= '.' <decimal literal>
<decimal exponent> ::= <floating point e> <sign opt> <decimal literal>
<hexadecimal fraction> ::= '.' <hexadecimal digit> <hexadecimal literal characters opt>
<hexadecimal exponent> ::= <floating point p> <sign opt> <decimal literal>
<floating point e> ::= 'e' | 'E'
<floating point p> ::= 'p' | 'P'
<sign> ::= '+' | '-'

# Grammar of a string literal
# ---------------------------
#
# Note: the grammar is ambiguous if we let <static string literal> and <interpolated string literal> have optional content, because their
# delimiters are the same.
# We give implicit precedence to <static string literal> by saying that the content of a <interpolated string literal> is not optional.
#
<string literal> ::= <static string literal> rank => 1 | <interpolated string literal>
<string literal opening delimiter> ::= <extended string literal delimiter opt> '"'
<string literal closing delimiter> ::= '"' <extended string literal delimiter opt>
<static string literal> ::= <string literal opening delimiter> <quoted text opt> <string literal closing delimiter>
<static string literal> ::= <multiline string literal opening delimiter> <multiline quoted text opt> <multiline string literal closing delimiter>
<multiline string literal opening delimiter> ::= <extended string literal delimiter opt> '"""'
<multiline string literal closing delimiter> ::= '"""' <extended string literal delimiter opt>
<extended string literal delimiter> ::= '#' <extended string literal delimiter opt>
<quoted text> ::= <quoted text item> <quoted text opt>
<quoted text item> ::= <escaped character>
<quoted text item> ::= [^"\\\x{000A}\x{000D}]
<multiline quoted text> ::= <multiline quoted text item> <multiline quoted text opt>
<multiline quoted text item> ::= <escaped character>
<multiline quoted text item> ::= [^\\]
<multiline quoted text item> ::= <escaped newline>
# <interpolated string literal> ::= <string literal opening delimiter> <interpolated text opt> <string literal closing delimiter>
<interpolated string literal> ::= <string literal opening delimiter> <interpolated text> <string literal closing delimiter>
<interpolated string literal> ::= <multiline string literal opening delimiter> <multiline interpolated text opt> <multiline string literal closing delimiter>
<interpolated text> ::= <interpolated text item> <interpolated text opt>
<interpolated text item> ::= '\\(' <expression> ')' | <quoted text item>
<multiline interpolated text> ::= <multiline interpolated text item> <multiline interpolated text opt>
<multiline interpolated text item> ::= '\\(' <expression> ')' | <multiline quoted text item>
<escape sequence> ::= '\\' <extended string literal delimiter>
<escaped character> ::= <escape sequence> '0' | <escape sequence> '\\' | <escape sequence> 't' | <escape sequence> 'n' | <escape sequence> 'r' | <escape sequence> '"' | <escape sequence> "'"
<escaped character> ::= <escape sequence> 'u' '{' <unicode scalar digits> '}'
<unicode scalar digits> ::= /[0-9a-fA-F]{1-8}/
<escaped newline> ::= <escape sequence> <inline spaces opt> <line break>

# Grammar of a regular expression literal : we ESLIF regexp, which is PCRE2 plus ESLIF's regex options
# ---------------------------------------
<regular expression literal> ::= /(#?)\/(?![*\/])(?:[^\\\/]*(?:\\.[^\\\/]*)*)\/\1/su
<regular expression literal> ::= /(#?)\/(?![*\/])(?:[^\\\/]*(?:\\.[^\\\/]*)*)\/\1/su /[eijmnsxDJUuaNbcA]+/
<regular expression literal> ::= /(#+\R)\/(?![*\/])(?:[^\\\/]*(?:\\.[^\\\/]*)*)\/\R\1/su
<regular expression literal> ::= /(#+\R)\/(?![*\/])(?:[^\\\/]*(?:\\.[^\\\/]*)*)\/\R\1/su /[eijmnsxDJUuaNbcA]+/

# Grammar of operators
# --------------------
<operator> ::= <operator head> <operator characters opt>
<operator> ::= <dot operator head> <dot operator characters>
<operator head> ::= '/' | '=' | '-' | '+' | '!' | '*' | '%' | '<' | '>' | '&' | '|' | '^' | '~' | '?'
<operator head> ::= [\x{00A1}-\x{00A7}]
<operator head> ::= [\x{00A9}x{00AB}]
<operator head> ::= [\{00AC}\x{00AE}]
<operator head> ::= [\x{00B0}-\x{00B1}]
<operator head> ::= [\x{00B6}\x{00BB}\x{00BF}\x{00D7}\x{00F7}]
<operator head> ::= [\x{2016}-\x{2017}]:u
<operator head> ::= [\x{2020}-\x{2027}]:u
<operator head> ::= [\x{2030}-\x{203E}]:u
<operator head> ::= [\x{2041}-\x{2053}]:u
<operator head> ::= [\x{2055}-\x{205E}]:u
<operator head> ::= [\x{2190}-\x{23FF}]:u
<operator head> ::= [\x{2500}-\x{2775}]:u
<operator head> ::= [\x{2794}-\x{2BFF}]:u
<operator head> ::= [\x{2E00}-\x{2E7F}]:u
<operator head> ::= [\x{3001}-\x{3003}]:u
<operator head> ::= [\x{3008}-\x{3020}]:u
<operator head> ::= [\x{3030}]:u
<operator character> ::= <operator head>
<operator character> ::= [\x{0300}-\x{036F}]:u
<operator character> ::= [\x{1DC0}-\x{1DFF}]:u
<operator character> ::= [\x{20D0}-\x{20FF}]:u
<operator character> ::= [\x{FE00}-\x{FE0F}]:u
<operator character> ::= [\x{FE20}-\x{FE2F}]:u
<operator character> ::= [\x{E0100}-\x{E01EF}]:u
<operator characters> ::= <operator character> <operator characters opt>
<dot operator head> ::= '.'
<dot operator character> ::= '.' | <operator character>
<dot operator characters> ::= <dot operator character> <dot operator characters opt>
<infix operator> ::= <operator>
<prefix operator> ::= <operator>
<postfix operator> ::= <operator>

# =====
# Types
# =====

# Grammar of a type
# -----------------
<type> ::= <function type>
<type> ::= <array type>
<type> ::= <dictionary type>
<type> ::= <type identifier>
<type> ::= <tuple type>
<type> ::= <optional type>
<type> ::= <implicitly unwrapped optional type>
<type> ::= <protocol composition type>
<type> ::= <opaque type>
<type> ::= <metatype type>
<type> ::= <any type>
<type> ::= <self type>
<type> ::= '(' <type> ')'

# Grammar of a type annotation
# ----------------------------
<type annotation> ::= ':' <attributes opt> <inout opt> <type>

# Grammar of a type identifier
# ----------------------------
<type identifier> ::= <type name> <generic argument clause opt> | <type name> <generic argument clause opt> '.' <type identifier>
<type name> ::= <identifier>

# Grammar of a tuple type
# -----------------------
<tuple type> ::= '(' ')' | '(' <tuple type element> ',' <tuple type element list> ')'
<tuple type element list> ::= <tuple type element> | <tuple type element> ',' <tuple type element list>
<tuple type element> ::= <element name> <type annotation> | <type>
<element name> ::= <identifier>

# Grammar of a function type
# --------------------------
<function type> ::= <attributes opt> <function type argument clause> <async opt> <throws opt> '->' <type>
<function type argument clause> ::= '(' ')'
<function type argument clause> ::= '(' <function type argument list> <dot dot dot opt> ')'
<function type argument list> ::= <function type argument> | <function type argument> ',' <function type argument list>
<function type argument> ::= <attributes opt> <inout opt> <type> | <argument label> <type annotation>
<argument label> ::= <identifier>

# Grammar of an array type
# ------------------------
<array type> ::= '[' <type> ']'

# Grammar of a dictionary type
# ----------------------------
<dictionary type> ::= '[' <type> ':' <type> ']'

# Grammar of an optional type
# ---------------------------
<optional type> ::= <type> '?'

# Grammar of an implicitly unwrapped optional type
# ------------------------------------------------
<implicitly unwrapped optional type> ::= <type> '!'

# Grammar of a protocol composition type
# --------------------------------------
<protocol composition type> ::= <type identifier> '&' <protocol composition continuation>
<protocol composition continuation> ::= <type identifier> | <protocol composition type>

# Grammar of an opaque type
# -------------------------
<opaque type> ::= 'some' <type>

# Grammar of a metatype type
# --------------------------
<metatype type> ::= <type> '.' 'Type' | <type> '.' 'Protocol'

# Grammar of an Any type
# ----------------------
<any type> ::= 'Any'

# Grammar of a Self type
# ----------------------
<self type> ::= 'Self'

# Grammar of a type inheritance clause
# -------------------------------------
<type inheritance clause> ::= ':' <type inheritance list>
<type inheritance list> ::= <attributes opt> <type identifier> | <attributes opt> <type identifier> ',' <type inheritance list>

# ===========
# Expressions
# ===========

# Grammar of an expression
# ------------------------
<expression> ::= <try operator opt> <await operator opt> <prefix expression> <infix expressions opt>
<expression list> ::= <expression> | <expression> ',' <expression list>

# Grammar of a prefix expression
# ------------------------------
<prefix expression> ::= <prefix operator opt> <postfix expression>
<prefix expression> ::= <in out expression>

# Grammar of an in-out expression
# -------------------------------
<in out expression> ::= '&' <identifier>

# Grammar of a try expression
# ---------------------------
<try operator> ::= 'try' | 'try' '?' | 'try' '!'

# Grammar of an await expression
# ------------------------------
<await operator> ::= 'await'

# Grammar of an infix expression
# ------------------------------
<infix expression> ::= <infix operator> <prefix expression>
<infix expression> ::= <assignment operator> <try operator opt> <prefix expression>
<infix expression> ::= <conditional operator> <try operator opt> <prefix expression>
<infix expression> ::= <type casting operator>
<infix expressions> ::= <infix expression> <infix expressions opt>

# Grammar of an assignment operator
# ---------------------------------
<assignment operator> ::= '='

# Grammar of a conditional operator
# ---------------------------------
<conditional operator> ::= '?' <expression> ':'

# Grammar of a type-casting operator
# ----------------------------------
<type casting operator> ::= 'is' <type>
<type casting operator> ::= 'as' <type>
<type casting operator> ::= 'as' '?' <type>
<type casting operator> ::= 'as' '!' <type>

# Grammar of a primary expression
# -------------------------------
<primary expression> ::= <identifier> <generic argument clause opt>
<primary expression> ::= <literal expression>
<primary expression> ::= <self expression>
<primary expression> ::= <superclass expression>
<primary expression> ::= <closure expression>
<primary expression> ::= <parenthesized expression>
<primary expression> ::= <tuple expression>
<primary expression> ::= <implicit member expression>
<primary expression> ::= <wildcard expression>
<primary expression> ::= <key path expression>
<primary expression> ::= <selector expression>
<primary expression> ::= <key path string expression>

# Grammar of a literal expression
# -------------------------------
<literal expression> ::= <literal>
<literal expression> ::= <array literal> | <dictionary literal> | <playground literal>
<literal expression> ::= '#file' | '#fileID' | '#filePath'
<literal expression> ::= '#line' | '#column' | '#function' | '#dsohandle'
<array literal> ::= '[' <array literal items opt> ']'
<array literal items> ::= <array literal item> <comma opt> | <array literal item> ',' <array literal items>
<array literal item> ::= <expression>
<dictionary literal> ::= '[' <dictionary literal items> ']' | '[' ':' ']'
<dictionary literal items> ::= <dictionary literal item> <comma opt> | <dictionary literal item> ',' <dictionary literal items>
<dictionary literal item> ::= <expression> ':' <expression>
<playground literal> ::= '#colorLiteral' '(' 'red' ':' <expression> ',' 'green' ':' <expression> ',' 'blue' ':' <expression> ',' 'alpha' ':' <expression> ')'
<playground literal> ::= '#fileLiteral' '(' 'resourceName' ':' <expression> ')'
<playground literal> ::= '#imageLiteral' '(' 'resourceName' ':' <expression> ')'

# Grammar of a self expression
# ----------------------------
<self expression> ::= 'self' | <self method expression> | <self subscript expression> | <self initializer expression>
<self method expression> ::= 'self' '.' <identifier>
<self subscript expression> ::= 'self' '[' <function call argument list> ']'
<self initializer expression> ::= 'self' '.' 'init'

# Grammar of a superclass expression
# ----------------------------------
<superclass expression> ::= <superclass method expression> | <superclass subscript expression> | <superclass initializer expression>
<superclass method expression> ::= 'super' '.' <identifier>
<superclass subscript expression> ::= 'super' '[' <function call argument list> ']'
<superclass initializer expression> ::= 'super' '.' 'init'

# Grammar of a closure expression
# -------------------------------
<closure expression> ::= '{' <attributes opt> <closure signature opt> <statements opt> '}'
<closure signature> ::= <capture list opt> <closure parameter clause> <async opt> <throws opt> <function result opt> 'in'
<closure signature> ::= <capture list> 'in'
<closure parameter clause> ::= '(' ')' | '(' <closure parameter list> ')' | <identifier list>
<closure parameter list> ::= <closure parameter> | <closure parameter> ',' <closure parameter list>
<closure parameter> ::= <closure parameter name> <type annotation opt>
<closure parameter> ::= <closure parameter name> <type annotation> <dot dot dot>
<closure parameter name> ::= <identifier>
<capture list> ::= '[' <capture list items> ']'
<capture list items> ::= <capture list item> | <capture list item> ',' <capture list items>
<capture list item> ::= <capture specifier opt> <identifier>
<capture list item> ::= <capture specifier opt> <identifier> '=' <expression>
<capture list item> ::= <capture specifier opt> <self expression>
<capture specifier> ::= 'weak' | 'unowned' | 'unowned(safe)' | 'unowned(unsafe)'

# Grammar of a implicit member expression
# ---------------------------------------
<implicit member expression> ::= '.' <identifier>
<implicit member expression> ::= '.' <identifier> '.' <postfix expression>

# Grammar of a parenthesized expression
# -------------------------------------
<parenthesized expression> ::= '(' <expression> ')'

# Grammar of a tuple expression
# -----------------------------
<tuple expression> ::= '(' ')' | '(' <tuple element> ',' <tuple element list> ')'
<tuple element list> ::= <tuple element> | <tuple element> ',' <tuple element list>
<tuple element> ::= <expression> | <identifier> ':' <expression>

# Grammar of a wildcard expression
# --------------------------------
<wildcard expression> ::= '_'

# Grammar of a key-path expression
# --------------------------------
<key path expression> ::= '\\' <type opt> '.' <key path components>
<key path components> ::= <key path component> | <key path component> ',' <key path components>
<key path component> ::= <identifier> <key path postfixes opt> | <key path postfixes>
<key path postfixes> ::= <key path postfix> <key path postfixes opt>
<key path postfix> ::= '?' | '!' | 'self' | '[' <function call argument list> ']'

# Grammar of a selector expression
# --------------------------------
<selector expression> ::= '#selector' '(' <expression> ')'
<selector expression> ::= '#selector' '(' 'getter:' <expression> ')'
<selector expression> ::= '#selector' '(' 'setter:' <expression> ')'

# Grammar of a key-path string expression
# ---------------------------------------
<key path string expression> ::= '#keyPath' '(' <expression> ')'

# Grammar of a postfix expression
# -------------------------------
<postfix expression> ::= <primary expression>
<postfix expression> ::= <postfix expression> <postfix operator>
<postfix expression> ::= <function call expression>
<postfix expression> ::= <initializer expression>
<postfix expression> ::= <explicit member expression>
<postfix expression> ::= <postfix self expression>
<postfix expression> ::= <subscript expression>
<postfix expression> ::= <forced value expression>
<postfix expression> ::= <optional chaining expression>

# Grammar of a function call expression
# -------------------------------------
<function call expression> ::= <postfix expression> <function call argument clause>
<function call expression> ::= <postfix expression> <function call argument clause opt> <trailing closures>
<function call argument clause> ::= '(' ')' | '(' <function call argument list> ')'
<function call argument list> ::= <function call argument> | <function call argument> ',' <function call argument list>
<function call argument> ::= <expression> | <identifier> ':' <expression>
<function call argument> ::= <operator> | <identifier> ':' <operator>
<trailing closures> ::= <closure expression> <labeled trailing closures opt>
<labeled trailing closures> ::= <labeled trailing closure> <labeled trailing closures opt>
<labeled trailing closure> ::= <identifier> ':' <closure expression>

# Grammar of an initializer expression
# -------------------------------------
<initializer expression> ::= <postfix expression> '.' 'init'
<initializer expression> ::= <postfix expression> '.' 'init' '(' <argument names> ')'

# Grammar of an explicit member expression
# ----------------------------------------
<explicit member expression> ::= <postfix expression> '.' <decimal digits>
<explicit member expression> ::= <postfix expression> '.' <identifier> <generic argument clause opt>
<explicit member expression> ::= <postfix expression> '.' <identifier> '(' <argument names> ')'
<explicit member expression> ::= <postfix expression> <conditional compilation block>
<argument names> ::= <argument name> <argument names opt>
<argument name> ::= <identifier> ':'

# Grammar of a postfix self expression
# ------------------------------------
<postfix self expression> ::= <postfix expression> '.' 'self'

# Grammar of a subscript expression
# ---------------------------------
<subscript expression> ::= <postfix expression> '[' <function call argument list> ']'

# Grammar of a forced-value expression
# ------------------------------------
<forced value expression> ::= <postfix expression> '!'

# Grammar of an optional-chaining expression
# ------------------------------------------
<optional chaining expression> ::= <postfix expression> '?'

# ==========
# Statements
# ==========

# Grammar of a statement
# ----------------------
<statement> ::= <expression> <semicolon opt>
<statement> ::= <declaration> <semicolon opt>
<statement> ::= <loop statement> <semicolon opt>
<statement> ::= <branch statement> <semicolon opt>
<statement> ::= <labeled statement> <semicolon opt>
<statement> ::= <control transfer statement> <semicolon opt>
<statement> ::= <defer statement> <semicolon opt>
<statement> ::= <do statement> <semicolon opt>
<statement> ::= <compiler control statement>
<statements> ::= <statement> <statements opt>

# Grammar of a loop statement
# ---------------------------
<loop statement> ::= <for in statement>
<loop statement> ::= <while statement>
<loop statement> ::= <repeat while statement>

# Grammar of a for-in statement
# -----------------------------
<for in statement> ::= 'for' <case opt> <pattern> 'in' <expression> <where clause opt> <code block>

# Grammar of a while statement
# ----------------------------
<while statement> ::= 'while' <condition list> <code block>
<condition list> ::= <condition> | <condition> ',' <condition list>
<condition> ::= <expression> | <availability condition> | <case condition> | <optional binding condition>
<case condition> ::= <case> <pattern> <initializer>
<optional binding condition> ::= 'let' <pattern initializer opt> | 'var' <pattern initializer opt>

# Grammar of a repeat-while statement
# -----------------------------------
<repeat while statement> ::= 'repeat' <code block> 'while' <expression>

# Grammar of a branch statement
# -----------------------------
<branch statement> ::= <if statement>
<branch statement> ::= <guard statement>
<branch statement> ::= <switch statement>

# Grammar of an if statement
# --------------------------
<if statement> ::= 'if' <condition list> <code block> <else clause opt>
<else clause> ::= 'else' <code block> | 'else' <if statement>

# Grammar of a guard statement
# ----------------------------
<guard statement> ::= 'guard' <condition list> 'else' <code block>

# Grammar of a switch statement
# -----------------------------
<switch statement> ::= 'switch' <expression> '{' <switch cases opt> '}'
<switch cases> ::= <switch case> <switch cases opt>
<switch case> ::= <case label> <statements>
<switch case> ::= <default label> <statements>
<switch case> ::= <conditional switch case>
<case label> ::= <attributes opt> 'case' <case item list> ':'
<case item list> ::= <pattern> <where clause opt> | <pattern> <where clause opt> ',' <case item list>
<default label> ::= <attributes opt> 'default' ':'
<where clause> ::= 'where' <where expression>
<where expression> ::= <expression>
<conditional switch case> ::= <switch if directive clause> <switch elseif directive clauses opt> <switch else directive clause opt> <endif directive>
<switch if directive clause> ::= <if directive> <compilation condition> <switch cases opt>
<switch elseif directive clauses> ::= <elseif directive clause> <switch elseif directive clauses opt>
<switch elseif directive clause> ::= <elseif directive> <compilation condition> <switch cases opt>
<switch else directive clause> ::= <else directive> <switch cases opt>

# Grammar of a labeled statement
# ------------------------------
<labeled statement> ::= <statement label> <loop statement>
<labeled statement> ::= <statement label> <if statement>
<labeled statement> ::= <statement label> <switch statement>
<labeled statement> ::= <statement label> <do statement>
<statement label> ::= <label name> ':'
<label name> ::= <identifier>

# Grammar of a control transfer statement
# ---------------------------------------
<control transfer statement> ::= <break statement>
<control transfer statement> ::= <continue statement>
<control transfer statement> ::= <fallthrough statement>
<control transfer statement> ::= <return statement>
<control transfer statement> ::= <throw statement>

# Grammar of a break statement
# ----------------------------
<break statement> ::= 'break' <label name opt>

# Grammar of a continue statement
# -------------------------------
<continue statement> ::= 'continue' <label name opt>

# Grammar of a fallthrough statement
# ----------------------------------
<fallthrough statement> ::= 'fallthrough'

# Grammar of a return statement
# -----------------------------
<return statement> ::= 'return' <expression opt>

# Grammar of a throw statement
# ----------------------------
<throw statement> ::= 'throw' <expression>

# Grammar of a defer statement
# ----------------------------
<defer statement> ::= 'defer' <code block>

# Grammar of a do statement
# -------------------------
<do statement> ::= 'do' <code block> <catch clauses opt>
<catch clauses> ::= <catch clause> <catch clauses opt>
<catch clause> ::= 'catch' <catch pattern list opt> <code block>
<catch pattern list> ::= <catch pattern> | <catch pattern> ',' <catch pattern list>
<catch pattern> ::= <pattern> <where clause opt>

# Grammar of a compiler control statement
# ---------------------------------------
<compiler control statement> ::= <conditional compilation block>
<compiler control statement> ::= <line control statement>
<compiler control statement> ::= <diagnostic statement>

# Grammar of a conditional compilation block
# ------------------------------------------
<conditional compilation block> ::= <if directive clause> <elseif directive clauses opt> <else directive clause opt> <endif directive>
<if directive clause> ::= <if directive> <compilation condition> <statements opt>
<elseif directive clauses> ::= <elseif directive clause> <elseif directive clauses opt>
<elseif directive clause> ::= <elseif directive> <compilation condition> <statements opt>
<else directive clause> ::= <else directive> <statements opt>
<if directive> ::= '#if'
<elseif directive> ::= '#elseif'
<else directive> ::= '#else'
<endif directive> ::= '#endif'
<compilation condition> ::= <platform condition>
<compilation condition> ::= <identifier>
<compilation condition> ::= <boolean literal>
<compilation condition> ::= '(' <compilation condition> ')'
<compilation condition> ::= '!' <compilation condition>
<compilation condition> ::= <compilation condition> '&&' <compilation condition>
<compilation condition> ::= <compilation condition> '||' <compilation condition>
<platform condition> ::= 'os' '(' <operating system> ')'
<platform condition> ::= 'arch' '(' <architecture> ')'
<platform condition> ::= 'swift' '(' '>=' <swift version> ')' | 'swift' '(' '<' <swift version> ')'
<platform condition> ::= 'compiler' '(' '>=' <swift version> ')' | 'compiler' '(' '<' <swift version> ')'
<platform condition> ::= 'canImport' '(' <import path> ')'
<platform condition> ::= 'targetEnvironment' '(' <environment> ')'
<operating system> ::= 'macOS' | 'iOS' | 'watchOS' | 'tvOS' | 'Linux' | 'Windows'
<architecture> ::= 'i386' | 'x86_64' | 'arm' | 'arm64'
<swift version> ::= <decimal digits> <swift version continuation opt>
<swift version continuation> ::= '.' <decimal digits> <swift version continuation opt>
<environment> ::= 'simulator' | 'macCatalyst'

# Grammar of a line control statement
# -----------------------------------
<line control statement> ::= '#sourceLocation' '(' 'file:' <file path> ',' 'line:' <line number> ')'
<line control statement> ::= '#sourceLocation' '(' ')'
<line number> ::= /[0-9]+/ # A decimal integer greater than zero
<file path> ::= <static string literal>

# Grammar of a compile-time diagnostic statement
# ----------------------------------------------
<diagnostic statement> ::= '#error' '(' <diagnostic message> ')'
<diagnostic statement> ::= '#warning' '(' <diagnostic message> ')'
<diagnostic message> ::= <static string literal>

# Grammar of an availability condition
# ------------------------------------
<availability condition> ::= '#available' '(' <availability arguments> ')'
<availability condition> ::= '#unavailable' '(' <availability arguments> ')'
<availability arguments> ::= <availability argument> | <availability argument> ',' <availability arguments>
<availability argument> ::= <platform name> <platform version>
<availability argument> ::= '*'
<platform name> ::= 'iOS' | 'iOSApplicationExtension'
<platform name> ::= 'macOS' | 'macOSApplicationExtension'
<platform name> ::= 'macCatalyst' | 'macCatalystApplicationExtension'
<platform name> ::= 'watchOS'
<platform name> ::= 'tvOS'
<platform version> ::= <decimal digits>
<platform version> ::= <decimal digits> '.' <decimal digits>
<platform version> ::= <decimal digits> '.' <decimal digits> '.' <decimal digits>

# ============
# Declarations
# ============

# Grammar of a declaration
# ------------------------
<declaration> ::= <import declaration>
<declaration> ::= <constant declaration>
<declaration> ::= <variable declaration>
<declaration> ::= <typealias declaration>
<declaration> ::= <function declaration>
<declaration> ::= <enum declaration>
<declaration> ::= <struct declaration>
<declaration> ::= <class declaration>
<declaration> ::= <actor declaration>
<declaration> ::= <protocol declaration>
<declaration> ::= <initializer declaration>
<declaration> ::= <deinitializer declaration>
<declaration> ::= <extension declaration>
<declaration> ::= <subscript declaration>
<declaration> ::= <operator declaration>
<declaration> ::= <precedence group declaration>
<declarations> ::= <declaration> <declarations opt>

# Grammar of a top-level declaration
# ----------------------------------
<top level declaration> ::= <statements opt>

# Grammar of a code block
# -----------------------
<code block> ::= '{' <statements opt> '}'

# Grammar of an import declaration
# --------------------------------
<import declaration> ::= <attributes opt> 'import' <import kind opt> <import path>
<import kind> ::= 'typealias' | 'struct' | 'class' | 'enum' | 'protocol' | 'let' | 'var' | 'func'
<import path> ::= <identifier> | <identifier> '.' <import path>

# Grammar of a constant declaration
# ---------------------------------
<constant declaration> ::= <attributes opt> <declaration modifiers opt> 'let' <pattern initializer list>
<pattern initializer list> ::= <pattern initializer> | <pattern initializer> ',' <pattern initializer list>
<pattern initializer> ::= <pattern> <initializer opt>
<initializer> ::= '=' <expression>

# Grammar of a variable declaration
# ---------------------------------
<variable declaration> ::= <variable declaration head> <pattern initializer list>
<variable declaration> ::= <variable declaration head> <variable name> <type annotation> <code block>
<variable declaration> ::= <variable declaration head> <variable name> <type annotation> <getter setter block>
<variable declaration> ::= <variable declaration head> <variable name> <type annotation> <getter setter keyword block>
<variable declaration> ::= <variable declaration head> <variable name> <initializer> <willSet didSet block>
<variable declaration> ::= <variable declaration head> <variable name> <type annotation> <initializer opt> <willSet didSet block>
<variable declaration head> ::= <attributes opt> <declaration modifiers opt> 'var'
<variable name> ::= <identifier>
<getter setter block> ::= <code block>
<getter setter block> ::= '{' <getter clause> <setter clause opt> '}'
<getter setter block> ::= '{' <setter clause> <getter clause> '}'
<getter clause> ::= <attributes opt> <mutation modifier opt> 'get' <code block>
<setter clause> ::= <attributes opt> <mutation modifier opt> 'set' <setter name opt> <code block>
<setter name> ::= '(' <identifier> ')'
<getter setter keyword block> ::= '{' <getter keyword clause> <setter keyword clause opt> '}'
<getter setter keyword block> ::= '{' <setter keyword clause> <getter keyword clause> '}'
<getter keyword clause> ::= <attributes opt> <mutation modifier opt> 'get'
<setter keyword clause> ::= <attributes opt> <mutation modifier opt> 'set'
<willSet didSet block> ::= '{' <willSet clause> <didSet clause opt> '}'
<willSet didSet block> ::= '{' <didSet clause> <willSet clause opt> '}'
<willSet clause> ::= <attributes opt> 'willSet' <setter name opt> <code block>
<didSet clause> ::= <attributes opt> 'didSet' <setter name opt> <code block>

# Grammar of a type alias declaration
# -----------------------------------
<typealias declaration> ::= <attributes opt> <access level modifier opt> 'typealias' <typealias name> <generic parameter clause opt> <typealias assignment>
<typealias name> ::= <identifier>
<typealias assignment> ::= '=' <type>

# Grammar of a function declaration
# ---------------------------------
<function declaration> ::= <function head> <function name> <generic parameter clause opt> <function signature> <generic where clause opt> <function body opt>
<function head> ::= <attributes opt> <declaration modifiers opt> 'func'
<function name> ::= <identifier> | <operator>
<function signature> ::= <parameter clause> <async opt> <throws opt> <function result opt>
<function signature> ::= <parameter clause> <async opt> 'rethrows' <function result opt>
<function result> ::= '->' <attributes opt> <type>
<function body> ::= <code block>
<parameter clause> ::= '(' ')' | '(' <parameter list> ')'
<parameter list> ::= <parameter> | <parameter> ',' <parameter list>
<parameter> ::= <external parameter name opt> <local parameter name> <type annotation> <default argument clause opt>
<parameter> ::= <external parameter name opt> <local parameter name> <type annotation>
<parameter> ::= <external parameter name opt> <local parameter name> <type annotation> '...'
<external parameter name> ::= <identifier>
<local parameter name> ::= <identifier>
<default argument clause> ::= '=' <expression>

# Grammar of an enumeration declaration
# -------------------------------------
<enum declaration> ::= <attributes opt> <access level modifier opt> <union style enum>
<enum declaration> ::= <attributes opt> <access level modifier opt> <raw value style enum>
<union style enum> ::= <indirect opt> 'enum' <enum name> <generic parameter clause opt> <type inheritance clause opt> <generic where clause opt> '{' <union style enum members opt> '}'
<union style enum members> ::= <union style enum member> <union style enum members opt>
<union style enum member> ::= <declaration> | <union style enum case clause> | <compiler control statement>
<union style enum case clause> ::= <attributes opt> <indirect opt> 'case' <union style enum case list>
<union style enum case list> ::= <union style enum case> | <union style enum case> ',' <union style enum case list>
<union style enum case> ::= <enum case name> <tuple type opt>
<enum name> ::= <identifier>
<enum case name> ::= <identifier>
<raw value style enum> ::= 'enum' <enum name> <generic parameter clause opt> <type inheritance clause> <generic where clause opt> '{' <raw value style enum members> '}'
<raw value style enum members> ::= <raw value style enum member> <raw value style enum members opt>
<raw value style enum member> ::= <declaration> | <raw value style enum case clause> | <compiler control statement>
<raw value style enum case clause> ::= <attributes opt> 'case' <raw value style enum case list>
<raw value style enum case list> ::= <raw value style enum case> | <raw value style enum case> ',' <raw value style enum case list>
<raw value style enum case> ::= <enum case name> <raw value assignment opt>
<raw value assignment> ::= '=' <raw value literal>
<raw value literal> ::= <numeric literal> | <static string literal> | <boolean literal>

# Grammar of a structure declaration
# ----------------------------------
<struct declaration> ::= <attributes opt> <access level modifier opt> 'struct' <struct name> <generic parameter clause opt> <type inheritance clause opt> <generic where clause opt> <struct body>
<struct name> ::= <identifier>
<struct body> ::= '{' <struct members opt> '}'
<struct members> ::= <struct member> <struct members opt>
<struct member> ::= <declaration> | <compiler control statement>

# Grammar of a class declaration
# ------------------------------
<class declaration> ::= <attributes opt> <access level modifier opt> <final opt> 'class' <class name> <generic parameter clause opt> <type inheritance clause opt> <generic where clause opt> <class body>
<class declaration> ::= <attributes opt> 'final' <access level modifier opt> 'class' <class name> <generic parameter clause opt> <type inheritance clause opt> <generic where clause opt> <class body>
<class name> ::= <identifier>
<class body> ::= '{' <class members opt> '}'
<class members> ::= <class member> <class members opt>
<class member> ::= <declaration> | <compiler control statement>

# Grammar of an actor declaration
# -------------------------------
<actor declaration> ::= <attributes opt> <access level modifier opt> 'actor' <actor name> <generic parameter clause opt> <type inheritance clause opt> <generic where clause opt> <actor body>
<actor name> ::= <identifier>
<actor body> ::= '{' <actor members opt> '}'
<actor members> ::= <actor member> <actor members opt>
<actor member> ::= <declaration> | <compiler control statement>

# Grammar of a protocol declaration
# ---------------------------------
<protocol declaration> ::= <attributes opt> <access level modifier opt> 'protocol' <protocol name> <type inheritance clause opt> <generic where clause opt> <protocol body>
<protocol name> ::= <identifier>
<protocol body> ::= '{' <protocol members opt> '}'
<protocol members> ::= <protocol member> <protocol members opt>
<protocol member> ::= <protocol member declaration> | <compiler control statement>
<protocol member declaration> ::= <protocol property declaration>
<protocol member declaration> ::= <protocol method declaration>
<protocol member declaration> ::= <protocol initializer declaration>
<protocol member declaration> ::= <protocol subscript declaration>
<protocol member declaration> ::= <protocol associated type declaration>
<protocol member declaration> ::= <typealias declaration>

# Grammar of a protocol property declaration
# ------------------------------------------
<protocol property declaration> ::= <variable declaration head> <variable name> <type annotation> <getter setter keyword block>

# Grammar of a protocol method declaration
# ----------------------------------------
<protocol method declaration> ::= <function head> <function name> <generic parameter clause opt> <function signature> <generic where clause opt>

# Grammar of a protocol initializer declaration
# ---------------------------------------------
<protocol initializer declaration> ::= <initializer head> <generic parameter clause opt> <parameter clause> <throws opt> <generic where clause opt>
<protocol initializer declaration> ::= <initializer head> <generic parameter clause opt> <parameter clause> 'rethrows' <generic where clause opt>

# Grammar of a protocol subscript declaration
# -------------------------------------------
<protocol subscript declaration> ::= <subscript head> <subscript result> <generic where clause opt> <getter setter keyword block>

# Grammar of a protocol associated type declaration
# -------------------------------------------------
<protocol associated type declaration> ::= <attributes opt> <access level modifier opt> 'associatedtype' <typealias name> <type inheritance clause opt> <typealias assignment opt> <generic where clause opt>

# Grammar of an initializer declaration
# -------------------------------------
<initializer declaration> ::= <initializer head> <generic parameter clause opt> <parameter clause> <async opt> <throws opt> <generic where clause opt> <initializer body>
<initializer declaration> ::= <initializer head> <generic parameter clause opt> <parameter clause> <async opt> 'rethrows' <generic where clause opt> <initializer body>
<initializer head> ::= <attributes opt> <declaration modifiers opt> 'init'
<initializer head> ::= <attributes opt> <declaration modifiers opt> 'init' '?'
<initializer head> ::= <attributes opt> <declaration modifiers opt> 'init' '!'
<initializer body> ::= <code block>

# Grammar of a deinitializer declaration
# --------------------------------------
<deinitializer declaration> ::= <attributes opt> 'deinit' <code block>

# Grammar of an extension declaration
# -----------------------------------
<extension declaration> ::= <attributes opt> <access level modifier opt> 'extension' <type identifier> <type inheritance clause opt> <generic where clause opt> <extension body>
<extension body> ::= '{' <extension members opt> '}'
<extension members> ::= <extension member> <extension members opt>
<extension member> ::= <declaration> | <compiler control statement>

# Grammar of a subscript declaration
# ----------------------------------
<subscript declaration> ::= <subscript head> <subscript result> <generic where clause opt> <code block>
<subscript declaration> ::= <subscript head> <subscript result> <generic where clause opt> <getter setter block>
<subscript declaration> ::= <subscript head> <subscript result> <generic where clause opt> <getter setter keyword block>
<subscript head> ::= <attributes opt> <declaration modifiers opt> 'subscript' <generic parameter clause opt> <parameter clause>
<subscript result> ::= '->' <attributes opt> <type>

# Grammar of an operator declaration
# ----------------------------------
<operator declaration> ::= <prefix operator declaration> | <postfix operator declaration> | <infix operator declaration>
<prefix operator declaration> ::= 'prefix' 'operator' <operator>
<postfix operator declaration> ::= 'postfix' 'operator' <operator>
<infix operator declaration> ::= 'infix' 'operator' <operator> <infix operator group opt>
<infix operator group> ::= ':' <precedence group name>

# Grammar of a precedence group declaration
# -----------------------------------------
<precedence group declaration> ::= 'precedencegroup' <precedence group name> '{' <precedence group attributes opt> '}'
<precedence group attributes> ::= <precedence group attribute> <precedence group attributes opt>
<precedence group attribute> ::= <precedence group relation>
<precedence group attribute> ::= <precedence group assignment>
<precedence group attribute> ::= <precedence group associativity>
<precedence group relation> ::= 'higherThan' ':' <precedence group names>
<precedence group relation> ::= 'lowerThan' ':' <precedence group names>
<precedence group assignment> ::= 'assignment' ':' <boolean literal>
<precedence group associativity> ::= 'associativity' ':' 'left'
<precedence group associativity> ::= 'associativity' ':' 'right'
<precedence group associativity> ::= 'associativity' ':' 'none'
<precedence group names> ::= <precedence group name> | <precedence group name> ',' <precedence group names>
<precedence group name> ::= <identifier>

# Grammar of a declaration modifier
# ---------------------------------
<declaration modifier> ::= 'class' | 'convenience' | 'dynamic' | 'final' | 'infix' | 'lazy' | 'optional' | 'override' | 'postfix' | 'prefix' | 'required' | 'static' | 'unowned' | 'unowned' '(' 'safe' ')' | 'unowned' '(' 'unsafe' ')' | 'weak'
<declaration modifier> ::= <access level modifier>
<declaration modifier> ::= <mutation modifier>
<declaration modifier> ::= <actor isolation modifier>
<declaration modifiers> ::= <declaration modifier> <declaration modifiers opt>
<access level modifier> ::= 'private' | 'private' '(' 'set' ')'
<access level modifier> ::= 'fileprivate' | 'fileprivate' '(' 'set' ')'
<access level modifier> ::= 'internal' | 'internal' '(' 'set' ')'
<access level modifier> ::= 'public' | 'public' '(' 'set' ')'
<access level modifier> ::= 'open' | 'open' '(' 'set' ')'
<mutation modifier> ::= 'mutating' | 'nonmutating'
<actor isolation modifier> ::= 'nonisolated'

# ==========
# Attributes
# ==========

# Grammar of an attribute
# -----------------------
<attribute> ::= '@' <attribute name> <attribute argument clause opt>
<attribute name> ::= <identifier>
<attribute argument clause> ::= '(' <balanced tokens opt> ')'
<attributes> ::= <attribute> <attributes opt>
<balanced tokens> ::= <balanced token> <balanced tokens opt>
<balanced token> ::= '(' <balanced tokens opt> ')'
<balanced token> ::= '[' <balanced tokens opt> ']'
<balanced token> ::= '{' <balanced tokens opt> '}'
<balanced token> ::= <identifier> | <all keywords> | <literal> | <operator> # Any identifier, keyword, literal, or operator
<balanced token> ::= '.' | ',' | ':' | ';' | '=' | '@' | '#' | '&'| '->' | '`' | '?' | '!' # Any punctuation except (, ), [, ], {, or }

# ========
# Patterns
# ========

# Grammar of a pattern
# --------------------
<pattern> ::= <wildcard pattern> <type annotation opt>
<pattern> ::= <identifier pattern> <type annotation opt>
<pattern> ::= <value binding pattern>
<pattern> ::= <tuple pattern> <type annotation opt>
<pattern> ::= <enum case pattern>
<pattern> ::= <optional pattern>
<pattern> ::= <type casting pattern>
<pattern> ::= <expression pattern>

# Grammar of a wildcard pattern
# -----------------------------
<wildcard pattern> ::= '_'

# Grammar of an identifier pattern
# --------------------------------
<identifier pattern> ::= <identifier>

# Grammar of a value-binding pattern
# ----------------------------------
<value binding pattern> ::= 'var' <pattern> | 'let' <pattern>

# Grammar of a tuple pattern
# --------------------------
<tuple pattern> ::= '(' <tuple pattern element list opt> ')'
<tuple pattern element list> ::= <tuple pattern element> | <tuple pattern element> ',' <tuple pattern element list>
<tuple pattern element> ::= <pattern> | <identifier> ':' <pattern>

# Grammar of an enumeration case pattern
# --------------------------------------
<enum case pattern> ::= <type identifier opt> '.' <enum case name> <tuple pattern opt>

# Grammar of an optional pattern
# ------------------------------
<optional pattern> ::= <identifier pattern> '?'

# Grammar of a type casting pattern
# ---------------------------------
<type casting pattern> ::= <is pattern> | <as pattern>
<is pattern> ::= 'is' <type>
<as pattern> ::= <pattern> 'as' <type>

# Grammar of an expression pattern
# --------------------------------
<expression pattern> ::= <expression>

# ================================
# Generic Parameters and Arguments
# ================================

# Grammar of a generic parameter clause
# -------------------------------------
<generic parameter clause> ::= '<' <generic parameter list> '>'
<generic parameter list> ::= <generic parameter> | <generic parameter> ',' <generic parameter list>
<generic parameter> ::= <type name>
<generic parameter> ::= <type name> ':' <type identifier>
<generic parameter> ::= <type name> ':' <protocol composition type>
<generic where clause> ::= 'where' <requirement list>
<requirement list> ::= <requirement> | <requirement> ',' <requirement list>
<requirement> ::= <conformance requirement> | <same type requirement>
<conformance requirement> ::= <type identifier> ':' <type identifier>
<conformance requirement> ::= <type identifier> ':' <protocol composition type>
<same type requirement> ::= <type identifier> '==' <type>

# Grammar of a generic argument clause
# ------------------------------------
<generic argument clause> ::= '<' <generic argument list> '>'
<generic argument list> ::= <generic argument> | <generic argument> ',' <generic argument list>
<generic argument> ::= <type>

# =======
# Helpers
# =======
<whitespace opt> ::= <whitespace>
<whitespace opt> ::=
<inline spaces opt> ::= <inline spaces>
<inline spaces opt> ::=
<comment text opt> ::= <comment text>
<comment text opt> ::=
<multiline comment text opt> ::= <multiline comment text>
<multiline comment text opt> ::=
<identifier characters opt> ::= <identifier characters>
<identifier characters opt> ::=
<minus character opt> ::= '-'
<minus character opt> ::=
<binary literal characters opt> ::= <binary literal characters>
<binary literal characters opt> ::=
<octal literal characters opt> ::= <octal literal characters>
<octal literal characters opt> ::=
<decimal literal characters opt> ::= <decimal literal characters>
<decimal literal characters opt> ::=
<decimal digits opt> ::= <decimal digits>
<decimal digits opt> ::=
<hexadecimal literal characters opt> ::= <hexadecimal literal characters>
<hexadecimal literal characters opt> ::=
<decimal fraction opt> ::= <decimal fraction>
<decimal fraction opt> ::=
<decimal exponent opt> ::= <decimal exponent>
<decimal exponent opt> ::=
<hexadecimal fraction opt> ::= <hexadecimal fraction>
<hexadecimal fraction opt> ::=
<sign opt> ::= <sign>
<sign opt> ::=
<extended string literal delimiter opt> ::= <extended string literal delimiter>
<extended string literal delimiter opt> ::=
<quoted text opt> ::= <quoted text>
<quoted text opt> ::=
<multiline quoted text opt> ::= <multiline quoted text>
<multiline quoted text opt> ::=
<interpolated text opt> ::= <interpolated text>
<interpolated text opt> ::=
<multiline interpolated text opt> ::= <multiline interpolated text opt>
<multiline interpolated text opt> ::=
# <extended regular expression literal delimiter opt> ::= <extended regular expression literal delimiter>
# <extended regular expression literal delimiter opt> ::=
<operator characters opt> ::= <operator characters>
<operator characters opt> ::=
<dot operator characters opt> ::= <dot operator characters>
<dot operator characters opt> ::=
<attributes opt> ::= <attributes>
<attributes opt> ::=
<inout> ::= 'inout'
<inout opt> ::= <inout>
<inout opt> ::=
<generic argument clause opt> ::= <generic argument clause>
<generic argument clause opt> ::=
<async> ::= 'async'
<async opt> ::= <async>
<async opt> ::=
<throws> ::= 'throws'
<throws opt> ::= <throws>
<throws opt> ::=
<dot dot dot> ::= '...'
<dot dot dot opt> ::= <dot dot dot>
<dot dot dot opt> ::=
<try operator opt> ::= <try operator>
<try operator opt> ::=
<await operator opt> ::= <await operator>
<await operator opt> ::=
<infix expressions opt> ::= <infix expressions>
<infix expressions opt> ::=
<prefix operator opt> ::= <prefix operator>
<prefix operator opt> ::=
<array literal items opt> ::= <array literal items>
<array literal items opt> ::=
<comma opt> ::= ','
<comma opt> ::=
<closure signature opt> ::= <closure signature>
<closure signature opt> ::=
<statements opt> ::= <statements>
<statements opt> ::=
<capture list opt> ::= <capture list>
<capture list opt> ::=
<function result opt> ::= <function result>
<function result opt> ::=
<type annotation opt> ::= <type annotation>
<type annotation opt> ::=
<capture specifier opt> ::= <capture specifier>
<capture specifier opt> ::=
<type opt> ::= <type>
<type opt> ::=
<key path postfixes opt> ::= <key path postfixes>
<key path postfixes opt> ::=
<function call argument clause opt> ::= <function call argument clause>
<function call argument clause opt> ::=
<labeled trailing closures opt> ::= <labeled trailing closures>
<labeled trailing closures opt> ::=
<argument names opt> ::= <argument names>
<argument names opt> ::=
<semicolon opt> ::= ';'
<semicolon opt> ::=
<case> ::= 'case'
<case opt> ::= <case>
<case opt> ::=
<where clause opt> ::= <where clause>
<where clause opt> ::=
<pattern initializer opt> ::= <pattern initializer>
<pattern initializer opt> ::=
<else clause opt> ::= <else clause>
<else clause opt> ::=
<switch cases opt> ::= <switch cases>
<switch cases opt> ::=
<switch elseif directive clauses opt> ::= <switch elseif directive clauses>
<switch elseif directive clauses opt> ::=
<switch else directive clause opt> ::= <switch else directive clause>
<switch else directive clause opt> ::=
<label name opt> ::= <label name>
<label name opt> ::=
<expression opt> ::= <expression>
<expression opt> ::=
<catch clauses opt> ::= <catch clauses>
<catch clauses opt> ::=
<catch pattern list opt> ::= <catch pattern list>
<catch pattern list opt> ::=
<elseif directive clauses opt> ::= <elseif directive clauses>
<elseif directive clauses opt> ::=
<else directive clause opt> ::= <else directive clause>
<else directive clause opt> ::=
<swift version continuation opt> ::= <swift version continuation>
<swift version continuation opt> ::=
<declarations opt> ::= <declarations>
<declarations opt> ::=
<import kind opt> ::= <import kind>
<import kind opt> ::=
<declaration modifiers opt> ::= <declaration modifiers>
<declaration modifiers opt> ::=
<setter clause opt> ::= <setter clause>
<setter clause opt> ::=
<setter keyword clause opt> ::= <setter keyword clause>
<setter keyword clause opt> ::=
<didSet clause opt> ::= <didSet clause>
<didSet clause opt> ::=
<willSet clause opt> ::= <willSet clause>
<willSet clause opt> ::=
<indirect opt> ::= 'indirect'
<indirect opt> ::=
<union style enum members opt> ::= <union style enum members>
<union style enum members opt> ::=
<raw value style enum members opt> ::= <raw value style enum members>
<raw value style enum members opt> ::=
<raw value assignment opt> ::= <raw value assignment>
<raw value assignment opt> ::=
<struct members opt> ::= <struct members>
<struct members opt> ::=
<final opt> ::= 'final'
<final opt> ::=
<class members opt> ::= <class members>
<class members opt> ::=
<actor members opt> ::= <actor members>
<actor members opt> ::=
<protocol members opt> ::= <protocol members>
<protocol members opt> ::=
<extension members opt> ::= <extension members>
<extension members opt> ::=
<infix operator group opt> ::= <infix operator group>
<infix operator group opt> ::=
<precedence group attributes opt> ::= <precedence group attributes>
<precedence group attributes> ::=
<attribute argument clause opt> ::= <attribute argument clause>
<attribute argument clause opt> ::=
<balanced tokens opt> ::= <balanced tokens>
<balanced tokens opt> ::=
<all keywords> ::= 'associatedtype' | 'class' | 'deinit' | 'enum' | 'extension' | 'fileprivate' | 'func' | 'import' | 'init' | 'inout' | 'internal' | 'let' | 'open' | 'operator' | 'private' | 'precedencegroup' | 'protocol' | 'public' | 'rethrows' | 'static' | 'struct' | 'subscript' | 'typealias' | 'var' | 'break' | 'case' | 'catch' | 'continue' | 'default' | 'defer' | 'do' | 'else' | 'fallthrough' | 'for' | 'guard' | 'if' | 'in' | 'repeat' | 'return' | 'throw' | 'switch' | 'where' | 'while' | 'Any' | 'as' | 'false' | 'is' | 'nil' | 'self' | 'Self' | 'super' | 'throws' | 'true' | 'try' | '_' | '#available' | '#colorLiteral' | '#column' | '#dsohandle' | '#elseif' | '#else' | '#endif' | '#error' | '#fileID' | '#fileLiteral' | '#filePath' | '#file' | '#function' | '#if' | '#imageLiteral' | '#keyPath' | '#line' | '#selector' | '#sourceLocation #warning' | 'associativity' | 'convenience' | 'didSet' | 'dynamic' | 'final' | 'get' | 'indirect' | 'infix' | 'lazy' | 'left' | 'mutating' | 'none' | 'nonmutating' | 'optional' | 'override' | 'postfix' | 'precedence' | 'prefix' | 'Protocol' | 'required' | 'right' | 'set' | 'some' | 'Type' | 'unowned' | 'weak' | 'willSet'
<tuple pattern element list opt> ::= <tuple pattern element list>
<tuple pattern element list opt> ::=
<tuple pattern opt> ::= <tuple pattern>
<tuple pattern opt> ::=
<initializer opt> ::= <initializer>
<initializer opt> ::=
<mutation modifier opt> ::= <mutation modifier>
<mutation modifier opt> ::=
<setter name opt> ::= <setter name>
<setter name opt> ::=
<access level modifier opt> ::= <access level modifier>
<access level modifier opt> ::=
<generic parameter clause opt> ::= <generic parameter clause>
<generic parameter clause opt> ::=
<generic where clause opt> ::= <generic where clause>
<generic where clause opt> ::=
<function body opt> ::= <function body>
<function body opt> ::=
<external parameter name opt> ::= <external parameter name>
<external parameter name opt> ::=
<default argument clause opt> ::= <default argument clause>
<default argument clause opt> ::=
<type inheritance clause opt> ::= <type inheritance clause>
<type inheritance clause opt> ::=
<tuple type opt> ::= <tuple type>
<tuple type opt> ::=
<typealias assignment opt> ::= <typealias assignment>
<typealias assignment opt> ::=
<type identifier opt> ::= <type identifier>
<type identifier opt> ::=
