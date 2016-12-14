
#include "tfr-window-gener.hh"

/* **************************************************************************** */
/* window constructor and deconstructor */

tfr_window_generator :: tfr_window_generator(tfr_window_config* a_conf){

  /* This is constructor of analyzing window. 
    which cen generate window waveforms... */

  if(a_conf->length<=0)
    call_error("length <= 0");
  
  if(a_conf->rate<=0)
    call_error("rate <= 0");
  
  /* you can check parameters for specific window */
  switch(a_conf->code){

  case WINDOW_BLACKMAN_HARRIS:  
    if(a_conf->crate!=0.0)
      call_warning("chirplet window is used");
    break;

  default:
    call_error("unknown window type");
  }

  /* save arguments */
  m_window_conf = new tfr_window_config;
  memcpy(m_window_conf, a_conf, sizeof(tfr_window_config));
}

tfr_window_generator :: ~tfr_window_generator(){

  /* deconstructor */
  
  delete m_window_conf;
}

/* **************************************************************************** */

tfr_window_config* tfr_window_generator :: get_window_configuration(){

  /* return window parameters as structure */

  tfr_window_config* conf = new tfr_window_config;
  memcpy(conf, m_window_conf, sizeof(tfr_window_config));

  return conf;
}

tfr_window_config* tfr_window_generator :: get_config(){

  /* short */
  
  return get_window_configuration();
}

/* **************************************************************************** */

double tfr_window_generator :: get_window_sampling_rate(){
  
  /* window sampling rate */

  return m_window_conf->rate;
}

double tfr_window_generator :: get_rate(){
  
  /* short */

  return get_window_sampling_rate();
}

/* **************************************************************************** */

int tfr_window_generator :: get_window_length(){
  
  /* window length in samples */

  return m_window_conf->length;
}

int tfr_window_generator :: get_length(){
  
  /* short */

  return get_window_length();
}

/* **************************************************************************** */

double tfr_window_generator :: get_window_duration(){
  
  /* get absolute duration */

  /* len - 0.5 <- why ??? */
  if (m_window_conf->code==0) 
    return ((double)m_window_conf->length-0.5) / m_window_conf->rate;
  
  else
    call_error("have to be implemented");
}

double tfr_window_generator :: get_duration(){
  
  /* short */

  return get_window_duration();
}
/* **************************************************************************** */

double tfr_window_generator :: get_window_chirp_rate(){

  /* get window chirp rate */

  return m_window_conf->crate;
}    

double tfr_window_generator :: get_crate(){
  
  /* short */

  return get_window_chirp_rate();
}

/* **************************************************************************** */

void tfr_window_generator :: update_window_chirp_rate(double a_crate){

  /* update window chirp rate */

  m_window_conf->crate = a_crate;
}    

void tfr_window_generator :: update_crate(double a_crate){
  
  /* short */

  update_window_chirp_rate(a_crate);
}

/* **************************************************************************** */

tfr_complex_signal* tfr_window_generator :: get_empty_window(){
  
  /* returns configured window,
     however its waveform is not fullfill */

  tfr_signal_config* sig_conf = new tfr_signal_config;
  sig_conf->start = -0.5 * (double)m_window_conf->length / m_window_conf->rate; 
  sig_conf->length = m_window_conf->length; 
  sig_conf->rate = m_window_conf->rate; 
  
  tfr_complex_signal* win = new tfr_complex_signal(sig_conf);
  delete sig_conf;
  return win;
}

/* **************************************************************************** */

tfr_complex_signal* tfr_window_generator :: get_window(int a_d_order, int a_t_order){

  /* short */

  return get_window_waveform(a_d_order, a_t_order);
}

tfr_complex_signal* tfr_window_generator :: get_window_waveform(int a_d_order, int a_t_order){

  if(a_t_order<0)
    call_error("time order < 0");

  if(a_d_order<0)
    call_error("derivative order < 0");

  tfr_complex_signal* win = NULL;
  switch(m_window_conf->code){
  case WINDOW_BLACKMAN_HARRIS:
    switch(a_d_order){
    case 0:
      win = get_blackman_harris_win();
      break;
    case 1:
      win = get_blackman_harris_dwin();
      break;
    case 2:
      win = get_blackman_harris_d2win();
      break;
    default:
      call_error("derivative for this order for this window is not defined");
    }
    break;
  default:
    call_error("unknown window type");
  }
  
  /* multiply by timeramp */
  if(a_t_order>0)
    for(int n=0;n<m_window_conf->length;n++){
    
      double ramp = (double)n/(m_window_conf->length-1) - 0.5;  
      ramp *= get_duration();

      win->m_waveform[n] *= pow(ramp, a_t_order) ; 
    }    

  return win;
}


