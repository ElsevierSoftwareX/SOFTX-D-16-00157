
#include "tfr-bartlett-pergram.hh"

/* **************************************************************************** */

tfr_bartlett_pergram :: tfr_bartlett_pergram(tfr_array_config* a_array_conf){

  /* constructor */

  if(a_array_conf==NULL)
    call_error("arg is null");

  if(a_array_conf->length<2)
    call_error("length < 2");

  if(a_array_conf->max<=a_array_conf->min)
    call_error("start > stop");

  m_array_conf = new tfr_array_config;
  memcpy(m_array_conf, a_array_conf, sizeof(tfr_array_config));

  m_periodogram = new double[m_array_conf->length];
  int byte_size = m_array_conf->length * sizeof(double);
  memset(m_periodogram, 0x0, byte_size);

  m_added_frames = 0;
  m_rate = 0.0;
}

tfr_bartlett_pergram :: ~tfr_bartlett_pergram(){
  
  /* deconstructor */

  delete [] m_periodogram;
  delete m_array_conf;
}

/* **************************************************************************** */

tfr_array_config* tfr_bartlett_pergram :: get_array_configuration(){
  
  /* return array parameters (frequency axis) as structure */
  
  tfr_array_config* conf = new tfr_array_config;
  memcpy(conf, m_array_conf, sizeof(tfr_array_config));
  
  return conf;
}

tfr_array_config* tfr_bartlett_pergram :: get_config(){

  /* short */
  
  return get_array_configuration();
}

/* **************************************************************************** */

double tfr_bartlett_pergram :: get_sampling_rate(){

  return m_rate;
}

double tfr_bartlett_pergram :: get_rate(){

  /* short */
  
  return get_sampling_rate();
}

/* **************************************************************************** */

double tfr_bartlett_pergram :: calc_energy(){
  
  /* return the whole energy of periodogram */

  double sum = 0.0;  
  for(int n=0; n<m_array_conf->length; n++)
    sum += m_periodogram[n];

  return sum;
}

/* **************************************************************************** */

double tfr_bartlett_pergram :: add_signal(tfr_complex_signal* a_signal, int a_hop){
  
  /* short */
  
  return add_complex_signal(a_signal, a_hop);
}

double tfr_bartlett_pergram :: add_complex_signal(tfr_complex_signal* a_signal, int a_hop){

  if(a_signal==NULL)
    call_error("arg is null");

  if(a_hop<=0)
    call_error("hop is smaller than one");

  tfr_signal_config* arg_conf = a_signal->get_config();
  if(m_added_frames==0){
    m_rate = arg_conf->rate;

    if(m_array_conf->min<-m_rate/2){
      call_warning("min is updated");
      m_array_conf->min=-m_rate/2;
    }
    
    if(m_array_conf->max>m_rate/2){
      call_warning("max is updated");
      m_array_conf->max=m_rate/2;
    }
  }
  else
    if(m_rate != arg_conf->rate)
      call_error("rates are different");

  double bandwidth = m_array_conf->max - m_array_conf->min;
  int spectra_length =  m_array_conf->length * m_rate / bandwidth; 
  int frame_number = 1 + floor((double)(arg_conf->length - spectra_length) / a_hop);
  int start_index = spectra_length * (m_array_conf->min+m_rate/2) / m_rate;
  
  if(frame_number<=0 or arg_conf->length < spectra_length){
    call_warning("signal is too short");
    return 0.0; 
  }

  tfr_signal_config conf;
  conf.start = arg_conf->start;
  conf.length = spectra_length;
  conf.rate = m_rate;

  tfr_complex_signal *sig = new tfr_complex_signal(&conf);
  
  double added_energy = 0.0;
  for(int n=0; n<frame_number; n++){

    int copy_index = n*a_hop;
    sig->copy(a_signal, copy_index);
    tfr_fourier_spectrum* fspec = sig->get_spectrum();

    for(int k=0; k<m_array_conf->length; k++){
      
      int i = start_index + k;
      double energy = pow(creal(fspec->m_spectrum[i]), 2.0);
      energy += pow(cimag(fspec->m_spectrum[i]), 2.0);
      m_periodogram[k] += energy;
      added_energy += energy;
    }

    delete fspec;
  }

  delete sig;
  return added_energy;
}

/* **************************************************************************** */

void tfr_bartlett_pergram :: save(const char * a_fname){

  /* short */

  save_to_file(a_fname);
}

void tfr_bartlett_pergram :: save_to_file(const char * a_fname){

  /* open file to write */
  FILE *fds = fopen(a_fname, "w");
  if (fds==NULL)
    call_error("cannot save");
  
  /* save frequency axis conf */
  fprintf(fds, "#LENGTH=%e\n", m_array_conf->length);
  fprintf(fds, "#START=%e\n", m_array_conf->min);
  fprintf(fds, "#STOP=%e\n", m_array_conf->max);

  /* save periodogram */
  double delta = (m_array_conf->max-m_array_conf->min) / m_array_conf->length;
  for(int n=0; n<m_array_conf->length; n++){
    
    double freq = m_array_conf->min + n*delta;
    fprintf(fds, "%e\t%e\n", freq, m_periodogram[n]);
  }

  fclose(fds);  
  fprintf(stderr, "(info) save file: %s\n", a_fname);
}

/* **************************************************************************** */
