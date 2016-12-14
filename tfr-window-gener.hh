#ifndef _tfr_window_generator_
#define _tfr_window_generator_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

/* macros */

#define WINDOW_BLACKMAN_HARRIS 0

/* struct for window generator */

struct tfr_window_config{

  double crate;
  double rate;
  int length;
  int code;
};
  
/* window generator class definition */

class tfr_window_generator{
private:

  /* configuration */
  tfr_window_config* m_window_conf;
  
  tfr_complex_signal* get_blackman_harris_win();
  tfr_complex_signal* get_blackman_harris_dwin();
  tfr_complex_signal* get_blackman_harris_d2win();
  
  tfr_complex_signal* get_empty_window();

 public:

  tfr_window_generator(tfr_window_config*);
  ~tfr_window_generator();

  double get_window_sampling_rate();
  double get_rate();

  int get_window_length();
  int get_length();

  double get_window_duration();    
  double get_duration();

  double get_window_chirp_rate();    
  double get_crate();
  
  void update_window_chirp_rate(double);
  void update_crate(double);
  
  tfr_window_config* get_window_configuration();
  tfr_window_config* get_config();

  tfr_complex_signal* get_window_waveform(int =0, int =0);
  tfr_complex_signal* get_window(int =0, int =0);
};

#endif

