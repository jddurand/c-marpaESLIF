#!env perl
use strict;
use POSIX qw/EXIT_SUCCESS EXIT_FAILURE/;
use Data::Dumper;
use Getopt::Long;

my $rulefile;
my $mapfile;
my $outfile;
my $safe = 0;
my $prefix = '';
my $postfix = '_t';
my $errlogger = '';
my $tracelogger = '';
my $tracecond = '';
my $help = 0;
if (! GetOptions("rule=s"          => \$rulefile,
                 "map=s"           => \$mapfile,
                 "safe!"           => \$safe,
                 "prefix=s"        => \$prefix,
                 "postfix=s"       => \$postfix,
                 "errlogger=s"     => \$errlogger,
                 "tracelogger=s"   => \$tracelogger,
                 "tracecond=s"     => \$tracecond,
                 "out=s"           => \$outfile,
                 "help!"           => \$help)) {
  exit(EXIT_FAILURE);
}

if ($help || ! $rulefile || ! $mapfile) {
  print <<USAGE;
  Usage: $0 [options]

  where options can be:

  --rule          rulefile       Load methods configuration from rulefile. Default: undef.
  --map           mapfile        Load type mappings from mapfile. Default: undef.
  --safe                         Safe mode. Generates extra processing by tagging every item on the stack. Default: false.
  --prefix        typeprefix     String preprended to every generated type. Default: "$prefix".
  --postfix       typepostfix    String appended to every generated type. Default: "$postfix".
  --out           outfile        Output. If not specified, it goes to STDOUT.
  --errlogger     method         If user context is logger aware, the name of method which accept the following arguments: method(void *userDatavp, char *fmts, ...). Eventual returns value does not matter. Default is none.
  --tracelogger   method         If user context is logger aware, the name of method which accept the following arguments: method(void *userDatavp, char *fmts, ...). Eventual returns value does not matter. Default is none.
  --tracecond     condition      A preprocessor condition to generate TRACE logs via the genericLogger. Default is none.
  --help                         This help

  Example:

  perl $0 --rule etc\ESLIF.rules --map etc\ESLIF.map --prefix marpaESLIF --errlogger JDDerr --tracelogger JDDtrace --tracecond "#ifndef JDD"
USAGE
  exit(EXIT_SUCCESS);
}

my $out;
if ($outfile) {
  open($out, '>', $out) || die "$out, $!";
} else {
  $out = \*STDOUT;
}

my @rule = ();
my %type = ();
my %map = ();

loadRule($rulefile, \@rule, \%type);
loadMap($mapfile, \%map);
checkType(\%type, \%map);
generate(\%type, \%map);

die "No type detected" unless %type;

if ($outfile) {
  close($out, '>', $out) || warn "$out, $!";
}

sub loadRule {
  my ($file, $rulep, $typep) = @_;

  open(my $fh, '<', $file) || die "$file: $!";
  my $line = 0;
  while (chomp($_ = <$fh>)) {
    ++$line;
    #
    # We expect data to contain lines in the forms:
    # TYPE = METHOD(TYPE*)
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
    if ($input =~ m/^\s*(\w+)\s*=\s*(\w+)\s*\((.*)\)\s*;$/smg) {
      my ($outtype, $method, $intypes) = ($1, $2, $3 || '');
      $typep->{$outtype}++;
      $intypes =~ s/\s+//g;
      my @intypes = ();
      foreach my $item (split(/,/, $intypes)) {
        my ($intype, $minmax);

        die "Problem at line $line: $input\n" if (! $item);
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
          die "Problem with $item at line $line: $_\n";
        }
        $typep->{$intype}++;
        push(@intypes, { type => $intype, minmax => $minmax });
      }
      push(@{$rulep}, { outtype => $outtype, method => $method, intypes => \@intypes });
    } else {
      die "Problem with line $line: $_\n";
    }
  }
  close($fh) || warn "$fh, $!";
}

