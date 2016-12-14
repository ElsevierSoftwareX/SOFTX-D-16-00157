
#include "tfr-pulse-gener.hh"

/* **************************************************************************** */

/* generator constructor and deconstructor */

tfr_pulse_generator :: tfr_pulse_generator(tfr_pulse_config* a_conf){
  
  /* generator produces series of pulses 
     which are tapering by Tukey window */

  srand(time(NULL));

  m_pulse_conf = new tfr_pulse_config;
  update_configuration(a_conf);
}

tfr_pulse_generator :: ~tfr_pulse_generator(){
  
  /* deconstructor */

  delete m_pulse_conf;
}

/* **************************************************************************** */

void tfr_pulse_generator :: update_configuration(tfr_pulse_config* a_conf){

  /* configuration can be updated 
     each time if needed */

  if(a_conf==NULL)
    call_error("generator conf is null");

  if(a_conf->transition<0 or a_conf->transition>1)
    call_error("transition should be in <0, 1> range");
  
  memcpy(m_pulse_conf, a_conf, sizeof(tfr_pulse_config));
}

/* **************************************************************************** */

tfr_pulse_config* tfr_pulse_generator :: get_config(){

  /* short */

  return get_pulse_configuration();
}

tfr_pulse_config* tfr_pulse_generator ::  get_pulse_configuration(){

  /* return generator parameters as structure */
  
  tfr_pulse_config* conf = new tfr_pulse_config;
  memcpy(conf, m_pulse_conf, sizeof(tfr_pulse_config));

  return conf;
}

/* **************************************************************************** */

double tfr_pulse_generator :: get_tukey(double a_base_time){

  /* this method is used for pulses tapering 
     transition should be in <0, 1> range */
  
  if (m_pulse_conf->transition==0)
    return 1.0;
  
  double time = a_base_time / m_pulse_conf->width;
  if(time<-0.5) return 0.0;
  if(time>0.5) return 0.0;
  time += 0.5;
  
  if (time <= m_pulse_conf->transition / 2)
    return 0.5 * (1 + cos(M_PI*(2.0 * time / m_pulse_conf->transition -1)));
  
  if (time >= 1-m_pulse_conf->transition / 2)
    return 0.5 * (1 + cos(M_PI*(2.0 * time / m_pulse_conf->transition - 2.0 / m_pulse_conf->transition +1)));
  
  return 1.0;  
}

/* **************************************************************************** */

bool tfr_pulse_generator :: check_pulse(double a_time){

  /* check pulse occurs in a given time */
  
  double time = a_time - m_pulse_conf->delay;
  
  while(time<-m_pulse_conf->period/2)
    time += m_pulse_conf->period;
  
  while(time>m_pulse_conf->period/2)
    time -= m_pulse_conf->period;
  
  if(time<-m_pulse_conf->width/2) return false;
  if(time>m_pulse_conf->width/2) return false;
  
  return true;
}

/* **************************************************************************** */

complex double tfr_pulse_generator :: get_harmonic(double a_time){

  /* calculate a sample of harmonic signal (without frequency modulation) */

  double time = a_time - m_pulse_conf->delay;

  while(time<-m_pulse_conf->period/2)
    time += m_pulse_conf->period;

  while(time>m_pulse_conf->period/2)
    time -= m_pulse_conf->period;

  if(time<-m_pulse_conf->width/2) return 0.0;
  if(time>m_pulse_conf->width/2) return 0.0;

  double arg = TWO_PI * time * m_pulse_conf->frequency + m_pulse_conf->phase;  
  return get_tukey(time) * m_pulse_conf->amplitude * cexp(I*arg);
}

double tfr_pulse_generator :: add_harmonic(tfr_complex_signal* a_sig){

  /* generate signal and add it to a given argument */

  if(a_sig==NULL)
    call_error("signal is null");
  
  tfr_signal_config* sig_conf = a_sig->get_config();

  /* fullfill waveform */
  double added_energy = 0.0;
  for (int n=0; n<sig_conf->length; n++){

    double time = sig_conf->start + (double)n/sig_conf->rate;
    complex double sample = get_harmonic(time);
    added_energy += pow(cabs(sample), 2.0);
    a_sig->m_waveform[n] += sample;
  }
  
  delete sig_conf;
  return added_energy;
}

/* **************************************************************************** */

