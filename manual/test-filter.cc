/* example for simple signal generation */
/* as well as of its filtering by a single IIR filter */
 
/* external headers */
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>

/* internal tfr headers */
#include "tfr.hh"

int main(void){

  /* two-component signal generation *************************************************** */

  /* sampling rate and number of samples */
  double rate = 1000.0;
  double start = 0.0;
  int length = 1001;

  /* if struct of signal configuration is used (not pointer)
     it will be removed automatically */
  tfr_signal_config sig_conf; 
  sig_conf.length = length; 
  sig_conf.start = start; 
  sig_conf.rate = rate; 

  /* signal construction */
  tfr_complex_signal* in_signal_ptr = new tfr_complex_signal(&sig_conf);

  /* parameters of generated signals */
  /* both have constant frequency */
  double freq_1 = 100.0;
  double freq_2 = 200.0;
  double amp_1 = 2.0;
  double amp_2 = 1.0;

  /* calculation of sample values */
  for (int n=0; n<length; n++){
    double time = (double)n/rate;
    double arg_1 = TWO_PI * freq_1 * time;
    double arg_2 = TWO_PI * freq_2 * time;

    in_signal_ptr->m_waveform[n] = amp_1 * cexp(1I*arg_1) + amp_2 * cexp(1I*arg_2);
  }

  /* example of using signal method */
  double carrier_frequency = 50.0;
  in_signal_ptr->modulate(carrier_frequency);

  /* recursive filter definition *************************************************** */

  tfr_filter_config f_conf;
  f_conf.frequency = 250.0;
  f_conf.spread = 0.1;
  f_conf.rate = rate;
  f_conf.order = 5;
  
  tfr_recursive_filter* filter_ptr = new tfr_recursive_filter(&f_conf);

  /* filtering ********************************************************************** */

  tfr_complex_signal* out_signal_ptr = filter_ptr->filtering(in_signal_ptr);

  /* removed after filtering */
  delete filter_ptr;

  /* check filtering by spectra cmp *************************************************** */

  tfr_fourier_spectrum* in_spec_ptr = in_signal_ptr->get_spectrum();
  tfr_fourier_spectrum* out_spec_ptr = out_signal_ptr->get_spectrum();

  /* removed after using */
  delete in_signal_ptr;
  delete out_signal_ptr;

  /* save both spectra to txtfiles */
  in_spec_ptr->save("in-spec.txt");
  out_spec_ptr->save("out-spec.txt");
  
  /* removed after printing */
  delete out_spec_ptr;
  delete in_spec_ptr;
    
  /* obtained spectra are print on standard output 
     you can catch their in console:
     ./test-filter > data.txt */
  
  /* ending *************************************************** */
  
  return EXIT_SUCCESS;
}
