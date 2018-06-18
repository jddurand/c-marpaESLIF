#
# This file is adapted from MarpaX::ESLIF::ECMA404
#
package MyRecognizerInterface;
use strict;
use diagnostics;

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

sub new                { my ($pkg) = @_; bless { result => undef }, $pkg }
sub isWithHighRankOnly { 1 }
sub isWithOrderByRank  { 1 }
sub isWithAmbiguous    { 0 }
sub isWithNull         { 0 }
sub maxParses          { 0 }
sub getResult          { $_[0]->{result} }
sub setResult          { $_[0]->{result} = $_[1] }

package main;
use strict;
use warnings FATAL => 'all';
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

my $DATA = do { local $/; <DATA>; };

my @inputs = (
    "{\"test\":[1,2,3]}",
    "{\"test\":\"1\"}",
    "{\"test\":true}",
    "{\"test\":false}",
    "{\"test\":null}",
    "{\"test\":null, \"test2\":\"hello world\"}",
    "{\"test\":\"1.25\"}",
    "{\"test\":\"1.25e4\"}",
    "[]",
    "[
       { 
          \"precision\": \"zip\",
          \"Latitude\":  37.7668,
          \"Longitude\": -122.3959,
          \"Address\":   \"\",
          \"City\":      \"SAN FRANCISCO\",
          \"State\":     \"CA\",
          \"Zip\":       \"94107\",
          \"Country\":   \"US\"
       },
       {
          \"precision\": \"zip\",
          \"Latitude\":  37.371991,
          \"Longitude\": -122.026020,
          \"Address\":   \"\",
          \"City\":      \"SUNNYVALE\",
          \"State\":     \"CA\",
          \"Zip\":       \"94085\",
          \"Country\":   \"US\"
       }
     ]",
    "{
       \"Image\": {
         \"Width\":  800,
         \"Height\": 600,
         \"Title\":  \"View from 15th Floor\",
         \"Thumbnail\": {
             \"Url\":    \"http://www.example.com/image/481989943\",
             \"Height\": 125,
             \"Width\":  \"100\"
         },
         \"IDs\": [116, 943, 234, 38793]
       }
     }",
    "{
       \"source\" : \"<a href=\\\"http://janetter.net/\\\" rel=\\\"nofollow\\\">Janetter</a>\",
       \"entities\" : {
           \"user_mentions\" : [ {
                   \"name\" : \"James Governor\",
                   \"screen_name\" : \"moankchips\",
                   \"indices\" : [ 0, 10 ],
                   \"id_str\" : \"61233\",
                   \"id\" : 61233
               } ],
           \"media\" : [ ],
           \"hashtags\" : [ ],
          \"urls\" : [ ]
       },
       \"in_reply_to_status_id_str\" : \"281400879465238529\",
       \"geo\" : {
       },
       \"id_str\" : \"281405942321532929\",
       \"in_reply_to_user_id\" : 61233,
       \"text\" : \"\@monkchips Ouch. Some regrets are harsher than others.\",
       \"id\" : 281405942321532929,
       \"in_reply_to_status_id\" : 281400879465238529,
       \"created_at\" : \"Wed Dec 19 14:29:39 +0000 2012\",
       \"in_reply_to_screen_name\" : \"monkchips\",
       \"in_reply_to_user_id_str\" : \"61233\",
       \"user\" : {
           \"name\" : \"Sarah Bourne\",
           \"screen_name\" : \"sarahebourne\",
           \"protected\" : false,
           \"id_str\" : \"16010789\",
           \"profile_image_url_https\" : \"https://si0.twimg.com/profile_images/638441870/Snapshot-of-sb_normal.jpg\",
           \"id\" : 16010789,
          \"verified\" : false
       }
     }"
    );
@inputs = (
    "[]"
    );

my $eslif = MarpaX::ESLIF->new($log);
isa_ok($eslif, 'MarpaX::ESLIF');

$log->info('Creating JSON grammar');
my $GRAMMAR = MarpaX::ESLIF::Grammar->new($eslif, $DATA);

foreach (0..$#inputs) {
    my $recognizerInterface = MyRecognizerInterface->new($inputs[$_]);
    my $marpaESLIFRecognizerJson = MarpaX::ESLIF::Recognizer->new($GRAMMAR, $recognizerInterface);
    if (! doparse($marpaESLIFRecognizerJson, $inputs[$_], 0)) {
        BAIL_OUT("Failure when parsing:\n$inputs[$_]\n");
    }
}