complex double tfr_pulse_generator :: get_lfm_chirp(double a_time){

  /* calculate a sample of linear frequency modulated signal */
  
  double time = a_time - m_pulse_conf->delay;

  while(time<-m_pulse_conf->period/2)
    time += m_pulse_conf->period;

  while(time>m_pulse_conf->period/2)
    time -= m_pulse_conf->period;

  if(time<-m_pulse_conf->width/2) return 0.0;
  if(time>m_pulse_conf->width/2) return 0.0;

  double chirprate = m_pulse_conf->bandwidth / m_pulse_conf->width;
  double arg = TWO_PI * time * m_pulse_conf->frequency + m_pulse_conf->phase;  
  arg += M_PI * chirprate * pow(time, 2.0);
  
  return get_tukey(time) * m_pulse_conf->amplitude * cexp(I*arg);
}

double tfr_pulse_generator :: add_lfm_chirp(tfr_complex_signal* a_sig){

  /* generate signal and add it to a given argument */

  if(a_sig==NULL)
    call_error("signal is null");
  
  tfr_signal_config* sig_conf = a_sig->get_config();

  /* fullfill waveform */
  double added_energy = 0.0;
  for (int n=0; n<sig_conf->length; n++){

    double time = sig_conf->start + (double)n/sig_conf->rate;
    complex double sample = get_lfm_chirp(time);
    added_energy += pow(cabs(sample), 2.0);
    a_sig->m_waveform[n] += sample;
  }
  
  delete sig_conf;
  return added_energy;
}

/* **************************************************************************** */

complex double tfr_pulse_generator :: get_hfm_chirp(double a_time){

  /* calculate a sample of hyperbolic frequency modulated signal */

  double time = a_time - m_pulse_conf->delay;

  while(time<-m_pulse_conf->period/2)
    time += m_pulse_conf->period;

  while(time>m_pulse_conf->period/2)
    time -= m_pulse_conf->period;

  if(time<-m_pulse_conf->width/2) return 0.0;
  if(time>m_pulse_conf->width/2) return 0.0;

  /* f_stop should be grater than f_start - however it is not tested */
  /* f_start, f_stop should be grater than 0 - however it is not tested */  
  double f_start = m_pulse_conf->frequency - m_pulse_conf->bandwidth / 2;
  double f_stop = m_pulse_conf->frequency + m_pulse_conf->bandwidth / 2;
  double c_rate = m_pulse_conf->bandwidth / m_pulse_conf->width;
  
  double arg = m_pulse_conf->phase;
  arg -= TWO_PI*f_start*f_stop * log(f_start+f_stop-2*time*c_rate) / c_rate;

  return get_tukey(time) * m_pulse_conf->amplitude * cexp(I*arg);
}

double tfr_pulse_generator :: add_hfm_chirp(tfr_complex_signal* a_sig){

  /* generate signal and add it to a given argument */

  if(a_sig==NULL)
    call_error("signal is null");
  
  tfr_signal_config* sig_conf = a_sig->get_config();

  /* fullfill waveform */
  double added_energy = 0.0;
  for (int n=0; n<sig_conf->length; n++){

    double time = sig_conf->start + (double)n/sig_conf->rate;
    complex double sample = get_hfm_chirp(time);
    added_energy += pow(cabs(sample), 2.0);
    a_sig->m_waveform[n] += sample;
  }
  
  delete sig_conf;
  return added_energy;
}

/* **************************************************************************** */

double tfr_pulse_generator :: add_random_fsk(tfr_complex_signal* a_sig){

  /* generate signal and add it to a given argument */

  if(a_sig==NULL)
    call_error("signal is null");
  
  tfr_signal_config* sig_conf = a_sig->get_config();
  bool pulse_flag = false;

  /* fullfill waveform */
  double added_energy = 0.0;
  double conf_frequency = m_pulse_conf->frequency;
  for (int n=0; n<sig_conf->length; n++){

    double time = sig_conf->start + (double)n/sig_conf->rate;

    /* change frequency for each new pulse */
    if (!pulse_flag){
      pulse_flag = check_pulse(time);
      if (pulse_flag){
	double low_frequency = m_pulse_conf->bandwidth * ((double)rand() / RAND_MAX);
	m_pulse_conf->frequency = low_frequency  + conf_frequency;
      }
    }
    else
      pulse_flag = check_pulse(time);

    complex double sample = get_harmonic(time);
    added_energy += pow(cabs(sample), 2.0);
    a_sig->m_waveform[n] += sample;
  }

  /* original frequency is restored */
  m_pulse_conf->frequency = conf_frequency;

  delete sig_conf;
  return added_energy;
}

/* **************************************************************************** */
