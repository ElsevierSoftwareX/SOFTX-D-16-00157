
CC := g++

LIBS := -lm -lsndfile -lfftw3 -ansi
FLAGS := -pedantic -w -Wall -g -O2
CXXFLAGS := $(FLAGS) $(LIBS)

TESTS:=  test-filter test-wavfile test-synthetic test-noise test-speech
test: $(TESTS)

OBJS :=
OBJS := $(OBJS) tfr-misc.o
tfr-misc.o: tfr-misc.cc tfr-misc.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-misc.cc 

OBJS := $(OBJS) tfr-reass.o
tfr-reass.o: tfr-reass.cc tfr-reass.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-reass.cc 

OBJS := $(OBJS) tfr-raster-image.o
tfr-raster-image.o: tfr-raster-image.cc tfr-raster-image.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-raster-image.cc 

OBJS := $(OBJS) tfr-real-array.o
tfr-real-array.o: tfr-real-array.cc tfr-real-array.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-real-array.cc 

OBJS := $(OBJS) tfr-complex-signal.o
tfr-complex-signal.o: tfr-complex-signal.cc tfr-complex-signal.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-complex-signal.cc

OBJS := $(OBJS) tfr-recursive-filter.o
tfr-recursive-filter.o: tfr-recursive-filter.cc tfr-recursive-filter.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-recursive-filter.cc

OBJS := $(OBJS) tfr-filter-bank.o
tfr-filter-bank.o: tfr-filter-bank.cc tfr-filter-bank.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-filter-bank.cc 

OBJS := $(OBJS) tfr-pulse-gener.o
tfr-pulse-gener.o: tfr-pulse-gener.cc tfr-pulse-gener.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-pulse-gener.cc

OBJS := $(OBJS) tfr-noise-gener.o
tfr-noise-gener.o: tfr-noise-gener.cc tfr-noise-gener.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-noise-gener.cc

OBJS := $(OBJS) tfr-fourier-spectr.o
tfr-fourier-spectr.o: tfr-fourier-spectr.cc tfr-fourier-spectr.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-fourier-spectr.cc

OBJS := $(OBJS) tfr-bartlett-pergram.o
tfr-bartlett-pergram.o: tfr-bartlett-pergram.cc tfr-bartlett-pergram.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-bartlett-pergram.cc

OBJS := $(OBJS) tfr-window-gener.o
tfr-window-gener.o: tfr-window-gener.cc tfr-window-gener.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-window-gener.cc

OBJS := $(OBJS) tfr-analyzer.o
tfr-analyzer.o: tfr-analyzer.cc tfr-analyzer.hh
	$(CC) $(CXXFLAGS) -c -o $@ tfr-analyzer.cc 

OBJS := $(OBJS) tfr-iir-analyzer.o
tfr-iir-analyzer.o: tfr-iir-analyzer.cc tfr-iir-analyzer.hh 
	$(CC) $(CXXFLAGS) -c -o $@ tfr-iir-analyzer.cc 

OBJS := $(OBJS) tfr-fft-analyzer.o
tfr-fft-analyzer.o: tfr-fft-analyzer.cc tfr-fft-analyzer.hh 
	$(CC) $(CXXFLAGS) -c -o $@ tfr-fft-analyzer.cc 


test-filter: test-filter.cc $(OBJS)
	$(CC) $@.cc $(OBJS) $(CXXFLAGS) -o $@
test-wavfile: test-wavfile.cc $(OBJS)
	$(CC) $@.cc $(OBJS) $(CXXFLAGS) -o $@
test-synthetic: test-synthetic.cc $(OBJS)
	$(CC) $@.cc $(OBJS) $(CXXFLAGS) -o $@
test-noise: test-noise.cc $(OBJS)
	$(CC) $@.cc $(OBJS) $(CXXFLAGS) -o $@
test-speech: test-speech.cc $(OBJS)
	$(CC) $@.cc $(OBJS) $(CXXFLAGS) -o $@


REGEXP1 := "s/define TFR_VERSION \"[0-9]-[0-9][0-9]\"/define TFR_VERSION \"$(shell cat VERSION)\"/g"
REGEXP2 := "s/[0-9]-[0-9][0-9]/$(shell cat VERSION)/g"
version:
	sed -i $(REGEXP1) tfr-misc.hh
	sed -i $(REGEXP2) manual/*.html

VERSION := $(shell cat VERSION)
zip: package
package:
	mkdir ccTFR-$(VERSION)
	cp -r manual ccTFR-$(VERSION)   
	cp *.cc *hh makefile *.gp *.wav ccTFR-$(VERSION)
	cp README AUTHORS LICENSE VERSION ccTFR-$(VERSION)
	zip -mr ccTFR-$(VERSION).zip ccTFR-$(VERSION)

copy:
	cp -f plot-*.gp manual
	cp -f flute.wav manual
	cp -f test-filter.cc manual
	cp -f test-synthetic.cc manual
	cp -f test-wavfile.cc manual
	cp -f test-noise.cc manual
	cp -f test-speech.cc manual
	scp manual/*.wav manual/*.png manual/*.gp manual/*.cc manual/*.html manual/*.css git.eti.pg.gda.pl:/var/www/html/ccTFR/


clean:	
	${RM} ${OBJS} $(TESTS) *~ \#* .\#* 
purge: clean
	${RM} *.png *.txt *.o

