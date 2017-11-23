/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    cloak.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef GENERICHASH_CLOAK_GUARD_H
#define GENERICHASH_CLOAK_GUARD_H

#define GENERICHASH_CAT(a, ...) GENERICHASH_PRIMITIVE_CAT(a, __VA_ARGS__)
#define GENERICHASH_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define GENERICHASH_COMPL(b) GENERICHASH_PRIMITIVE_CAT(GENERICHASH_COMPL_, b)
#define GENERICHASH_COMPL_0 1
#define GENERICHASH_COMPL_1 0

#define GENERICHASH_BITAND(x) GENERICHASH_PRIMITIVE_CAT(GENERICHASH_BITAND_, x)
#define GENERICHASH_BITAND_0(y) 0
#define GENERICHASH_BITAND_1(y) y

#define GENERICHASH_INC(x) GENERICHASH_PRIMITIVE_CAT(INC_, x)
#define GENERICHASH_INC_0 1
#define GENERICHASH_INC_1 2
#define GENERICHASH_INC_2 3
#define GENERICHASH_INC_3 4
#define GENERICHASH_INC_4 5
#define GENERICHASH_INC_5 6
#define GENERICHASH_INC_6 7
#define GENERICHASH_INC_7 8
#define GENERICHASH_INC_8 9
#define GENERICHASH_INC_9 9

#define GENERICHASH_DEC(x) GENERICHASH_PRIMITIVE_CAT(GENERICHASH_DEC_, x)
#define GENERICHASH_DEC_0 0
#define GENERICHASH_DEC_1 0
#define GENERICHASH_DEC_2 1
#define GENERICHASH_DEC_3 2
#define GENERICHASH_DEC_4 3
#define GENERICHASH_DEC_5 4
#define GENERICHASH_DEC_6 5
#define GENERICHASH_DEC_7 6
#define GENERICHASH_DEC_8 7
#define GENERICHASH_DEC_9 8

#define GENERICHASH_CHECK_N_MSVC_BUG(x, n, ...) n
#define GENERICHASH_CHECK_N(args) GENERICHASH_CHECK_N_MSVC_BUG args
#define GENERICHASH_CHECK(...) GENERICHASH_EVAL(GENERICHASH_CHECK_N((__VA_ARGS__, 0,)))
#define GENERICHASH_PROBE(x) x, 1,

#define GENERICHASH_IS_PAREN(x) GENERICHASH_CHECK(GENERICHASH_IS_PAREN_PROBE x)
#define GENERICHASH_IS_PAREN_PROBE(...) GENERICHASH_PROBE(~)

#define GENERICHASH_NOT(x) GENERICHASH_CHECK(GENERICHASH_PRIMITIVE_CAT(GENERICHASH_NOT_, x))
#define GENERICHASH_NOT_0 GENERICHASH_PROBE(~)

#define GENERICHASH_COMPL(b) GENERICHASH_PRIMITIVE_CAT(GENERICHASH_COMPL_, b)
#define GENERICHASH_COMPL_0 1
#define GENERICHASH_COMPL_1 0

#define GENERICHASH_BOOL(x) GENERICHASH_COMPL(GENERICHASH_NOT(x))

#define GENERICHASH_IIF(c) GENERICHASH_PRIMITIVE_CAT(GENERICHASH_IIF_, c)
#define GENERICHASH_IIF_0(t, ...) __VA_ARGS__
#define GENERICHASH_IIF_1(t, ...) t

#define GENERICHASH_IF(c) GENERICHASH_IIF(GENERICHASH_BOOL(c))

#define GENERICHASH_EAT(...)
#define GENERICHASH_EXPAND(...) __VA_ARGS__
#define GENERICHASH_WHEN(c) GENERICHASH_IF(c)(GENERICHASH_EXPAND, GENERICHASH_EAT)

#define GENERICHASH_EMPTY()
#define GENERICHASH_DEFER(id) id GENERICHASH_EMPTY()
#define GENERICHASH_OBSTRUCT(id) id GENERICHASH_DEFER(GENERICHASH_EMPTY)()

#define GENERICHASH_EVAL(...)  GENERICHASH_EVAL1(GENERICHASH_EVAL1(GENERICHASH_EVAL1(__VA_ARGS__)))
#define GENERICHASH_EVAL1(...) GENERICHASH_EVAL2(GENERICHASH_EVAL2(GENERICHASH_EVAL2(__VA_ARGS__)))
#define GENERICHASH_EVAL2(...) GENERICHASH_EVAL3(GENERICHASH_EVAL3(GENERICHASH_EVAL3(__VA_ARGS__)))
#define GENERICHASH_EVAL3(...) GENERICHASH_EVAL4(GENERICHASH_EVAL4(GENERICHASH_EVAL4(__VA_ARGS__)))
#define GENERICHASH_EVAL4(...) GENERICHASH_EVAL5(GENERICHASH_EVAL5(GENERICHASH_EVAL5(__VA_ARGS__)))
#define GENERICHASH_EVAL5(...) __VA_ARGS__

#define GENERICHASH_REPEAT(count, macro, ...) \
    GENERICHASH_WHEN(count) \
    ( \
        GENERICHASH_OBSTRUCT(GENERICHASH_REPEAT_INDIRECT) () \
        ( \
            GENERICHASH_DEC(count), macro, __VA_ARGS__ \
        ) \
        GENERICHASH_OBSTRUCT(macro) \
        ( \
            GENERICHASH_DEC(count), __VA_ARGS__ \
        ) \
    )
#define GENERICHASH_REPEAT_INDIRECT() GENERICHASH_REPEAT

#define GENERICHASH_WHILE(pred, op, ...) \
    IF(pred(__VA_ARGS__)) \
    ( \
        GENERICHASH_OBSTRUCT(GENERICHASH_WHILE_INDIRECT) () \
        ( \
            pred, op, op(__VA_ARGS__) \
        ), \
        __VA_ARGS__ \
    )
#define GENERICHASH_WHILE_INDIRECT() GENERICHASH_WHILE

#define GENERICHASH_PRIMITIVE_COMPARE(x, y) GENERICHASH_IS_PAREN \
( \
    GENERICHASH_COMPARE_ ## x ( GENERICHASH_COMPARE_ ## y) (())  \
)

#define GENERICHASH_IS_COMPARABLE(x) GENERICHASH_IS_PAREN( GENERICHASH_CAT(GENERICHASH_COMPARE_, x) (()) )

#define GENERICHASH_NOT_EQUAL(x, y) \
GENERICHASH_IIF(GENERICHASH_BITAND(GENERICHASH_IS_COMPARABLE(x))(GENERICHASH_IS_COMPARABLE(y)) ) \
( \
   GENERICHASH_PRIMITIVE_COMPARE, \
   1 GENERICHASH_EAT \
)(x, y)

#define GENERICHASH_EQUAL(x, y) GENERICHASH_COMPL(GENERICHASH_NOT_EQUAL(x, y))

#define GENERICHASH_COMMA() ,

#define GENERICHASH_COMMA_IF(n) GENERICHASH_IF(n)(GENERICHASH_COMMA, GENERICHASH_EAT)()

#endif /* GENERICHASH_CLOAK_GUARD_H */