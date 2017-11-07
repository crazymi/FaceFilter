/*
 * face.h
 *
 *  Created on: Nov 7, 2017
 *      Author: tazo
 */

#ifndef FACE_H_
#define FACE_H_

#include <mv_face.h>
#include <image_util.h>
#include <camera.h>

struct _facedata_s {
	mv_source_h g_source;
	mv_engine_config_h g_engine_config;
};
typedef struct _facedata_s facedata_s;
static facedata_s facedata;

int init_facedata();
int wrap_mv_source_fill(camera_preview_data_s *frame, void *user_data);
bool _mv_supported_attribute_cb(mv_config_attribute_type_e attribute_type,
		const char *attribute_name, void *user_data);

#endif /* FACE_H_ */
