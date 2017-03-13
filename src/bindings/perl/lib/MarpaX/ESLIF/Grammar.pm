use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Grammar;
use Moo;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

=head1 DESCRIPTION

ESLIFGrammar is the second step after getting an ESLIF instance. As many grammars as wanted can be created using the same ESLIF parent, though dispose of resources should follow the reverse order of creation, i.e.:

  my $eslif = MarpaX::ESLIF->new()
  my $eslifGrammar = MarpaX::ESLIF::Grammar->new(eslif => $eslif, grammar => $grammar);
  # ...
  $eslifGrammar = undef;
  $eslif = undef;

=cut

use Scalar::Util qw/blessed/;
use Types::Standard qw/InstanceOf/;
use Encode qw/encode/;

sub BUILD {
    my ($self, $args) = @_;

    my $ut8_octets = encode('UTF-8', $args->{grammar}, Encode::FB_CROAK);

    MarpaX::ESLIF::Grammar::Thin::new($self,
                                      $args->{eslif},
                                      $ut8_octets);
}

sub DEMOLISH {
    my ($self, $in_global_destruction) = @_;
    MarpaX::ESLIF::Grammar::Thin::free($self,
                                       $self->_getMarpaESLIFGrammarp);
}

sub ngrammar {
    my ($self) = @_;

    MarpaX::ESLIF::Grammar::Thin::ngrammar($self,
                                           $self->_getMarpaESLIFGrammarp);
}

sub currentLevel {
    my ($self) = @_;

    MarpaX::ESLIF::Grammar::Thin::currentLevel($self,
                                               $self->_getMarpaESLIFGrammarp);
}

sub currentDescription {
    my ($self) = @_;

    MarpaX::ESLIF::Grammar::Thin::currentDescription($self,
                                                     $self->_getMarpaESLIFGrammarp);
}

sub descriptionByLevel {
    my ($self, $level) = @_;

    MarpaX::ESLIF::Grammar::Thin::descriptionByLevel($self,
                                                     $self->_getMarpaESLIFGrammarp,
                                                     $level);
}

#
# Internal methods and attributes, the XS know they exist
#

has 'eslif'   => (
    is => 'ro',
    reader => '_getMarpaESLIFp',
    isa => InstanceOf['MarpaX::ESLIF'],
    required => 1
    );
has 'grammar' => (
    is => 'ro',
    required => 1
    );

has 'marpaESLIFGrammarp' => (
    is => 'rw',
    reader => '_getMarpaESLIFGrammarp',
    writer => '_setMarpaESLIFGrammarp',
    default => sub {
        0
    }
    );

1;
