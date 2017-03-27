#!env perl
use strict;
use diagnostics;
use Marpa::R2;

my $dsl = do { local $/; <DATA>};
my $panda_grammar = Marpa::R2::Scanless::G->new({ source => \$dsl } );
my $panda_recce = Marpa::R2::Scanless::R->new( { grammar => $panda_grammar, trace_terminals => 1 } );

my $sentence = 'abc';
$panda_recce->read( \$sentence );
my $asf = Marpa::R2::ASF->new( { slr=>$panda_recce } );
my $full_result = $asf->traverse( {}, \&full_traverser );

print join("\n", @{$full_result}) . "\n";

sub penn_tag {
    my ($symbol_name) = @_;
    return q{.} if $symbol_name eq 'period';
    return $symbol_name;
}

sub full_traverser {

    # This routine converts the glade into a list of Penn-tagged elements.  It is called recursively.
    my ($glade, $scratch)     = @_;
    my $rule_id     = $glade->rule_id();
    my $symbol_id   = $glade->symbol_id();
    my $symbol_name = $panda_grammar->symbol_name($symbol_id);

    print STDERR "[full_traverserCallbacki] => ruleIdi=" . ($rule_id // -1) . ", symbolIdi=$symbol_id\n";
    print STDERR "[full_traverserCallbacki] ... symbol $symbol_name\n";

    # A token is a single choice, and we know enough to fully Penn-tag it
    if ( not defined $rule_id ) {
	my $literal = $glade->literal();
	my $penn_tag = penn_tag($symbol_name);
	return ["($penn_tag $literal)"];
    } ## end if ( not defined $rule_id )

    my $rule_name = $panda_grammar->rule_name($rule_id);
    print STDERR "[full_traverserCallbacki] ... rule $symbol_name\n";

    # Our result will be a list of choices
    my @return_value = ();

  CHOICE: while (1) {

      # The results at each position are a list of choices, so
      # to produce a new result list, we need to take a Cartesian
      # product of all the choices
      my $length = $glade->rh_length();
      my @results = ( [] );
      for my $rh_ix ( 0 .. $length - 1 ) {
	  my @new_results = ();
	  for my $old_result (@results) {
              my $child_value = $glade->rh_value($rh_ix);
              for my $new_value ( @{ $child_value } ) {
                  push @new_results, [ @{$old_result}, $new_value ];
              }
	  }
	  @results = @new_results;
      } ## end for my $rh_ix ( 0 .. $length - 1 )

      # Special case for the start rule
      if ( $symbol_name eq '[:start]' ) {
	  return [ map { join q{}, @{$_} } @results ];
      }

      # Now we have a list of choices, as a list of lists.  Each sub list
      # is a list of Penn-tagged elements, which we need to join into
      # a single Penn-tagged element.  The result will be to collapse
      # one level of lists, and leave us with a list of Penn-tagged
      # elements
      my $join_ws = q{ };
      $join_ws = qq{\n   } if $symbol_name eq 'S';
      push @return_value,
      map { '(' . penn_tag($symbol_name) . q{ } . ( join $join_ws, @{$_} ) . ')' }
      @results;

      # Look at the next alternative in this glade, or end the
      # loop if there is none
      last CHOICE if not defined $glade->next();

  } ## end CHOICE: while (1)

    # Return the list of Penn-tagged elements for this glade
    return \@return_value;
} ## end sub full_traverser

__DATA__
:default ::= action => [ values ]
lexeme default = action => [ value ]

S ::= firsts seconds

firsts ::= first+
seconds ::= second+
first  ::= A | B
second ::= B | C

A ~ 'a'
B ~ 'b'
C ~ 'c'
