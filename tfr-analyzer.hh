#ifndef _tfr_analyzer_
#define _tfr_analyzer_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-real-array.hh"
struct tfr_array_config;

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

#include "tfr-filter-bank.hh"
class tfr_filter_bank;

#include "tfr-raster-image.hh"
struct tfr_image_config;
class tfr_raster_image;

/* macros */

#define CR_K_ESTIMATOR 0
#define CR_D_ESTIMATOR 1
#define CR_F_ESTIMATOR 2

/* analyzer class definition */

class tfr_analyzer{
protected:
 
  /* configuration */
  tfr_array_config* m_bank_conf;
     
  int m_hop;
  tfr_complex_signal *m_input_signal;

  virtual int get_width() =0;
  virtual int get_height() =0;
  
  void set_signal(tfr_complex_signal*, int =1);

public:
 
  virtual tfr_raster_image* create_empty_image() = 0;
  
  virtual tfr_raster_image* get_spectral_delay() = 0;
  virtual tfr_raster_image* get_spectral_energy() = 0;
  virtual tfr_raster_image* get_instantaneous_frequency() = 0;
  
  tfr_raster_image* get_chirp_rate(int=0);
 
  virtual tfr_raster_image* get_chirp_rate_by_k_estimator() = 0;
  virtual tfr_raster_image* get_chirp_rate_by_d_estimator() = 0;
  virtual tfr_raster_image* get_chirp_rate_by_f_estimator() = 0;
};

#endif

