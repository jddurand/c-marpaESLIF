use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Registry;
use Carp qw/croak/;

# ABSTRACT: Registry for thread-safe ESLIF objects

# AUTHORITY

# VERSION

=head1 DESCRIPTION

This class ensures that L<MarpaX::ESLIF> and L<MarpaX::ESLIF::Symbol> instancecs correctly with perl's C<CLONE> and C<DESTROY>.

=cut

#
# ESLIF Registry:
#
# Every element is a MarpaX::ESLIF instance that an array reference of [ MarpaX::ESLIF::Engine pointer, $loggerInterface ] blessed to MarpaX::ESLIF
#
my @ESLIF_REGISTRY = ();

sub _find_eslif {
    my ($class, $loggerInterface) = @_;

    my $definedLoggerInterface = defined($loggerInterface); # It is legal to create an eslif with no logger interface

    foreach (@ESLIF_REGISTRY) {
        my $_definedLoggerInterface = defined($_[1]);
	return $_
            if (
                (! $definedLoggerInterface && ! $_definedLoggerInterface)
                ||
                ($definedLoggerInterface && $_definedLoggerInterface && ($loggerInterface == $_[1]))
            )
    }

    return
}

#
# ESLIF Symbol Registry:
#
# Every element is an array reference that is: [ MarpaX::ESLIF::Symbol instance, MarpaX::ESLIF::Engine pointer, $type, $pattern, $encoding, $modifiers ]
#
my @ESLIFSYMBOL_REGISTRY = ();

sub _find_eslifSymbol {
    my ($class, $engine, $type, $pattern, $encoding, $modifiers) = @_;

    my $definedEncoding = defined($encoding); # It is legal to create a symbol with no encoding
    my $definedModifiers = defined($modifiers); # It is legal to create a symbol with no modifier

    foreach (@ESLIFSYMBOL_REGISTRY) {
        my $_definedEncoding = defined($_->[4]);
        my $_definedModifiers = defined($_->[5]);
	return $_->[0] if
	    $engine == $_->[1]
	    &&
	    $type eq $_->[2]
	    &&
	    $pattern eq $_->[3]
	    &&
	    ((! $definedEncoding && ! $_definedEncoding) || ($definedEncoding && $_definedEncoding && ($encoding eq $_->[4])))
	    &&
	    ((! $definedModifiers && ! $_definedModifiers) || ($definedModifiers && $_definedModifiers && ($modifiers eq $_->[4])))
    }

    return
}

#
# ESLIF Grammar Registry:
#
# Every element is an array reference that is: [ MarpaX::ESLIF::Grammar instance, MarpaX::ESLIF::Engine pointer, $bnf, $encoding ]
#
my @ESLIFGRAMMAR_REGISTRY = ();

sub _find_eslifGrammar {
    my ($class, $engine, $bnf, $encoding) = @_;

    my $definedEncoding = defined($encoding); # It is legal to create a grammar with no encoding

    foreach (@ESLIFGRAMMAR_REGISTRY) {
        my $_definedEncoding = defined($_->[3]);
	return $_->[0] if
	    $engine == $_->[1]
	    &&
	    $bnf eq $_->[2]
	    &&
	    ((! $definedEncoding && ! $_definedEncoding) || ($definedEncoding && $_definedEncoding && ($encoding eq $_->[3])))
    }

    return
}

=head1 METHODS

=head2 ESLIF_new($class, $loggerInterface)

Class method that return a singleton instance of a L<MarpaX::ESLIF>.

=cut

sub ESLIF_new {
  my ($class, $bless, $loggerInterface) = @_;

  my $eslif = $class->_find_eslif($loggerInterface);

  push(@ESLIF_REGISTRY, $eslif = bless [ MarpaX::ESLIF::Engine->allocate($loggerInterface), $loggerInterface ], 'MarpaX::ESLIF') if ! defined($eslif);

  return $eslif
}

=head2 ESLIF_getEngine($class, $eslif)

