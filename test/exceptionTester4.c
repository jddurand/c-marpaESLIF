#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);

#define UTF_8_STRING "UTF-8"

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *xmls =
"#\n"
"# From https://www.w3.org/TR/REC-xml (5th edition)\n"
"#\n"
"# Take care, original has several ambiguities:\n"
"# - https://lists.w3.org/Archives/Public/xml-editor/2011OctDec/0000 (applied)\n"
"# - https://lists.w3.org/Archives/Public/xml-editor/2011OctDec/0001 (applied)\n"
"# - https://lists.w3.org/Archives/Public/xml-editor/2011OctDec/0002 (applied)\n"
"#\n"
"# Note:\n"
"# Concerning XML Exceptions there are three categories:\n"
"# - Comment interior: We create a character class without for Char minus the '-' character\n"
"# - PITarget        : Native BNF exception is doing it\n"
"# - Others          : They are ALL in the form: <character> - ( <character>* <exception longer than one character> <character>* )\n"
"#                     where <exception longer than one character> is always an expected terminal preceeding and/or succeeding <character>* !\n"
"#                     So this mean there is NO needed to write exception...: the grammar will natively stop <character>* parsing as soon\n"
"#                     as it sees <exception longer than one character> in stream, because it is always working in the LATM (Longest Acceptable\n"
"#                     Token Match) mode...\n"
"\n"
"# event document$ = completed document\n"
"document           ::= prolog element <Misc any>\n"
"# event Char$ = completed Char\n"
"Char               ::= [\\x{9}\\x{A}\\x{D}\\x{20}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]:u name => Char\n"
"# event S1$ = completed S1\n"
"S1                 ::= [\\x{20}\\x{9}\\x{D}\\x{A}]\n"
"# event S$ = completed S\n"
"S                  ::= S1+\n"
"# event NameStartChar$ = completed NameStartChar\n"
"NameStartChar      ::= [:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}]:u\n"
"# event NameChar$ = completed NameChar\n"
"NameChar           ::= [:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}\\-.0-9\\x{B7}\\x{0300}-\\x{036F}\\x{203F}-\\x{2040}]:u\n"
"# event Name$ = completed Name\n"
"# Name               ::= NameStartChar <NameChar any>\n"
"Name               ::= <NAME>\n"
"# event Names$ = completed Names\n"
"Names              ::= Name+ separator => [\\x{20}]\n"
"# event Nmtoken$ = completed Nmtoken\n"
"Nmtoken            ::= NameChar+\n"
"# event Nmtokens$ = completed Nmtokens\n"
"Nmtokens           ::= Nmtoken+ separator => [\\x{20}]\n"
"# event EntityValue$ = completed EntityValue\n"
"EntityValue        ::= '\"' <EntityValue1 any>   '\"'\n"
"                     | \"'\" <EntityValue2 any>   \"'\"\n"
"# event AttValue$ = completed AttValue\n"
"AttValue           ::= '\"' <AttValue1 any>      '\"'\n"
"                     | \"'\" <AttValue2 any>      \"'\"\n"
"# event SystemLiteral$ = completed SystemLiteral\n"
"SystemLiteral      ::= '\"' <SystemLiteral1 any> '\"'\n"
"                     | \"'\" <SystemLiteral2 any> \"'\"\n"
"# event PubidLiteral$ = completed PubidLiteral\n"
"PubidLiteral       ::= '\"' <PubidChar1 any>     '\"'\n"
"                     | \"'\" <PubidChar2 any>     \"'\"\n"
"# event PubidChar$ = completed PubidChar\n"
"PubidChar          ::= [\\x{20}\\x{D}\\x{A}a-zA-Z0-9\\-'()+,./:=?;!*#@$_%]\n"
"# event CharData$ = completed CharData\n"
"CharData           ::= <CharData Exceptioned>\n"
"# event Comment$ = completed Comment\n"
"Comment            ::= '<!--' <Comment Interior> '-->'\n"
"# event PI$ = completed PI\n"
"PI                 ::= '<?' PITarget                    '?>'\n"
"                     | '<?' PITarget S <PI Exceptioned> '?>'\n"
"# event CDSect$ = completed CDSect\n"
"CDSect             ::= CDStart CData CDEnd\n"
"# event CDStart$ = completed CDStart\n"
"CDStart            ::= '<![CDATA['\n"
"# event CData$ = completed CData\n"
"CData              ::= <CData Exceptioned>\n"
"# event CDEnd$ = completed CDEnd\n"
"CDEnd              ::= ']]>'\n"
"# event prolog$ = completed prolog\n"
"prolog             ::= <XMLDecl maybe> <Misc any>\n"
"                     | <XMLDecl maybe> <Misc any> doctypedecl <Misc any>\n"
"# event XMLDecl$ = completed XMLDecl\n"
"#\n"
"# Note: it is important to split '<?xml' into '<?' 'xml' because of PI whose defintion is: '<?' PITarget\n"
"#\n"
"XMLDecl            ::= '<?' 'xml' VersionInfo <EncodingDecl maybe> <SDDecl maybe> <S maybe> '?>' ## Decl_action => ::copy[3]\n"
"# event VersionInfo$ = completed VersionInfo\n"
"VersionInfo        ::= S 'version' Eq \"'\" VersionNum \"'\"\n"
"                     | S 'version' Eq '\"' VersionNum '\"'\n"
"# event Eq$ = completed Eq\n"
"Eq                 ::= <S maybe> '=' <S maybe>\n"
"# event VersionNum$ = completed VersionNum\n"
"VersionNum         ::= '1.' <digit many>\n"
"#\n"
"# https://lists.w3.org/Archives/Public/xml-editor/2011OctDec/0002\n"
"#\n"
"# Use S1 instead of S in Misc\n"
"#\n"
"# event Misc$ = completed Misc\n"
"Misc               ::= Comment\n"
"                     | PI\n"
"                     | S1\n"
"# event doctypedecl$ = completed doctypedecl\n"
"doctypedecl        ::= '<!DOCTYPE' S Name              <S maybe>                             '>'\n"
"                     | '<!DOCTYPE' S Name              <S maybe> '[' intSubset ']' <S maybe> '>'\n"
"                     | '<!DOCTYPE' S Name S ExternalID <S maybe>                             '>'\n"
"                     | '<!DOCTYPE' S Name S ExternalID <S maybe> '[' intSubset ']' <S maybe> '>'\n"
"#\n"
"# https://lists.w3.org/Archives/Public/xml-editor/2011OctDec/0001\n"
"#\n"
"# Change S in DeclSep to S1\n"
"#\n"
"# event DeclSep$ = completed DeclSep\n"
"DeclSep            ::= PEReference\n"
"                     | S1\n"
"# event intSubset$ = completed intSubset\n"
"intSubset          ::= <intSubset1 any>\n"
"# event markupdecl$ = completed markupdecl\n"
"markupdecl         ::= elementdecl\n"
"                     | AttlistDecl\n"
"                     | EntityDecl\n"
"                     | NotationDecl\n"
"                     | PI\n"
"                     | Comment\n"
"# event extSubset$ = completed extSubset\n"
"extSubset          ::=          extSubsetDecl\n"
"                     | TextDecl extSubsetDecl\n"
"# event extSubsetDecl$ = completed extSubsetDecl\n"
"extSubsetDecl      ::= <extSubsetDecl1 any>\n"
"# event SDDecl$ = completed SDDecl\n"
"SDDecl             ::= S 'standalone' Eq \"'\" <yes or no> \"'\"\n"
"                     | S 'standalone' Eq '\"' <yes or no> '\"'\n"
"element            ::= EmptyElemTag\n"
"                     | STag content ETag\n"
"                     | ELEMENT_VALUE\n"
"# event STag$ = completed STag\n"
"STag               ::= ELEMENT_START Name <STag1 any> <S maybe> '>'\n"
"# event Attribute$ = completed Attribute\n"
"Attribute          ::= Name Eq AttValue\n"
"# event ETag$ = completed ETag\n"
"ETag               ::= '</' Name <S maybe> '>'\n"
"# event content$ = completed content\n"
"content            ::= <CharData maybe> <content1 any>\n"
"# event EmptyElemTag$ = completed EmptyElemTag\n"
"EmptyElemTag       ::= ELEMENT_START Name <EmptyElemTag1 any> <S maybe> '/>'\n"
"# event elementdecl$ = completed elementdecl\n"
"elementdecl        ::= '<!ELEMENT' S Name S contentspec <S maybe> '>'\n"
"# event contentspec$ = completed contentspec\n"
"contentspec        ::= 'EMPTY' | 'ANY' | Mixed | children\n"
"# event children$ = completed children\n"
"children           ::= <choice or seq> <sequence maybe>\n"
"# event cp$ = completed cp\n"
"cp                 ::= <Name or choice or seq> <sequence maybe>\n"
"# event choice$ = completed choice\n"
"choice             ::= '(' <S maybe> cp <choice1 many> <S maybe> ')'\n"
"# event seq$ = completed seq\n"
"seq                ::= '(' <S maybe> cp <seq1 any>     <S maybe> ')'\n"
"# event Mixed$ = completed Mixed\n"
"Mixed              ::= '(' <S maybe> '#PCDATA' <Mixed1 any> <S maybe> ')*'\n"
"                     | '(' <S maybe> '#PCDATA'              <S maybe> ')'\n"
"# event AttlistDecl$ = completed AttlistDecl\n"
"AttlistDecl        ::= '<!ATTLIST' S Name <AttDef any> <S maybe> '>'\n"
"# event AttDef$ = completed AttDef\n"
"AttDef             ::= S Name S AttType S DefaultDecl\n"
"# event AttType$ = completed AttType\n"
"AttType            ::= StringType | TokenizedType | EnumeratedType\n"
"# event StringType$ = completed StringType\n"
"StringType         ::= 'CDATA'\n"
"# event TokenizedType$ = completed TokenizedType\n"
"TokenizedType      ::= 'ID'\n"
"                     | 'IDREF'\n"
"                     | 'IDREFS'\n"
"                     | 'ENTITY'\n"
"                     | 'ENTITIES'\n"
"                     | 'NMTOKEN'\n"
"                     | 'NMTOKENS'\n"
"# event EnumeratedType$ = completed EnumeratedType\n"
"EnumeratedType     ::= NotationType\n"
"                     | Enumeration\n"
"# event NotationType$ = completed NotationType\n"
"NotationType       ::= 'NOTATION' S '(' <S maybe> Name    <NotationType1 any> <S maybe> ')'\n"
"# event Enumeration$ = completed Enumeration\n"
"Enumeration        ::=              '(' <S maybe> Nmtoken <Enumeration1 any>  <S maybe> ')'\n"
"# event DefaultDecl$ = completed DefaultDecl\n"
"DefaultDecl        ::= '#REQUIRED'\n"
"                     | '#IMPLIED'\n"
"                     |            AttValue\n"
"                     | '#FIXED' S AttValue\n"
"# event conditionalSect$ = completed conditionalSect\n"
"conditionalSect    ::= includeSect | ignoreSect\n"
"# event includeSect$ = completed includeSect\n"
"includeSect        ::= '<![' <S maybe> 'INCLUDE' <S maybe> '[' extSubsetDecl ']]>'\n"
"#\n"
"# The rule <ignoreSectContents any>  ::= ignoreSectContents* will trigger MARPA_ERR_COUNTED_NULLABLE: Nullable symbol on RHS of a sequence rule\n"
"# because ignoreSectContents is a nullable, so we revisit the whole ignore sections by making\n"
"# Ignore not nullable.\n"
"#\n"
"# ORIGINAL:\n"
"# ignoreSect         ::= '<![' <S maybe> 'IGNORE' <S maybe> '[' <ignoreSectContents any> ']]>'\n"
"# ignoreSectContents ::= Ignore <ignoreSectContents1 any>\n"
"# Ignore             ::= <CHARDATA any> - <IGNORE EXCEPTION>\n"
"# Ignore             ::= # Because a lexeme cannot be a nullable\n"
"\n"
"# event ignoreSect$ = completed ignoreSect\n"
"ignoreSect         ::= '<![' <S maybe> 'IGNORE' <S maybe> '[' <ignoreSectContents any> ']]>'\n"
"                     | '<![' <S maybe> 'IGNORE' <S maybe> '['                          ']]>'\n"
"# event ignoreSectContents$ = completed ignoreSectContents\n"
"ignoreSectContents ::= Ignore <ignoreSectContents1 any>\n"
"# event Ignore$ = completed Ignore\n"
"Ignore             ::= <Ignore Exceptioned>\n"
"# event CharRef$ = completed CharRef\n"
"CharRef            ::= '&#' <digit many> ';'\n"
"                     | '&#x' <hexdigit many> ';'\n"
"# event Reference$ = completed Reference\n"
"Reference          ::= EntityRef\n"
"                     | CharRef\n"
"# event EntityRef$ = completed EntityRef\n"
"EntityRef          ::= '&' Name ';'\n"
"# event PEReference$ = completed PEReference\n"
"PEReference        ::= '%' Name ';'\n"
"# event EntityDecl$ = completed EntityDecl\n"
"EntityDecl         ::= GEDecl | PEDecl\n"
"# event GEDecl$ = completed GEDecl\n"
"GEDecl             ::= '<!ENTITY' S Name S EntityDef <S maybe> '>'\n"
"# event PEDecl$ = completed PEDecl\n"
"PEDecl             ::= '<!ENTITY' S '%' S Name S PEDef <S maybe> '>'\n"
"# event EntityDef$ = completed EntityDef\n"
"EntityDef          ::= EntityValue\n"
"                     | ExternalID\n"
"                     | ExternalID NDataDecl\n"
"# event PEDef$ = completed PEDef\n"
"PEDef              ::= EntityValue | ExternalID\n"
"# event ExternalID$ = completed ExternalID\n"
"ExternalID         ::= 'SYSTEM' S SystemLiteral\n"
"                     | 'PUBLIC' S PubidLiteral S SystemLiteral\n"
"# event NDataDecl$ = completed NDataDecl\n"
"NDataDecl          ::= S 'NDATA' S Name\n"
"# event TextDecl$ = completed TextDecl\n"
"#\n"
"# Note: it is important to split '<?xml' into '<?' 'xml' because of PI whose defintion is: '<?' PITarget\n"
"#\n"
"TextDecl           ::= '<?' 'xml' <VersionInfo maybe> EncodingDecl <S maybe> '?>'\n"
"# event extParsedEnt$ = completed extParsedEnt\n"
"extParsedEnt       ::= <TextDecl maybe> content\n"
"# event EncodingDecl$ = completed EncodingDecl\n"
"EncodingDecl       ::= S 'encoding' Eq '\"' EncName '\"'                               ## Decl_action => ::copy[4]\n"
"                     | S 'encoding' Eq \"'\" EncName \"'\"                               ## Decl_action => ::copy[4]\n"
"# event EncName$ = completed EncName\n"
"EncName            ::= <EncName header> <EncName trailer any>\n"
"# event NotationDecl$ = completed NotationDecl\n"
"NotationDecl       ::= '<!NOTATION' S Name S ExternalID <S maybe> '>'\n"
"                     | '<!NOTATION' S Name S PublicID   <S maybe> '>'\n"
"# event PublicID$ = completed PublicID\n"
"PublicID           ::= 'PUBLIC' S PubidLiteral\n"
"\n"
"# event Misc_any$ = completed <Misc any>\n"
"<Misc any>                ::= Misc*\n"
"# event NameChar_any$ = completed <NameChar any>\n"
"<NameChar any>            ::= NameChar*\n"
"# event EntityValue1$ = completed <EntityValue1>\n"
"<EntityValue1>            ::= EntityValueDQInner\n"
"                            | PEReference\n"
"                            | Reference\n"
"# event EntityValue2$ = completed <EntityValue2>\n"
"<EntityValue2>            ::= EntityValueSQInner\n"
"                            | PEReference\n"
"                            | Reference\n"
"# event EntityValue1_any$ = completed <EntityValue1 any>\n"
"<EntityValue1 any>        ::= <EntityValue1>*\n"
"# event EntityValue2_any$ = completed <EntityValue2 any>\n"
"<EntityValue2 any>        ::= <EntityValue2>*\n"
"# event AttValue1$ = completed <AttValue1>\n"
"<AttValue1>               ::= AttValueDQInner\n"
"                            | Reference\n"
"# event AttValue2$ = completed <AttValue2>\n"
"<AttValue2>               ::= AttValueSQInner\n"
"                            | Reference\n"
"# event AttValue1_any$ = completed <AttValue1 any>\n"
"<AttValue1 any>           ::= <AttValue1>*\n"
"# event AttValue2_any$ = completed <AttValue2 any>\n"
"<AttValue2 any>           ::= <AttValue2>*\n"
"# event SystemLiteral1$ = completed <SystemLiteral1>\n"
"<SystemLiteral1>          ::= SystemLiteralDQInner\n"
"# event SystemLiteral2$ = completed <SystemLiteral2>\n"
"<SystemLiteral2>          ::= SystemLiteralSQInner\n"
"# event SystemLiteral1_any$ = completed <SystemLiteral1 any>\n"
"<SystemLiteral1 any>      ::= <SystemLiteral1>*\n"
"# event SystemLiteral2_any$ = completed <SystemLiteral2 any>\n"
"<SystemLiteral2 any>      ::= <SystemLiteral2>*\n"
"# event PubidChar1_any$ = completed <PubidChar1 any>\n"
"<PubidChar1 any>          ::= PubidChar*\n"
"# event PubidChar2$ = completed <PubidChar2>\n"
"<PubidChar2>              ::= [\\x{20}\\x{D}\\x{A}a-zA-Z0-9\\-()+,./:=?;!*#@$_%]  # Same as PubidChar but without '\n"
"# event PubidChar2_any$ = completed <PubidChar2 any>\n"
"<PubidChar2 any>          ::= <PubidChar2>*\n"
"# event XMLDecl_maybe$ = completed <XMLDecl maybe>\n"
"<XMLDecl maybe>           ::= XMLDecl\n"
"<XMLDecl maybe>           ::=\n"
"# event EncodingDecl_maybe$ = completed <EncodingDecl maybe>\n"
"<EncodingDecl maybe>      ::= EncodingDecl\n"
"<EncodingDecl maybe>      ::=\n"
"# event SDDecl_maybe$ = completed <SDDecl maybe>\n"
"<SDDecl maybe>            ::= SDDecl\n"
"<SDDecl maybe>            ::=\n"
"# event S_maybe$ = completed <S maybe>\n"
"<S maybe>                 ::= S\n"
"<S maybe>                 ::=\n"
"# event digit$ = completed <digit>\n"
"<digit>                   ::= [0-9]\n"
"# event digit_many$ = completed <digit many>\n"
"<digit many>              ::= <digit>+\n"
"# event hexdigit$ = completed <hexdigit>\n"
"<hexdigit>                ::= [0-9a-fA-F]\n"
"# event hexdigit_many$ = completed <hexdigit many>\n"
"<hexdigit many>           ::= <hexdigit>+\n"
"# event intSubset1$ = completed <intSubset1>\n"
"<intSubset1>              ::= markupdecl\n"
"                            | DeclSep\n"
"# event intSubset1_any$ = completed <intSubset1 any>\n"
"<intSubset1 any>          ::= <intSubset1>*\n"
"# event extSubsetDecl1$ = completed <extSubsetDecl1>\n"
"<extSubsetDecl1>          ::= markupdecl\n"
"                            | conditionalSect\n"
"                            | DeclSep\n"
"# event extSubsetDecl1_any$ = completed <extSubsetDecl1 any>\n"
"<extSubsetDecl1 any>      ::= <extSubsetDecl1>*\n"
"# event yes_or_no$ = completed <yes or no>\n"
"<yes or no>               ::= 'yes' | 'no'\n"
"# event STag1$ = completed <STag1>\n"
"<STag1>                   ::= S Attribute\n"
"# event STag1_any$ = completed <STag1 any>\n"
"<STag1 any>               ::= <STag1>*\n"
"# event CharData_maybe$ = completed <CharData maybe>\n"
"<CharData maybe>          ::= CharData\n"
"<CharData maybe>          ::=\n"
"# event content1$ = completed <content1>\n"
"<content1>                ::= element   <CharData maybe>\n"
"                            | Reference <CharData maybe>\n"
"                            | CDSect    <CharData maybe>\n"
"                            | PI        <CharData maybe>\n"
"                            | Comment   <CharData maybe>\n"
"# event content1_any$ = completed <content1 any>\n"
"<content1 any>            ::= <content1>*\n"
"# event EmptyElemTag1$ = completed <EmptyElemTag1>\n"
"<EmptyElemTag1>           ::= S Attribute\n"
"# event EmptyElemTag1_any$ = completed <EmptyElemTag1 any>\n"
"<EmptyElemTag1 any>       ::= <EmptyElemTag1>*\n"
"# event choice_or_seq$ = completed <choice or seq>\n"
"<choice or seq>           ::= choice | seq\n"
"# event sequence$ = completed <sequence>\n"
"<sequence>                ::= '?' | '*' | '+'\n"
"# event sequence_maybe$ = completed <sequence maybe>\n"
"<sequence maybe>          ::= <sequence>\n"
"<sequence maybe>          ::=\n"
"# event Name_or_choice_or_seq$ = completed <Name or choice or seq>\n"
"<Name or choice or seq>   ::= Name | choice | seq\n"
"# event choice1$ = completed <choice1>\n"
"<choice1>                 ::= <S maybe> '|' <S maybe> cp\n"
"# event choice1_many$ = completed <choice1 many>\n"
"<choice1 many>            ::= <choice1>+\n"
"# event seq1$ = completed <seq1>\n"
"<seq1>                    ::= <S maybe> ',' <S maybe> cp\n"
"# event seq1_any$ = completed <seq1 any>\n"
"<seq1 any>                ::= <seq1>*\n"
"# event Mixed1$ = completed <Mixed1>\n"
"<Mixed1>                  ::= <S maybe> '|' <S maybe> Name\n"
"# event Mixed1_any$ = completed <Mixed1 any>\n"
"<Mixed1 any>              ::= <Mixed1>*\n"
"# event AttDef_any$ = completed <AttDef any>\n"
"<AttDef any>              ::= AttDef*\n"
"# event NotationType1$ = completed <NotationType1>\n"
"<NotationType1>           ::= <S maybe> '|' <S maybe> Name\n"
"# event NotationType1_any$ = completed <NotationType1 any>\n"
"<NotationType1 any>       ::= <NotationType1>*\n"
"# event Enumeration1$ = completed <Enumeration1>\n"
"<Enumeration1>            ::= <S maybe> '|' <S maybe> Nmtoken\n"
"# event Enumeration1_any$ = completed <Enumeration1 any>\n"
"<Enumeration1 any>        ::= <Enumeration1>*\n"
"# event ignoreSectContents_any$ = completed <ignoreSectContents any>\n"
"<ignoreSectContents any>  ::= ignoreSectContents*\n"
"# event ignoreSectContents1$ = completed <ignoreSectContents1>\n"
"<ignoreSectContents1>     ::= '<![' ignoreSectContents ']]>' Ignore\n"
"# event ignoreSectContents1_any$ = completed <ignoreSectContents1 any>\n"
"<ignoreSectContents1 any> ::= <ignoreSectContents1>*\n"
"# event VersionInfo_maybe$ = completed <VersionInfo maybe>\n"
"<VersionInfo maybe>       ::= VersionInfo\n"
"<VersionInfo maybe>       ::=\n"
"# event TextDecl_maybe$ = completed <TextDecl maybe>\n"
"<TextDecl maybe>          ::= TextDecl\n"
"<TextDecl maybe>          ::=\n"
"# event EncName_header$ = completed <EncName header>\n"
"<EncName header>          ::= [A-Za-z]\n"
"# event EncName_trailer$ = completed <EncName trailer>\n"
"<EncName trailer>         ::= [A-Za-z0-9._-]\n"
"# event EncName_trailer_any$ = completed <EncName trailer any>\n"
"<EncName trailer any>     ::= <EncName trailer>*\n"
"\n"
"#############################\n"
"# Grammar subtilities\n"
"#############################\n"
"# event EntityValueDQInner$ = completed <EntityValueDQInner>\n"
"<EntityValueDQInner>      ::= [\\x{9}\\x{A}\\x{D}\\x{20}-\\x{21}\\x{23}-\\x{24}\\x{27}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]:u\n"
"# event EntityValueSQInner$ = completed <EntityValueSQInner>\n"
"<EntityValueSQInner>      ::= [\\x{9}\\x{A}\\x{D}\\x{20}-\\x{24}\\x{28}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]:u\n"
"# event AttValueDQInner$ = completed <AttValueDQInner>\n"
"<AttValueDQInner>         ::= /[\\x{9}\\x{A}\\x{D}\\x{20}-\\x{21}\\x{23}-\\x{25}\\x{27}-\\x{3b}\\x{3d}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]+/u\n"
"# event AttValueSQInner$ = completed <AttValueSQInner>\n"
"<AttValueSQInner>      ::= /[\\x{9}\\x{A}\\x{D}\\x{20}-\\x{25}\\x{28}-\\x{3b}\\x{3d}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]+/u\n"
"# event SystemLiteralDQInner$ = completed <SystemLiteralDQInner>\n"
"SystemLiteralDQInner      ::= /[\\x{9}\\x{A}\\x{D}\\x{20}-\\x{21}\\x{23}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]+/u\n"
"# event SystemLiteralSQInner$ = completed <SystemLiteralSQInner>\n"
"SystemLiteralSQInner      ::= /[\\x{9}\\x{A}\\x{D}\\x{20}-\\x{26}\\x{28}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]+/u\n"
"\n"
"#############################\n"
"# For element start detection\n"
"#############################\n"
":symbol ::= ELEMENT_START pause => before event => ^ELEMENT_START\n"
"ELEMENT_START               ~ '<'\n"
"\n"
"#############################\n"
"# For element valuation injected in parent recognizer\n"
"#############################\n"
"ELEMENT_VALUE               ~ [^\\s\\S]\n"
"\n"
"#########\n"
"# Lexemes\n"
"#########\n"
"# <_NAMESTARTCHAR>           ~ [:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}]:u\n"
"# <_NAMECHAR>                ~ [:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}\\-.0-9\\x{B7}\\x{0300}-\\x{036F}\\x{203F}-\\x{2040}]:u\n"
"# <_NAMECHAR any>            ~ <_NAMECHAR>*\n"
"# <_NAME>                    ~ <_NAMESTARTCHAR> <_NAMECHAR any>\n"
"\n"
"<_NAME>                    ~ /[:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}][:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}\\-.0-9\\x{B7}\\x{0300}-\\x{036F}\\x{203F}-\\x{2040}]*/u\n"
"# :symbol ::= NAME pause => after event => NAME$\n"
"<NAME>                     ~ <_NAME>\n"
"\n"
"################\n"
"# XML Exceptions\n"
"################\n"
"#\n"
"# -------------------------------------------------------------\n"
"# Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'\n"
"# -------------------------------------------------------------\n"
"#\n"
"# event Char_minus_sign$ = completed <Char minus sign>\n"
"<Char minus sign>       ::= [\\x{9}\\x{A}\\x{D}\\x{20}-\\x{2C}\\x{2E}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]:u  # '-' is \\x{2D}\n"
"# event Comment_Interior_Unit$ = completed <Comment Interior Unit>\n"
"<Comment Interior Unit> ::=     <Char minus sign>\n"
"                          | '-' <Char minus sign>\n"
"# event Comment_Interior$ = completed <Comment Interior>\n"
"<Comment Interior>      ::= <Comment Interior Unit>*\n"
"#\n"
"# -----------------------------------------------------------\n"
"# PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'\n"
"# -----------------------------------------------------------\n"
"#\n"
"# No need for exception, because '?>' is longer than Char\n"
"#\n"
"# event PI_Exceptioned$ = completed <PI Exceptioned>\n"
"<PI Exceptioned>        ::= Char*\n"
"#\n"
"# ---------------------------------------------------------\n"
"# PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))\n"
"# ---------------------------------------------------------\n"
"#\n"
"# The following is working, but we want this module to be\n"
"# more user-friendly, saying that a PITarget cannot be 'xml':i more explicitly.\n"
"# Since we will use events anyway because of SAX support, we add an explicit\n"
"## event for PITarget\n"
"<_XML>                     ~ [Xx] [Mm] [Ll]\n"
"# event PITarget$ = completed PITarget\n"
"<PITarget>              ::= <_NAME> - 'xml':i\n"
"\n"
"#\n"
"# If you like to handle this in user-space, this could be... with an event on PITarget$, then getting lastLexemePause('PITarget'):\n"
"#\n"
"# <NAMESTARTCHAR>           ~ [:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}]:u\n"
"# <NAMECHAR>                ~ [:A-Z_a-z\\x{C0}-\\x{D6}\\x{D8}-\\x{F6}\\x{F8}-\\x{2FF}\\x{370}-\\x{37D}\\x{37F}-\\x{1FFF}\\x{200C}-\\x{200D}\\x{2070}-\\x{218F}\\x{2C00}-\\x{2FEF}\\x{3001}-\\x{D7FF}\\x{F900}-\\x{FDCF}\\x{FDF0}-\\x{FFFD}\\x{10000}-\\x{EFFFF}\\-.0-9\\x{B7}\\x{0300}-\\x{036F}\\x{203F}-\\x{2040}]:u\n"
"# <NAMECHAR any>            ~ <NAMECHAR>*\n"
"# <NAME>                    ~ <NAMESTARTCHAR> <NAMECHAR any>\n"
"# :symbol ::= PITarget pause => after event => PITarget$\n"
"# <PITarget>                ~ <NAME>\n"
"\n"
"#\n"
"# ---------------------------------------\n"
"# CData ::= (Char* - (Char* ']]>' Char*))\n"
"# ---------------------------------------\n"
"#\n"
"# No need for exception, because ']]>' is longer than Char\n"
"#\n"
"# event CData_Exceptioned$ = completed <CData Exceptioned>\n"
"<CData Exceptioned>     ::= Char*\n"
"#\n"
"# ------------------------------------------------\n"
"# Ignore ::= Char+ - (Char+ ('<![' | ']]>') Char+)\n"
"# ------------------------------------------------\n"
"#\n"
"# Note that we made Ignore not nullable.\n"
"# No need for exception, because '<![' and ']]>' are longer than Char\n"
"#\n"
"# event Ignore_Exceptioned$ = completed <Ignore Exceptioned>\n"
"<Ignore Exceptioned>    ::= Char+\n"
"#\n"
"# -------------------------------------------\n"
"# CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)\n"
"# -------------------------------------------\n"
"#\n"
"# Note that we made CharData not nullable.\n"
"# No need for exception, because ']]>' is longer than <CharData Unit>\n"
"#\n"
"# All text that is not markup constitutes the character data of the document, and since\n"
"# a character data cannot contain markup characters (nor CDATA section-close delimiter)\n"
"# we raise its priority.\n"
"#\n"
"<_CHARDATA UNIT>          ~ [\\x{9}\\x{A}\\x{D}\\x{20}-\\x{25}\\x{26}-\\x{3b}\\x{3d}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]:u\n"
"<_CHARDATA UNIT ANY>      ~ <_CHARDATA UNIT>*\n"
"<CHARDATA>                ~ <_CHARDATA UNIT ANY>\n"
"#<CHARDATA>                ~ /[\\x{9}\\x{A}\\x{D}\\x{20}-\\x{25}\\x{26}-\\x{3b}\\x{3d}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]+/u\n"
"<CHARDATA EXCEPTION>      ~ /.*\\]\\]>/u  # Faster with a regexp, because it works on an already matched area: <CHARDATA>, so no need to rematch <_CHARDATA UNIT ANY>\n"
"\n"
"# :symbol ::= CHARDATA pause => after event => CHARDATA$ priority => 1\n"
"<CharData Exceptioned>  ::= <CHARDATA> - <CHARDATA EXCEPTION>\n"
"\n"
"#event CharData_Unit$ = completed <CharData Unit>\n"
"#<CharData Unit>         ::= [^<&]\n"
"#event CharData_Exceptioned$ = completed <CharData Exceptioned>\n"
"#<CharData Exceptioned>  ::= <CharData Unit>+\n"
"#\n"
  ;

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  genericLogger_t             *genericLoggerp;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.bytep               = (void *) xmls;
  marpaESLIFGrammarOption.bytel               = strlen(xmls);
  marpaESLIFGrammarOption.encodings           = UTF_8_STRING;
  marpaESLIFGrammarOption.encodingl           = strlen(UTF_8_STRING);
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  marpaESLIFRecognizerOption.userDatavp               = &marpaESLIFTester_context; /* User specific context */
  marpaESLIFRecognizerOption.readerCallbackp          = inputReaderb; /* Reader */
  marpaESLIFRecognizerOption.disableThresholdb        = 0; /* Default: 0 */
  marpaESLIFRecognizerOption.exhaustedb               = 0; /* Exhaustion event. Default: 0 */
  marpaESLIFRecognizerOption.newlineb                 = 1; /* Count line/column numbers. Default: 0 */
  marpaESLIFRecognizerOption.trackb                   = 1; /* Absolute position tracking. Default: 0 */
  marpaESLIFRecognizerOption.bufsizl                  = 0; /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  marpaESLIFRecognizerOption.buftriggerperci          = 50; /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  marpaESLIFRecognizerOption.bufaddperci              = 50; /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
  marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
  marpaESLIFRecognizerOption.eventActionResolverp     = NULL;
  marpaESLIFRecognizerOption.regexActionResolverp     = NULL;
  marpaESLIFRecognizerOption.generatorActionResolverp = NULL;

  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         =
