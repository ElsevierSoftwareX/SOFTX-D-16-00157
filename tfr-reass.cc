
#include "tfr-reass.hh"

/* ********************************************************** */

tfr_raster_image* reassign_time_frequency(
					  tfr_raster_image* a_sdelay,
					  tfr_raster_image* a_ifreq,
					  tfr_raster_image* a_senergy,
					  tfr_raster_image* a_output){

  /* reassignment in time and in frequency */

  /* check args cohesion */
  tfr_image_config *conf = a_senergy->get_config();

  if (!a_sdelay->compare_configuration(conf)) call_error("images are not compact");
  if (!a_ifreq->compare_configuration(conf))  call_error("images are not compact");

  /* allocate output if necessary*/  
  tfr_raster_image* output = a_output;
  if(a_output==NULL)
    output = new tfr_raster_image(conf);
  
  tfr_image_config *out_conf = output->get_config();
  
  /* reassignment */
  for(int n=0; n<conf->width; n++){
    for(int k=0; k<conf->height; k++){
      
      double i_freq = a_ifreq->m_data[n][k];
      double c_time = a_senergy->get_time_by_index(n);
      c_time += a_sdelay->m_data[n][k];

      int t_index = output->get_index_by_time(c_time);
      if (t_index>=out_conf->width) continue;
      if (t_index<0) continue;

      int f_index = output->get_index_by_frequency(i_freq);
      if (f_index>=out_conf->height) continue;
      if (f_index<0) continue;

      output->m_data[t_index][f_index] += a_senergy->m_data[n][k];
    }

    progress_bar(n+1, conf->width, "reass");
  }
  
  delete conf;
  delete out_conf;
  progress_bar(0, 0, "reass");    
  return output;
}

/* ********************************************************** */

tfr_raster_image* reassign_frequency(
				     tfr_raster_image* a_sdelay,
				     tfr_raster_image* a_crate,
				     tfr_raster_image* a_ifreq,
				     tfr_raster_image* a_senergy,
				     tfr_raster_image* a_output){

  /* reassignment only in frequency */

  /* check args cohesion */
  tfr_image_config *conf = a_senergy->get_config();

  if (!a_ifreq->compare_configuration(conf)) call_error("images are not compact");
  if (!a_crate->compare_configuration(conf)) call_error("images are not compact");
  if (!a_sdelay->compare_configuration(conf)) call_error("images are not compact");

  /* allocate output if necessary*/  
  tfr_raster_image* output = a_output;
  if(a_output==NULL)
    output = new tfr_raster_image(conf);  
  else
    if (!a_output->compare_time_configuration(conf))
      call_error("images are not compact in time");

  tfr_image_config *out_conf = output->get_config();
  
  /* reassignment */
  for(int n=0; n<conf->width; n++){
    for(int k=0; k<conf->height; k++){
      
      double i_freq = a_ifreq->m_data[n][k] - a_sdelay->m_data[n][k] * a_crate->m_data[n][k];
      int f_index = output->get_index_by_frequency(i_freq);
      if (f_index>=out_conf->height) continue;
      if (f_index<0) continue;

      output->m_data[n][f_index] += a_senergy->m_data[n][k];
    }

    progress_bar(n+1, conf->width, "reass");
  }

  delete conf;
  delete out_conf;
  progress_bar(0, 0, "reass");    
  return output;
}


/* ********************************************************** */

tfr_raster_image* reassign_frequency(
				     tfr_raster_image* a_ifreq,
				     tfr_raster_image* a_senergy,
				     tfr_raster_image* a_output){

  /* reassignment only in frequency */

  /* check args cohesion */
  tfr_image_config *conf = a_senergy->get_config();
  if (!a_ifreq->compare_configuration(conf))
    call_error("images are not compact");

  /* allocate output if necessary*/  
  tfr_raster_image* output = a_output;
  if(a_output==NULL)
    output = new tfr_raster_image(conf);
  else
    if (!a_output->compare_time_configuration(conf))
      call_error("images are not compact in time");
  
  tfr_image_config *out_conf = output->get_config();

  /* reassignment */
  for(int n=0; n<conf->width; n++){
    for(int k=0; k<conf->height; k++){
      
      double i_freq = a_ifreq->m_data[n][k];
      int f_index = output->get_index_by_frequency(i_freq);
      if (f_index>=out_conf->height) continue;
      if (f_index<0) continue;

      output->m_data[n][f_index] += a_senergy->m_data[n][k];
    }

    progress_bar(n+1, conf->width, "reass");
  }

  delete conf;
  delete out_conf;
  progress_bar(0, 0, "reass");    
  return output;
}

/* ********************************************************** */

tfr_real_array* calculate_profile(
				  tfr_raster_image* a_values,
				  tfr_raster_image* a_energy,
				  tfr_array_config* a_arr_conf){

  /* calc histogram like profile
   and save it as a signal */

  tfr_image_config* conf = a_energy->get_config();
  if(!a_values->compare_configuration(conf))
    call_error("images are not compact");

  /* args */
  if(a_arr_conf->length<0)
    call_error("length < 0");

  if(a_arr_conf->max<=a_arr_conf->min)
    call_error("max <= min");
  double delta = (a_arr_conf->max-a_arr_conf->min) / (a_arr_conf->length-1);
  
  /* new empty array */
  tfr_real_array* arr_ptr = new tfr_real_array(a_arr_conf);
  
  //tfr_signal_config* sig_conf = new tfr_signal_config;
  //sig_conf->length = a_arr_conf->length; 
  //sig_conf->start = a_arr_conf->min; 
  //sig_conf->rate = 1.0 / delta; 

  // tfr_complex_signal* signal_ptr = new tfr_complex_signal(sig_conf);
  // delete sig_conf;
  
  /* fill profile */
  for(int n=0; n<conf->width; n++)
    for(int k=0; k<conf->height; k++){
      
      int index = round((a_values->m_data[n][k] - a_arr_conf->min) / delta);
      if (index>=a_arr_conf->length) continue;
      if (index<0) continue;

      arr_ptr->m_data[index] += a_energy->m_data[n][k];
    }

  /* return profile as tfr_real_array */
  return arr_ptr;
}

/* ********************************************************** */
