#ifndef _tfr_complex_signal_
#define _tfr_complex_signal_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-fourier-spectr.hh"
class tfr_fourier_spectrum;

/* macros */

#define WHITE_NOISE 0

/* struct for complex signal */

struct tfr_signal_config{
  
  double start;
  double rate;
  int length;
};

/* complex signal class definition */

class tfr_complex_signal{
private:

  /* configuration */
  tfr_signal_config* m_signal_conf;
  
  bool check_real();
  bool check_imag();

 public:

  tfr_complex_signal(tfr_signal_config*);
  tfr_complex_signal(char*, int=0);
  ~tfr_complex_signal();

  bool compare_configuration(tfr_signal_config*);
  tfr_signal_config* get_signal_configuration();
  tfr_signal_config* get_config();

  complex double* m_waveform; 
  
  int copy_waveform(tfr_complex_signal*, int =0);
  int copy(tfr_complex_signal*, int =0);

  void clear_waveform();
  void clear();
  
  void modulate_waveform(double);
  void modulate(double);

  int cut_waveform(double, double);
  int cut(double, double);

  double calc_energy();
  
  tfr_fourier_spectrum* get_spectrum();
  tfr_fourier_spectrum* get_fourier_spectrum();

  void save(const char *);
  void save_to_file(const char *);

  void save_wav(const char *);
  void save_to_wavfile(const char *);

  void operator += (tfr_complex_signal*);
  void operator -= (tfr_complex_signal*);
};


#endif

