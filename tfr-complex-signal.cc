
#include "tfr-complex-signal.hh"

/* **************************************************************************** */
/* signal constructor and deconstructor */

tfr_complex_signal :: tfr_complex_signal(tfr_signal_config* a_conf){

  /* This is constructor of signal object. 
     New object contains zeros as samples 
     in m_waveform. */

  if(a_conf == NULL)
    call_error("arg is null");
  
  /* check args */
  if (a_conf->length <= 0)
    call_error("length <= 0");
  if (a_conf->rate<0)
    call_error("rate < 0");

  /* save arguments */
  m_signal_conf = new tfr_signal_config;
  memcpy(m_signal_conf, a_conf, sizeof(tfr_signal_config));
  
  /* allocate memory for samples */
  m_waveform = new complex double [m_signal_conf->length];
  int byte_size = m_signal_conf->length * sizeof(complex double);
  memset(m_waveform, 0x0, byte_size);
}

tfr_complex_signal :: tfr_complex_signal(char* a_wav_filename, int a_channel_number){

  /* This is constructor of signal object. 
     New object is loaded from a wav file. */

  /* only for awgn generation */  
  srand(time(NULL));

  /* check file extention */
  int len = strlen(a_wav_filename);
  if (len<5)
    call_error("not see as wav file");
  if (strcmp(&a_wav_filename[len-4], ".wav") and
      strcmp(&a_wav_filename[len-4], ".WAV"))
    call_error("not see as wav file");
    
  /* open snd file */
  SF_INFO info;
  SNDFILE *sndfile = sf_open(a_wav_filename, SFM_READ, &info);
  if (sndfile == NULL)
    call_error((char *)sf_strerror(sndfile));

  /* check channels number */
  if(a_channel_number>=info.channels or a_channel_number<0)
    call_error("wrong channel number");
    
  /* load samples */
  double *buffer = new double[info.channels*info.frames];
  int loaded = sf_readf_double(sndfile, buffer, info.frames);
  if (loaded!=info.frames)
    call_warning("cannot read all samples");

  /* configure signal object */
  m_signal_conf = new tfr_signal_config;
  m_signal_conf->start = 0.0;
  m_signal_conf->length = info.frames;
  m_signal_conf->rate = (double)info.samplerate;
  m_waveform = new complex double [m_signal_conf->length];
  int byte_size = m_signal_conf->length * sizeof(complex double);
  memset(m_waveform, 0x0, byte_size);
  for (int n=0; n<loaded; n++)
    m_waveform[n] = buffer[n*info.channels+a_channel_number];
  
  /* ending */
  delete [] buffer;
  sf_close(sndfile);
}

tfr_complex_signal :: ~tfr_complex_signal(){

  /* signal deconstructor */
  /* free memory for samples */

  delete m_signal_conf;
  delete [] m_waveform;
}

/* **************************************************************************** */

tfr_signal_config* tfr_complex_signal :: get_signal_configuration(){

  /* return signal parameters as structure */

  tfr_signal_config* conf = new tfr_signal_config;
  memcpy(conf, m_signal_conf, sizeof(tfr_signal_config));
    
  return conf;
}

tfr_signal_config* tfr_complex_signal :: get_config(){

  /* short name of get_signal_configuration() */
  
  return get_signal_configuration();
}

/* **************************************************************************** */

bool tfr_complex_signal :: check_real(){

  for(int n=0; n<m_signal_conf->length; n++)   
    if(creal(m_waveform[n])!=0.0)
      return true;
  
  return false;
}

bool tfr_complex_signal :: check_imag(){

  for(int n=0; n<m_signal_conf->length; n++)   
    if(cimag(m_waveform[n])!=0.0)
      return true;
  
  return false;
}

/* **************************************************************************** */
/* simple manimulation of wavfeform */

void tfr_complex_signal :: clear(){
  clear_waveform();
}

void tfr_complex_signal :: clear_waveform(){

  /* set all samples as zeros */
  
  int byte_size = m_signal_conf->length * sizeof(complex double);
  memset(m_waveform, 0x0, byte_size);
}

/* ********************** */

int tfr_complex_signal :: copy(tfr_complex_signal* a_sig, int a_index){

  /* short */
  
  return copy_waveform(a_sig, a_index);
}

int tfr_complex_signal :: copy_waveform(tfr_complex_signal* a_sig, int a_index){

  /* copy samples from other signal object */
  
  tfr_signal_config* conf = a_sig->get_config();
  
  if(a_index<0 or a_index>=conf->length)
    call_error("index has wrong value");

  int min_length = m_signal_conf->length;
  if (m_signal_conf->length>conf->length-a_index)
    min_length = conf->length-a_index;
  
  int byte_size = min_length * sizeof(complex double);
  memcpy(m_waveform, &a_sig->m_waveform[a_index], byte_size);

  delete conf;
  return min_length;
}
 
/* ********************** */

void tfr_complex_signal :: modulate(double a_frequency_shift){

  /* short */
  
  modulate_waveform(a_frequency_shift);
}

void tfr_complex_signal :: modulate_waveform(double a_frequency_shift){

  /* simple modulation */
  
  for(int n=0; n<m_signal_conf->length; n++){
    
    double time = (double)n/m_signal_conf->rate;
    double arg = TWO_PI * a_frequency_shift * time;
    m_waveform[n] *= cexp(1I*arg);
  }
}

/* ********************** */

int tfr_complex_signal :: cut(double a_new_start, double a_new_stop){
  return cut_waveform(a_new_start, a_new_stop);
}

