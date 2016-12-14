#!/usr/bin/gnuplot

set term pngcairo dashed size 1600,900 font "Helvetica,20"

unset grid
set pm3d map

set palette defined(    \
    0.0  1.0  1.0  1.0, \
    0.5  0.9  0.0  0.0, \
    2.5  1.0  0.2  0.0, \
    3.3  0.9  0.8  0.0, \
    3.9  0.3  1.0  0.0, \
    4.0  0.0  1.0  0.0, \
    4.1  0.0  1.0  0.4, \
    4.7  0.0  0.5  1.0, \
    5.5  0.0  0.2  1.0, \
    7.5  0.0  0.1  0.8, \
    8.0  1.0  1.0  1.0  \
) negative

set lmargin at screen 0.1
set rmargin at screen 0.85
set bmargin at screen 0.15
set tmargin at screen 0.97

# system("ln -fs data-i-freq.txt data.txt")
# system("ln -fs data-s-delay.txt data.txt")
system("ln -fs data-c-rate.txt data.txt")

TIME_MIN = `head data.txt | grep TIME_MIN | cut -f2 -d=`
TIME_MAX = `head data.txt | grep TIME_MAX | cut -f2 -d=`
FREQ_MIN = `head data.txt | grep FREQ_MIN | cut -f2 -d=`
FREQ_MAX = `head data.txt | grep FREQ_MAX | cut -f2 -d=`
VAL_MAX = `tail data.txt | grep VAL_MAX | cut -f2 -d=`

set xrange[TIME_MIN:TIME_MAX]
set yrange[FREQ_MIN/1000:FREQ_MAX/1000]

set xlabel "time (s)" offset 0,0.4
set ylabel "frequency (kHz)" offset -0.5,0

# ##########################################################################
# you can plot by uncommenting appropriate section 
# please set also cbrange
# ##########################################################################

set cbrange[-18:18]
set cblabel "chirp rare (kHz/s)" offset 0.9,0
set output "img-crate.png"
splot "data-c-rate.txt" u 1:($2/1000):($3/1000)  notitle
print "crate ... done!"

# ##########################################################################

# set cbrange[850:1200]
# set cblabel "frequency (Hz)" offset 0.9,0
# set output "img-i-freq.png"
# splot "data-i-freq.txt" u 1:($2/1000):3  notitle
# print "ifreq ... done!"

# ##########################################################################

# set cbrange[-0.1:0.1]
# set cblabel "delay (s)" offset 0.9,0
# set output "img-s-delay.png"
# splot "data-s-delay.txt" u 1:($2/1000):3  notitle
# print "sdelay ... done!"

# ##########################################################################
