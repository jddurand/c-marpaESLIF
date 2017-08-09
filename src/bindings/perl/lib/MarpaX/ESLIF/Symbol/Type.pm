use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Symbol::Type;

# ABSTRACT: ESLIF Symbol Type

# AUTHORITY

=head1 DESCRIPTION

ESLIF symbol type is made of constants. This module is giving access to them.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $symbolType;
  $symbolType = MarpaX::ESLIF::Symbol::Type->MARPAESLIF_SYMBOLTYPE_TERMINAL;      #  0
  $symbolType = MarpaX::ESLIF::Symbol::Type->MARPAESLIF_SYMBOLTYPE_META;          #  1

=cut

use Carp qw/croak/;

# VERSION

# This section should be replaced on-the-fly at build time
# AUTOLOAD

1;