"<?xml version=\"1.0\"?>\n"
"<?xml-stylesheet type=\"text/xsl\" href=\"style.xsl\"?>\n"
"\n"
"<gutenprint xmlns=\"http://gimp-print.sourceforge.net/xsd/gp.xsd-1.0\"\n"
"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
"xsi:schemaLocation=\"http://gimp-print.sourceforge.net/xsd/gp.xsd-1.0 gutenprint.xsd\">\n"
"<copyright>\n"
" *   Copyright 2008 Robert Krawitz (rlk@alum.mit.edu)\n"
" *\n"
" *   This program is free software; you can redistribute it and/or modify it\n"
" *   under the terms of the GNU General Public License as published by the Free\n"
" *   Software Foundation; either version 2 of the License, or (at your option)\n"
" *   any later version.\n"
" *\n"
" *   This program is distributed in the hope that it will be useful, but\n"
" *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\n"
" *   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License\n"
" *   for more details.\n"
" *\n"
" *   You should have received a copy of the GNU General Public License\n"
" *   along with this program; if not, write to the Free Software\n"
" *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.\n"
"</copyright>\n"
"<escp2PrinterWeaves name=\"standard\">\n"
" <weave translate=\"text\" name=\"Off\" text=\"Off\" command=\"\\033(i\\001\\000\\000\"/>\n"
" <weave translate=\"text\" name=\"On\" text=\"On\" command=\"\\033(i\\001\\000\\001\"/>\n"
"</escp2PrinterWeaves>\n"
"</gutenprint>\n"
    ;
  marpaESLIFTester_context.inputl         = strlen(marpaESLIFTester_context.inputs);

  /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
  exiti = marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */) ? 0 : 1;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 0; /* We say this is not a stream of characters - regexp will adapt and to UTF correctness if needed */
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;

  return 1;
}
