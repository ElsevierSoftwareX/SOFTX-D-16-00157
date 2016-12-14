/* example of loading wav file as signal object 
   and of time-frequency reassignment */
 
/* external headers */
#include <stdlib.h>
#include <stdio.h>

/* internal tfr headers */
#include "tfr.hh"

int main(void){

  /* WAV file loading  *************************************************** */

  /* example file is nonstationary vibrato perfomed on flute */
  
  /* signal load from a wav file */
  char* wav_name = "flute.wav";
  tfr_complex_signal* signal_ptr = new tfr_complex_signal(wav_name);

  /* analyze only firs second */
  signal_ptr->cut(0.0, 1.0);
 
  /* filter analyzer definition and filtering *************************************************** */

  /* get rate from signal */
  tfr_signal_config* conf_sig = signal_ptr->get_config();
  double rate = conf_sig->rate;
  delete conf_sig;

  /* causal filter definition */
  tfr_filter_config filter_conf;
  filter_conf.spread = 0.01;
  filter_conf.rate = rate;
  filter_conf.order = 5;

  /* parameters for sampling of frequency axis 
     we are focused on the main partial */  
  tfr_array_config bank_conf;
  bank_conf.min = 935.0;
  bank_conf.max = 1165.0;
  bank_conf.length = 500;
  
  /* create TF filter analyzer */
  tfr_iir_analyzer* analyzer_ptr = new tfr_iir_analyzer(&bank_conf, &filter_conf);

  /* set signal */
  int hop = 3;
  analyzer_ptr->set_signal(signal_ptr, hop);

  /* signal is copied, 
     so it can be removed after used */
  delete signal_ptr;
  
  /* get and save spectral energy */
  tfr_raster_image* s_energy = analyzer_ptr->get_spectral_energy();
  s_energy->save("data-s-energy.txt");
  
  /* get and save instantaneous frequency */
  tfr_raster_image* i_freq = analyzer_ptr->get_instantaneous_frequency();
  
  /* get and save spectral delay */
  tfr_raster_image* s_delay = analyzer_ptr->get_spectral_delay();

  /* analyzer is more needed */
  delete analyzer_ptr;

  /* time-frequency reassignment *************************************************** */

  tfr_image_config* img_conf = s_energy->get_config();

  /* incease raster density */
  img_conf->height = 1000;
  img_conf->width = 2000;
  
  tfr_raster_image* r_energy = new tfr_raster_image(img_conf);
  delete img_conf;
  
  reassign_time_frequency(s_delay, i_freq, s_energy, r_energy); 
  r_energy->save("data-r-energy.txt");

  /* remove all imagines */
  delete s_energy, r_energy;
  delete i_freq, s_delay;

  /* you can make png file 
     using plot.gp (gnuplot script) */
  
  /* ending *************************************************** */

  return EXIT_SUCCESS;
}
