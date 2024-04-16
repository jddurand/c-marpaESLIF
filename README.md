<div>
    <a href="https://badge.fury.io/gh/jddurand%2Fc-marpaESLIF"><img src="https://badge.fury.io/gh/jddurand%2Fc-marpaESLIF.svg" alt="GitHub version" height="18"></a> <a href="http://opensource.org/licenses/MIT" rel="nofollow noreferrer"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License MIT" height="18"></a> <a href="https://ci.appveyor.com/project/jddurand/c-marpaeslif"><img src="https://ci.appveyor.com/api/projects/status/github/jddurand/c-marpaESLIF?branch=master&svg=true&retina=true" alt="AppVeyor CI build status" height="18"></a> <a href="https://github.com/jddurand/c-marpaESLIF/actions/workflows/unix.yml"><img src="https://github.com/jddurand/c-marpaESLIF/actions/workflows/unix.yml/badge.svg?branch=master" alt="Github Unix workflow build status" height="18"></a> <a href="https://github.com/jddurand/c-marpaESLIF/actions/workflows/windows.yml"><img src="https://github.com/jddurand/c-marpaESLIF/actions/workflows/windows.yml/badge.svg?branch=master" alt="Github Windows workflow build status" height="18"></a><br><a href="https://www.patreon.com/marpa"><img src="https://edent.github.io/SuperTinyIcons/images/svg/patreon.svg" height="18" width="18" alt="Donate to Marpa via patreon.com" style="vertical-align:middle;border-color: transparent;">Donate to Marpa via patreon.com</a>
</div>

# NAME

marpaESLIF - Extended Marpa's Scanless Interface

# DESCRIPTION

```perl
/*
 * Example of a calulator with ESLIF BNF:
 *
 * Automatic discard of whitespaces
 * Correct association for expressions
 * Embedded action using anonymous lua functions
 *
*/
:discard ::= /[\s]+/

exp ::=
    /[\d]+/
    |    "("  exp ")"    assoc => group action => ::copy[1]
   || exp (- '**' -) exp assoc => right action => ::luac->function(x,y) return x^y end
   || exp (-  '*' -) exp                action => ::luac->function(x,y) return x*y end
    | exp (-  '/' -) exp                action => ::luac->function(x,y) return x/y end
   || exp (-  '+' -) exp                action => ::luac->function(x,y) return x+y end
    | exp (-  '-' -) exp                action => ::luac->function(x,y) return x-y end
```

