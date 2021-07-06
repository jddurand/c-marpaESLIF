#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);

#define UTF_8_STRING "UTF-8"
const static char *yamls = "\n"
  "<_C_PRINTABLE>                 ~ [\\x{9}] | [\\x{A}] | [\\x{D}] | [\\x{20}-\\x{7E}]          /* 8 bit */\n"
  "                             | [\\x{85}] | [\\x{A0}-\\x{D7FF}]:u | [\\x{E000}-\\x{FFFD}]:u /* 16 bit */\n"
  "                             | [\\x{10000}-\\x{10FFFF}]:u                                  /* 32 bit */\n"
  "<_NB_JSON>                     ~ [\\x{9}] | [\\x{20}-\\x{10FFFF}]:u\n"
  "<_C_BYTE_ORDER_MARK>           ~ [\\x{FEFF}]:u\n"
  "<_C_SEQUENCE_ENTRY>            ~ “-”\n"
  "<_C_MAPPING_KEY>               ~ “?”\n"
  "<_C_MAPPING_VALUE>             ~ “:”\n"
  "<_C_COLLECT_ENTRY>             ~ “,”\n"
  "<_C_SEQUENCE_START>            ~ “[”\n"
  "<_C_SEQUENCE_END>              ~ “]”\n"
  "<_C_MAPPING_START>             ~ “{”\n"
  "<_C_MAPPING_END>               ~ “}”\n"
  "<_C_FLOW_INDICATOR>            ~ “,” | “[” | “]” | “{” | “}”\n"
  "<_C_COMMENT>                   ~ “#”\n"
  "<_C_ANCHOR>                    ~ “&”\n"
  "<_C_ALIAS>                     ~ “*”\n"
  "<_C_TAG>                       ~ “!”\n"
  "<_C_LITERAL>                   ~ “|”\n"
  "<_C_FOLDED>                    ~ “>”\n"
  "<_C_SINGLE_QUOTE>              ~ “'”\n"
  "<_C_DOUBLE_QUOTE>              ~ “\"”\n"
  "<_C_DIRECTIVE>                 ~ “%”\n"
  "<_C_RESERVED>                  ~ “@” | “`”\n"
  "<_C_INDICATOR>                 ~ “-” | “?” | “:” | “,” | “[” | “]” | “{” | “}”\n"
  "                             | “#” | “&” | “*” | “!” | “|” | “>” | “'” | “\"”\n"
  "                             | “%” | “@” | “`”\n"
  "<_NB_CHAR>                     ~ <_C_PRINTABLE>\n"
  "<_B_LINE_FEED>                 ~ [\\x{A}]   /* LF */\n"
  "<_B_CARRIAGE_RETURN>           ~ [\\x{D}]   /* CR */\n"
  "<_B_BREAK>                     ~ <_B_CARRIAGE_RETURN> <_B_LINE_FEED> /* DOS, Windows */\n"
  "                             | <_B_CARRIAGE_RETURN>                /* MacOS upto 9.x */\n"
  "                             | <_B_LINE_FEED>                      /* UNIX, MacOS X */ \n"
  "<_B_CHAR>                      ~ <_B_LINE_FEED> | <_B_CARRIAGE_RETURN>\n"
  "<_S_SPACE>                     ~ [\\x{20}] /* SP */\n"
  "<_S_TAB>                       ~ [\\x{9}]  /* TAB */\n"
  "<_S_WHITE>                     ~ <_S_SPACE> | <_S_TAB>\n"
  "<_NS_DEC_DIGIT>                ~ [\\x{30}-\\x{39}] /* 0-9 */\n"
  "<_NS_HEX_DIGIT>                ~ <_NS_DEC_DIGIT>\n"
  "                             | [\\x{41}-\\x{46}] /* A-F */ | [\\x{61}-\\x{66}] /* a-f */\n"
  "<_NS_ASCII_LETTER>             ~ [\\x{41}-\\x{5A}] /* A-Z */ | [\\x{61}-\\x{7A}] /* a-z */\n"
  "<_NS_WORD_CHAR>                ~ <_NS_DEC_DIGIT> | <_NS_ASCII_LETTER> | “-”\n"
  "<_NS_URI_CHAR>                 ~ “%” <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> | <_NS_WORD_CHAR> | “#”\n"
  "                             | “;” | “/” | “?” | “:” | “@” | “&” | “=” | “+” | “$” | “,”\n"
  "                             | “_” | “.” | “!” | “~” | “*” | “'” | “(” | “)” | “[” | “]”\n"
  "<_C_ESCAPE>                    ~ “\\\\”\n"
  "<_NS_ESC_NULL>                 ~ “0”\n"
  "<_NS_ESC_BELL>                 ~ “a”\n"
  "<_NS_ESC_BACKSPACE>            ~ “b”\n"
  "<_NS_ESC_HORIZONTAL_TAB>       ~ “t” | [\\x{9}]\n"
  "<_NS_ESC_LINE_FEED>            ~ “n”\n"
  "<_NS_ESC_VERTICAL_TAB>         ~ “v”\n"
  "<_NS_ESC_FORM_FEED>            ~ “f”\n"
  "<_NS_ESC_CARRIAGE_RETURN>      ~ “r”\n"
  "<_NS_ESC_ESCAPE>               ~ “e”\n"
  "<_NS_ESC_SPACE>                ~ [\\x{20}]\n"
  "<_NS_ESC_DOUBLE_QUOTE>         ~ “\"”\n"
  "<_NS_ESC_SLASH>                ~ “/”\n"
  "<_NS_ESC_BACKSLASH>            ~ “\\\\”\n"
  "<_NS_ESC_NEXT_LINE>            ~ “N”\n"
  "<_NS_ESC_NON_BREAKING_SPACE>   ~ “_”\n"
  "<_NS_ESC_LINE_SEPARATOR>       ~ “L”\n"
  "<_NS_ESC_PARAGRAPH_SEPARATOR>  ~ “P”\n"
  "<_NS_ESC_8_BIT>                ~ “x” <_NS_HEX_DIGIT> <_NS_HEX_DIGIT>\n"
  "<_NS_ESC_16_BIT>               ~ “x” <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT>\n"
  "<_NS_ESC_32_BIT>               ~ “x” <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT> <_NS_HEX_DIGIT>\n"
  "<_C_NS_ESC_CHAR>               ~ “\\\\” ( <_NS_ESC_NULL> | <_NS_ESC_BELL> | <_NS_ESC_BACKSPACE>\n"
  "                                        | <_NS_ESC_HORIZONTAL_TAB> | <_NS_ESC_LINE_FEED>\n"
  "                                        | <_NS_ESC_VERTICAL_TAB> | <_NS_ESC_FORM_FEED>\n"
  "                                        | <_NS_ESC_CARRIAGE_RETURN> | <_NS_ESC_ESCAPE> | <_NS_ESC_SPACE>\n"
  "                                        | <_NS_ESC_DOUBLE_QUOTE> | <_NS_ESC_SLASH> | <_NS_ESC_BACKSLASH>\n"
  "                                        | <_NS_ESC_NEXT_LINE> | <_NS_ESC_NON_BREAKING_SPACE>\n"
  "                                        | <_NS_ESC_LINE_SEPARATOR> | <_NS_ESC_PARAGRAPH_SEPARATOR>\n"
  "                                        | <_NS_ESC_8_BIT> | <_NS_ESC_16_BIT> | <_NS_ESC_32_BIT> )\n"
  "<_C_QUOTED_QUOTE>              ~ “'” “'”\n"
  "\n"
  "<C_PRINTABLE>                  ~ <_C_PRINTABLE>\n"
  "<NB_JSON>                      ~ <_NB_JSON>\n"
  "<C_BYTE_ORDER_MARK>            ~ <_C_BYTE_ORDER_MARK>\n"
  "<C_SEQUENCE_ENTRY>             ~ <_C_SEQUENCE_ENTRY>\n"
  "<C_MAPPING_KEY>                ~ <_C_MAPPING_KEY>\n"
  "<C_MAPPING_VALUE>              ~ <_C_MAPPING_VALUE>\n"
  "<C_COLLECT_ENTRY>              ~ <_C_COLLECT_ENTRY>\n"
  "<C_SEQUENCE_START>             ~ <_C_SEQUENCE_START>\n"
  "<C_SEQUENCE_END>               ~ <_C_SEQUENCE_END>\n"
  "<C_MAPPING_START>              ~ <_C_MAPPING_START>\n"
  "<C_MAPPING_END>                ~ <_C_MAPPING_END>\n"
  "<C_COMMENT>                    ~ <_C_COMMENT>\n"
  "<C_ANCHOR>                     ~ <_C_ANCHOR>\n"
  "<C_ALIAS>                      ~ <_C_ALIAS>\n"
  "<C_TAG>                        ~ <_C_TAG>\n"
  "<C_LITERAL>                    ~ <_C_LITERAL>\n"
  "<C_FOLDED>                     ~ <_C_FOLDED>\n"
  "<C_SINGLE_QUOTE>               ~ <_C_SINGLE_QUOTE>\n"
  "<C_DOUBLE_QUOTE>               ~ <_C_DOUBLE_QUOTE>\n"
  "<C_DIRECTIVE>                  ~ <_C_DIRECTIVE>\n"
  "<C_RESERVED>                   ~ <_C_RESERVED>\n"
  "<C_INDICATOR>                  ~ <_C_INDICATOR>\n"
  "<C_FLOW_INDICATOR>             ~ <_C_FLOW_INDICATOR>\n"
  "<NB_CHAR>                      ~ <_NB_CHAR>\n"
  "<NB_CHAR_EXCEPTION>            ~ <_B_CHAR> | <_C_BYTE_ORDER_MARK>\n"
  "<B_LINE_FEED>                  ~ <_B_LINE_FEED>\n"
  "<B_CARRIAGE_RETURN>            ~ <_B_CARRIAGE_RETURN>\n"
  "<B_BREAK>                      ~ <_B_BREAK>\n"
  "<B_CHAR>                       ~ <_B_CHAR>\n"
  "<S_SPACE>                      ~ <_S_SPACE>\n"
  "<S_TAB>                        ~ <_S_TAB>\n"
  "<S_WHITE>                      ~ <_S_WHITE>\n"
  "<NS_CHAR>                      ~ <_NB_CHAR>\n"
  "<NS_CHAR_EXCEPTION>            ~ <_S_WHITE>\n"
  "<NS_ANCHOR_CHAR>               ~ <_NB_CHAR>\n"
  "<NS_ANCHOR_CHAR_EXCEPTION>     ~ <_C_FLOW_INDICATOR>\n"
  "<NS_DEC_DIGIT>                 ~ <_NS_DEC_DIGIT>\n"
  "<NS_HEX_DIGIT>                 ~ <_NS_HEX_DIGIT>\n"
  "<NS_ASCII_LETTER>              ~ <_NS_ASCII_LETTER>\n"
  "<NS_WORD_CHAR>                 ~ <_NS_WORD_CHAR>\n"
  "<NS_URI_CHAR>                  ~ <_NS_URI_CHAR>\n"
  "<NS_TAG_CHAR>                  ~ <_NS_URI_CHAR>\n"
  "<NS_TAG_CHAR_EXCEPTION>        ~ “!” | <_C_FLOW_INDICATOR>\n"
  "<C_ESCAPE>                     ~ <_C_ESCAPE>\n"
  "<NS_ESC_NULL>                  ~ <_NS_ESC_NULL>\n"
  "<NS_ESC_BELL>                  ~ <_NS_ESC_BELL>\n"
  "<NS_ESC_BACKSPACE>             ~ <_NS_ESC_BACKSPACE>\n"
  "<NS_ESC_HORIZONTAL_TAB>        ~ <_NS_ESC_HORIZONTAL_TAB>\n"
  "<NS_ESC_LINE_FEED>             ~ <_NS_ESC_LINE_FEED>\n"
  "<NS_ESC_VERTICAL_TAB>          ~ <_NS_ESC_VERTICAL_TAB>\n"
  "<NS_ESC_FORM_FEED>             ~ <_NS_ESC_FORM_FEED>\n"
  "<NS_ESC_CARRIAGE_RETURN>       ~ <_NS_ESC_CARRIAGE_RETURN>\n"
  "<NS_ESC_ESCAPE>                ~ <_NS_ESC_ESCAPE>\n"
  "<NS_ESC_SPACE>                 ~ <_NS_ESC_SPACE>\n"
  "<NS_ESC_DOUBLE_QUOTE>          ~ <_NS_ESC_DOUBLE_QUOTE>\n"
  "<NS_ESC_SLASH>                 ~ <_NS_ESC_SLASH>\n"
  "<NS_ESC_BACKSLASH>             ~ <_NS_ESC_BACKSLASH>\n"
  "<NS_ESC_NEXT_LINE>             ~ <_NS_ESC_NEXT_LINE>\n"
  "<NS_ESC_NON_BREAKING_SPACE>    ~ <_NS_ESC_NON_BREAKING_SPACE>\n"
  "<NS_ESC_LINE_SEPARATOR>        ~ <_NS_ESC_LINE_SEPARATOR>\n"
  "<NS_ESC_PARAGRAPH_SEPARATOR> ~ <_NS_ESC_PARAGRAPH_SEPARATOR>\n"
  "<NS_ESC_8_BIT>                 ~ <_NS_ESC_8_BIT>\n"
  "<NS_ESC_16_BIT>                ~ <_NS_ESC_16_BIT>\n"
  "<NS_ESC_32_BIT>                ~ <_NS_ESC_32_BIT>\n"
  "<C_NS_ESC_CHAR>                ~ <_C_NS_ESC_CHAR>\n"
  "<NB_DOUBLE_CHAR>               ~ <_NB_JSON>\n"
  "<NB_DOUBLE_CHAR_EXCEPTION>     ~ “\\\\” | “\"”\n"
  "<NS_DOUBLE_CHAR>               ~ <_NB_JSON>\n"
  "<NS_DOUBLE_CHAR_EXCEPTION>     ~ “\\\\” | “\"” | <_S_WHITE>\n"
  "<C_QUOTED_QUOTE>               ~ <_C_QUOTED_QUOTE>\n"
  "<NB_SINGLE_CHAR>               ~ <_NB_JSON>\n"
  "<NB_SINGLE_CHAR_EXCEPTION>     ~ “'”\n"
  "<NS_SINGLE_CHAR>               ~ <_NB_JSON>\n"
  "<NS_SINGLE_CHAR_EXCEPTION>     ~ “'” | <_S_WHITE>\n"
  "<NS_PLAIN_FIRST>               ~ <_NB_CHAR> /* Because <NS_CHAR> is <_NB_CHAR> */\n"
  "<NS_PLAIN_FIRST_EXCEPTION>     ~ <_C_INDICATOR>\n"
  "<NS_PLAIN_SAFE_IN>             ~  <_NB_CHAR> /* Because <NS_CHAR> is <_NB_CHAR> */\n"
  "<NS_PLAIN_SAFE_IN_EXCEPTION>   ~ <_C_INDICATOR>\n"
  "<NS_PLAIN_SAFE_CHAR>           ~  <_NB_CHAR> /* Because <NS_CHAR> is <_NB_CHAR> */\n"
  "<NS_PLAIN_SAFE_CHAR_EXCEPTION> ~ “:” | “#”\n"
  "<NS_PLAIN_SAFE_CHAR_IN>           ~  <_NB_CHAR> /* Because <NS_CHAR> is <_NB_CHAR> */\n"
  "<NS_PLAIN_SAFE_CHAR_IN_EXCEPTION> ~ <_C_INDICATOR> | “:” | “#”\n"
  "\n"
  "<c_printable>                ::= <C_PRINTABLE>\n"
  "<nb_json>                    ::= <NB_JSON>\n"
  "<c_byte_order_mark>          ::= <C_BYTE_ORDER_MARK>\n"
  "<c_sequence_entry>           ::= <C_SEQUENCE_ENTRY>\n"
  "<c_mapping_key>              ::= <C_MAPPING_KEY>\n"
  "<c_mapping_value>            ::= <C_MAPPING_VALUE>\n"
  "<c_collect_entry>            ::= <C_COLLECT_ENTRY>\n"
  "<c_sequence_start>           ::= <C_SEQUENCE_START>\n"
  "<c_sequence_end>             ::= <C_SEQUENCE_END>\n"
  "<c_mapping_start>            ::= <C_MAPPING_START>\n"
  "<c_mapping_end>              ::= <C_MAPPING_END>\n"
  "<c_comment>                  ::= <C_COMMENT>\n"
  "<c_anchor>                   ::= <C_ANCHOR>\n"
  "<c_alias>                    ::= <C_ALIAS>\n"
  "<c_tag>                      ::= <C_TAG>\n"
  "<c_literal>                  ::= <C_LITERAL>\n"
  "<c_folded>                   ::= <C_FOLDED>\n"
  "<c_single_quote>             ::= <C_SINGLE_QUOTE>\n"
  "<c_double_quote>             ::= <C_DOUBLE_QUOTE>\n"
  "<c_directive>                ::= <C_DIRECTIVE>\n"
  "<c_reserved>                 ::= <C_RESERVED>\n"
  "<c_indicator>                ::= <C_INDICATOR>\n"
  "<c_flow_indicator>           ::= <C_FLOW_INDICATOR>\n"
  "<b_line_feed>                ::= <B_LINE_FEED>\n"
  "<b_carriage_return>          ::= <B_CARRIAGE_RETURN>\n"
  "<b_char>                     ::= <B_CHAR>\n"
  "<nb_char>                    ::= <NB_CHAR> - <NB_CHAR_EXCEPTION>\n"
  "<b_break>                    ::= <B_BREAK>\n"
  "<b_as_line_feed>             ::= <b_break>\n"
  "<b_non_content>              ::= <b_break>\n"
  "<s_space>                    ::= <S_SPACE>\n"
  "<s_tab>                      ::= <S_TAB>\n"
  "<s_white>                    ::= <s_space> | <s_tab>\n"
  "<ns_char>                    ::= <NS_CHAR> - <NS_CHAR_EXCEPTION>\n"
  "<ns_dec_digit>               ::= <NS_DEC_DIGIT>\n"
  "<ns_hex_digit>               ::= <NS_HEX_DIGIT>\n"
  "<ns_ascii_letter>            ::= <NS_ASCII_LETTER>\n"
  "<ns_word_char>               ::= <NS_WORD_CHAR>\n"
  "<ns_uri_char>                ::= <NS_URI_CHAR>\n"
  "<ns_tag_char>                ::= <NS_TAG_CHAR> - <NS_TAG_CHAR_EXCEPTION>\n"
  "<c_escape>                   ::= <C_ESCAPE>\n"
  "<ns_esc_null>                ::= <NS_ESC_NULL>\n"
  "<ns_esc_bell>                ::= <NS_ESC_BELL>\n"
  "<ns_esc_backspace>           ::= <NS_ESC_BACKSPACE>\n"
  "<ns_esc_horizontal_tab>      ::= <NS_ESC_HORIZONTAL_TAB>\n"
  "<ns_esc_line_feed>           ::= <NS_ESC_LINE_FEED>\n"
  "<ns_esc_vertical_tab>        ::= <NS_ESC_VERTICAL_TAB>\n"
  "<ns_esc_form_feed>           ::= <NS_ESC_FORM_FEED>\n"
  "<ns_esc_carriage_return>     ::= <NS_ESC_CARRIAGE_RETURN>\n"
  "<ns_esc_escape>              ::= <NS_ESC_ESCAPE>\n"
  "<ns_esc_space>               ::= <NS_ESC_SPACE>\n"
  "<ns_esc_double_quote>        ::= <NS_ESC_DOUBLE_QUOTE>\n"
  "<ns_esc_slash>               ::= <NS_ESC_SLASH>\n"
  "<ns_esc_backslash>           ::= <NS_ESC_BACKSLASH>\n"
  "<ns_esc_next_line>           ::= <NS_ESC_NEXT_LINE>\n"
  "<ns_esc_non_breaking_space>  ::= <NS_ESC_NON_BREAKING_SPACE>\n"
  "<ns_esc_line_separator>      ::= <NS_ESC_LINE_SEPARATOR>\n"
  "<ns_esc_paragraph_separator> ::= <NS_ESC_PARAGRAPH_SEPARATOR>\n"
  "<ns_esc_8_bit>               ::= <NS_ESC_8_BIT>\n"
  "<ns_esc_16_bit>              ::= <NS_ESC_16_BIT>\n"
  "<ns_esc_32_bit>              ::= <NS_ESC_32_BIT>\n"
  "<c_ns_esc_char>              ::= <C_NS_ESC_CHAR>\n"
  "\n"
  "event ^s_indent_n = predicted <s_indent><-(n)\n"
  "<s_indent><-(n)              ::= <S_INDENT_N> /* <s_space> × n */\n"
  "event ^s_indent_lt_n = predicted <s_indent_lt_n><-(n)\n"
  "<s_indent_lt_n><-(n)         ::= <S_INDENT_LT_N> /* <s_space> × m Where m < n */\n"
  "event ^s_indent_le_n = predicted <s_indent_le_n><-(n)\n"
  "<s_indent_le_n><-(n)         ::= <S_INDENT_LE_N> /* <s_space> × m Where m <= n */\n"
  "\n"
  "<s_separate_in_line>         ::= <s_white many> | :sol /* Start of line */\n"
  "event ^s_line_prefix = predicted <s_line_prefix><-(n,c)\n"
  "<s_line_prefix><-(n,c)       ::= <C IS BLOCK OUT> <s_block_line_prefix>->(n)\n"
  "                               | <C IS BLOCK IN>  <s_block_line_prefix>->(n)\n"
  "                               | <C IS FLOW OUT>  <s_flow_line_prefix>->(n)\n"
  "                               | <C IS FLOW IN>   <s_flow_line_prefix>->(n)\n"
  "<s_block_line_prefix><-(n)   ::= <s_indent>->(n)\n"
  "<s_flow_line_prefix><-(n)    ::= <s_indent>->(n) <s_separate_in_line maybe>\n"
  "<l_empty><-(n,c)             ::= ( <s_line_prefix>->(n,c) | <s_indent_lt_n>->(n) ) <b_as_line_feed>\n"
  "<b_l_trimmed><-(n,c)         ::= <b_non_content> <l_empty n c many>->(n,c)\n"
  "<b_as_space>                 ::= <b_break>\n"
  "<b_l_folded><-(n,c)          ::= <b_l_trimmed>->(n,c) | <b_as_space>\n"
  "<s_flow_folded><-(n)         ::= <s_separate_in_line maybe> <b_l_folded>->(n,'flow-in') <s_flow_line_prefix>->(n)\n"
  "<c_nb_comment_text>          ::= “#” <nb_char any>\n"
  "<b_comment>                  ::= <b_non_content> | :eof /* End of file */\n"
  "<s_b_comment>                ::= <s_separate_in_line> <c_nb_comment_text maybe> <b_comment>\n"
  "                               |                                                <b_comment>\n"
  "<l_comment>                  ::= <s_separate_in_line> <c_nb_comment_text maybe> <b_comment>\n"
  "<s_l_comments>               ::= ( <s_b_comment> | :sol /* Start of line */ ) <l_comment any>\n"
  "event ^s_separate = predicted <s_separate><-(n,c)\n"
  "<s_separate><-(n,c)          ::= <C IS BLOCK OUT> <s_separate_lines>->(n)\n"
  "                               | <C IS BLOCK IN>  <s_separate_lines>->(n)\n"
  "                               | <C IS FLOW OUT>  <s_separate_lines>->(n)\n"
  "                               | <C IS FLOW IN>   <s_separate_lines>->(n)\n"
  "                               | <C IS BLOCK KEY> <s_separate_in_line>\n"
  "                               | <C IS FLOW KEY>  <s_separate_in_line>\n"
  "<s_separate_lines><-(n)      ::= <s_l_comments> <s_flow_line_prefix>->(n)\n"
  "                               | <s_separate_in_line>\n"
  "<l_directive>                ::= “%”\n"
  "                                 ( <ns_yaml_directive>\n"
  "                                 | <ns_tag_directive>\n"
  "                                 | <ns_reserved_directive> )\n"
  "                                 <s_l_comments>\n"
  "<ns_reserved_directive>      ::= <ns_directive_name> <ns_reserved_directive end any>\n"
  "<ns_directive_name>          ::= <ns_char>+\n"
  "<ns_directive_parameter>     ::= <ns_char>+ \n"
  "<ns_yaml_directive>          ::= “Y” “A” “M” “L” <s_separate_in_line> <ns_yaml_version>\n"
  "<ns_yaml_version>            ::= <ns_dec_digit many> “.” <ns_dec_digit many>\n"
  "<ns_tag_directive>           ::= “T” “A” “G” <s_separate_in_line> <c_tag_handle> <s_separate_in_line> <ns_tag_prefix>\n"
  "<c_tag_handle>               ::= <c_named_tag_handle>\n"
  "                               | <c_secondary_tag_handle>\n"
  "                               | <c_primary_tag_handle>\n"
  "<c_primary_tag_handle>       ::= “!”\n"
  "<c_secondary_tag_handle>     ::= “!” “!”\n"
  "<c_named_tag_handle>         ::= “!” <ns_word_char many> “!”\n"
  "<ns_tag_prefix>              ::= <c_ns_local_tag_prefix> | <ns_global_tag_prefix>\n"
  "<c_ns_local_tag_prefix>      ::= “!” <ns_uri_char any>\n"
  "<ns_global_tag_prefix>       ::= <ns_tag_char> <ns_uri_char any>\n"
  "<c_ns_properties><-(n,c)     ::= <c_ns_tag_property> <s_separate>->(n,c) <c_ns_anchor_property>\n"
  "                               | <c_ns_tag_property>\n"
  "                               | <c_ns_anchor_property> <s_separate>->(n,c) <c_ns_tag_property>\n"
  "                               | <c_ns_anchor_property>\n"
  "<c_ns_tag_property>          ::= <c_verbatim_tag>\n"
  "                               | <c_ns_shorthand_tag>\n"
  "                               | <c_non_specific_tag>\n"
  "<c_verbatim_tag>             ::= “!” “<” <ns_uri_char many> “>”\n"
  "<c_ns_shorthand_tag>         ::= <c_tag_handle> <ns_tag_char many>\n"
  "<c_non_specific_tag>         ::= “!”\n"
  "<c_ns_anchor_property>       ::= “&” <ns_anchor_name>\n"
  "<ns_anchor_char>             ::= <NS_ANCHOR_CHAR> - <NS_ANCHOR_CHAR_EXCEPTION>\n"
  "<ns_anchor_name>             ::= <ns_anchor_char>+\n"
  "<c_ns_alias_node>            ::= “*” <ns_anchor_name>\n"
  "<e_scalar>                   ::= /* Empty */\n"
  "<e_node>                     ::= <e_scalar>\n"
  "<nb_double_char>             ::= <c_ns_esc_char> | ( <NB_DOUBLE_CHAR> - <NB_DOUBLE_CHAR_EXCEPTION> )\n"
  "<ns_double_char>             ::= <c_ns_esc_char> | ( <NS_DOUBLE_CHAR> - <NS_DOUBLE_CHAR_EXCEPTION> ) /* <nb-double-char - s-white : Note that <s_white> cannot be in <c_ns_esc_char> */\n"
  "<c_double_quoted><-(n,c)     ::= “\"” <nb_double_text>->(n,c) “\"”\n"
  "event ^nb_double_text = predicted <nb_double_text><-(n,c)\n"
  "<nb_double_text><-(n,c)      ::= <C IS FLOW OUT> <nb_double_multi_line>->(n)\n"
  "                               | <C IS FLOW IN>  <nb_double_multi_line>->(n)\n"
  "                               | <C IS BLOCK KEY> <nb_double_one_line>\n"
  "                               | <C IS FLOW KEY> <nb_double_one_line>\n"
  "<nb_double_one_line>         ::= <nb_double_char>*\n"
  "<s_double_escaped><-(n)      ::= <s_white any> “\\\\” <b_non_content> <l_empty n c any>->(n,'flow-in') <s_flow_line_prefix>->(n)\n"
  "<s_double_break><-(n)        ::= <s_double_escaped>->(n) | <s_flow_folded>->(n)\n"
  "<nb_ns_double_in_line>       ::= <nb_ns_double_in_line unit>*\n"
  "<s_double_next_line><-(n)    ::= <s_double_break>->(n) <ns_double_char> <nb_ns_double_in_line> ( <s_double_next_line>->(n) | <s_white any> )\n"
  "                               | <s_double_break>->(n)\n"
  "<nb_double_multi_line><-(n)  ::= <nb_ns_double_in_line> ( <s_double_next_line>->(n) | <s_white any> )\n"
  "<c_quoted_quote>             ::= <C_QUOTED_QUOTE>\n"
  "<nb_single_char>             ::= <c_quoted_quote> | ( <NB_SINGLE_CHAR> - <NB_SINGLE_CHAR_EXCEPTION> )\n"
  "<ns_single_char>             ::= <c_quoted_quote> | ( <NS_SINGLE_CHAR> - <NS_SINGLE_CHAR_EXCEPTION> ) /* <nb_single_char> - <s_white> : Note that <s_white> canno be in <c_quoted_quote> */\n"
  "<c_single_quoted><-(n,c)     ::= “'” <nb_single_text>->(n,c) “'”\n"
  "event ^nb_single_text = predicted <nb_single_text><-(n,c)\n"
  "<nb_single_text><-(n,c)      ::= <C IS FLOW OUT> <nb_single_multi_line>->(n)\n"
  "                               | <C IS FLOW IN> <nb_single_multi_line>->(n)\n"
  "                               | <C IS BLOCK KEY> <nb_single_one_line>\n"
  "                               | <C IS FLOW KEY> <nb_single_one_line>\n"
  "<nb_single_one_line>         ::= <nb_single_char>*\n"
  "<nb_ns_single_in_line>       ::= <nb_ns_single_in_line unit>*\n"
  "<s_single_next_line><-(n)    ::= <s_flow_folded>->(n) <ns_single_char> <nb_ns_single_in_line> ( <s_single_next_line>->(n) | <s_white any> )\n"
  "                               | <s_flow_folded>->(n)\n"
  "<nb_single_multi_line><-(n)  ::= <nb_ns_single_in_line> ( <s_single_next_line>->(n) | <s_white any> ) \n"
  "<ns_plain_first><-(c)        ::= ( <NS_PLAIN_FIRST> - <NS_PLAIN_FIRST_EXCEPTION> )\n"
  "                               | ( “?” | “:” | “-” ) <ns_plain_safe c lookup>->(c) /* Followed by an ns-plain-safe(c)) */\n"
  "event ^ns_plain_safe = predicted <ns_plain_safe><-(c)\n"
  "<ns_plain_safe><-(c)         ::= <C IS FLOW OUT> <ns_plain_safe_out>\n"
  "                               | <C IS FLOW IN> <ns_plain_safe_in>\n"
  "                               | <C IS BLOCK KEY> <ns_plain_safe_out>\n"
  "                               | <C IS FLOW KEY> <ns_plain_safe_in>\n"
  "<ns_plain_safe_out>          ::= <ns_char>\n"
  "<ns_plain_safe_in>           ::= <NS_PLAIN_SAFE_IN> - <NS_PLAIN_SAFE_IN_EXCEPTION>\n"
  "<ns_plain_char><-(c)         ::= <ns_plain_safe_char>->(c)\n"
  "                               | <ns_char preceding> /* An ns-char preceding */ “#”\n"
  "                               | “:” <ns_plain_safe c lookup>->(c) /* Followed by an ns-plain-safe(c) */\n"
  " /* We rewrite <ns_plain_safe>->(c) - “:” - “#” as <ns_plain_safe_char>->(c) */\n"
  "event ^ns_plain_safe_char = predicted <ns_plain_safe_char><-(c)\n"
  "<ns_plain_safe_char><-(c)    ::= <C IS FLOW OUT> <ns_plain_safe_char_out>\n"
  "                               | <C IS FLOW IN> <ns_plain_safe_char_in>\n"
  "                               | <C IS BLOCK KEY> <ns_plain_safe_char_out>\n"
  "                               | <C IS FLOW KEY> <ns_plain_safe_char_in>\n"
  "<ns_plain_safe_char_out>     ::= <NS_PLAIN_SAFE_CHAR> - <NS_PLAIN_SAFE_CHAR_EXCEPTION>\n"
  "<ns_plain_safe_char_in>      ::= <NS_PLAIN_SAFE_CHAR_IN> - <NS_PLAIN_SAFE_CHAR_IN_EXCEPTION>\n"
  "<ns_plain><-(n,c)            ::= <C IS FLOW OUT> <ns_plain_multi_line>->(n,c)\n"
  "                               | <C IS FLOW IN> <ns_plain_multi_line>->(n,c)\n"
  "                               | <C IS BLOCK KEY> <ns_plain_one_line>->(c)\n"
  "                               | <C IS FLOW KEY> <ns_plain_one_line>->(c)\n"
  "<nb_ns_plain_in_line><-(c)   ::= <nb_ns_plain_in_line unit>->(c)*\n"
  "<ns_plain_one_line><-(c)     ::= <ns_plain_first>->(c) <nb_ns_plain_in_line>->(c)\n"
  "<s_ns_plain_next_line><-(n,c)::= <s_flow_folded>->(n) <ns_plain_char>->(c) <nb_ns_plain_in_line>->(c)\n"
  "<ns_plain_multi_line><-(n,c) ::= <ns_plain_one_line>->(c) <s_ns_plain_next_line any>->(n,c)\n"
  " /* in-flow(c) is a lua function */\n"
  "<luascript>\n"
  "function in_flow(c)\n"
  "  if c == 'flow-out' then return 'flow-in' end\n"
  "  if c == 'flow-in' then return 'flow-in' end\n"
  "  if c == 'block-key' then return 'flow-key' end\n"
  "  if c == 'flow-key' then return 'flow-key' end\n"
  "  error('Invalid c: '..tostring(c))\n"
  "end\n"
  "</luascript>\n"
  "<c_flow_sequence><-(n,c)        ::= “[” <s_separate maybe>->(n,c) <ns_s_flow_seq_entries maybe>->(n,in_flow(c)) “]”\n"
  "<ns_s_flow_seq_entries><-(n,c)  ::= <ns_flow_seq_entry>->(n,c) <s_separate maybe>->(n,c) “,” <s_separate maybe>->(n,c) <ns_s_flow_seq_entries maybe>->(n,c)\n"
  "                                  | <ns_flow_seq_entry>->(n,c) <s_separate maybe>->(n,c)\n"
  "<ns_flow_seq_entry><-(n,c)      ::= <ns_flow_pair>->(n,c) | <ns_flow_node>->(n,c)\n"
  "<c_flow_mapping><-(n,c)         ::= “{” <s_separate maybe>->(n,c) <ns_s_flow_map_entries maybe>->(n,in_flow(c)) “}”\n"
  "<ns_s_flow_map_entries><-(n,c)  ::= <ns_flow_map_entry>->(n,c) <s_separate maybe>->(n,c) “,” <s_separate maybe>->(n,c) <ns_s_flow_map_entries maybe>->(n,c)\n"
  "                                  | <ns_flow_map_entry>->(n,c) <s_separate maybe>->(n,c)\n"
  "<ns_flow_map_entry><-(n,c)      ::= “?” <s_separate>->(n,c) <ns_flow_map_explicit_entry>->(n,c)\n"
  "                                  | <ns_flow_map_implicit_entry>->(n,c)\n"
  "<ns_flow_map_explicit_entry><-(n,c) ::= <ns_flow_map_implicit_entry>->(n,c)\n"
  "                                      | <e_node> /* Key */ <e_node> /* Value */\n"
  "<ns_flow_map_implicit_entry><-(n,c) ::= <ns_flow_map_yaml_key_entry>->(n,c)\n"
  "                                      | <c_ns_flow_map_empty_key_entry>->(n,c)\n"
  "                                      | <c_ns_flow_map_json_key_entry>->(n,c)\n"
  "<ns_flow_map_yaml_key_entry><-(n,c) ::= <ns_flow_yaml_node>->(n,c) ( ( <s_separate maybe>->(n,c) <c_ns_flow_map_separate_value>->(n,c) ) | <e_node> )\n"
  "<c_ns_flow_map_empty_key_entry><-(n,c) ::= <e_node> /* Key */ <c_ns_flow_map_separate_value>->(n,c)\n"
  "<c_ns_flow_map_separate_value><-(n,c)  ::= “:” <ns_plain_safe c negative lookup>->(c) /* Not followed by an ns-plain-safe(c) */ ( ( <s_separate>->(n,c) <ns_flow_node>->(n,c) ) | <e_node> /* Value */ )\n"
  "<c_ns_flow_map_json_key_entry><-(n,c) ::= <c_flow_json_node>->(n,c) ( ( <s_separate maybe>->(n,c) <c_ns_flow_map_adjacent_value>->(n,c) ) | <e_node> )\n"
  "<c_ns_flow_map_adjacent_value><-(n,c) ::= “:” ( ( <s_separate maybe>->(n,c) <ns_flow_node>->(n,c) ) | <e_node> ) /* Value */\n"
  "<ns_flow_pair><-(n,c)                 ::= ( “?” <s_separate>->(n,c) <ns_flow_map_explicit_entry>->(n,c) )\n"
  "                                        | <ns_flow_pair_entry>->(n,c)\n"
  "<ns_flow_pair_entry><-(n,c)           ::= <ns_flow_pair_yaml_key_entry>->(n,c)\n"
  "                                        | <c_ns_flow_map_empty_key_entry>->(n,c)\n"
  "                                        | <c_ns_flow_pair_json_key_entry>->(n,c)\n"
  "<ns_flow_pair_yaml_key_entry><-(n,c)  ::= <ns_s_implicit_yaml_key>->('flow-key') <c_ns_flow_map_separate_value>->(n,c)\n"
  "<c_ns_flow_pair_json_key_entry><-(n,c) ::= <c_s_implicit_json_key>->('flow-key') <c_ns_flow_map_adjacent_value>->(n,c)\n"
  "event ns_s_implicit_yaml_key$ = completed <ns_s_implicit_yaml_key><-(c)\n"
  "<ns_s_implicit_yaml_key><-(c)          ::= <ns_flow_yaml_node>->(nil,c) <s_separate_in_line maybe> /* At most 1024 characters altogether */\n"
  "event c_s_implicit_json_key$ = completed <c_s_implicit_json_key><-(c)\n"
  "<c_s_implicit_json_key><-(c)           ::= <c_flow_json_node>->(nil,c) <s_separate_in_line maybe> /* At most 1024 characters altogether */\n"
  "\n"
  "<l_empty n c many><-(n,c)       ::= <l_empty>->(n,c)+\n"
  "<s_white many>                  ::= <s_white>+\n"
  "<s_separate_in_line maybe>      ::= <s_separate_in_line>\n"
  "<s_separate_in_line maybe>      ::=\n"
  "<nb_char any>                   ::= <nb_char>*\n"
  "<c_nb_comment_text maybe>       ::= <c_nb_comment_text>\n"
  "<c_nb_comment_text maybe>       ::=\n"
  "<l_comment any>                 ::= <l_comment>*\n"
  "<ns_reserved_directive end>     ::= <s_separate_in_line> <ns_directive_parameter>\n"
  "<ns_reserved_directive end any> ::= <ns_reserved_directive end>*\n"
  "<ns_dec_digit many>             ::= <ns_dec_digit>+\n"
  "<ns_word_char many>             ::= <ns_word_char>+\n"
  "<ns_uri_char any>               ::= <ns_uri_char>*\n"
  "<ns_uri_char many>              ::= <ns_uri_char>+\n"
  "<ns_tag_char many>              ::= <ns_tag_char>+\n"
  "<s_white any>                   ::= <s_white>*\n"
  "<l_empty n c any><-(n,c)        ::= <l_empty>->(n,c)*\n"
  "<nb_ns_double_in_line unit>     ::= <s_white any> <ns_double_char>\n"
  "<nb_ns_single_in_line unit>     ::= <s_white any> <ns_single_char>\n"
  "event ^ns_plain_safe_c_lookup = predicted <ns_plain_safe c lookup><-(c)\n"
  "<ns_plain_safe c lookup><-(c)   ::= <NS_PLAIN_SAFE c LOOKUP>\n"
  "<ns_char preceding>             ::= <NS_CHAR PRECEDING>\n"
  "<nb_ns_plain_in_line unit><-(c) ::= <s_white any> <ns_plain_char>->(c)\n"
  "<s_ns_plain_next_line any><-(n,c) ::= <s_ns_plain_next_line>->(n,c)*\n"
  "<s_separate maybe><-(n,c)       ::= <s_separate>->(n,c)\n"
  "<s_separate maybe><-(n,c)       ::=\n"
  "<ns_s_flow_seq_entries maybe><-(n,c) ::= <ns_s_flow_seq_entries>->(n,c)\n"
  "<ns_s_flow_seq_entries maybe><-(n,c) ::=\n"
  "<ns_s_flow_map_entries maybe><-(n,c) ::= <ns_s_flow_map_entries>->(n,c)\n"
  "<ns_s_flow_map_entries maybe><-(n,c) ::=\n"
  "event ^ns_plain_safe_c_negative_lookup = predicted <ns_plain_safe c negative lookup><-(c)\n"
  "<ns_plain_safe c negative lookup><-(c)   ::= <NS_PLAIN_SAFE c NEGATIVE LOOKUP>\n"
  "\n"
  "<C IS BLOCK OUT>             ::= [\\s\\S]\n"
  "<C IS BLOCK IN>              ::= [\\s\\S]\n"
  "<C IS FLOW OUT>              ::= [\\s\\S]\n"
  "<C IS FLOW IN>               ::= [\\s\\S]\n"
  "<C IS BLOCK KEY>             ::= [\\s\\S]\n"
  "<C IS FLOW KEY>              ::= [\\s\\S]\n"
  "<S_INDENT_N>                 ::= [\\s\\S]\n"
  "<S_INDENT_LT_N>              ::= [\\s\\S]\n"
  "<S_INDENT_LE_N>              ::= [\\s\\S]\n"
  "<NS_PLAIN_SAFE c LOOKUP>     ::= [\\s\\S]\n"
  "<NS_PLAIN_SAFE c NEGATIVE LOOKUP> ::= [\\s\\S]\n"
  "<NS_CHAR PRECEDING>          ::= [\\s\\S]\n"
  ;

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
  short                        continueb;
  short                        exhaustedb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }
  marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
  if (marpaESLIFOptionp == NULL) {
    goto err;
  }

  /* Compile YAML grammar */
  marpaESLIFGrammarOption.bytep               = (void *) yamls;
  marpaESLIFGrammarOption.bytel               = strlen(yamls);
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
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "YAML grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
    }
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammarp, &grammarscripts)) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "--------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "YAML grammar script:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "--------------------\n%s", grammarscripts);

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
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
  size_t                      sendl;

  *inputsp              = (char *) yamls;
  *inputlp              = strlen(yamls);
  *characterStreambp    = 1;
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;
  *eofbp                = 1;

  return 1;
}
