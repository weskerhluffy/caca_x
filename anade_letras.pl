#!/usr/bin/perl
use strict;
use warnings;
use Tie::File;
use Math::Random::Secure qw(irand);

my @arreglo;

tie @arreglo, 'Tie::File', "ass.txt" or die "when theres trouble \n";

for (@arreglo) {
	my $tipo_query  = "";
	my $query_query = 0;

	$query_query = irand(2);
	$tipo_query  = $query_query ? "Q" : "U";

#	print "teen titans $tipo_query\n";

	s/^/$tipo_query /g
	  ;    # Replace PERL with Perl everywhere in the file
}

untie @arreglo;    # all finished
