use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

use vars qw/$VERSION/;

BEGIN {
    # VERSION
    $VERSION = '2.0.17';

    require XSLoader;
    XSLoader::load(__PACKAGE__, $VERSION);
}

# Load our explicit sub-modules
use MarpaX::ESLIF::Event::Type;
use MarpaX::ESLIF::Grammar::Properties;
use MarpaX::ESLIF::Grammar::Rule::Properties;
use MarpaX::ESLIF::Grammar::Symbol::Properties;
use MarpaX::ESLIF::Logger::Level;
use MarpaX::ESLIF::Symbol::PropertyBitSet;
use MarpaX::ESLIF::Symbol::Type;
use MarpaX::ESLIF::Value::Type;
use MarpaX::ESLIF::Rule::PropertyBitSet;

=head1 DESCRIPTION

ESLIF is derived from perl's L<Marpa::R2>, and has its own BNF, documented in L<MarpaX::ESLIF::BNF>.

The main features of this BNF are:

=over

=item Sub-grammars

The number of sub grammars is unlimited.

=item Regular expressions

Native support of regular expression using the L<PCRE2|http://www.pcre.org/> library (i.e. this is <not> exactly perl regexps, although very closed).

=item Streaming

Native support of streaming input.

=back

Beginners might want to look at L<MarpaX::ESLIF::Introduction>.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new(); # But prefer multiton please
  printf "ESLIF library version: %s\n", $eslif->version;

With a logger, using Log::Any::Adapter::Stderr as an example:

  use MarpaX::ESLIF;
  use Log::Any qw/$log/;
  use Log::Any::Adapter ('Stderr', log_level => 'trace' );

  my $eslif = MarpaX::ESLIF->new($log); # But prefer multiton please
  printf "ESLIF library version: %s\n", $eslif->version;

This class and its derivatives are thread-safe. Although there can be many ESLIF instances, in practice a single instance is enough, unless you want different logging interfaces. This is why the C<multiton> method, available on demand, is better than the C<new> method. Once a MarpaX::ESLIF instance is created, the user should create a L<MarpaX::ESLIF::Grammar> instance to have a working grammar.

=head1 METHODS

=head2 MarpaX::ESLIF->new($loggerInterface)

  my $loggerInterface = My::Logger::Interface->new();
  my $eslif = MarpaX::ESLIF->new();

Returns an instance of MarpaX::ESLIF, noted C<$eslif> below.

C<$loggerInterface> is an optional parameter that, when its exists, must be an object instance that can do the methods documented in L<MarpaX::ESLIF::Logger::Interface>, or C<undef>.

An example of logging implementation can be a L<Log::Any> adapter.

Since, for a given <$loggerInterface>, an ESLIF instance can be shared, the L<multiton> method is to be preferred.

=head2 $eslif->version()

  printf "ESLIF library version: %s\n", $eslif->version;

Returns a string containing the current underlying ESLIF library version.

=head2 MarpaX::ESLIF->multiton($loggerInterface)

  my $loggerInterface = My::Logger::Interface->new();
  my $eslif = MarpaX::ESLIF->multiton($logger); # or no logger, or undef

Returns an per-thread perl-logger shared instance of MarpaX::ESLIF.

Despite it is available only on demand, it is preferred over the C<new> method.

=cut

my %multiton = ();

sub multiton {
  my ($class, $logger) = @_;

  return $multiton{$logger // ''} //= $class->new($logger)
}

=head1 NOTES

The perl interface is an I<all-in-one> version of L<marpaESLIF|https://github.com/jddurand/c-marpaESLIF> library, which means that character conversion is using C<iconv> (or C<iconv>-like on Windows) instead of ICU, even if the later is available on your system.

=head1 SEE ALSO

L<MarpaX::ESLIF::Introduction>, L<PCRE2|http://www.pcre.org/>, L<MarpaX::ESLIF::BNF>, L<MarpaX::ESLIF::Logger::Interface>, L<MarpaX::ESLIF::Grammar>, L<MarpaX::ESLIF::Recognizer>.

=cut

1;
