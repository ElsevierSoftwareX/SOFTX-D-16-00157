#ifndef _tfr_reass_
#define _tfr_reass_

/* headers and declarations */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-real-array.hh"
struct tfr_array_config;

#include "tfr-raster-image.hh"
struct tfr_image_config;
class tfr_raster_image;

#include "tfr-complex-signal.hh"
struct tfr_signal_config;
class tfr_complex_signal;

/* function signatures */

tfr_raster_image* 
reassign_time_frequency
(tfr_raster_image*, tfr_raster_image*, tfr_raster_image*, tfr_raster_image* =NULL);

tfr_raster_image* 
reassign_frequency
(tfr_raster_image*, tfr_raster_image*, tfr_raster_image* =NULL);

tfr_raster_image* 
reassign_frequency
(tfr_raster_image*, tfr_raster_image*, tfr_raster_image*, tfr_raster_image*, tfr_raster_image* =NULL);

tfr_real_array*
calculate_profile
(tfr_raster_image*, tfr_raster_image*, tfr_array_config*);

#endif
