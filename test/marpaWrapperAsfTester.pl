#!env perl
use strict;
use diagnostics;
use Marpa::R2;
use Data::Scan::Printer;

our $dsl = do { local $/; <DATA> };

my $sentence = 'apandaeatsshootsandleaves.';
my $panda_grammar = Marpa::R2::Scanless::G->new(
    { source => \$dsl, bless_package => 'PennTags', } );
my $panda_recce = Marpa::R2::Scanless::R->new( { grammar => $panda_grammar } );
$panda_recce->read( \$sentence );
my $asf = Marpa::R2::ASF->new( { slr=>$panda_recce } );
my $pruned_result = $asf->traverse( {}, \&pruning_traverser );

print $pruned_result;

sub penn_tag {
    my ($symbol_name) = @_;
    return q{.} if $symbol_name eq 'period';
    return $symbol_name;
}

sub pruning_traverser {

    # This routine converts the glade into a list of Penn-tagged elements.  It is called recursively.
    my ($glade, $scratch)     = @_;
    my $rule_id     = $glade->rule_id();
    my $symbol_id   = $glade->symbol_id();
    my $symbol_name = $panda_grammar->symbol_name($symbol_id);

    print STDERR "ruleIdi=" . ($rule_id // 'undef') . "\n";
    print STDERR "symbolIdi=$symbol_id\n";

    
    # A token is a single choice, and we know enough to fully Penn-tag it
    if ( not defined $rule_id ) {
	my $literal = $glade->literal();
	my $penn_tag = penn_tag($symbol_name);
	return "($penn_tag $literal)";
    }

    my $length = $glade->rh_length();
    my @return_value = map { $glade->rh_value($_) } 0 .. $length - 1;

    # Special case for the start rule
    return (join q{ }, @return_value) . "\n" if  $symbol_name eq '[:start]' ;

    my $join_ws = q{ };
    $join_ws = qq{\n   } if $symbol_name eq 'S';
    my $penn_tag = penn_tag($symbol_name);
    return "($penn_tag " . ( join $join_ws, @return_value ) . ')';
}

__DATA__
:default ::= action => [ values ] bless => ::lhs
  lexeme default = action => [ value ] bless => ::name

  S   ::= NP  VP  period  bless => S

  NP  ::= NN              bless => NP
      |   NNS          bless => NP
      |   DT  NN          bless => NP
      |   NN  NNS         bless => NP
      |   NNS CC NNS  bless => NP

  VP  ::= VBZ NP          bless => VP
      | VP VBZ NNS        bless => VP
      | VP CC VP bless => VP
      | VP VP CC VP bless => VP
      | VBZ bless => VP

  period ~ '.'

  CC ~ 'and'
  DT  ~ 'a' | 'an'
  NN  ~ 'panda'
  NNS  ~ 'shoots' | 'leaves'
  VBZ ~ 'eats' | 'shoots' | 'leaves'
