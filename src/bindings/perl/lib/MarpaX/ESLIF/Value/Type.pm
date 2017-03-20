use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Value::Type;

# ABSTRACT: ESLIF Value Types

# AUTHORITY

=head1 DESCRIPTION

ESLIF values are mapped to constants. This module is giving access to them.

=head1 SYNOPSIS

  my $valueType;
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_UNDEF;         #  0
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_CHAR;          #  1
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_SHORT;         #  2
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_INT;           #  3
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_LONG;          #  4
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_FLOAT;         #  5
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_DOUBLE;        #  6
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_PTR;           #  7
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_PTR_SHALLOW;   #  8
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_ARRAY;         #  9
  $valueType = MarpaX::ESLIF::Value::Type::MARPAESLIF_VALUE_TYPE_ARRAY_SHALLOW; # 10

=cut

require XSLoader;
use vars qw/$VERSION/;

BEGIN {
    # VERSION

    XSLoader::load __PACKAGE__, $VERSION;
}

1;