Class method that return the L<MarpaX::ESLIF::Engine> pointer associated to a L<MarpaX::ESLIF>.

=cut

sub ESLIF_getEngine {
    my ($class, $eslif) = @_;

    return $eslif->[0]
}

=head2 ESLIFSymbol_new($class, $eslif, $type, $pattern)

Class method that return a singleton instance of a L<MarpaX::ESLIF::Symbol>.

=cut

sub _ESLIFSymbol_new {
    my ($class, $engine, $type, $pattern, $encoding, $modifiers) = @_;

    return ($type eq 'string')
        ?
        MarpaX::ESLIF::Symbol->string_new($engine, $pattern, bytes::length($pattern), $encoding, $modifiers)
        :
        (($type eq 'regex')
         ?
         MarpaX::ESLIF::Symbol->regex_new($engine, $pattern, bytes::length($pattern), $encoding, $modifiers)
         :
         croak "Type must be 'string' or 'regex'"
        )
}

sub ESLIFSymbol_new {
  my ($class, $eslif, $type, $pattern, $encoding, $modifiers) = @_;

  my $engine = $class->ESLIF_getEngine($eslif);
  my @args = ($engine, $type, $pattern, $encoding, $modifiers);

  my $eslifSymbol = $class->_find_eslifSymbol(@args);

  push(@ESLIFSYMBOL_REGISTRY, [ $eslifSymbol = $class->_ESLIFSymbol_new(@args), @args ]) if ! defined($eslifSymbol);

  return $eslifSymbol
}

=head2 ESLIFGrammar_new($class, $eslif, $bnf[, $encoding])

Class method that return a singleton instance of a L<MarpaX::ESLIF::Grammar>.

=cut

sub _ESLIFGrammar_new {
    my $class = shift;

    return MarpaX::ESLIF::Grammar->_new(@_)
}

sub ESLIFGrammar_new {
  my ($class, $eslif, @rest) = @_;

  my $engine = $class->ESLIF_getEngine($eslif);
  my @args = ($engine, @rest);

  my $eslifGrammar = $class->_find_eslifGrammar(@args);

  push(@ESLIFGRAMMAR_REGISTRY, [ $eslifGrammar = $class->_ESLIFGrammar_new(@args), @args ]) if ! defined($eslifGrammar);

  return $eslifGrammar
}

=head2 CLONE()

Manages singleton thread-safe objects of type L<MarpaX::ESLIF> and L<MarpaX::ESLIF::Symbol>.

=cut

sub CLONE {
    #
    # We keep track of engines that got replaced
    #
    my %ENGINES = ();

    foreach (@ESLIF_REGISTRY) {
	my $old_engine = $_->[0];
	my $new_engine = $_->[0] = MarpaX::ESLIF::Engine->allocate($_->[1]);
	#
	# $old_engine is stringified but this is ok
	#
	$ENGINES{$old_engine} = $new_engine
    }
    #
    # Clone symbols that referenced the old engine
    #
    foreach (@ESLIFSYMBOL_REGISTRY) {
	my $old_engine = $_->[1];
	my $new_engine = $ENGINES{$old_engine} // croak "Failed to get new engine that replaces $old_engine";
	my $type = $_->[2];
	my $pattern = $_->[3];
	my $encoding = $_->[4];
	my $modifiers = $_->[5];
	$_->[0] = __PACKAGE__->_ESLIFSymbol_new($_->[1] = $new_engine, $type, $pattern, $encoding, $modifiers)
    }
    #
    # Clone grammars that referenced the old engine
    #
    foreach (@ESLIFGRAMMAR_REGISTRY) {
	my $old_engine = $_->[1];
	my $new_engine = $ENGINES{$old_engine} // croak "Failed to get new engine that replaces $old_engine";
	$_->[1] = $new_engine;
	my @args = @{$_};
	shift @args;
	$_->[0] = __PACKAGE__->_ESLIFGrammar_new(@args)
    }
}

1;
