#ifndef _tfr_bartlett_pergram_
#define _tfr_bartlett_pergram_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-real-array.hh"
struct tfr_array_config;

#include "tfr-complex-signal.hh"
class tfr_complex_signal;
struct tfr_signal_config;

#include "tfr-fourier-spectr.hh"
class tfr_fourier_spectrum;

/* fourier spectrum class definition */

class tfr_bartlett_pergram{
private:

  /* configuration */
  tfr_array_config* m_array_conf;

  int m_added_frames; 
  double m_rate;

public:
  
  tfr_bartlett_pergram(tfr_array_config*);
  ~tfr_bartlett_pergram();

  double get_rate();
  double get_sampling_rate();

  tfr_array_config* get_array_configuration();
  tfr_array_config* get_config();

  double add_complex_signal(tfr_complex_signal*, int =1);
  double add_signal(tfr_complex_signal*, int =1);
  
  double* m_periodogram; 
  
  double calc_energy();

  void save(const char*);
  void save_to_file(const char*);
};

#endif