/* **************************************************************************** */
/* blackman-harris window definition */

tfr_complex_signal* tfr_window_generator :: get_blackman_harris_win(){

  /* create window object for return */
  tfr_complex_signal* win = get_empty_window();
  
  /* fill waveform */
  for(int n=0;n<m_window_conf->length;n++)
    win->m_waveform[n] = calc_blackman_harris(n, m_window_conf->length, 0);
     
  /* for chirplet transform */
  if(m_window_conf->crate!=0.0){
    double half_len = (double)(m_window_conf->length-1)/2; 
    
    for(int n=0;n<m_window_conf->length;n++){
      double time = (double)(-half_len + n) / m_window_conf->rate;
      double arg = -M_PI*m_window_conf->crate * pow(time, 2.0); 
      complex double crr = cexp(1I*arg);
      win->m_waveform[n] *= crr;
    }
  }
  
  return win;
}

/* **************************************************************************** */

tfr_complex_signal* tfr_window_generator :: get_blackman_harris_dwin(){

  /* create output window object  */
  tfr_complex_signal* win = get_empty_window();
  
  /* fill waveform */
  for(int n=0;n<m_window_conf->length;n++){
    win->m_waveform[n] = calc_blackman_harris(n, m_window_conf->length, 1);
    win->m_waveform[n] /= get_duration();
  }    
  
  /* for chirplet transform */
  if(m_window_conf->crate!=0.0){

    double half_len = (double)(m_window_conf->length-1)/2; 
    for(int n=0;n<m_window_conf->length;n++){
      double ramp = get_duration() * ((double)n/(m_window_conf->length-1) - 0.5); 

      complex double tmp = calc_blackman_harris(n, m_window_conf->length, 0);      
      tmp *= TWO_PI * m_window_conf->crate; 
      tmp *= ramp;      
      tmp *= -1I;
      
      win->m_waveform[n] += tmp;

      double time = (double)(-half_len + n) / m_window_conf->rate;
      double arg = -M_PI * m_window_conf->crate * pow(time, 2.0); 
      complex double crr = cexp(1I*arg);      
      win->m_waveform[n] *= crr;
    }
  }

  return win;
}

/* **************************************************************************** */

tfr_complex_signal* tfr_window_generator :: get_blackman_harris_d2win(){

  /* create window object for return */
  tfr_complex_signal* win = get_empty_window();
    
  /* fill waveform */
  for(int n=0;n<m_window_conf->length;n++){
    win->m_waveform[n] = calc_blackman_harris(n, m_window_conf->length, 2);      
    win->m_waveform[n] /= pow(get_duration(), 2.0);
  }    
  
  if(m_window_conf->crate!=0.0){

    double half_len = (double)(m_window_conf->length-1)/2; 
    for(int n=0;n<m_window_conf->length;n++){       
      double ramp = get_duration() * ((double)n/(m_window_conf->length-1) - 0.5); 

      complex double tmp = calc_blackman_harris(n, m_window_conf->length, 1);      
      tmp *= TWO_PI * m_window_conf->crate;
      tmp *= ramp / get_duration();
      tmp *= -2I;
      
      win->m_waveform[n] += tmp;

      tmp = calc_blackman_harris(n, m_window_conf->length, 0);  
      tmp *= -1I  -  TWO_PI * m_window_conf->crate * pow(ramp, 2.0);
      tmp *=  m_window_conf->crate * TWO_PI;
      
      win->m_waveform[n] += tmp;

      double time = (double)(-half_len + n) / m_window_conf->rate;
      double arg = -M_PI * m_window_conf->crate * pow(time, 2.0); 
      complex double crr = cexp(1I*arg);      
      win->m_waveform[n] *= crr;
    }
  }

  return win;
}

/* **************************************************************************** */
