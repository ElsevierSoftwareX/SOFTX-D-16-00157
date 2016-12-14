#ifndef _tfr_noise_gener_
#define _tfr_noise_gener_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

/* macros */

#define PINK_ORDER 5
//#define PINK_MAXKEY 0x1f
#define PINK_MAXKEY 0xf
#define PINK_RANGE 6e6

/* noise generator definition */

class tfr_noise_generator{
private:

  unsigned int* m_white_buffer;
  int m_pink_key;

  double rand_pink();

public:
  tfr_noise_generator();
  ~tfr_noise_generator();

  double add_awgn_using_snr(tfr_complex_signal*, double);
  double add_awgn_using_sigma(tfr_complex_signal*, double);
  double add_awgn_at_exact_snr(tfr_complex_signal*, double);

  double add_pink_using_range(tfr_complex_signal*, double);
  double add_pink_at_exact_snr(tfr_complex_signal*, double);
};
  
#endif