my $newFromOrshared = 0;
sub doparse {
    my ($marpaESLIFRecognizer, $inputs, $recursionLevel) = @_;
    my $rc;

    if (defined($inputs)) {
        $log->infof('[%d] Scanning JSON', $recursionLevel);
        $log->info ('-------------');
        $log->infof('%s', $inputs);
        $log->info ('-------------');
    } else {
        $log->infof("[%d] Scanning JSON's object", $recursionLevel);
    }
    my $ok = $marpaESLIFRecognizer->scan(1); # Initial events
    while ($ok && $marpaESLIFRecognizer->isCanContinue()) {
        #
        # Resume
        #
        $ok = $marpaESLIFRecognizer->resume();
    }
    if (! $ok) {
        BAIL_OUT("Failure when parsing:\n$inputs\n");
    }
    my $valueInterface = MyValueInterface->new();
    my $status = MarpaX::ESLIF::Value->new($marpaESLIFRecognizer, $valueInterface)->value();
    if (! $status) {
        BAIL_OUT("Failure when valuating:\n$inputs\n");
    }

    my $value = $valueInterface->getResult();

    $rc = 1;
    goto done;

  err:
    $rc = 0;

  done:
    return $rc;
}

done_testing();

__DATA__
#
# Default action is to propagate the first RHS value
#
:default ::= action => ::shift

                   #######################################################
                   # >>>>>>>>>>>>>>>> Strict JSON Grammar <<<<<<<<<<<<<<<<
                   #######################################################

# ----------------
# Start is a value
# ----------------
:start ::= value

# -------------------
# Composite separator
# -------------------
comma    ::= ','                                  action         => ::undef   # No-op anyway, override ::shift (default action)

# ----------
# JSON value
# ----------
value    ::= string                                                           # ::shift (default action)
           | number                                                           # ::shift (default action)
           | object                                                           # ::shift (default action)
           | array                                                            # ::shift (default action)
           | 'true'                               action         => ::lua->do_true      # Returns a true value
           | 'false'                              action         => ::lua->do_false     # Returns a false value
           | 'null'

# -----------
# JSON object
# -----------
object   ::= '{' inc members '}' dec              action         => ::copy[2] # Returns members
members  ::= pairs*                               action         => ::lua->do_members # Returns a table { pair[1] => pair[2] }
                                                  separator      => comma     # ... separated by comma
                                                  proper         => 1         # ... with no trailing separator
                                                  hide-separator => 1         # ... and hide separator in the action
                                                  
pairs    ::= string ':' value                     action         => ::lua->do_pairs  # Returns the table {string, value}

# -----------
# JSON Arrays
# -----------
array    ::= '[' inc elements ']' dec             action         => ::copy[2]   # Returns elements
elements ::= value*                               action => ::lua->do_elements  # returns elements packed in a table
                                                  separator      => comma       # ... separated by comma
                                                  proper         => 1           # ... with no trailing separator
                                                  hide-separator => 1           # ... and hide separator in the action
                                                  

# ------------
# JSON Numbers
# ------------
number ::= NUMBER                                 action => ::lua->do_number

NUMBER   ~ _INT
         | _INT _FRAC
         | _INT _EXP
         | _INT _FRAC _EXP
_INT     ~ _DIGIT
         | _DIGIT19 _DIGITS
         | '-' _DIGIT
         | '-' _DIGIT19 _DIGITS
_DIGIT   ~ [0-9]
_DIGIT19 ~ [1-9]
_FRAC    ~ '.' _DIGITS
_EXP     ~ _E _DIGITS
_DIGITS  ~ _DIGIT+
_E       ~ /e[+-]?/i

# -----------
# JSON String
# -----------
string     ::= '"' discardOff chars '"' discardOn action => ::copy[2]               # Only chars is of interest
discardOff ::=                                    action => ::undef                 # Nullable rule used to disable discard
discardOn  ::=                                    action => ::undef                 # Nullable rule used to enable discard

event :discard[on]  = nulled discardOn                                                           # Implementation of discard disabing using reserved ':discard[on]' keyword
event :discard[off] = nulled discardOff                                                          # Implementation of discard enabling using reserved ':discard[off]' keyword

