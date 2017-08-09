use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Rule::PropertyBitSet;

# ABSTRACT: ESLIF Rule Property Bit Set

# AUTHORITY

=head1 DESCRIPTION

ESLIF rule property bitset is made of constants, mapping the low-level Marpa view of rule capabilities. This module is giving access to them.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $rulePropertyBitSet;
  $rulePropertyBitSet = MarpaX::ESLIF::Rule::PropertyBitSet->MARPAESLIF_RULE_IS_ACCESSIBLE;     #  0x01
  $rulePropertyBitSet = MarpaX::ESLIF::Rule::PropertyBitSet->MARPAESLIF_RULE_IS_NULLABLE;       #  0x02
  $rulePropertyBitSet = MarpaX::ESLIF::Rule::PropertyBitSet->MARPAESLIF_RULE_IS_NULLING;        #  0x04
  $rulePropertyBitSet = MarpaX::ESLIF::Rule::PropertyBitSet->MARPAESLIF_RULE_IS_LOOP;           #  0x08
  $rulePropertyBitSet = MarpaX::ESLIF::Rule::PropertyBitSet->MARPAESLIF_RULE_IS_PRODUCTIVE;     #  0x10

=cut

use Carp qw/croak/;

# VERSION

# This section should be replaced on-the-fly at build time
# AUTOLOAD

1;