marpaESLIF is a Scanless Interface expressed in a BNF format.
That is, it uses [marpaWrapper](https://github.com/jddurand/c-marpaWrapper), which itself is a thin interface on top of the
[libmarpa](https://jeffreykegler.github.io/Marpa-web-site/libmarpa.html) parser.

The [marpaESLIF BNF](https://github.com/jddurand/c-marpaESLIF/tree/master/doc/BNF) is inspired by [Marpa::R2's DSL](https://metacpan.org/pod/distribution/Marpa-R2/pod/Scanless/DSL.pod), though with some incompatible changes and add-ons:

- Native regular expression support, including regex callbacks into user-space and substitution
- Syntactic exception
- Unlimited number of sub-grammars
- Streaming compatible architecture
- Zero-length symbols
- Embedded lua language into the grammar
- Bindings to java, lua and perl languages
- JSON (strict and extended) grammars for decoding/encoding
- Declarative lua action directly into the grammar
- Parameterized rules
- Dynamic sub-grammars
- Lookahead

The following sections present the architecture of marpaESLIF architecture,
its features and things generally helpful to know.

# ARCHITECTURE

## Grammars

The ESLIF is nothing else but a _sparse array_ of grammars, identified by an index called _level_ and starting with value 0, or a description:

```
[------------------------------------------------------------------]
| Indice | Level 0 | N/A | Level 2 | Level 3 | N/A | Level 5 | ... |
| Name   | nameof0 | N/A | nameof2 | nameof3 | N/A | nameof5 | ... |
[------------------------------------------------------------------]
```

There **must** be a grammar at level index 0. Then any grammar can access any symbol of any other grammar:

```
[-------------------------------------------------------------------]
| Indice: | Level 0 | N/A | Level 2 | Level 3 | N/A | Level 5 | ... |
| Name  : | nameof0 | N/A | nameof2 | nameof3 | N/A | nameof5 | ... |
[-------------------------------------------------------------------]
| Symbol: | +>X             +>X       +>Xx                          |
| Symbol: | |  Y            |  Y      |  Yy                         |
| Symbol: | |  |            |  |      |  |             +>Zzz        |
| |  |            |  |      |  |             |   |        |         |
| |  |____________|  |______|  |_____________|   |        |         |
| |______________________________________________|        |         |
[-------------------------------------------------------------------]
```

Let S\[i\] be our notation for _the symbol S of grammar level i_.
Then the schema above says that Y\[0\] is a reference to X\[2\], that Y\[2\] is a reference to Xx\[3\], that Yy\[3\] is a reference to Zzz\[5\], and that Zzz\[5\] is a reference to Y\[0\]. Any symbol of any grammar that is accessed _via a reference_ is considered as part of a lexing phase, and the user will have no control until this phase is over, this symbol being recognized or not.

This is why it is strongly encouraged that there be a grammar at level 0 exist.
The author believes that it is common practice for the _top level_ grammar
to be at level 0. However is not enforced --
it is possible to specify a grammar
other than the one at level 0 as the starting grammar.

## Recognizer

### Top recognizer

Parsing from the point of an ESLIF user, consists of, for a given _location_ in the top-level grammar,

- assigning

    a set of symbols (these are called _alternatives_)

- commiting

    them (we say that we _complete_ the set of alternatives), and

- moving

    on to the next location.

We call this interface "scanless" because you do not have to write your own lexer.
The grammars define what is expected, and recognizer determines all the alternatives for you,
commits them, and moves on in the input stream.
We say input stream: this is another dimension (we suppose from now on that the top-level grammar is at level 0):

```
#
# Note
#
#    ::= is an alias for grammar level 0
#      ~ is an alias for grammar level 1
# :[n]:= is the generic form for grammar level n
#
        [---------------------------------------------------] STREAM MANAGEMENT
        | Rule is X ::= x y                                 |
        [---------------------------------------------------] STEP 0
        | Location is start of rule X[0]:                   |
        | X ::= . x y                                       |
        | Suppose that expected "terminals" are T1 and T2:  |
        [---------------------------------------------------] STEP 1
        | Try to match T1                                   |
        |   Nothing yet in the stream ?                     |<-----> Stream reader callback
        |   T1 may match but we are not sure                |<-----> Stream reader callback
        |   Repeat until T1 matches for sure or not         |
        [---------------------------------------------------] STEP 2
        | Try to match T2                                   |
        |   T2 may match but we are not sure                |<-----> Stream reader callback
        |   Repeat until T2 matches for sure or not         |
        [---------------------------------------------------]
        | No match ? End of scanning                        | STEP 3
        | Match ? Commit T1 and T2 and continue             |
        [---------------------------------------------------]
```

### Sub-recognizers

The stream management mentioned above applies to all grammars:
As soon as "terminal" is in reality a referenced symbol,
a sub-recognizer is instantiated and it shares the stream with its parent:

```
                TOP RECOGNIZER ON GRAMMAR LEVEL 0

      [---------------------------------------------------]           STREAM MANAGEMENT
      | Rule is X ::= x y                                 |
      [---------------------------------------------------] STEP 0.0
      | Location is start of rule X[0]:                   |
      | X ::= . x y                                       |
      | Suppose that expected "terminals" are T1 and T2:  |
      [---------------------------------------------------] STEP 0.1
      | Try to match T1                                   |
      |   Nothing yet in the stream ?                     |<-----> Stream reader callback
      |   T1 may match but we are not sure                |<-----> Stream reader callback
      |   Repeat until T1 matches for sure or not         |
      [---------------------------------------------------] STEP 0.2
      | Try to match T2                                   |
      |   T2 is a referenced symbol in grammar n          |
      [---------------------------------------------------]

                SUB-RECOGNIZER ON GRAMMAR LEVEL n

        [-------------------------------------------------]
        | Rule is T2 :[n]:= a b                           |
        [-------------------------------------------------] STEP 1.0
        | Location is start of rule T2[n]:                |
        | T2 :[n]:= . a b                                 |
        | Suppose that expected "terminals" are U1 and U2:|
        [-------------------------------------------------] STEP 1.1
        | Try to match U1                                 |
        |   Nothing yet in the stream ?                   |<-----> Stream reader callback
        |   U1 may match but we are not sure              |<-----> Stream reader callback
        |   Repeat until U1 matches for sure or not       |
        [-------------------------------------------------] STEP 1.2
        | Try to match U2                                 |
        |   U2 may match but we are not sure              |<-----> Stream reader callback
        |   Repeat until U2 matches for sure or not       |
        [-------------------------------------------------]
        | No match ? End of scanning for T2[n]            | STEP 1.3
        | Match ? Commit U1 and/or U2 and continue        |
        [-------------------------------------------------]
        | Do internal valuation                           | STEP 1.4
        [-------------------------------------------------]

             BACK TO TOP RECOGNIZER ON GRAMMAR LEVEL 0

      [---------------------------------------------------]
      | No match ? End of scanning                        | STEP 0.3
      | Match ? Commit T1 and/or T2 and continue          |
      | If T2 matches it is a parse tree value            |
      [---------------------------------------------------]
```

This is recursive: there will as many sub-recognizers instantiated as there are sub-grammars involved.
For instance, if terminal `U2` above is a referenced symbol at grammar level `l`,
a second sub-recognizer will be instantiated by the first sub-recognizer.
Every child recognizer shares all information about stream management.
The main difference between the top recognizer
and any child recognizer is that a child recognizer
is always doing an internal valuation to retreive the span in the input stream for,
and give that back to its parent.

The internal valuation is a forced mode that concatenates all matched bytes in the input stream.

### Discard and sub-recognizers

You might ask, "Why explicitly do an internal valuation?"
After all, the match is where sub-recognizer started and where it ended.
But an explicit internal valuation is needed because any grammar can have it own _discard_ mechanism.
This mean that what a sub-recognizer matched may be shorter
than the number of bytes effectively consumed from the input stream.

This raises the issue of the _discard_ mechanism.
A discard symbol is an ordinary symbol of a grammar, but with a special semantics.
In the BNF, the name of this special semantics is `:discard`.
For example:

```
:discard   ::= whitespace
whitespace   ~ /[\s]+/
```

This means that the grammar at level 0 tries to match the `whitespace` symbol
when it fails to match any of the expected terminals.

As soon as there is no match, and if `:discard` rule exist, any recognizer is always trying to get a match on it using a sub-recognizer, exactly like when it is executing a sub-recognizer for a terminal referencing a symbol in another grammar.
Furthermore nothing distinguishes the special symbol `:discard` from the others: it can also reference any symbol in any other sub-grammar. Though there is one major difference between discard sub-recognizers and terminal sub-recognizers: a discard sub-recognizer will never instantiate another discard sub-sub-recognizer.
This means that in the following:

```
:discard    ::= whitespace
:discard      ~ somethingelse
whitespace    ~ /[\s]+/
somethingelse ~ 'a string'
```

if a discard tentative is instantiated on grammar at level 0 using the symbol `whitespace` of level 1, and if `whitespace` of level 1 does not match, there will be no tentative for try to discard in level 1, even it is has a `:discard` rule that is defined to be `somethingelse`.

# STREAMING, CHARACTER AND BINARY MODES

Everytime any recognizer need more data, a callback to userspace is triggered. It is legal to not give the encoding when it is a character stream, then the engine will guess (the user should give enough data so that the guess is correct, though).

Internally, all chunks of characters are converted to UTF-8. This guarantees three things:

- Validation of well-formed characters
- uniform internal processing
- native compatibility with the regular expression engine

A recognizer always owns a stream, the later is shared in two cases:

- Lexeme search

    A sub-recognizer is started, and it shares the stream with its parent. Nevertheless parent stream is guaranteed to never crunch any data until this sub-recognizer finishes. At most, new data may be appended. When this sub-recognizer finishes, it updates the parent position in the stream if the lexeme it was looking for is found. The end-user never has control on such sub-recognizer.

- Shared top-level recognizer

    The end-user can create a new top-level recognizer that shares the stream with another top-level recognizer. Then, it is guaranteed that everytime one of them updates its stream, the other's stream changes the same way.

# TERMINALS AND REGULAR EXPRESSIONS

As mentionned above, regular expression are totally handled using [PCRE2](http://www.pcre.org/). Therefore the syntax of regular expression is the PCRE2 syntax. It is obvious that a regular expression define an internal "terminal", and there are three ways to define such a terminal, all of them being converted to a regular expression:

- String
- Character class
- Regular expression

Each of these three terminal types support eventual modifiers. The most central modifier is the need or not of having the notion of "valid characters", especially outside of the ASCII range. This is called the PCRE2\_UTF flag, and is mentionned thoroughly in the next sections.

- String

    A string is delimited expression in the grammar, where allowed start/and delimiters are `''` and `""`. When a string is recognized in the grammar, escaping is allowed using the backslash `\` character, and only the start delimited or backslash itself can be escaped. Absolutely any other character is taken `as is`, eventually internally escaped by marpaESLIF to remove its signification in PCRE2, when there is one. For example:

    - 'Example'

        is translated to the UTF-8 pattern `Example`

    - '{Example}'

        is translated to the UTF-8 pattern `\{Example`

    - "{Example}"

        is translated to the UTF-8 pattern `\{Example`

    - '{Example\[\]\\}'

        will trigger an error because only `'` or `\` itself can be backslashed.

    - '{Example\[\]\\\\}'

        is translated to the UTF-8 pattern `\{Example\[]\\}`

    - 'Black Heart Suite Character: ♥'

        is translated to the UTF-8 pattern `Black Heart Suite Character: \x{2665}`

    A string is always scanned character per character by marpaESLIF, and an ASCII compatible pattern is generated, using \\x{...} codepoint notation whenever this is an ASCII special character or a character outside of original ASCII 7-bits character set. So marpaESLIF know if there is need for unicode support or not in PCRE2 terminology (which is: any code point greater than 255, else byte matching is enough). This is important because PCRE2 yells if a pattern is using a large codepoint and if this internal PCRE2\_UTF flag is not set accordingly.

    The presence of this flag has an important consequence: if at least one string in the grammar implied the PCRE2\_UTF flag, then the whole remaining chunk of data is translated and validated as an UTF-8 sequence of bytes. In such a case, either the user input reader informed that this is stream of characters, then marpaESLIF prepared in advance the conversion/validation to UTF-8, either this is done lazily as soon as a match is attempted using a string requiring the PCRE2\_UTF flag.

- String modifiers

    String modifiers must be appended directly after the end delimiter of the string. They are restricted to `:i`, meaning that the match is caseless sensitive:

    - 'Black Heart Suite Character: ♥':i

        A dump of it in terms of PCRE2 (c.f. the API specification for dump facility) would show the `PCRE2_CASELESS` flag:

        ```
        #      Pattern: Black Heart Suite Character: \x{2665}
        #        Flags: PCRE2_ANCHORED|PCRE2_CASELESS|PCRE2_UTF
        ```

        You notice the presence of:

        - `PCRE2_ANCHORED`

            Strings are always anchored at the point where match is attempted.

        - `PCRE2_UTF`

            This flag is automatically set when the scanning of the string that is in the grammar, done internally by marpaESLIF, reveal the need for it.

    - 'Example':i

        would give the following dump:

        ```
        #      Pattern: Example
        #        Flags: PCRE2_ANCHORED|PCRE2_CASELESS
        ```

- Character class

    A character class is very closed to a regular expression (see later), except that it looks like a string, with start/end delimiters being `[]`, and that the pattern is NOT scanned. marpaESLIF will lets PCRE2 eventually yell if there is a use of codepoints and if the internal PCRE2\_UTF flag is not set.

    marpaESLIF will try to guess the need for PCRE2\_UTF flag by scanning the UTF-8 bytes composing the character class, but will do _no modification_. For example:

    - \[a-z\]

        will be dumped as:

        ```
        #      Pattern:
        #     0x000000: 5b 61 2d 7a 5d                                  [a-z]
        #        Flags: PCRE2_ANCHORED
        ```

    - \[a-z♥\]

        is dumped as:

        ```
        #      Pattern:
        #     0x000000: 5b 61 2d 7a e2 99 a5 5d                         [a-z...]
        #        Flags: PCRE2_ANCHORED|PCRE2_UTF
        ```

        You notice that the sequence `e299a5` that is the UTF-8 representation of the Black Heart Suite Character. marpaESLIF detected it `as an explicit character`, so it was able to put the PCRE2\_UTF flag automatically. But this will not work if you are using codepoints:

    - \[a-z\\x{2665}\]

        will yield automatically the following error, and this will come from the PCRE2 engine itself:

        ```
        /[a-z\x{2665}]/: pcre2_compile failure at offset 11: character code point value in \x{} or \o{} is too large.
        ```

        So there is a need for a modifier. Please see the section on "Character class and Regular expression modifiers". For instance, here, one would say:

    - \[a-z\\x{2665}\]:u

        leaving to the following dump:

        ```
        #     0x000000: 5b 61 2d 7a 5c 78 7b 32 36 36 35 7d 5d          [a-z\x{2665}]
        #        Flags: PCRE2_ANCHORED|PCRE2_UTF
        ```

- Regular expression

    Nothing really distinguished regular expression and character classes in the grammar, except that _sequence modifiers_ can be embedded directly in a regular expression, so that they are managed by PCRE2 instead of marpaESLIF, i.e:

    - /\[a-z\]/

        is stricly equivalent to the character `[a-z]`.

    - /\[a-z\]+/

        really mean that the sequence is embedded in the regular expression. The dump of the later would say:

        ```
        #      Pattern:
        #     0x000000: 5b 61 2d 7a 5d 2b                               [a-z]+
        ```

    In conclusion determining the need of the PCRE2\_UTF will always be exact: either marpaESLIF will detect it correctly, either PCRE2 will yell, and you will have to explicitely set it using modifiers. Since character class is nothing else but a regular expression limited to a range of character, they both share the same possible set of modifiers.

- Character class and Regular expression modifiers

    The only difference between the twos is how modifiers are expressed: for a character class they must be preceeded by the `:` character, while for a regular expression they can be set directly after the `/` end delimiter (as in the Perl language).

    The explicit regular expression, being sent directly as-is to PCRE2, support de-facto all of the native PCRE2 pattern language, i.e. one can set regular expression options that have no single option equivalent when using a regular expression, for example:

    - /(\*LIMIT\_MATCH=15)\[a-z\]+/

        is setting an internal PCRE2 match limit to 15. The dump does not show that as an explicit flag:

        ```
        #      Pattern:
        #     0x000000: 28 2a 4c 49 4d 49 54 5f 4d 41 54 43 48 3d 31 35 (*LIMIT_MATCH=15
        #     0x000010: 29 5b 61 2d 7a 5d 2b                            )[a-z]+
        #        Flags: PCRE2_ANCHORED
        ```

    It is highly recommended to read the [pcre2pattern](http://www.pcre.org/current/doc/html/pcre2pattern.html) documentation to know all the possible settings that can be _embedded_ into the regular expression. Explicit modifiers are insipired by the [jpcre2](https://github.com/jpcre2/jpcre2) and [Perl language](http://www.perl.org) (most of the descriptions below are copy/pasted from jpcre2). Please refer to [marpaESLIF BNF](https://github.com/jddurand/c-marpaESLIF/tree/master/doc/BNF) for the exhaustive list of modifiers.

# LOGGING

All the logging is done via [genericLogger](https://github.com/jddurand/c-genericLogger), and if the later is not set, nothing is logged.

# GRAMMAR

Please refer to [marpaESLIF BNF](https://github.com/jddurand/c-marpaESLIF/tree/master/doc/BNF) for the ESLIF BNF expressed with itself. Fundamentals of the ESLIF grammar are the followings (incompatible change v.s. [Marpa::R2's DSL](https://metacpan.org/pod/distribution/Marpa-R2/pod/Scanless/DSL.pod) are highlighted):

- Grammar levels

    The number of levels is only limited by memory for your program -; Any symbol that have an impact on grammar at level `n` must be defined with such level explicitely:

    - `::=` is an alias for level 0
    - `~` is an alias for level 1
    - `:[n]:=` is the general form for level n

    As a consequence the definition of a `:discard` symbol is incompatible with [Marpa::R2's DSL](https://metacpan.org/pod/distribution/Marpa-R2/pod/Scanless/DSL.pod), in which a discard rule affecting level 0 have the alias `~`, for ESLIF it is `::=`.

- built-in actions and adverb lists

    Any [Marpa::R2](https://metacpan.org/pod/distribution/Marpa-R2/pod/Scanless/DSL.pod)'s action or adverb list that require the Perl langage has been removed, for example `::array`, `::bless`

- LATM is true by default

    LATM (Longest Acceptable Token Match) is preventing the scanner to push alternatives of lower length than the longest of the alternatives.

- pausing is allowed with discard events
- `:default` statement is unique per level

    ... instead of being lexically scoped with [Marpa::R2's DSL](https://metacpan.org/pod/distribution/Marpa-R2/pod/Scanless/DSL.pod).

- syntactic exception is supported

    ... and it is managed at _valuation_ phase.

- native regular expression support (via [PCRE2](http://www.pcre.org/))
- comments are extended to `C++`/`C` styles

# VERSIONING

marpaESLIF follows the [Semantic Versioning 2.0.0](https://semver.org/spec/v2.0.0.html), i:e:

- MAJOR for incompatible API changes
- MINOR for added functionality in a backwards-compatible manner
- PATCH for backwards-compatible bug fixes

# BUILD

This project is [CMake](http://cmake.org) based, version 3.26 at least. Recommended pattern is:

```
cmake -Wno-dev -S c-marpaESLIF -B c-marpaESLIF-build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build c-marpaESLIF-build
ctest --test-dir c-marpaESLIF-build
# If you want the package (recommended on Windows)
cmake --build c-marpaESLIF-build --target package
# If you want to install (not recommended on Windows)
cmake --install c-marpaESLIF-build
```

If you use valgrind, the recommended `cmake` arguments are:

```
cmake -Wno-dev -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTCONV_USE_ICU=NO -DPCRE2_SUPPORT_VALGRIND=1 .
```

# SEE ALSO

[marpaESLIF\_BNF](https://github.com/jddurand/c-marpaESLIF/tree/master/doc/BNF), [marpaESLIF\_BINDINGS](https://github.com/jddurand/c-marpaESLIF/tree/master/src/bindings), [marpaESLIF\_API](https://github.com/jddurand/c-marpaESLIF/tree/master/doc/API), [marpaESLIF\_Tutorial](https://github.com/jddurand/c-marpaESLIF/tree/master/doc/Tutorial), [genericLogger](https://github.com/jddurand/c-genericLogger), [PCRE2](http://www.pcre.org/), [jpcre2](https://github.com/jpcre2/jpcre2), [Perl language](http://www.perl.org), [Java language](http://www.java.com), [Lua language](https://www.lua.org).
