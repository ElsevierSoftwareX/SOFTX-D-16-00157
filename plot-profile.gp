#!/usr/bin/gnuplot

set term pngcairo dashed size 1600,1200 font "Helvetica,20"

unset grid

set lmargin at screen 0.15
set rmargin at screen 0.8
set bmargin at screen 0.15
set tmargin at screen 0.97

START = `head profile.txt | grep START | cut -f2 -d=`
STOP = `head profile.txt | grep STOP | cut -f2 -d=`

set xrange[START:STOP]

set xlabel "chirp-rate (Hz/s)" offset 0,0.4
set ylabel "energy" offset -0.5,0

# ##########################################################################

set output "img-profile.png"
plot "profile.txt" u 1:2  with lines notitle
print "profile ... done!"

# ##########################################################################
