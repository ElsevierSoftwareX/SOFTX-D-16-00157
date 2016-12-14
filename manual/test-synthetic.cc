/* example of TF anayzer using synthetic signals */
 
/* external headers */
#include <stdlib.h>
#include <stdio.h>

/* internal tfr headers */
#include "tfr.hh"

int main(void){

  tf_print_info();
  
  /* two-component signal construction *************************************************** */  


  /* the first stage is empty signal construction */
  int length = 1200;
  double start = 0.0;
  double rate = 1000.0;

  tfr_signal_config sig_conf;
  sig_conf.length = length; 
  sig_conf.start = start; 
  sig_conf.rate = rate; 

  tfr_complex_signal* signal_ptr = new tfr_complex_signal(&sig_conf);

  /* generator definition *************************************************** */

  /* generator configuration is used depended on which signal is generated */
  /* here LFM chirp definition */

  tfr_pulse_config gen_conf;
  gen_conf.transition = 0.5;
  gen_conf.period = 0.5;
  gen_conf.delay = 0.35;
  gen_conf.width = 0.3;

  /* first component */
  double crate1 = 1000.0;
  gen_conf.bandwidth = crate1 * gen_conf.width; /* 300.0; */ 
  gen_conf.frequency = 200.0;
  gen_conf.amplitude = 1.0;
  gen_conf.phase = 0.0;
  
  /* new generator has configured parameters  */
  tfr_pulse_generator* pulse_gener_ptr = new tfr_pulse_generator(&gen_conf);
  
  /* generator add a generated signal to the argument 
     the added signal type can be choosen by using 
     an apropriate generator method */
  pulse_gener_ptr->add_lfm_chirp(signal_ptr);

  /* random fsk signal */
  /* gener_ptr->add_random_fsk(signal_ptr); */

  /* generator configuration can be updated */
  /* second component */
  double crate2 = 500.0;
  gen_conf.bandwidth = crate2 * gen_conf.width; /* 150.0; */
  gen_conf.frequency = 400.0;
  gen_conf.amplitude = 1.0;
  
  pulse_gener_ptr->update_configuration(&gen_conf);  
  pulse_gener_ptr->add_lfm_chirp(signal_ptr);
  
  /* add awgn also by using the generator */
  tfr_noise_generator* noise_gener_ptr = new tfr_noise_generator();
  double snr = 20.0;
  
  double sig_energy = signal_ptr->calc_energy();
  double noise_energy = noise_gener_ptr->add_awgn_at_exact_snr(signal_ptr, snr);

  double true_snr = 10.0*log10(sig_energy/noise_energy); 
  fprintf(stdout, "(info) true SNR is %.3f dB\n", true_snr);

  /* save to wavfile */
  signal_ptr->save_wav("output.wav"); 
  
  /* save signal */
  signal_ptr->save("signal.txt");
  
  /* array configuration is used in filter analyzer as well as in Fourier analyzer ************* */

  /* parameters for sampling of frequency axis */
  tfr_array_config arr_conf;
  arr_conf.min = -10.0;
  arr_conf.max = 500.0;
  arr_conf.length = 1024;

  /* first variant of analysis */
  /* filter analyzer definition and filtering *************************************************** */
  
  /* causal filter definition */
  tfr_filter_config filter_conf;

  /* time spread which corresponds to effective duration of impulse response */
  filter_conf.spread = 0.01;

  /* sampling rate should be the same as for the analyzed signal */
  filter_conf.rate = rate;

  /* IIR filter order */
  filter_conf.order = 5;

  /* create TF filter analyzer */
  /* this line can be uncomment instead of tfr_fft_analyzer --> 
  tfr_iir_analyzer* analyzer_ptr = new tfr_iir_analyzer(&arr_conf, &filter_conf); /**/

  /* second variant of analysis */
  /* window definition and Fourier transformation (STFT) ********************************************* */

  /* finite window definition */
  tfr_window_config win_conf;

  /* time length of the window in samples, the most important window propertie */
  win_conf.length = 100;
  
  /* if crate is diffrent than 0, then chirplet transformation is performed */
  win_conf.crate = 1000.0; 

  /* sampling rate should be the same as for the analyzed signal */
  win_conf.rate = rate;

  /* At the moment, only 0 works which means Blackman-Harris window */
  win_conf.code = 0;

  /* ******************************************************************************************** */
  
  /* create TF Fourier analyzer */
  /* this line has to be comment if tfr_iir_analyzer is used  --> */
  tfr_fft_analyzer* analyzer_ptr = new tfr_fft_analyzer(&arr_conf, &win_conf); /**/

  /* ******************************************************************************************** */
  
  /* signal introduction */
  analyzer_ptr->set_signal(signal_ptr);

  /* signal is copied, 
     so they can be removed after used */
  delete signal_ptr;
  
  /* get and save spectral energy */
  tfr_raster_image* s_energy = analyzer_ptr->get_spectral_energy();
  s_energy->save("data-s-energy.txt"); 

  /* get and save instantaneous frequency */
  tfr_raster_image* i_freq = analyzer_ptr->get_instantaneous_frequency();
  /* i_freq->save("data-i-freq.txt"); */

  /* get and save spectral delay */
  tfr_raster_image* s_delay = analyzer_ptr->get_spectral_delay();
  /* s_delay->save("data-s-delay.txt"); */

  /* get and save chirp rate */
  tfr_raster_image* c_rate = analyzer_ptr->get_chirp_rate(2);
  c_rate->save("data-c-rate.txt"); 

  /* analyzer is more needed */
  delete analyzer_ptr;

  /* time-frequency reassignment ************************************* */

  /* we have two functions dedicated for energy reassignment.
     First (time-frequency) uses only spectral delay and instantaneous frequency.
     Second (frequency) uses spectral delay, instantaneous frequency, as well as chirp rate.
   */
  
  /* time frequency reassignment */
  tfr_raster_image* r_energy = reassign_time_frequency(s_delay, i_freq, s_energy); 
  
  /* frequency reassignment - you can check it by switch on the next lines: */
  /* tfr_raster_image* r_energy = reassign_frequency(s_delay, c_rate, i_freq, s_energy); */
  /* tfr_raster_image* r_energy = reassign_frequency(i_freq, s_energy); */

  /* save energy */
  r_energy->save("data-r-energy.txt");

  /* reassignment as profile ******************************************* */
  /* this is an additional statistic analysis */

  /* chirp-rate profile (histogram) calculation */
  tfr_array_config arr_prof_conf;
  arr_prof_conf.min = 0.0;
  arr_prof_conf.max = 1500.0;
  arr_prof_conf.length = 10000;
  
  tfr_real_array* cr_profile_ptr = calculate_profile(c_rate, s_energy, &arr_prof_conf);
  
  cr_profile_ptr->save("profile.txt");
  /* plot it in gnuplot as
     plot 'profile.txt' u 1:2 with lines */

  /* vertical marginal distribution of chirp rate */
  tfr_real_array* cr_marginal_ptr = c_rate->get_average_in_frequency(s_energy);
  cr_marginal_ptr->save("marginal.txt");
  
  /* remove all imagines */
  delete s_delay, i_freq, c_rate;
  delete r_energy, s_energy;
  
  /* you can make png file 
     using plot.gp (gnuplot script) */
  
  /* ending *************************************************** */
  
  return EXIT_SUCCESS;
}
