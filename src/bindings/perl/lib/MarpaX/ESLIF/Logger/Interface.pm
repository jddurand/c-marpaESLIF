use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Logger::Interface;
use Moo::Role;
use vars qw/$VERSION/;

# ABSTRACT: Required logging methods

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIFLoggerInterface is listing all required logging methods.

A logger interface may be associated to an L<MarpaX::ESLIF> instance.
This interface is explicitly reproducing the formalism of the L<genericLogger|href="http://github.com/jddurand/c-genericLogger"> C library.

=cut

=head1 METHODS

=head2 trace($message)

Log messages with level C<TRACE>.

=cut

requires 'trace';

=head2 debug($message)

Log messages with level C<DEBUG>.

=cut

requires 'debug';

=head2 info($message)

Log messages with level C<INFO>.

=cut

requires 'info';

=head2 warning($message)

Log messages with level C<WARNING>.

=cut

requires 'warning';

=head2 error($message)

Log messages with level C<ERROR>.

=cut

requires 'error';

=head2 critical($message)

Log messages with level C<CRITICAL>.

=cut

requires 'critical';

=head2 alert($message)

Log messages with level C<ALERT>.

=cut

requires 'alert';

=head2 emergency($message)

Log messages with level C<EMERGENCY>.

=cut

requires 'emergency';

=head1 SEE ALSO

L<MarpaX::ESLIF::Logger::Level>

=cut

1;
