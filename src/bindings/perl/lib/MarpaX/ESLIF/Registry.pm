use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Registry;
use Carp qw/croak/;

# ABSTRACT: ESLIF registry

# AUTHORITY

# VERSION

=head1 DESCRIPTION

This class ensures is a generic constructor, destructor and cloner for all objects in the L<MarpaX::ESLIF> namespace. It ensures in particular the singleton and clonable behaviour of L<MarpaX::ESLIF>, L<MarpaX::ESLIF::Grammar> and L<MarpaX::ESLIF::Symbol> objects.

=cut

#
# Different registries exist per clonable class, because we want to control the order when cloning
# If eq_ref is defined, then the object is implicitly a singleton
#

my %REGISTRY;

sub _find {
    my ($eq_ref, $objects_ref, @args) = @_;

    foreach (@{$objects_ref}) {
        return $_ if $eq_ref->($_->{args_ref}, @args)
    }

    return
}

=head1 METHODS

=head2 $class->new($bless, $clonable, $eq_ref, $allocate_ref, @args)

Generic constructor of a C<$bless> instance created using C<$allocate_ref->(@args)>. If C<$eq_ref> is set, the instance is implicitly a singleton. If C<$clonable> is a true value, the instance is clonable.

=cut

sub new {
    my ($bless, $clonable, $eq_ref, $allocate_ref, $dispose_ref, @args) = @_;

    #
    # Some arguments MUST be provided
    #
    croak 'bless must be set' if (! defined($bless));
    croak 'clonable must be set' if (! defined($clonable));
    croak 'allocate_ref must be set' if (! defined($allocate_ref));
    croak 'dispose_ref must be set' if (! defined($dispose_ref));

    #
    # We keep track of all objects in order to not loose any referenced dependency
    #
    my $registry_ref = $REGISTRY{$bless} //= { $bless => { objects_ref => [] }};
    my $singleton = defined($eq_ref);

    my $self;
    if ($singleton && defined($self = _find($eq_ref, $registry_ref->{objects_ref}, @args))) {
        return $self
    }

    push(@{$registry_ref->{objects_ref}},
         $self = bless { engine => $bless->$allocate_ref(@args),
                         allocate_ref => $allocate_ref,
                         dispose_ref => $dispose_ref,
                         singleton => $singleton,
                         clonable => $clonable,
                         args_ref => \@args}, $bless);

    return $self
}

=head2 $self->DESTROY()

Generic destructor. It always calls C<$self>'s C<dispose> method.

=cut

sub DESTROY {
    my ($self) = @_;

    my $bless = ref($self);
    my $engine = $self->{engine};
    my $dispose_ref = $self->{dispose_ref};

    if ($self->{singleton}) {
        my $objects_ref = $REGISTRY{$bless}->{objects_ref};

        foreach (0..$#{@{$objects_ref}}) {
            if ($objects_ref->[$_] == $self) {
                $bless->$dispose_ref($engine);
                splice(@{$objects_ref}, $_, 1);
                return
            }
        }

        warn "Unregistered object $self";
    }

    $bless->$dispose_ref($engine)
}

=head2 CLONE()

Manages singleton thread-safe objects of type L<MarpaX::ESLIF>, L<MarpaX::ESLIF::Grammar> and L<MarpaX::ESLIF::Symbol>.

=cut

sub _registry_clone {
    my ($registry_ref) = @_;

    my $objects_ref = $registry_ref->{objects_ref};

    foreach my $self (@{$objects_ref}) {
        if ($self->{clonable}) {
            my $allocate_ref = $self->{allocate_ref};
            my $args_ref = $self->{args_ref};
            $self->{engine} = $self->$allocate_ref(@{$args_ref})
        } else {
            $self->{engine} = undef
        }
    }
}

#
# When cloning every package that has the CLONE method is considered, even if there is no instance of that package
#

sub CLONE {
    #
    # We keep track of engines that got replaced
    #
    #
    # We always look at MarpaX::ESLIF first.
    #
    my $eslif_bless = 'MarpaX::ESLIF';
    if (exists($REGISTRY{$eslif_bless})) {
        _registry_clone($REGISTRY{$eslif_bless})
    }
    #
    # We know that all other objects are derived immediately from ESLIF, not more
    #
    foreach my $bless (map { grep $_ ne $eslif_bless } keys %REGISTRY) {
        _registry_clone($REGISTRY{$bless})
    }
}

1;