int tfr_complex_signal :: cut_waveform(double a_new_start, double a_new_stop){
    
  /* waveform shortening */

  /*checking arguments */
  if (a_new_start>=a_new_stop)
    call_error("start should be smalles than stop");
  if (a_new_start<m_signal_conf->start)
    call_warning("given start is smaller than current start");
  double max_time = m_signal_conf->start+(double)(m_signal_conf->length-1)/m_signal_conf->rate;
  if (a_new_stop>max_time)
    call_warning("given stop is larger than current stop");

  /* new shorter waveform allocation */  
  int new_length = (a_new_stop-a_new_start) * m_signal_conf->rate;
  if (new_length<3) call_error("new length is too short ");
  complex double* new_waveform = new complex double [new_length];
    
  /* copying of samples */
  int new_initial = (a_new_start - m_signal_conf->start) * m_signal_conf->rate;
  memcpy(new_waveform, &m_waveform[new_initial], sizeof(complex double) * new_length);
  delete m_waveform;

  /* actualization of configuration */
  m_signal_conf->start = (double)new_initial/m_signal_conf->rate;
  m_signal_conf->length = new_length;
  m_waveform = new_waveform;
}

/* ********************** */

double tfr_complex_signal :: calc_energy(){

  /* calculation of signal energy */
  
  double energy = 0.0;
  for(int n=0; n<m_signal_conf->length; n++){

    double re = creal(m_waveform[n]);
    double im = cimag(m_waveform[n]);
    energy += pow(re, 2.0) + pow(im, 2.0);
  }

  return energy;
}

/* ********************** */

/* auxiliary method 
 for Fourier spectrum */

tfr_fourier_spectrum* tfr_complex_signal :: get_spectrum(){

  /* short */
  
  return get_fourier_spectrum();
}

tfr_fourier_spectrum* tfr_complex_signal :: get_fourier_spectrum(){

  return new tfr_fourier_spectrum(this);
}

/* **************************************************************************** */

void tfr_complex_signal :: save(const char * a_fname){

  /* short */
  
  save_to_file(a_fname);
}

void tfr_complex_signal :: save_to_file(const char * a_fname){

  /* save signal to a file */
  
  /* open file to write */
  FILE *fds = fopen(a_fname, "w");
  if (fds==NULL)
    call_error("cannot save");

  /* write start and stop */
  double stop = m_signal_conf->start + (double)(m_signal_conf->length-1) / m_signal_conf->rate;
  fprintf(fds, "#LENGTH=%e\n", m_signal_conf->length);
  fprintf(fds, "#START=%e\n", m_signal_conf->start);
  fprintf(fds, "#STOP=%e\n", stop);

  for(int n=0; n<m_signal_conf->length; n++){
    
    double time = m_signal_conf->start + (double) n/m_signal_conf->rate;
    fprintf(fds, "%e\t%e\t%e\n", time, creal(m_waveform[n]), cimag(m_waveform[n]));
  }
  
  fclose(fds);  
  fprintf(stderr, "(info) save file: %s\n", a_fname);
}

/* **************************************************************************** */

void tfr_complex_signal :: save_wav(const char * a_fname){

  /* short */
  
  save_to_wavfile(a_fname);
}

void tfr_complex_signal :: save_to_wavfile(const char * a_wav_fname){

  /* check file extention */
  int len = strlen(a_wav_fname);
  if (len<5)
    call_error("not see as wav file");
  if (strcmp(&a_wav_fname[len-4], ".wav") and
      strcmp(&a_wav_fname[len-4], ".WAV"))
    call_error("not see as wav file");

  /* set configuration */
  SF_INFO info;
  info.channels = 1;
  info.samplerate = m_signal_conf->rate;
  info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  
  if (check_imag())
    info.channels = 2;

  /* open snd file */
  SNDFILE *sndfile = sf_open(a_wav_fname, SFM_WRITE, &info);
  if (sndfile == NULL)
    call_error((char *)sf_strerror(sndfile));

  /* save samples */
  double *buffer = new double[info.channels*m_signal_conf->length];
  if(info.channels==2){
    for(int n=0; n<m_signal_conf->length; n++){   
      buffer[2*n+1] = cimag(m_waveform[n]);
      buffer[2*n] = creal(m_waveform[n]);
    }
  }
  else
    for(int n=0; n<m_signal_conf->length; n++)
      buffer[n] = creal(m_waveform[n]);

  int saved = sf_writef_double (sndfile, buffer, m_signal_conf->length) ;
  if (saved!=m_signal_conf->length)
    call_warning("cannot write all samples");

  /* ending */
  sf_write_sync(sndfile);
  delete [] buffer;  
  sf_close(sndfile);
  fprintf(stderr, "(info) save file: %s\n", a_wav_fname);
}

/* **************************************************************************** */

bool tfr_complex_signal :: compare_configuration(tfr_signal_config* a_conf){

  if (a_conf->rate != m_signal_conf->rate)
    return false;
  if (a_conf->start != m_signal_conf->start)
    return false;
  if (a_conf->length != m_signal_conf->length)
    return false;

  return true;
}

/* **************************************************************************** */

void tfr_complex_signal :: operator += (tfr_complex_signal* a_sig){

  /* is used e.g. in generator */
  
  tfr_signal_config* conf = a_sig->get_config();  
  if (!compare_configuration(conf))
    call_error("signals are not compact");
  delete conf;
  
  for(int n=0; n<m_signal_conf->length; n++){
    
    m_waveform[n] += a_sig->m_waveform[n];
  }  
}

void tfr_complex_signal :: operator -= (tfr_complex_signal* a_sig){
  
  tfr_signal_config* conf = a_sig->get_config();  
  if (!compare_configuration(conf))
    call_error("signals are not compact");
  delete conf;
  
  for(int n=0; n<m_signal_conf->length; n++){
    
    m_waveform[n] -= a_sig->m_waveform[n];
  }  
}

/* **************************************************************************** */
