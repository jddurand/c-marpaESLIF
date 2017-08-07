use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Rule::Type;
use MarpaX::ESLIF;

# ABSTRACT: ESLIF Rule Types

# AUTHORITY

=head1 DESCRIPTION

ESLIF rule type is a bitset made of constants. This module is giving access to them.

=head1 SYNOPSIS

  my $ruleType;
  $ruleType = MarpaX::ESLIF::Rule::Type->MARPAESLIF_RULE_IS_ACCESSIBLE;     #  0x01
  $ruleType = MarpaX::ESLIF::Rule::Type->MARPAESLIF_RULE_IS_NULLABLE;       #  0x02
  $ruleType = MarpaX::ESLIF::Rule::Type->MARPAESLIF_RULE_IS_NULLING;        #  0x04
  $ruleType = MarpaX::ESLIF::Rule::Type->MARPAESLIF_RULE_IS_LOOP;           #  0x08
  $ruleType = MarpaX::ESLIF::Rule::Type->MARPAESLIF_RULE_IS_PRODUCTIVE;     #  0x10

=cut

use Carp qw/croak/;

# VERSION

# This section should be replaced on-the-fly at build time
# AUTOLOAD

1;
