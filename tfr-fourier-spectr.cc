
#include "tfr-fourier-spectr.hh"

/* **************************************************************************** */
/* spectrum constructor and deconstructor */

tfr_fourier_spectrum :: tfr_fourier_spectrum(tfr_complex_signal* a_signal){

  /* constructor for rectangular window */
  
  m_signal_conf = a_signal->get_config();

  m_spectrum = new complex double [m_signal_conf->length];
  int byte_size = m_signal_conf->length * sizeof(complex double);
  memset(m_spectrum, 0x0, byte_size);
  
  fftw_plan pl = fftw_plan_dft_1d(m_signal_conf->length, a_signal->m_waveform, m_spectrum, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(pl);  
  fft_shift();
}

tfr_fourier_spectrum :: ~tfr_fourier_spectrum(){

  /* deconstructor */

  delete m_signal_conf;
  delete [] m_spectrum;
}

/* **************************************************************************** */

void tfr_fourier_spectrum :: fft_shift(){

  /* fftshift */

  complex double* tmp_buffer = new complex double[1+m_signal_conf->length/2];
  int byte_size = (m_signal_conf->length/2) * sizeof(complex double);
  memset(tmp_buffer, 0x0, byte_size + sizeof(complex double));
  
  if(m_signal_conf->length%2==0){					
    memcpy(tmp_buffer, m_spectrum, byte_size);
    memcpy(m_spectrum, &m_spectrum[m_signal_conf->length/2], byte_size);
    memcpy(&m_spectrum[m_signal_conf->length/2], tmp_buffer, byte_size);
  }
  else{
    memcpy(tmp_buffer, m_spectrum, byte_size+sizeof(complex double));
    memcpy(m_spectrum, &m_spectrum[1+m_signal_conf->length/2], byte_size);
    memcpy(&m_spectrum[m_signal_conf->length/2], tmp_buffer, byte_size+sizeof(complex double));
  }

  delete [] tmp_buffer;
}

/* **************************************************************************** */

tfr_complex_signal* tfr_fourier_spectrum :: get_signal(){
  
  /* short */
  
  return get_complex_signal();
}

tfr_complex_signal* tfr_fourier_spectrum :: get_complex_signal(){

  /* signal recovery - inverse transformation */
  
  tfr_complex_signal* signal = new tfr_complex_signal(m_signal_conf);
  complex double* tmp_buffer = new complex double [m_signal_conf->length];
  int byte_size = sizeof(complex double) * (m_signal_conf->length / 2);

  /* inverse fftshift */
  if(m_signal_conf->length%2==0){
    memcpy(tmp_buffer, &m_spectrum[m_signal_conf->length/2], byte_size);
    memcpy(&tmp_buffer[m_signal_conf->length/2], m_spectrum, byte_size);
  }
  else{
    memcpy(tmp_buffer, &m_spectrum[m_signal_conf->length/2], byte_size+sizeof(complex double));
    memcpy(&tmp_buffer[m_signal_conf->length/2+1], m_spectrum, byte_size);
  }

  /* inverse transform */
  fftw_plan pl = fftw_plan_dft_1d(m_signal_conf->length, tmp_buffer, signal->m_waveform, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(pl);  
  fftw_destroy_plan(pl);

  for (int n=0; n<m_signal_conf->length; n++)  
    signal->m_waveform[n] /= m_signal_conf->length;

  delete [] tmp_buffer;
  return signal;
}

/* **************************************************************************** */

tfr_signal_config* tfr_fourier_spectrum :: get_signal_configuration(){

  /* return signal parameters as structure */

  tfr_signal_config* conf = new tfr_signal_config;
  memcpy(conf, m_signal_conf, sizeof(tfr_signal_config));
  
  return conf;
}

tfr_signal_config* tfr_fourier_spectrum :: get_config(){

  /* short name of get_signal_configuration() */
  
  return get_signal_configuration();
}

/* **************************************************************************** */

double tfr_fourier_spectrum :: calc_energy(){

  /* calculation of signal energy */
  
  double energy = 0.0;
  for(int n=0; n<m_signal_conf->length; n++){

    double re = creal(m_spectrum[n]);
    double im = cimag(m_spectrum[n]);
    energy += pow(re, 2.0) + pow(im, 2.0);
  }

  return energy;
}

/* **************************************************************************** */

void tfr_fourier_spectrum :: save(const char* a_fname){

  /* short */
  
  save_to_file(a_fname);
}

void tfr_fourier_spectrum :: save_to_file(const char*  a_fname){

  /* fourier spectrum to a file 
     in polar form */
  
  /* open file to write */
  FILE *fds = fopen(a_fname, "w");
  if (fds==NULL)
    call_error("cannot save");

  /* write start and stop */
  fprintf(fds, "#LENGTH=%e\n", m_signal_conf->length);
  fprintf(fds, "#START=%e\n", -m_signal_conf->rate/2);
  fprintf(fds, "#STOP=%e\n", m_signal_conf->rate/2);

  for(int n=0; n<m_signal_conf->length; n++){
    
    double freq = -m_signal_conf->rate/2 + (double) n*m_signal_conf->rate/m_signal_conf->length;
    double avalue = cabs(m_spectrum[n]);
    double phase = carg(m_spectrum[n]);
    fprintf(fds, "%e\t%e\t%e\n", freq, avalue, phase);
  }
  
  fclose(fds);  
  fprintf(stderr, "(info) save file: %s\n", a_fname);  
}

/* **************************************************************************** */
