use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Symbol::PropertyBitSet;

# ABSTRACT: ESLIF Symbol Property Bit Set

# AUTHORITY

=head1 DESCRIPTION

ESLIF symbol property bitset is made of constants, mapping the low-level Marpa view of symbol capabilities. This module is giving access to them.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $symbolPropertyBitSet;
  $symbolPropertyBitSet = MarpaX::ESLIF::Symbol::PropertyBitSet->MARPAESLIF_SYMBOL_IS_ACCESSIBLE;     #  0x01
  $symbolPropertyBitSet = MarpaX::ESLIF::Symbol::PropertyBitSet->MARPAESLIF_SYMBOL_IS_NULLABLE;       #  0x02
  $symbolPropertyBitSet = MarpaX::ESLIF::Symbol::PropertyBitSet->MARPAESLIF_SYMBOL_IS_NULLING;        #  0x04
  $symbolPropertyBitSet = MarpaX::ESLIF::Symbol::PropertyBitSet->MARPAESLIF_SYMBOL_IS_PRODUCTIVE;     #  0x08
  $symbolPropertyBitSet = MarpaX::ESLIF::Symbol::PropertyBitSet->MARPAESLIF_SYMBOL_IS_START;          #  0x10
  $symbolPropertyBitSet = MarpaX::ESLIF::Symbol::PropertyBitSet->MARPAESLIF_SYMBOL_IS_TERMINAL;       #  0x20

=cut

use Carp qw/croak/;

# VERSION

# This section should be replaced on-the-fly at build time
# AUTOLOAD

1;
