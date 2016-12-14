#ifndef _tfr_iir_analyzer_
#define _tfr_iir_analyzer_

/* headers and declarations */

#include "tfr-analyzer.hh"
class tf_analyzer;

#include "tfr-filter-bank.hh"
struct tfr_filter_config;
class tfr_filter_bank;

/* filter analyzer class definition */

class tfr_iir_analyzer
  : public tfr_analyzer{
private:

  /* configuration */
  tfr_filter_config* m_filter_conf;

  /* filters and filtered signals slots */
  tfr_complex_signal*** m_filtered_signals;
  tfr_filter_bank** m_bank_array;

  /* width depends on signal length 
     height on filter number */
  int get_height();
  int get_width();

public:

  tfr_iir_analyzer(tfr_array_config*, tfr_filter_config*);
  ~tfr_iir_analyzer();  

  void set_signal(tfr_complex_signal*, int =1);

  tfr_raster_image* create_empty_image();

  tfr_raster_image* get_spectral_delay();
  tfr_raster_image* get_spectral_energy();
  tfr_raster_image* get_instantaneous_frequency();

  tfr_raster_image* get_chirp_rate_by_k_estimator();
  tfr_raster_image* get_chirp_rate_by_d_estimator();
  tfr_raster_image* get_chirp_rate_by_f_estimator();
};

#endif

