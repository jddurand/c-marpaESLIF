#!env perl
package MyActions;

sub converterTable {
    my ($self, $tags, $converterLines) = @_;

    # return { tags => $tags, converterLines => $converterLines };
    
    my %category2aliases;
    my %alias2categories;
    foreach my $converterLine (@{$converterLines}) {
        my ($category, $aliases) = @{$converterLine};

        my $normalizedcategory = $category;
        $normalizedcategory = lc($normalizedcategory);
        $normalizedcategory =~ s/[_\s-]//g;

        $category2aliases{$category} //= [];
        foreach my $x (@{$aliases}, $normalizedcategory) {
            push(@{$category2aliases{$category}}, $x) unless grep { $_ eq $x } @{$category2aliases{$category}};
        }

        foreach my $alias (@{$aliases}) {
            $alias2categories{$alias} //= [];
            push(@{$alias2categories{$alias}}, $category) unless grep { $_ eq $category } @{$alias2categories{$alias}};
        }
        #
        # For every category, add an alias2categories entry using the normalized category
        #
        $alias2categories{$normalizedcategory} //= [];
        push(@{$alias2categories{$normalizedcategory}}, $category) unless grep { $_ eq $category } @{$alias2categories{$normalizedcategory}};
    }

    # return { category2aliases => \%category2aliases, alias2categories => \%alias2categories };

    #
    # We return a form that is suitable for iconv.c
    #
    #
    # ... We precompute the maximum number of categories
    #
    my $categoryl = 0;
    foreach my $alias (sort keys %alias2categories) {
        my $categories = $alias2categories{$alias};
        $categoryl = scalar(@{$categories}) if (scalar(@{$categories}) > $categoryl);
    }
    
    my $nbalias = keys %alias2categories;
    my $typedef = "/* This structure is generated with convrtrs.pl */\n
#define TCONV_ICONV_MAX_CATEGORY $categoryl
typedef struct tconv_iconv_alias2category {
  char *alias;                                 /* Normalized version */
  char *categoriesp[TCONV_ICONV_MAX_CATEGORY]; /* Categories */
} tconv_iconv_alias2category_t;
    
#define TCONV_ICONV_NB_ALIAS $nbalias
static tconv_iconv_alias2category_t alias2category[] = {
";
    my @content;
    #
    # We precompute the maximum number of categories
    # Most probable categories are on the top.
    #
    my %weight;
    foreach (keys %alias2categories) {
             if ($_ eq 'utf8')     { $weight{$_} = -11;
        } elsif ($_ eq 'utf16')    { $weight{$_} = -10;
        } elsif ($_ eq 'utf16le')  { $weight{$_} =  -9;
        } elsif ($_ eq 'utf16be')  { $weight{$_} =  -8;
        } elsif ($_ eq 'utf32')    { $weight{$_} =  -7;
        } elsif ($_ eq 'utf32le')  { $weight{$_} =  -6;
        } elsif ($_ eq 'utf32be')  { $weight{$_} =  -5;
        } elsif ($_ =~ /^utf16/)   { $weight{$_} =  -4;
        } elsif ($_ =~ /^utf32/)   { $weight{$_} =  -3;
        } elsif ($_ =~ /^windows/) { $weight{$_} =  -2;
        } elsif ($_ =~ /^cp/)      { $weight{$_} =  -1;
        } else                     { $weight{$_} =   0;
        }
    }
    foreach my $alias (sort
                       {
                           ($weight{$a} <=> $weight{$b}) || ($a cmp $b)
                       } keys %alias2categories) {
        my $categories = $alias2categories{$alias};
        my @categories = map { "\"$_\"" } @{$categories};
        for (my $i = scalar(@categories); $i < $categoryl; $i++) {
            push(@categories, "NULL");
        }
        push(@content, "  { \"$alias\", { " . join(', ', @categories) . " } }");
    }
    $typedef .= join(",\n", @content);
    $typedef .= "\n};\n";

    return $typedef;
}

sub converterLines {
    my ($self, @converterLines) = @_;

    return \@converterLines;
}

sub converterLine {
    my ($self, $converterName, $tags, $taggedAliases, $newline) = @_;

    return [ $converterName, $taggedAliases ];
}

sub taggedAliases {
    my ($self, @taggedAlias) = @_;

    return \@taggedAlias;
}

sub taggedAlias {
    my ($self, $alias, $tags) = @_;

    #
    # A tagged alias is used for name matching:
    # - case-insensitive
    # - '-', '_' and ' ' are ignored
    #
    $alias = lc($alias);
    $alias =~ s/[_\s-]//g;
    return $alias;
}

sub tags {
    my ($self, undef, $tagNames, undef) = @_;

    $tagNames //= [];

    return $tagNames;
}

sub tagNames {
    my $self = shift;

    return \@_;
}

sub tagName {
    my ($self, $tagName, $notused) = @_;

    return $tagName;
}

sub converterName {
    my ($self, $converterName) = @_;

    #
    # Remove the newline -;
    #
    substr($converterName, 0, 1, '');

    return $converterName;
}

sub alias {
    my ($self, $converterName) = @_;

    return $converterName;
}

package main;
use strict;
use diagnostics;
use Marpa::R2;
use POSIX qw/EXIT_SUCCESS/;

my $DATA = do { local $/; <DATA> };
my $G = Marpa::R2::Scanless::G->new({source => \$DATA});

my $convrtrs = shift || 'convrtrs.txt';
open(my $fh, '<', $convrtrs) || die "Cannot open $convrtrs, $!";
my $map = do { local $/; <$fh> };
close($fh) || warn "Cannot close $convrtrs, $!";

my $value = $G->parse(\$map, 'MyActions',
                      # { trace_terminals => 999 }
    );
use Data::Dumper;
print ${$value};

exit(EXIT_SUCCESS);

__DATA__
:default ::= action => [name,values]
lexeme default = latm => 1

converterTable   ::= tags converterLines                    action => converterTable
converterLines   ::= converterLine*                         action => converterLines
converterLine    ::= converterName tags taggedAliases       action => converterLine
taggedAliases    ::= taggedAlias*                           action => taggedAliases
taggedAlias      ::= alias tags                             action => taggedAlias
tags             ::= '{' tagNames '}'                       action => tags
tags             ::=                                        action => tags
tagNames         ::= tagName*                               action => tagNames
tagName          ::= standard                               action => tagName
                   | standard '*'                           action => tagName
_newline           ~ [\n]
__name             ~ [0-9a-zA-Z:_,=.+-]+
_converterName     ~ _newline __name
converterName    ::= _converterName                         action => converterName
standard           ~ [0-9a-zA-Z:_-]+
_name              ~ __name
alias            ::= _name                                  action => alias

:discard             ~ whitespace
whitespace         ~ [\s]

:discard           ~ comment
commenttrailer     ~ [^\n]*
comment            ~ '#' commenttrailer
