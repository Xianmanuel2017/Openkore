#########################################################################
#  OpenKore - Indexed set
#  Copyright (c) 2006 OpenKore Team
#
#  This software is open source, licensed under the GNU General Public
#  License, version 2.
#  Basically, this means that you're allowed to modify and distribute
#  this software. However, if you distribute modified versions, you MUST
#  also distribute the source code.
#  See http://www.gnu.org/licenses/gpl.html for the full license.
#########################################################################
##
# MODULE DESCRIPTION: Indexed set
#
# A set (as in mathematics) is a collection with no duplicate items.
# The Set class implements an indexed set. Not only do items only occur
# once, each item can be accessed by index. So a Set is essentially
# an array with no duplicate items.
#
# <h3>Usage</h3>
# To manipulate a Set, use its methods. You can access items in a Set
# just like you access items in an array reference:
# <pre class="example">
# my $set = new Set();
# $set->add("hello");
# $set->add("world");
# $set->add("hello");   # Has no effect. "hello" is already in the set.
#
# foreach my $item (@{$set}) {
#     do_something($item);
# }
#
# $set->[0];            # "hello"
# $set->get(0);         # "hello"
# $set->has("hello");   # 1 (true)
# $set->has("foo");     # undef (false)
#
# $set->remove("hello");
# $set->has("hello");   # undef (false)
# </pre>
#
# However, <b>do not</b> manipulate a Set like it's an array. The
# following will result in corruption of the Set:
# <pre class="example">
# delete $set->[0];
# @{$set} = ();
# splice @{$set}, 1, 1;
# </pre>
#
# <h3>How items are considered equal</h3>
# Two items are considered equal if their strings are the same.
# That is, $a and $b are considered equal if:
# <pre class="example">
# "$a" eq "$b"
# </pre>
package Set;
use strict;
use Scalar::Util qw(refaddr);

# MultiThreading Support
use threads;
use threads::shared;

use Utils::Splice qw(splice_shared);

use overload '@{}' => \&getArray;
use overload '[]' => \&get;
use overload '""' => \&_toString;

##
# Set Set->new([elements...])
# elements: The elements to add to this set.
#
# Create a new Set, possibly with predefined elements.
sub new {
	my $class = shift;
	my $self = {
		# The items themselves.
		items => [],
	};
	$self = bless $self, $class;
	foreach my $item (@_) {
		$self->add($item);
	}
	return $self;
}

##
# void $set->add(item)
# Requires: defined($item)
# Ensures: $self->has($item)
#
# Add $item to the set if it isn't already in the set.
sub add {
	my ($self, $item) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));
	$item = shared_clone($item) if ((is_shared($self)) && (!is_shared($item)));

	if (!$self->has($item)) {
		push @{$self->{items}}, $item;
	}
}

##
# void $set->remove(item)
# Requires: defined($item)
# Ensures: !$self->has($item)
#
# Removes $item from the set if it's there.
sub remove {
	my ($self, $item) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));
	# Grr. This can make bug's with "find" function.
	# But, if we seek non_shared $item in shared environment then that item might not exist!
	$item = shared_clone($item) if ((is_shared($self)) && (!is_shared($item)));

	if ($self->has($item)) {
		my $index = $self->find($item);

		# perl can't splice shared arrays!
		if (is_shared(@{$self->{items}})) {
			splice_shared($self->{items}, $index, 1);
		} else {
			splice(@{$self->{items}}, $index, 1);
		}
	}
}

##
# void $set->clear()
# Ensures: @{$self} == 0
#
# Remove all items in the set.
sub clear {
	my ($self) = @_;

	# MultiThreading Support
	if (is_shared($self)) {
		lock ($self);
		$self->{items} = &share([]);
	} else {
		$self->{items} = [];
	};
}

##
# $set->get(int index)
# Requires: 0 <= $index < @{$set}
#
# Returns the item at the specified index.
sub get {
	my ($self, $index) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));
	# assert($index >= 0) if DEBUG;

	return $self->{items}[$index];
}

##
# boolean $set->has(item)
#
# Check whether $item is in the set.
sub has {
	my ($self, $item) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));

	return 1 if ($self->find($item) >= 0);
	return 0;
}

##
# int $set->find(item)
#
# Find $item index in "Set" array.
sub find {
	my ($self, $item) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));
	for (my $i = 0; $i < @{$self->{items}}; $i++) {
		my $existing_item = $self->{items}[$i];
		if (is_shared($self)) {
			# Check by internal shared refaddr
			return $i if (is_shared($existing_item) == is_shared($item));
		} else {
			# Check by normal refaddr
			return $i if (refaddr($existing_item) == refaddr($item));
		};
	}
	return -1;
}

##
# int $set->size()
# Ensures: result >= 0
#
# Returns the number of elements in this set.
sub size {
	my ($self) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));

	return scalar(@{$self->{items}});
}

##
# Array $set->getArray()
# Ensures:
#     defined(result)
#     for all $element in result: defined($element)
#
# Return the set's internal array. You must not manipulate this array.
sub getArray {
	my ($self) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));

	return $self->{items};
}

##
# Set $set->deepCopy()
# Ensures: defined(result)
#
# Create a deep copy of this set. The items themselves are not copied.
sub deepCopy {
	my ($self) = @_;

	# MultiThreading Support
	lock ($self) if (is_shared($self));

	my $copy = new Set();
	$copy->{items} = [ @{$self->{items}} ];

	# $copy = shared_clone($copy) if (is_shared($self));
	return $copy;
}

sub _toString {
	return sprintf("%s(0x%x)",
		Scalar::Util::blessed($_[0]),
		Scalar::Util::refaddr($_[0]));
}

1;
