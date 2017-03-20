use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Event::Type;

# ABSTRACT: ESLIF Event Types

# AUTHORITY

=head1 DESCRIPTION

ESLIF events are mapped to constants. This module is giving access to them.

=head1 SYNOPSIS

  my $eventType;
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_NONE;       # 0x00
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_COMPLETED;  # 0x01, /* Grammar event */
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_NULLED;     # 0x02, /* Grammar event */
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_PREDICTED;  # 0x04, /* Grammar event */
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_BEFORE;     # 0x08, /* Just before lexeme is commited */
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_AFTER;      # 0x10, /* Just after lexeme is commited */
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_EXHAUSTED;  # 0x20, /* Exhaustion */
  $eventType = MarpaX::ESLIF::Event::Type::MARPAESLIF_EVENTTYPE_DISCARD;    # 0x40  /* Discard */

=cut

require XSLoader;
use vars qw/$VERSION/;

BEGIN {
    # VERSION

    XSLoader::load __PACKAGE__, $VERSION;
}

1;
