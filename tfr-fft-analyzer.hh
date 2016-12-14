#ifndef _tfr_fft_analyzer_
#define _tfr_fft_analyzer_

/* headers and declarations */

#include "tfr-analyzer.hh"
class tf_analyzer;

#include "tfr-window-gener.hh"
struct tfr_window_config;
class tfr_window_generator;

/* macros */

#define CODE_WIN_D2 0
#define CODE_WIN_D 1
#define CODE_WIN 2
#define CODE_WIN_T 3
#define CODE_WIN_T2 4
#define CODE_WIN_DT 5

/* filter analyzer class definition */

class tfr_fft_analyzer
  : public tfr_analyzer{
private:
  
  /* window generator */
  tfr_window_generator* m_window_gen;
  
  /* STFT for various windows */
  complex double *** m_fourier_spectra;
  complex double ** transforming(int, int);
  
  /* width depends on signal length, then height 
     on fft length, as well as on frequency bandwidth */
  int get_height();
  int get_width();
  int m_height;
  int m_width;
  

public:

  tfr_fft_analyzer(tfr_array_config*, tfr_window_config*);
  ~tfr_fft_analyzer(); 
  
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

