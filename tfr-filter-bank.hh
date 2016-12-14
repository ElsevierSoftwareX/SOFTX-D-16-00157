#ifndef _tfr_filter_bank_
#define _tfr_filter_bank_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-real-array.hh"
struct tfr_array_config;

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

#include "tfr-recursive-filter.hh"
class tfr_recursive_filter;
struct tfr_filter_config;

/* filter bank class definition */

class tfr_filter_bank{
private:

  /* configuration */
  tfr_filter_config* m_filter_conf;
  tfr_array_config* m_bank_conf;

public:

  tfr_filter_bank(tfr_array_config*, tfr_filter_config*);
  ~tfr_filter_bank();

  int get_processed_sample_number();
  tfr_filter_config* get_filter_configuration(int=0);
  tfr_array_config* get_bank_configuration();
  tfr_array_config* get_config();
  
  tfr_complex_signal** filtering(tfr_complex_signal*, int =1);
  void filtering(complex double);

  tfr_recursive_filter** m_filter_array;
};

#endif