sub loadMap {
  my ($file, $mapp) = @_;

  open(my $fh, '<', $file) || die "$file: $!";
  my $line = 0;
  while (chomp($_ = <$fh>)) {
    ++$line;
    #
    # We expect data to contain lines in the forms:
    # TYPE = STACKTYPE
    #
    # or
    #
    # TYPE = STACKTYPE{Ctype};
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
    # unless if STACKTYPE is PTR, then it is forced to TYPE*
    # It is illegal to have more than occurence of TYPE per line
    #
    # Multiline is not supported: you have to write the prototype entirely.
    #
    my $input = $_;
    next if ($input =~ /^\s*#/);
    next if ($input =~ /^\s*$/);
    if ($input =~ m/^\s*(\w+)\s*=\s*(\w+)(?:\s*\{(.*)\s*\})?\s*;$/smg) {
      my ($outtype, $stacktype, $ctype) = ($1, $2, $3 || "GENERICSTACKITEMTYPE2TYPE_$2");
      die "Outtype $outtype already defined upper" if (exists($mapp->{$outtype}));
      $mapp->{$outtype} = {stacktype => $stacktype, ctype => $ctype };
    } else {
      die "Problem with line $line: $_\n";
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
  my ($typep, $mapp) = @_;

  #
  # First generate an enum for all the types
  #
  print  $out "\n";
  printf $out "/* This is a generated file as of %s */\n", "" . localtime;
  print  $out "\n";
  print  $out "#include <stdlib.h>        /* malloc, etc. */\n";
  print  $out "#include <errno.h>         /* errno */\n";
  print  $out "#include <string.h>        /* strerror(errno) */\n";
  print  $out "#include <genericStack.h>  /* generic stack */\n";
  if ($safe) {
    print  $out "\n";
    print  $out "/* -------------------------------------------- */\n";
    print  $out "/* Every item in the stack is explicitly tagged */\n";
    print  $out "/* -------------------------------------------- */\n";
    printf $out "typedef enum %s_itemType {\n", ${prefix};
    foreach (sort keys %{$typep}) {
      printf $out uc(sprintf("\t%s_itemType_%s\n", ${prefix}, $_));
    }
    print $out "} ${prefix}_itemType_t;\n";
  }
  #
  # Then all the mappings as per the user
  #
  print $out "\n";
  print $out "/* ---------------------------------------------------------------- */\n";
  print $out "/* Every item of every rule has an explicit type as per the mapping */\n";
  print $out "/* ---------------------------------------------------------------- */\n";
  foreach (sort keys %{$typep}) {
    printf "typedef %s %s;\n", $mapp->{$_}->{ctype}, x2type($_);
  }
  #
  # Generate all dependencies
  #
  print $out "\n";
  print $out "/* ------------------------------------------------------------------------------- */\n";
  print $out "/* Every type mapped to a genericStack's PTR generates free and clone dependencies */\n";
  print $out "/* ------------------------------------------------------------------------------- */\n";
  print $out "\n";
  foreach (sort keys %{$typep}) {
    next unless $mapp->{$_}->{stacktype} eq 'PTR';
    printf "static void %s(void *userDatavp, %s %s);\n", x2meth($_, 'free'), x2type($_), $_;
  }
  print $out "\n";
  foreach (sort keys %{$typep}) {
    next unless $mapp->{$_}->{stacktype} eq 'PTR';
    printf "static %s %s(void *userDatavp, %s %s);\n", x2type($_), x2meth($_, 'clone'), x2type($_), $_;
  }
  #
  # Methods that we generate
  #
  print  $out "\n";
  print  $out "/* -------------------------------- */\n";
  print  $out "/* Stack constructor and destructor */\n";
  print  $out "/* -------------------------------- */\n";
  print  $out "\n";
  printf $out "typedef %s %s;\n", stackManager(), stackManager_t();
  printf $out "static %s *%s(void *userDatavp);\n", stackManager_t(), stackManager_newp();
  printf $out "static void %s(void *userDatavp, %s *stackManagerp);\n", stackManager_freev(), stackManager_t();

  print  $out "\n";
  printf $out "static %s *%s(void *userDatavp);\n", stackManager_t(), stackManager_newp();
  print  $out "}\n";
  printf $out "static void %s(void *userDatavp, %s *stackManagerp);\n", stackManager_freev(), stackManager_t();
  print  $out "}\n";
  print  $out "\n";
  stackManager_trace('"%s"', "test");
}

sub x2type { return sprintf("${prefix}_%s_t", shift) }
sub x2meth { return sprintf("${prefix}_%s_%s", @_) }
sub stackManager {
  if ($safe) {
    return "struct stackManager { genericStack_t *dataStackp; genericStack_t *typeStackp; }";
  } else {
    return "genericStack_t";
  }
}
sub stackManager_t { return "${prefix}_stackManager_t" }
sub stackManager_newp { return sprintf("%s_newp", stackManager()) }
sub stackManager_freev { return sprintf("%s_freev", stackManager()) }
sub stackManager_trace {
  if ($tracelogger) {
    if ($tracecond) {
      print $out "$tracecond\n";
    }
    printf $out "  $tracelogger(%s, %s);\n", "userDatavp", join(', ', @_);
    if ($tracecond) {
      print $out "#endif\n";
    }
  }
}

exit(EXIT_SUCCESS);
