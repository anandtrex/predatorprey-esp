#!/usr/bin/perl

open FILE, $ARGV[0] or die $!;

while(<FILE>){
	if($_ =~ m/\sgeneration\s(\d+):.*?Generation\sAverage\sfitness\s\s(\d+.\d+)/){
		print $1." ".$2."\n";
	}
}
