
#include "tfr-raster-image.hh"

/* ******************************************************************************** */

tfr_raster_image :: tfr_raster_image(tfr_image_config* a_conf){

  /* constructor */

  /* check args */
  if(a_conf==NULL) call_error("conf is null");
  if(a_conf->width<1) call_error("width is smaller than 1");
  if(a_conf->height<1) call_error("height is smaller than 1");

  if(a_conf->min_time >= a_conf->max_time)
    call_error("time min is greater than max");
  if(a_conf->min_frequency>=a_conf->max_frequency)
    call_error("frequency min is greater than max");

  /* copy config data */
  m_image_conf = new tfr_image_config;
  memcpy(m_image_conf, a_conf, sizeof(tfr_image_config));
  
  /* allocate memory for data */
  m_data = new double*[m_image_conf->width];
  int byte_size = m_image_conf->height * sizeof(double);
  for(int n=0; n<m_image_conf->width; n++){
    m_data[n] = new double[m_image_conf->height];    
    memset(m_data[n], 0x0, byte_size);
  }
}

tfr_raster_image :: ~tfr_raster_image(){  

  /* deconstructor */

  delete m_image_conf;
  
  for(int n=0;n<m_image_conf->width;n++)
    delete [] m_data[n];  
  delete [] m_data;
}


/* ******************************************************************************** */

void tfr_raster_image :: clear(){

  /* short of clear_data function */

  clear_data();
}

void tfr_raster_image :: clear_data(){

  /* clear data */
  
  int byte_size = m_image_conf->height * sizeof(double);
  for(int n=0; n<m_image_conf->width; n++)
    memset(m_data[n], 0x0, byte_size);
}


/* ******************************************************************************** */

tfr_image_config* tfr_raster_image :: get_config(){

  /* short */

  return get_image_configuration();
}

tfr_image_config* tfr_raster_image :: get_image_configuration(){

  /* return image configuration */
  
  tfr_image_config* conf = new tfr_image_config;
  memcpy(conf, m_image_conf, sizeof(tfr_image_config));
  
  return conf;
}

/* ******************************************************************************** */

bool tfr_raster_image :: compare_configuration(tfr_image_config* a_conf){

  /* It returns true if configurations are identical */
  
  if(m_image_conf->width!=a_conf->width) return false;
  if(m_image_conf->height!=a_conf->height) return false;
  
  if(m_image_conf->min_time!=a_conf->min_time) return false;
  if(m_image_conf->max_time!=a_conf->max_time) return false;
  
  if(m_image_conf->min_frequency!=a_conf->min_frequency) return false;  
  if(m_image_conf->max_frequency!=a_conf->max_frequency) return false;

  return true;
}

bool tfr_raster_image :: compare_time_configuration(tfr_image_config* a_conf){

  /* It returns true if time configurations are identical */
  
  if(m_image_conf->width!=a_conf->width) return false;
  if(m_image_conf->min_time!=a_conf->min_time) return false;  
  if(m_image_conf->max_time!=a_conf->max_time) return false;

  return true;
}

/* ******************************************************************************** */

/* get time-frequency values */

double tfr_raster_image :: get_time_by_index(int a_index){
  
  double hop = (double)(m_image_conf->max_time - m_image_conf->min_time) / (m_image_conf->width - 1);  
  return m_image_conf->min_time + a_index * hop;
}

double tfr_raster_image :: get_frequency_by_index(int a_index){

  double hop = (double)(m_image_conf->max_frequency - m_image_conf->min_frequency) / (m_image_conf->height - 1);
  return m_image_conf->min_frequency + a_index * hop;
}

/* ******************************************************************************** */

/* get the nearest indexes */

int tfr_raster_image :: get_index_by_time(double a_time){

  double hop = (double)(m_image_conf->max_time - m_image_conf->min_time) / (m_image_conf->width - 1);
  double tmp = (a_time - m_image_conf->min_time);
  return round(tmp/hop);
}

int tfr_raster_image :: get_index_by_frequency(double a_freq){

  double hop = (double)(m_image_conf->max_frequency - m_image_conf->min_frequency) / (m_image_conf->height - 1);
  double tmp = (a_freq - m_image_conf->min_frequency);
  return round(tmp/hop);
}

/* ******************************************************************************** */

