#!env perl
use strict;
use POSIX qw/EXIT_SUCCESS EXIT_FAILURE/;
use Data::Dumper;
use Getopt::Long;

my $rulefile;
my $mapfile;
my $outfile;
my $logfile;
my $before = '';
my $after = '';
my $prefix = '';
my $postfix = '_t';
my $impl = '_impl';
my $errlogger = '';
my $tracelogger = '';
my $tracecond = '';
my $NA = 1;
my $help = 0;
if (! GetOptions("rule=s"          => \$rulefile,
                 "map=s"           => \$mapfile,
                 "before=s"        => \$before,
                 "after=s"         => \$after,
                 "prefix=s"        => \$prefix,
                 "postfix=s"       => \$postfix,
                 "errlogger=s"     => \$errlogger,
                 "tracelogger=s"   => \$tracelogger,
                 "tracecond=s"     => \$tracecond,
                 "outfile=s"       => \$outfile,
                 "logfile=s"       => \$logfile,
                 "NA!"             => \$NA,
                 "help!"           => \$help)) {
  exit(EXIT_FAILURE);
}

if ($help || ! $rulefile || ! $mapfile || ! $impl) {
  print <<USAGE;
  Usage: $0 [options]

  where options can be:

  --rule          rulefile       Load methods configuration from rulefile. Default: undef.
  --map           mapfile        Load type mappings from mapfile. Default: undef.
  --before        method         Name of a method guaranteed to be called before processing, with this signature: short method(void *userDatavp, char *methodname). Must return true for processing to happen. Default is none.
  --after         method         Name of a method guaranteed to be called after processing, with this signature: short method(void *userDatavp, char *methodname). Must return true to indicate success. Default is none.
  --prefix        typeprefix     String preprended to every generated type. Default: "$prefix".
  --postfix       typepostfix    String appended to every generated type. Default: "$postfix".
  --impl          implprefix     String preprended to every method that use have to implement. Cannot be empty. Default: "$impl".
  --outfile       filepath       Output. If not specified, it goes to STDOUT.
  --logfile       filepath       Log. If not specified, it goes to STDERR.
  --errlogger     method         If user context is logger aware, the name of method which accept the following arguments: method(void *userDatavp, char *fmts, ...). Eventual returns value does not matter. Default is none.
  --tracelogger   method         If user context is logger aware, the name of method which accept the following arguments: method(void *userDatavp, char *fmts, ...). Eventual returns value does not matter. Default is none.
  --tracecond     condition      A preprocessor condition to generate TRACE logs via the genericLogger. Default is none.
  --NA                           Method outputing NA and having all parameters as NA are asumed to be meaningless and generated automatially to return success. Say --no-NA to implement them anyway. Default is true.
  --help                         This help

  Example:

  perl $0 --rule etc/ESLIF.rules --map etc/ESLIF.map --prefix marpaESLIF_bootstrap --errlogger marpaESLIF_bootstrap_logerrorv --tracelogger marpaESLIF_bootstrap_logtracev --tracecond "#ifndef MARPAESLIF_NTRACE"
USAGE
  exit(EXIT_SUCCESS);
}

my $out;
if ($outfile) {
  open($out, '>', $outfile) || die "$outfile, $!";
} else {
  $out = \*STDOUT;
}

my $log;
if ($logfile) {
  open($log, '>', $logfile) || die "$logfile, $!";
} else {
  $log = \*STDERR;
}

my @rule = ();
my @symbol = ();
my @nullable = ();
my %type = ();
my %map = ();

_print([$log], "-------------------------------------------------\n");
_print([$log], "Methods to be writen by hand will be listed here:\n");
_print([$log], "-------------------------------------------------\n");

loadRule($rulefile, \@rule, \@symbol, \@nullable, \%type);
die "No type detected" unless %type;
die "No rule detected" unless @rule;

loadMap($mapfile, \%map);
die "No map detected" unless %map;

checkType(\%type, \%map);
checkRule(\@rule, \%map);
generate(\@rule, \@symbol, \@nullable, \%type, \%map);

exit(EXIT_SUCCESS);


if ($outfile) {
  close($out) || warn "$out, $!";
}
if ($logfile) {
  close($log) || warn "$log, $!";
}

