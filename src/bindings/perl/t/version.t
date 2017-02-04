use strict;
use warnings FATAL => 'all';
use IO::Handle;

use Test::More tests => 2;
BEGIN { require_ok('MarpaX::ESLIF') };

my $eslif = MarpaX::ESLIF->new();
tests($eslif);
1;

sub tests {
    my ($eslif) = @_;

    test_version($eslif);
}

sub test_version {
    my ($eslif) = @_;

    my $version = $eslif->ESLIF_version;
    if (defined($version)) {
	ok (1, "version is $version");
    } else {
	ok (0, "version is undefined");
    }
}

