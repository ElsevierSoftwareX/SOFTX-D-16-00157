#ifndef _tfr_fourier_spectrum_
#define _tfr_fourier_spectrum_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-complex-signal.hh"
class tfr_complex_signal;
struct tfr_signal_config;

/* fourier spectrum class definition */

class tfr_fourier_spectrum{
private:

  /* configuration */
  tfr_signal_config* m_signal_conf;

  void fft_shift();

public:

  tfr_fourier_spectrum(tfr_complex_signal*);
  ~tfr_fourier_spectrum();

  tfr_signal_config* get_signal_configuration();
  tfr_signal_config* get_config();

  tfr_complex_signal* get_complex_signal();
  tfr_complex_signal* get_signal();
  
  complex double* m_spectrum; 
  
  void save(const char *);
  void save_to_file(const char *);

  double calc_energy();
};

#endif

