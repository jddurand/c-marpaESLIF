use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Logger::Level;

# ABSTRACT: ESLIF Logger levels

# AUTHORITY

=head1 DESCRIPTION

ESLIF logger levels are mapped to constants. This module is giving access to them.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $loggerLevelType;
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_TRACE;      #  0
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_DEBUG;      #  1
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_INFO;       #  2
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_NOTICE;     #  3
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_WARNING;    #  4
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_ERROR;      #  5
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_CRITICAL;   #  6
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_ALERT;      #  7
  $loggerLevelType = MarpaX::ESLIF::Logger::Level->GENERICLOGGER_LOGLEVEL_EMERGENCY;  #  8

=cut

use Carp qw/croak/;

# VERSION

# This section should be replaced on-the-fly at build time
# AUTOLOAD

1;
