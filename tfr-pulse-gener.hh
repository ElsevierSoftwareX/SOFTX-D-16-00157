#ifndef _tfr_pulse_gener_
#define _tfr_pulse_gener_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

/* struct for signal generator */

struct tfr_pulse_config{
  
  double transition;
  double period;
  double width;

  double amplitude;
  double delay;
  double phase;
  
  double frequency;
  double bandwidth;  
};


/* pulse generator definition */

class tfr_pulse_generator{
private:

  /* configuration */
  tfr_pulse_config* m_pulse_conf;

  double get_tukey(double);
  bool check_pulse(double);
  
  complex double get_harmonic(double);
  complex double get_lfm_chirp(double);
  complex double get_hfm_chirp(double);
  
public:

  tfr_pulse_generator(tfr_pulse_config*);
  ~tfr_pulse_generator();

  void update_configuration(tfr_pulse_config*);
  tfr_pulse_config* get_pulse_configuration();
  tfr_pulse_config* get_config();

  double add_harmonic(tfr_complex_signal*);
  double add_lfm_chirp(tfr_complex_signal*);
  double add_hfm_chirp(tfr_complex_signal*);

  double add_random_fsk(tfr_complex_signal*);
};
  
#endif

