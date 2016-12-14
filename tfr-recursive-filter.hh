#ifndef _tfr_recursive_filter_
#define _tfr_recursive_filter_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

/* struct for filter configuration */

struct tfr_filter_config{
  
  double frequency;
  double spread;
  double rate;
  int order;
};

/* recursive filter class definition */

class tfr_recursive_filter{
private:

  /* configuration */
  tfr_filter_config* m_filter_conf;

  complex double* m_bcoeff; 
  complex double* m_acoeff;
  complex double* m_buffer; 

  int m_processed;
  
public:
  
  tfr_recursive_filter(tfr_filter_config*);
  ~tfr_recursive_filter();

  int get_processed_sample_number();
  complex double get_p_factor();
  
  tfr_filter_config* get_filter_configuration();
  tfr_filter_config* get_config();

  complex double m_output;
  
  complex double filtering(complex double);
  tfr_complex_signal* filtering(tfr_complex_signal*, int =1);
};

#endif

