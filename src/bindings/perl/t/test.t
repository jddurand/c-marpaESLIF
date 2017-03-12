use strict;
use warnings FATAL => 'all';
use IO::Handle;
use Log::Log4perl qw/:easy/;
use Log::Any::Adapter;
use Log::Any qw/$log/;
#
# Init log
#
our $defaultLog4perlConf = '
log4perl.rootLogger              = TRACE, Screen
log4perl.appender.Screen         = Log::Log4perl::Appender::Screen
log4perl.appender.Screen.stderr  = 0
log4perl.appender.Screen.layout  = PatternLayout
log4perl.appender.Screen.layout.ConversionPattern = %d %-5p %6P %m{chomp}%n
';
Log::Log4perl::init(\$defaultLog4perlConf);
Log::Any::Adapter->set('Log4perl');

use Test::More tests => 2;
BEGIN { require_ok('MarpaX::ESLIF') };

my $eslif = MarpaX::ESLIF->new(_logger => Log::Any->get_logger);
tests($eslif);
1;

sub tests {
    my ($eslif) = @_;
}

sub test_version {
    my ($eslif) = @_;

    my $version = $eslif->version;
    if (defined($version)) {
	ok (1, "version is $version");
    } else {
	ok (0, "version is undefined");
    }
}