sub loadRule {
  my ($file, $rulep, $symbolp, $nullablep, $typep) = @_;

  open(my $fh, '<', $file) || die "$file: $!";
  my $line = 0;
  while (chomp($_ = <$fh>)) {
    ++$line;
    #
    # We expect date to contain lines in the forms:
    # TYPE = METHOD(TYPE*)
    # or
    # TYPE ~ METHOD()
    # or
    # TYPE ~ METHOD
    #
    # First one is for rules.
    # Second is for symbols.
    # Third is for nullables.
    #
    # Perl'like comment lines are ignored
    # Space-only lines are ignored
    #
    # where a type is writen like this:
    # whatYouWant, or
    # whatYouWant [Quantifier]
    #
    # A Quantifier can be:
    # - '*' or nothing for an array of unknown size
    # - '+' for an array of size >= 1
    # - [N] for an array of N elements
    # - [N1..N2] for an array of minimum N1 to maximum N2 elements
    # - [N1..] for an array of minimum N1
    # - [..N2] for an array of maximum N2
    #
    # Please note that, ouf course, saying X[1] is different that X, as in C: the
    # first case is an array of size 1, the second case is X directly on the stack.
    #
    # Multiline is not supported: you have to write the prototype entirely.
    #
    my $input = $_;
    next if ($input =~ /^\s*#/);
    next if ($input =~ /^\s*$/);
    if ($input =~ m/^\s*(\w+)\s*=\s*(\w+)\s*\((.*)\)\s*;?\s*$/smg) {
      my ($outtype, $method, $intypes) = ($1, $2, $3 || '');
      $typep->{$outtype}++;
      $intypes =~ s/\s+//g;
      my @intypes = ();
      foreach my $item (split(/,/, $intypes)) {
        my ($intype, $minmax);

        die "$file.line: Failed to parse $input\n" if (! $item);
        if ($item =~ /^(\w+)$/) {
          ($intype, $minmax) = ($1, undef);
        } elsif ($item =~ /^(\w+)\+$/) {
          ($intype, $minmax) = ($1, [ 1, undef ]);
        } elsif ($item =~ /^(\w+)\*$/) {
          ($intype, $minmax) = ($1, [ 0, undef ]);
        } elsif ($item =~ /^(\w+)\[(\d+)\]$/) {
          ($intype, $minmax) = ($1, [ $2, $2 ]);
        } elsif ($item =~ /^(\w+)\[(\d+)\.\.(\d+)\]$/) {
          ($intype, $minmax) = ($1, [ $2, $3 ]);
        } elsif ($item =~ /^(\w+)\[(\d+)\.\.\]$/) {
          ($intype, $minmax) = ($1, [ $2, undef ]);
        } elsif ($item =~ /^(\w+)\[\.\.(\d+)\]$/) {
          ($intype, $minmax) = ($1, [ undef, $2 ]);
        } else {
          die "$file.$line: Problem with $item";
        }
        $typep->{$intype}++;
        push(@intypes, { type => $intype, minmax => $minmax });
      }
      push(@{$rulep}, { outtype => $outtype, method => $method, intypes => \@intypes });
    } elsif ($input =~ m/^\s*(\w+)\s*~\s*(\w+)\s*\(\s*\)\s*;?\s*$/smg) {
      my ($outtype, $method) = ($1, $2);
      $typep->{$outtype}++;
      push(@{$symbolp}, { outtype => $outtype, method => $method });
    } elsif ($input =~ m/^\s*(\w+)\s*~\s*(\w+)\s*;?\s*$/smg) {
      my ($outtype, $method) = ($1, $2);
      $typep->{$outtype}++;
      push(@{$nullablep}, { outtype => $outtype, method => $method });
    } else {
      die "$file.$line: not parsable";
    }
  }
  close($fh) || warn "$fh, $!";
}

sub loadMap {
  my ($file, $mapp) = @_;

  open(my $fh, '<', $file) || die"$file: $!";
  my $line = 0;
  while (chomp($_ = <$fh>)) {
    ++$line;
    #
    # We expect data to contain lines in the forms:
    # TYPE = STACKTYPE
    #
    # or
    #
    # TYPE = STACKTYPE{Ctype}
    #
    # Perl'like comment lines are ignored
    # Space-only lines are ignored
    #
    # where:
    # TYPE is what you want, should appear in the rule config file.
    # STACKTYPE is one supported by genericStack.h (INT, SHORT, LONG, PTR, ARRAY, etc...)
    # Ctype is the real C type behind.
    #
    # When Ctype is not specified, then the type is forced to GENERICSTACKITEMTYPE2TYPE_xxx where xxx is STACKTYPE,
    # unless if STACKTYPE is PTR, then it is forced to TYPE*.
    # STACKTYPE being equal to NA is translated to (void *)-1 in C terms
    # It is illegal to have more than occurence of TYPE per line
    #
    # Multiline is not supported: you have to write the prototype entirely.
    #
    my $input = $_;
    next if ($input =~ /^\s*#/);
    next if ($input =~ /^\s*$/);
    if ($input =~ m/^\s*(\w+)\s*=\s*(\w+)(?:\s*\{(.*)\s*\})?\s*;?\s*$/smg) {
      die "Type $1 already defined upper" if (exists($mapp->{$1}));
      $mapp->{$1} = {basictype => $2, stacktype => basictype2stacktype($2), ctype => $3 || basictype2ctype($2) };
    } else {
      die "$file.line: not parsable";
    }
  }
  close($fh) || warn "$fh, $!";
}

sub checkType {
  my ($typep, $mapp) = @_;

  foreach (sort keys %{$typep}) {
    die "Type $_ is not defined in the $mapfile" unless exists($mapp->{$_});
  }
}

sub generate {
  my ($rulep, $symbolp, $nullablep, $typep, $mapp) = @_;

  generateHeader();
  generateEnum($typep);
  generateDependencies($mapp);
  generateStackManagerConstrustor();
  generateStackManagerDestructor();
  generateStackManagerGettype($typep, $mapp);
  generateStackManagerTrace($typep, $mapp);
  generateMacros($rulep, $symbolp, $nullablep, $typep, $mapp);
  generateRuleWrapper($rulep, $symbolp, $nullablep, $typep, $mapp);
  generateRuleImpl($rulep, $typep, $mapp);
}

sub generateHeader {
  #
  # First generate an enum for all the types
  #
  my $now = localtime;
  _print([$out], "
/* This is a generated file as of $now */

#include <stdlib.h>        /* malloc, etc. */
#include <errno.h>         /* errno */
#include <string.h>        /* strerror(errno) */
#include <genericStack.h>  /* generic stack */
");
}

sub generateEnum {
  my ($typep) = @_;

  _print([$out], "
/* -------------------------------------------- */
/* Every item in the stack is explicitly tagged */
/* -------------------------------------------- */
typedef enum ${prefix}_itemType {
");
  my $i = 0;
  _print([$out], "  " . join(",\n  ", map { sprintf("%s_itemType_%s%s", ${prefix}, $_, ($i++ == 0) ? " = 1" : "") } sort keys %{$typep}));
  _print([$out], "

} ${prefix}_itemType_t;
");
}

sub generateDependencies {
  my ($mapp) = @_;

  _print([$log], "
/* ----------------------------------- */
/* Methods that have to be implemented */
/* ----------------------------------- */
/* Free methods */\n");
  foreach (grep { $mapp->{$_}->{basictype} eq 'PTR' } sort keys %{$mapp}) {
    _print([$out,$log], "static void %s(void *userDatavp, %s%s);\n", x2meth($_, 'freev'), $mapp->{$_}->{ctype}, $_);
  }
  _print([$log], "
/* Clone methods */\n");
  foreach (grep { $mapp->{$_}->{basictype} eq 'PTR' } sort keys %{$mapp}) {
    _print([$out,$log], "static %s%s(void *userDatavp, %s%s);\n", $mapp->{$_}->{ctype}, x2meth($_, 'clonep'), $mapp->{$_}->{ctype}, $_);
  }
}

sub basictype2ctype {
  my ($basictype) = @_;
  our %BASICTYPE2CTYPE = (
                          'CHAR' => 'char',
                          'SHORT' => 'short',
                          'INT' => 'int',
                          'LONG' => 'long',
                          'FLOAT' => 'float',
                          'DOUBLE' => 'double',
                          'PTR' => 'void *',
                          'LONG_LONG' => 'long long',
                          '_BOOL' => '_Bool',
                          'FLOAT_COMPLEX' => 'float _Complex',
                          'DOUBLE_COMPLEX' => 'double _Complex',
                          'LONG_DOUBLE_COMPLEX' => 'long double _Complex',
                          'NA' => 'void'
                         );
  return $BASICTYPE2CTYPE{$basictype} || die "Unsupported type $basictype when converting to a C type, should be one of: " . join(', ', sort keys %BASICTYPE2CTYPE);
}

sub basictype2stacktype {
  my ($basictype) = @_;
  our %BASICTYPE2STACKTYPE = (
                              'CHAR' => 'GENERICSTACKITEMTYPE2TYPE_CHAR',
                              'SHORT' => 'GENERICSTACKITEMTYPE2TYPE_SHORT',
                              'INT' => 'GENERICSTACKITEMTYPE2TYPE_INT',
                              'LONG' => 'GENERICSTACKITEMTYPE2TYPE_LONG',
                              'FLOAT' => 'GENERICSTACKITEMTYPE2TYPE_FLOAT',
                              'DOUBLE' => 'GENERICSTACKITEMTYPE2TYPE_DOUBLE',
                              'PTR' => 'GENERICSTACKITEMTYPE2TYPE_PTR',
                              'LONG_LONG' => 'GENERICSTACKITEMTYPE2TYPE_LONG_LONG',
                              '_BOOL' => 'GENERICSTACKITEMTYPE2TYPE__BOOL',
                              'FLOAT_COMPLEX' => 'GENERICSTACKITEMTYPE2TYPE_FLOAT__COMPLEX',
                              'DOUBLE_COMPLEX' => 'GENERICSTACKITEMTYPE2TYPE_DOUBLE__COMPLEX',
                              'LONG_DOUBLE_COMPLEX' => 'GENERICSTACKITEMTYPE2TYPE_LONG_DOUBLE__COMPLEX',
                              'NA' => 'GENERICSTACKITEMTYPE2TYPE_NA'
                             );
  return $BASICTYPE2STACKTYPE{$basictype} || die "Unsupported type $basictype when converting to a stack type, should be one of: " . join(', ', sort keys %BASICTYPE2STACKTYPE);
}
sub x2meth { return sprintf("${prefix}_%s_%s", @_) }
sub stackManager {
  return "struct stackManager {
  genericStack_t *dataStackp;
  genericStack_t *typeStackp;
}";
}
sub stackManager_t { return "${prefix}_stackManager_t" }
sub stackManager_newp { return "${prefix}_stackManager_newp" }
sub stackManager_freev { return "${prefix}_stackManager_freev" }
sub stackManager_i_tracev { return "${prefix}_stackManager_i_tracev" }
sub stackManager_i_gettypei { return "${prefix}_stackManager_i_gettypei" }
sub stackManager_error {
  my $indent = shift;
  if ($errlogger) {
    _print([$out], "%*s$errlogger(%s, %s);\n", $indent, " ", "userDatavp", join(', ', @_));
  }
}
sub stackManager_trace {
  my $indent = shift;
  if ($tracelogger) {
    if ($tracecond && $indent > 0) {
      _print([$out], "$tracecond\n");
    }
    _print([$out], "%*s$tracelogger(%s, %s);\n", $indent, " ", "userDatavp", join(', ', @_));
    if ($tracecond && $indent > 0) {
      _print([$out], "#endif /* $tracecond */\n");
    }
  }
}

sub generateStackManagerConstrustor {
  _print([$out], "

/* ------------------------- */
/* Stack Manager constructor */
/* ------------------------- */
");
  _print([$out], "static %s *%s(void *userDatavp) {\n", stackManager_t(), stackManager_newp());
  _print([$out], "  %s *stackManagerp;\n", stackManager_t());
  _print([$out], "\n");
  _print([$out], "  stackManagerp = (%s *) malloc(sizeof(%s));\n", stackManager_t(), stackManager_t());
  _print([$out], "  if (stackManagerp == NULL) {\n");
  stackManager_error(4, '"malloc failure, %s"', "strerror(errno)");
  _print([$out], "    goto err;\n");
  _print([$out], "  }\n");
  _print([$out], "\n");
  _print([$out], "  stackManagerp->dataStackp = NULL;\n");
  _print([$out], "  stackManagerp->typeStackp = NULL;\n");
  _print([$out], "\n");
  _print([$out], "  GENERICSTACK_INIT(stackManagerp->dataStackp);\n");
  _print([$out], "  if (GENERICSTACK_ERROR(stackManagerp->dataStackp)) {\n");
  stackManager_error(4, '"stackManagerp->dataStackp initializationfailure, %s"', "strerror(errno)");
  _print([$out], "    goto err;\n");
  _print([$out], "  }\n");
  _print([$out], "\n");
  _print([$out], "  GENERICSTACK_INIT(stackManagerp->typeStackp);\n");
  _print([$out], "  if (GENERICSTACK_ERROR(stackManagerp->typeStackp)) {\n");
  stackManager_error(4, '"stackManagerp->typeStackp initializationfailure, %s"', "strerror(errno)");
  _print([$out], "    goto err;\n");
  _print([$out], "  }\n");
  _print([$out], "  goto done;\n");
  _print([$out], "\n");
  _print([$out], " err:\n");
  _print([$out], "  %s(userDatavp, stackManagerp);\n", stackManager_freev());
  _print([$out], "  stackManagerp = NULL;\n");
  _print([$out], "\n");
  _print([$out], " done:\n");
  _print([$out], "  return stackManagerp;\n");
  _print([$out], "}\n");
}

sub generateStackManagerDestructor {
  _print([$out], "

/* ------------------------- */
/* Stack Manager destructor */
/* ------------------------- */
");
  _print([$out], "static void %s(void *userDatavp, %s *stackManagerp) {\n", stackManager_freev(), stackManager_t());
  _print([$out], "
  int i;
  int usedi;

  if (stackManagerp != NULL) {
    if (stackManagerp->dataStackp != NULL) {
      usedi = GENERICSTACK_USED(stackManagerp->dataStackp);
      for (i = 0; i < usedi; i++) {
      }
    }
    if (stackManagerp->typeStackp != NULL) {
      GENERICSTACK_FREE(stackManagerp->typeStackp); /* It contain only enums, nothing on the heap */
    }
    free(stackManagerp);
  }
}
");
}

sub generateStackManagerGettype {
  my ($typep, $mapp) = @_;
  _print([$out], "\n");
  _print([$out], "
/* ------------------------- */
/* Stack Manager type getter */
/* ------------------------- */
");
  _print([$out], "static short %s(void *userDatavp, %s *stackManagerp, int i, ${prefix}_itemType_t *itemTypep) {\n", stackManager_i_gettypei(), stackManager_t());
  _print([$out], "  ${prefix}_itemType_t itemType;\n");
  _print([$out], "  genericStack_t *typeStackp;\n");
  _print([$out], "  short rcb;

  if (stackManagerp == NULL) {\n");
  stackManager_error(4, '"stackManagerp is %s"', "\"NULL\"");
  _print([$out], "    goto err;
  }
  typeStackp = stackManagerp->typeStackp;
  if (typeStackp == NULL) {\n");
  stackManager_error(4, '"typeStackp is %s"', "\"NULL\"");
  _print([$out], "    goto err;
  }
  if (GENERICSTACK_IS_INT(typeStackp, i)) {
    itemType = (${prefix}_itemType_t) GENERICSTACK_GET_INT(typeStackp, i);
    switch (itemType) {
");
  foreach (sort keys %{$typep}) {
    my $isPtr = $mapp->{$_}->{basictype} eq 'PTR';
    my $case = sprintf("%s_itemType_%s", ${prefix}, $_);
    _print([$out], "    case  $case:\n");
    if ($isPtr) {
      _print([$out], "    case -$case: /* shallow copy */\n");
    }
  }
  _print([$out], "      break;\n");
  _print([$out], "    default:\n");
  stackManager_error(6, '"itemType is unknown (%d))"', "itemType");
  _print([$out], "      goto err;\n");
  _print([$out], "    }
  } else {\n");
  stackManager_error(4, '"GENERICSTACK_IS_INT(typeStackp, %d) returned false (type is %d instead of %d)"', "i", "GENERICSTACKITEMTYPE(stackName, i)", "GENERICSTACKITEMTYPE_INT");
  _print([$out], "      goto err;\n");
  _print([$out], "  }

  *itemTypep = itemType;
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}\n");
}

sub generateStackManagerTrace {
  my ($typep, $mapp) = @_;
  _print([$out], "
/* ------------------- */
/* Stack Manager trace */
/* ------------------- */");
  if ($tracelogger) {
    _print([$out], "\n");
    if ($tracecond) {
      _print([$out], "$tracecond\n");
    }
    _print([$out], "static void %s(void *userDatavp, %s *stackManagerp, int i) {\n", stackManager_i_tracev(), stackManager_t());
    _print([$out], "  ${prefix}_itemType_t itemType;\n");
    _print([$out], "  genericStack_t *typeStackp;\n");
    _print([$out], "

  if (stackManagerp != NULL) {
    typeStackp = stackManagerp->typeStackp;
    if (typeStackp != NULL) {
      if (GENERICSTACK_IS_INT(typeStackp, i)) {
        itemType = (${prefix}_itemType_t) GENERICSTACK_GET_INT(typeStackp, i);
        switch (itemType) {
");
    foreach (sort keys %{$typep}) {
      my $isPtr = $mapp->{$_}->{basictype} eq 'PTR';
      my $basecase = $_;
      my $case = sprintf("%s_itemType_%s", ${prefix}, $_);
      _print([$out], "        case  $case:\n");
      stackManager_trace(-10, '"stack[%d] type is ' . $basecase . '"', "i");
      _print([$out], "          break;\n");
      if ($isPtr) {
        _print([$out], "        case -$case:\n");
        stackManager_trace(-10, '"stack[%d] type is ' . $basecase . ' (shallow copy)"', "i");
        _print([$out], "          break;\n");
      }
    }
    _print([$out], "        default:\n");
    stackManager_trace(-10, '"stack[%d] type is unknown"', "i");
    _print([$out], "          break;\n");
    _print([$out], "
        }
      }
    }
  }
");
    _print([$out], "}\n");
    if ($tracecond) {
      _print([$out], "#endif /* $tracecond */\n");
    }
  }
}

sub _print {
  my ($handlesp, $fmt, @args) = @_;
  foreach my $handle (@{$handlesp}) {
    printf $handle $fmt, @args;
  }
}

sub checkRule {
  my ($rulep, $mapp) = @_;
  #
  # For every rule, we generate description, prototype, atLeastOneTypeisNotNA flag
  foreach my $rule (@{$rulep}) {
    my ($outtype, $method, $intypes) = ($rule->{outtype}, $rule->{method}, $rule->{intypes});
    my $description = sprintf(
                              "/* %s = %s(%s) */\n",
                              $outtype,
                              $method,
                              join(', ',
                                   map {
                                     $_->{type} . ($_->{minmax} ? "[" . ($_->{minmax}->[0] // 'undef') . "," . ($_->{minmax}->[1] // 'undef') . "]" : "")
                                   } @{$intypes}));
    $rule->{description} = $description;

    my $atLeastOneTypeisNotNA = ($mapp->{$outtype}->{basictype} eq 'NA') ? 0 : 1;
    $rule->{atLeastOneTypeisNotNA} = $atLeastOneTypeisNotNA;

    my @cintypes = ();
    my $intypes_i = 0;
    foreach (@{$intypes}) {
      my ($intype, $minmax) = ($_->{type}, $_->{minmax});
      if ($mapp->{$intype}->{basictype} eq 'NA') {
        push(@cintypes, "void *NA${intypes_i}p");
      } else {
        $atLeastOneTypeisNotNA++;
        if (! $_->{minmax}) {
          push(@cintypes, $mapp->{$intype}->{ctype} . " $intype");
        } else {
          push(@cintypes, "genericStack_t *$intype");
        }
      }
      $intypes_i++;
    }
    my $prototype = sprintf(
                            "static short %s(void *userDatavp, %s *%s, %s)",
                            $method, $mapp->{$outtype}->{ctype},
                            ($mapp->{$outtype}->{basictype} eq 'NA') ? 'out_NAp' : "out_${outtype}p",
                            join(', ', @cintypes)
                           );
    $rule->{prototype} = $prototype;
  }
}

sub generateMacros {
  my ($rulep, $symbolp, $nullablep, $typep, $mapp) = @_;

  foreach my $type (sort keys %{$typep}) {
  _print([$out], "\n");
  _print([$out], "/* ------------------------------------------\n");
  _print([$out], "   %s\n", $type);
  _print([$out], "\n");
  _print([$out], "   genericStack type: %s\n", $mapp->{$type}->{stacktype});
  _print([$out], "              C type: %s\n", $mapp->{$type}->{ctype});
  _print([$out], "   ------------------------------------------ */\n");
  _print([$out], "#define ${prefix}_DECL_%s(identifier) %s identifier\n", $type, $mapp->{$type}->{ctype});
  _print([$out], "#define ${prefix}_GET_%s(identifier, indice) do { \\\n", $type);
  _print([$out], "  ${prefix}_itemType_t _get_itemType; \\\n");
  _print([$out], "  %s *_get_stackManagerp \\\n", stackManager_t);
  _print([$out], "  if (! %s(userDatavp, _get_stackManagerp, (indice), &_get_itemType)) { \\\n", stackManager_i_gettypei);
  _print([$out], "    goto err; \\\n");
  _print([$out], "  } \\\n");
  my $isPtr = $mapp->{$type}->{basictype} eq 'PTR';
  my $case = sprintf("%s_itemType_%s", ${prefix}, $type);
  if ($isPtr) {
    _print([$out], "  if ((_get_itemType != $case) && (itemType != -$case)) { \\\n");
    stackManager_error(4, '"stack[%d] item type is %d instead of %d or %d"', "indice", "itemType", "$case", "-$case");
  } else {
    _print([$out], "  if (_get_itemType != $case) { \\\n");
    stackManager_error(4, '"stack[%d] item type is %d instead of %d"', "indice", "itemType", "$case");
  }

  if ($tracelogger) {
  }
  _print([$out], "    goto err; \\\n");
  _print([$out], "  } \\\n");
  _print([$out], "} while (0)\n");
  }
}

sub generateRuleWrapper {
  my ($rulep, $symbolp, $nullablep, $typep, $mapp) = @_;

  _print([$out], "\n");
  _print([$out], "/* Rule methods wrapper implementation (stack machinery) */\n");
  #
  # The rules
  #
  foreach (0..$#{$rulep}) {
    my $rule = $rulep->[$_];

    # my @handles = ($NA && ! $atLeastOneTypeisNotNA) ? ($out) : ($out,$log);
    # _print(\@handles, "%s;\n", $prototype);
    # if ($NA && ! $atLeastOneTypeisNotNA) {
      #
      # This method is outputing NA, with input all NAs - if it is called it is assumed to be meaningless
      #
    #   _print([$out], "%s { return 1; } /* Everything is NA: assumed to be meaningless */\n", $prototype);
    # }
    _print([$out], "%s {\n", $rule->{prototype});
    _print([$out], "}\n");
  }
  #
  # The symbols
  #
  # The nullables
  #
}

sub generateRuleImpl {
  my ($rulep, $typep, $mapp) = @_;

  #
  # First round is printing out the description and prototypes
  #
  _print([$out], "\n");
  _print([$out], "/* Rule methods prototypes */\n");
  foreach my $rule (@{$rulep}) {
    _print([$out], "%s%s;\n", $rule->{prototype});
  }
  #
  # Second round is generating the stack machinery in these wrappers
  #
  _print([$out], "\n");
  _print([$out], "/* Rule methods internal implementation (stack machinery) */\n");
  foreach (0..$#{$rulep}) {
    my $rule = $rulep->[$_];

    # my @handles = ($NA && ! $atLeastOneTypeisNotNA) ? ($out) : ($out,$log);
    # _print(\@handles, "%s;\n", $prototype);
    # if ($NA && ! $atLeastOneTypeisNotNA) {
      #
      # This method is outputing NA, with input all NAs - if it is called it is assumed to be meaningless
      #
    #   _print([$out], "%s { return 1; } /* Everything is NA: assumed to be meaningless */\n", $prototype);
    # }
    _print([$out], "%s {\n", $rule->{prototype});
    _print([$out], "}\n");
  }
}

