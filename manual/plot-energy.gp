#!/usr/bin/gnuplot

set term pngcairo dashed size 1600,900 font "Helvetica,20"

# this section is for eps format:
# set fontpath "/usr/share/matplotlib/mpl-data/fonts/ttf/cmr10.tt"
# set term postscript eps enhanced color font "cmr10, 14" size 9.5cm,7.0cm
# convert -density 300 img-energy.eps -resize 800x600  -flatten -colorspace RGB img-energy.png

unset grid
set pm3d map
set palette rgbformulae 30,31,32 negative

set lmargin at screen 0.1
set rmargin at screen 0.85
set bmargin at screen 0.15
set tmargin at screen 0.97

system("ln -fs data-s-energy.txt data.txt")

TIME_MIN = `head data.txt | grep TIME_MIN | cut -f2 -d=`
TIME_MAX = `head data.txt | grep TIME_MAX | cut -f2 -d=`
FREQ_MIN = `head data.txt | grep FREQ_MIN | cut -f2 -d=`
FREQ_MAX = `head data.txt | grep FREQ_MAX | cut -f2 -d=`
VAL_MAX = `tail data.txt | grep VAL_MAX | cut -f2 -d=`

#set xrange[0.1:2.75]
set xrange[TIME_MIN:TIME_MAX]
set yrange[FREQ_MIN/1000:FREQ_MAX/1000]

set xlabel "time (s)" offset 0,0.4
set ylabel "frequency (kHz)" offset -0.5,0

# ##########################################################################

set cbrange[-60:0]
set cblabel "energy (dBc)" offset 0.9,0
set output "img-senergy.png"
# set output "plot.eps"
splot "data.txt" u 1:($2/1000):(10*log10($3/VAL_MAX))  notitle
print "energy ... done!"

# ##########################################################################

system("ln -fs data-r-energy.txt data.txt")

TIME_MIN = `head data.txt | grep TIME_MIN | cut -f2 -d=`
TIME_MAX = `head data.txt | grep TIME_MAX | cut -f2 -d=`
FREQ_MIN = `head data.txt | grep FREQ_MIN | cut -f2 -d=`
FREQ_MAX = `head data.txt | grep FREQ_MAX | cut -f2 -d=`
VAL_MAX = `tail data.txt | grep VAL_MAX | cut -f2 -d=`

#set xrange[0.1:2.75]
set xrange[TIME_MIN:TIME_MAX]
set yrange[FREQ_MIN/1000:FREQ_MAX/1000]

set xlabel "time (s)" offset 0,0.4
set ylabel "frequency (kHz)" offset -0.5,0

# ##########################################################################

set cbrange[-70:0]
set cblabel "energy (dBc)" offset 0.9,0
set output "img-renergy.png"
splot "data.txt" u 1:($2/1000):(10*log10($3/VAL_MAX))  notitle
print "energy ... done!"

# ##########################################################################