chars   ::= filled                                                                               # ::shift (default action)
filled  ::= char+                                 action => ::concat                # Returns join('', char1, ..., charn)
chars   ::=                                       action => ::lua->do_empty_string            # Prefering empty string instead of undef
char    ::= [^"\\\x00-\x1F]                                                         # ::shift (default action) - take care PCRE2 [:cntrl:] includes DEL character
          | '\\' '"'                              action => ::copy[1]               # Returns double quote, already ok in data
          | '\\' '\\'                             action => ::copy[1]               # Returns backslash, already ok in data
          | '\\' '/'                              action => ::copy[1]               # Returns slash, already ok in data
          | '\\' 'b'                              action => ::u8"\x{08}"
          | '\\' 'f'                              action => ::u8"\x{0C}"
          | '\\' 'n'                              action => ::u8"\x{0A}"
          | '\\' 'r'                              action => ::u8"\x{0D}"
          | '\\' 't'                              action => ::u8"\x{09}"
          | /(?:\\u[[:xdigit:]]{4})+/             action => unicode


# -------------------------
# Unsignificant whitespaces
# -------------------------
:discard ::= /[\x{9}\x{A}\x{D}\x{20}]+/

# ------------------------------------------------------
# Needed for eventual depth extension - no op by default
# ------------------------------------------------------
inc ::=                                                        action => ::undef
dec ::=                                                        action => ::undef

                   #######################################################
                   # >>>>>>>>>>>>>>>>>> JSON Extensions <<<<<<<<<<<<<<<<<<
                   #######################################################

# --------------------------
# Unlimited commas extension
# --------------------------
# /* Unlimited commas */commas   ::= comma+

# --------------------------
# Perl comment extension
# --------------------------
# /* Perl comment */:discard ::= /(?:(?:#)(?:[^\n]*)(?:\n|\z))/u

# --------------------------
# C++ comment extension
# --------------------------
# /* C++ comment */:discard ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/

# --------------------------
# Max depth extension
# --------------------------
# /* max_depth */event inc[] = nulled inc                                                        # Increment depth
# /* max_depth */event dec[] = nulled dec                                                        # Decrement depth

# ----------------
# Number extension
# ----------------
#
# number ::= /\-?(?:(?:[1-9]?[0-9]+)|[0-9])(?:\.[0-9]+)?(?:[eE](?:[+-])?[0-9]+)?/ # /* bignum */action => number

# /* nan */number   ::= '-NaN':i                               action => nan
# /* nan */number   ::=  'NaN':i                               action => nan
# /* nan */number   ::= '+NaN':i                               action => nan
# /* inf */number   ::= '-Infinity':i                          action => negative_infinity
# /* inf */number   ::=  'Infinity':i                          action => positive_infinity
# /* inf */number   ::= '+Infinity':i                          action => positive_infinity
# /* inf */number   ::= '-Inf':i                               action => negative_infinity
# /* inf            ::=  'Inf':i                               action => positive_infinity
# /* inf */number   ::= '+Inf':i                               action => positive_infinity

# -----------------
# Control character
# -----------------
# /* cntrl */char      ::= /[\x00-\x1F]/                                                          # Because [:cntrl:] includes DEL (x7F)

<luascript>
  function do_true(context)
    print("< ::lua->do_true")
    local rc = true
    print("> ::lua->do_true returns: "..tostring(rc))
    return rc
  end

  function do_false(context)
    print("< ::lua->do_false")
    local rc = false
    print("> ::lua->do_false returns: "..tostring(rc))
    return rc
  end

  function do_members(context, ...)
    print("< ::lua->do_members")
    local args = table.pack(...)
    local i = 1
    local maxi = args.n
    while (i <= maxi)  do
      local arg = args[i]
      print(">> arg: "..tostring(arg))
      local key = arg[1]
      local value = arg[2]
      print(">> key: "..tostring(key)..", value: "..tostring(value))
      i = i + 1
    end
  end
     
  function do_empty_string(context)
    print("< ::lua->do_empty_string")
    local rc = ''
    print("> ::lua->do_empty_string returns: "..tostring(rc))
    return rc
  end

  function do_elements(context, ...)
    print("< ::lua->do_elements")
    local rc = table.pack(...)
    print("> ::lua->do_elements returns: "..tostring(rc))
    return rc
  end

  function do_number(context, number)
    print("< ::lua->do_number")
    local rc = tonumber(number)
    print("> ::lua->do_number returns: "..tostring(rc))
    return rc
  end

  function do_pairs(context, key, separator, value)
    print("< ::lua->do_pairs")
    local rc = {key, value}
    print("> ::lua->do_pairs returns: "..tostring(rc))
    return rc
  end
</luascript>
