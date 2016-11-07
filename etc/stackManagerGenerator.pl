#!env perl
use strict;
use POSIX qw/EXIT_SUCCESS EXIT_FAILURE/;
use Data::Dumper;
use Getopt::Long;

my $config;
my $help = 0;
if (! GetOptions("config=s" => \$config,
                 "help!"    => \$help)) {
  exit(EXIT_FAILURE);
}

if ($help || ! $config) {
  print <<USAGE;
  Usage: $0 [options]

  where options can be:

  --config configfile       Load configuration from configfile. Default: undef.
  --help                    This help
USAGE
  exit(EXIT_SUCCESS);
}

open(my $fh, '<', $config) || die "$config: $!";
my @rules = ();
my $line = 0;
while (chomp($_ = <$fh>)) {
  ++$line;
  #
  # We expect data to contain lines in the forms:
  # TYPE = METHOD(TYPE*)
  #
  # where a type is writen like this:
  # genericType, or
  # genericType [Quantifier]
  #
  # A genericType is one supported by genericStack.h, i.e. INT, SHORT, PTR, etc...
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
  while ($input =~ m/^\s*(\w+)\s*=\s*(\w+)\s*\((.*)\)\s*;$/smg) {
    my ($outtype, $method, $intypes) = ($1, $2, $3 || '');
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
      push(@intypes, { type => $intype, minmax => $minmax });
    }
    push(@rules, { outtype => $outtype, method => $method, intypes => \@intypes });
  }
}

print Dumper(\@rules);

exit(EXIT_SUCCESS);
