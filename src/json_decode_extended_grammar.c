static const char *marpaESLIFJSON_decode_extended_grammars =
  "                   #######################################################\n"
  "                   # >>>>>>>>>>>>>>> Extended JSON Grammar <<<<<<<<<<<<<<<\n"
  "                   #######################################################\n"
  "\n"
  "# --------------------------------------------------\n"
  "# Meta settings\n"
  "# --------------------------------------------------\n"
  ":desc              ::= 'Extended JSON Grammar'\n"
  ":default           ::= regex-action => marpaESLIFJsonRegexAction fallback-encoding => UTF-8 discard-is-fallback => 1\n"
  "\n"
  "# -----------------------------------------------------------\n"
  "# Discard unsignificant whitespaces, Perl comment C++ comment\n"
  "# -----------------------------------------------------------\n"
  ":discard           ::= /[\\x{9}\\x{A}\\x{D}\\x{20}]+|#[^\\n]*|\\/\\/[^\\n]*|\\/\\*(?:(?:[^\\*]+|\\*(?!\\/))*)\\*\\//u\n"
  "\n"
  "#\n"
  "# ---------------------------------------\n"
  "# Terminal aliases for the external world\n"
  "# ---------------------------------------\n"
  ":symbol ::= '{' name => LBRACKET\n"
  ":symbol ::= '}' name => RBRACKET\n"
  ":symbol ::= '[' name => LSQUARE\n"
  ":symbol ::= ']' name => RSQUARE\n"
  ":symbol ::= ':' name => COLUMN\n"
  ":symbol ::= ',' name => COMMA\n"
  "\n"
  "# ----------------\n"
  "# Start is a value\n"
  "# ----------------\n"
  ":start             ::= value\n"
  "\n"
  "# ----------\n"
  "# JSON value\n"
  "# ----------\n"
  "value              ::= object\n"
  "                     | array\n"
  "                     | string\n"
  "                     | constant_or_number\n"
  "\n"
  "# -----------\n"
  "# JSON object\n"
  "# -----------\n"
  "object             ::= $LBRACKET members $RBRACKET\n"
  "members            ::= pair*              separator => commas proper => 0 # Multiple commas everywhere\n"
  "pair               ::= string $COLUMN value\n"
  "\n"
  "# ----------\n"
  "# JSON Array\n"
  "# ----------\n"
  "array              ::= $LSQUARE elements $RSQUARE\n"
  "elements           ::= value*             separator => commas proper => 0 # Multiple commas everywhere\n"
  "\n"
  "# -----------\n"
  "# JSON String\n"
  "# -----------\n"
  "string             ::= STRING\n"
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
  "commas             ::= comma+\n"
  "\n"
  "# -------------------------------\n"
  "# Lexemes - only the caller knows\n"
  "# -------------------------------\n"
  "STRING             ~ [^\\s\\S]\n"
  "CONSTANT_OR_NUMBER ~ [^\\s\\S]\n"
  "\n"
  ;
