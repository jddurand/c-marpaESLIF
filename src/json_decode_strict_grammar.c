static const char *marpaESLIFJSON_decode_strict_grammars =
  "                   #####################################################\n"
  "                   # >>>>>>>>>>>>>>> Strict JSON Grammar <<<<<<<<<<<<<<<\n"
  "                   #####################################################\n"
  "\n"
  "# --------------------------------------------------\n"
  "# Meta settings\n"
  "# --------------------------------------------------\n"
  ":desc              ::= 'Strict JSON Grammar'\n"
  ":default           ::= event-action => marpaESLIFJsonEventAction fallback-encoding => UTF-8\n"
  "\n"
  "# ---------------------------------\n"
  "# Discard unsignificant whitespaces\n"
  "# ---------------------------------\n"
  ":discard           ::= /[\\x{9}\\x{A}\\x{D}\\x{20}]+/\n"
  "\n"
  "# ----------------\n"
  "# Start is a value\n"
  "# ----------------\n"
  ":start             ::= value\n"
  "\n"
  "# ------\n"
  "# Events\n"
  "# ------\n"
  ":terminal          ::= '['                pause => after  event => 0_inc$\n"
  ":terminal          ::= ']'                pause => after  event => 1_dec$\n"
  ":terminal          ::= '{'                pause => after  event => 0_inc$\n"
  ":terminal          ::= '}'                pause => after  event => 1_dec$\n"
  "\n"
  ":lexeme            ::= CHAR               pause => after  event => 2_CHAR$\n"
  ":lexeme            ::= CONSTANT_OR_NUMBER pause => after  event => 3_CONSTANT_OR_NUMBER$\n"
  ":terminal          ::= '\"'               pause => after  event => 4_dquote$\n"
  "\n"
  "# ----------\n"
  "# JSON value\n"
  "# ----------\n"
  "value              ::= string\n"
  "                     | object\n"
  "                     | array\n"
  "                     | constant_or_number\n"
  "\n"
  "# -----------\n"
  "# JSON String\n"
  "# -----------\n"
  "string             ::= '\"' chars '\"'\n"
  "chars              ::= char*\n"
  "char               ::= CHAR\n"
  "\n"
  "\n"
  "# -----------\n"
  "# JSON object\n"
  "# -----------\n"
  "object             ::= '{' members '}'\n"
  "members            ::= pair*              separator => comma proper => 1 # Strict comma separator\n"
  "pair               ::= string ':' value\n"
  "\n"
  "# ----------\n"
  "# JSON Array\n"
  "# ----------\n"
  "array              ::= '[' elements ']'\n"
  "elements           ::= value*             separator => comma proper => 1 # Strict comma separator\n"
  "\n"
  "# -----------------------\n"
  "# JSON constant or number\n"
  "# -----------------------\n"
  "constant_or_number ::= CONSTANT_OR_NUMBER\n"
  "\n"
  "# ---------\n"
  "# Separator\n"
  "# ---------\n"
  "comma              ::= ','\n"
  "\n"
  "# -------\n"
  "# Lexemes\n"
  "# -------\n"
  ":desc                ~ 'Strict JSON Grammar Lexemes'\n"
  ":default             ~ fallback-encoding => UTF-8\n"
  "\n"
  "CHAR                 ~ /(?:[^\"\\\\\\x00-\\x1F]+)|(?:\\\\[\"\\\\\\/bfnrt])|(?:(?:\\\\u[[:xdigit:]]{4})+)/\n"
  "CONSTANT_OR_NUMBER   ~ /true|false|null|(?:-?(?:0|[1-9][0-9]*)(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)/\n"
  ;
