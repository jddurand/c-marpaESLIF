use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Symbol::EventBitSet;

# ABSTRACT: ESLIF Symbol Event Bit Set

# AUTHORITY

=head1 DESCRIPTION

ESLIF symbol event bitset is made of constants, mapping the low-level Marpa view of symbol events. This module is giving access to them.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $symbolEventBitSet;
  $symbolEventBitSet = MarpaX::ESLIF::Symbol::EventBitSet->MARPAESLIF_SYMBOL_EVENT_COMPLETION;  #  0x01
  $symbolEventBitSet = MarpaX::ESLIF::Symbol::EventBitSet->MARPAESLIF_SYMBOL_EVENT_NULLED;      #  0x02
  $symbolEventBitSet = MarpaX::ESLIF::Symbol::EventBitSet->MARPAESLIF_SYMBOL_EVENT_PREDICTION;  #  0x04

=cut

use Carp qw/croak/;

# VERSION

# This section should be replaced on-the-fly at build time
# AUTOLOAD

1;
