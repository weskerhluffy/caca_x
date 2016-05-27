#!/usr/bin/perl -pi
#multi-line in place substitute - subs.pl
use strict;
use warnings;

BEGIN {undef $/;}

s/START.*STOP/replace_string/smg;
