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
  char* wav_name = "mail.wav";
  tfr_complex_signal* signal_ptr = new tfr_complex_signal(wav_name);
  signal_ptr->save("mail.txt");

  tfr_signal_config* conf_sig = signal_ptr->get_config();
  double rate = conf_sig->rate;
  delete conf_sig;

  /* array configuration is used in Fourier analyzer ************* */
  tfr_array_config arr_conf;
  arr_conf.min = 0.0;
  arr_conf.max = 1500.0;
  arr_conf.length = 4096;
  
  /* finite window definition ************* */
  tfr_window_config win_conf;
  win_conf.length = 900;
  win_conf.crate = 0.0; 
  win_conf.rate = rate;
  win_conf.code = 0;

  /* create TF Fourier analyzer */
  tfr_fft_analyzer* analyzer_ptr = new tfr_fft_analyzer(&arr_conf, &win_conf); /**/

  int hop = 10;
  analyzer_ptr->set_signal(signal_ptr, hop);
  delete signal_ptr;

  /* get and save spectral energy */
  tfr_raster_image* s_energy = analyzer_ptr->get_spectral_energy();
  s_energy->save("data-s-energy.txt");

  /* get and save instantaneous frequency */
  tfr_raster_image* i_freq = analyzer_ptr->get_instantaneous_frequency();

  /* get and save spectral delay */
  tfr_raster_image* s_delay = analyzer_ptr->get_spectral_delay();

  /* time frequency reassignment */
  tfr_raster_image* r_energy = reassign_time_frequency(s_delay, i_freq, s_energy); 
  r_energy->save("data-r-energy.txt");

  /* get and save chirp rate */
  tfr_raster_image* c_rate = analyzer_ptr->get_chirp_rate(2);
  c_rate->save("data-c-rate.txt"); 

  /* ending *************************************************** */

  return EXIT_SUCCESS;
}