tfr_real_array* tfr_raster_image :: get_horizontal_distribution(tfr_raster_image* a_energy){

  /* horizontal marginal distribution */

  /* config returned distribution */
  tfr_array_config conf;
  conf.length = m_image_conf->width;
  conf.min = m_image_conf->min_time;
  conf.max = m_image_conf->max_time;

  /* allocate returned distribution */
  tfr_real_array *array = new tfr_real_array(&conf);

  /* calc distribution if energy is not given */
  if(a_energy == NULL){
    for(int n=0; n<m_image_conf->width; n++){
      for(int k=0; k<m_image_conf->height; k++)
	array->m_data[n] += m_data[n][k];
      
      progress_bar(n+1, m_image_conf->width, "marg");
    }
  }

  /* calc distribution if energy is given */
  else{
    
    tfr_image_config *energy_conf = a_energy->get_config();
    if (!compare_configuration(energy_conf))
      call_error("images are not compact");
    delete energy_conf;
    
    for(int n=0; n<m_image_conf->width; n++){
      for(int k=0; k<m_image_conf->height; k++)
	array->m_data[n] += m_data[n][k] * a_energy->m_data[n][k];
      
      progress_bar(n+1, m_image_conf->width, "marg");
    }

    /* div by energy */
    tfr_real_array* energy = a_energy->get_horizontal_distribution();
    for(int n=0; n<m_image_conf->width; n++)
      array->m_data[n] /= energy->m_data[n];
    delete energy;
  }
  
  progress_bar(0, 0, "marg");
  return array;
}

tfr_real_array* tfr_raster_image :: get_vertical_distribution(tfr_raster_image* a_energy){

  /* vertical marginal distribution */

  /* config returned distribution */
  tfr_array_config conf;
  conf.length = m_image_conf->height;
  conf.min = m_image_conf->min_frequency;
  conf.max = m_image_conf->max_frequency;

  /* allocate returned distribution */
  tfr_real_array *array = new tfr_real_array(&conf);

  /* calc distribution if energy is not given */
  if(a_energy == NULL){
    for(int k=0; k<m_image_conf->height; k++){
      for(int n=0; n<m_image_conf->width; n++)
	array->m_data[k] += m_data[n][k];
      
      progress_bar(k+1, m_image_conf->height, "marg");
    }
  }

  /* calc distribution if energy is given */
  else{
    
    tfr_image_config *energy_conf = a_energy->get_config();
    if (!compare_configuration(energy_conf))
      call_error("images are not compact");
    delete energy_conf;
    
    for(int k=0; k<m_image_conf->height; k++){
      for(int n=0; n<m_image_conf->width; n++)
	array->m_data[k] += m_data[n][k] * a_energy->m_data[n][k];
      
      progress_bar(k+1, m_image_conf->height, "marg");
    }

    /* div by energy */
    tfr_real_array* energy = a_energy->get_vertical_distribution();
    for(int k=0; k<m_image_conf->height; k++)
      array->m_data[k] /= energy->m_data[k];
    delete energy;
  }
  
  progress_bar(0, 0, "marg");
  return array;
}

/* ******************************************************************************** */

void tfr_raster_image :: save(const char * a_fname){

  /* short of save_to_file function */
  
  save_to_file(a_fname);
}

void tfr_raster_image :: save_to_file(const char * a_fname){

  /* Save data to text file! 
     The results can be simply drawn 
     using Gnuplot. */
  
  /* open file to write */
  FILE *fds = fopen(a_fname, "w");
  if (fds==NULL)
    call_error("cannot save");

  /* write start and stop */
  fprintf(fds, "#TIME_MIN=%e\n", m_image_conf->min_time);
  fprintf(fds, "#TIME_MAX=%e\n", m_image_conf->max_time);
  fprintf(fds, "#FREQ_MIN=%e\n", m_image_conf->min_frequency);
  fprintf(fds, "#FREQ_MAX=%e\n", m_image_conf->max_frequency);

  /* init variables for finding min and max */
  double min_val = m_data[0][0];
  double max_val = m_data[0][0];

  double hop_time = (m_image_conf->max_time - m_image_conf->min_time) / (m_image_conf->width - 1);
  double hop_freq = (m_image_conf->max_frequency - m_image_conf->min_frequency) / (m_image_conf->height - 1);
  
  /* save data to file */
  for(int n=0; n<m_image_conf->width; n++){
    double time = m_image_conf->min_time + n * hop_time;
    for(int k=0; k<m_image_conf->height; k++){
      double freq = m_image_conf->min_frequency + k * hop_freq;

      fprintf(fds, "%e\t", time);
      fprintf(fds, "%e\t", freq);
      fprintf(fds, "%e\n", m_data[n][k]);

      /* finding min and max */
      if (min_val > m_data[n][k])
	min_val = m_data[n][k];
      if (max_val < m_data[n][k]) 
	max_val = m_data[n][k];
    }

    /* add empty line */
    fprintf(fds, "\n");
    progress_bar(n+1, m_image_conf->width, "save");
  }
  progress_bar(0, 0, "save");

  /* save min and max to file */
  fprintf(fds, "#VAL_MIN=%e\n", min_val);
  fprintf(fds, "#VAL_MAX=%e\n", max_val);
  fclose(fds);
  
  fprintf(stderr, "(info) save file: %s\n", a_fname);
}

/* ******************************************************************************** */
