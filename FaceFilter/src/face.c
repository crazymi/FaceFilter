/*
 * face.c
 *
 *  Created on: Nov 7, 2017
 *      Author: tazo
 */

#include <face.h>
#include <main.h>

int init_facedata(){
	int err_code = 0;
	err_code = mv_create_source(&facedata.g_source);
	if (err_code != MEDIA_VISION_ERROR_NONE) // error occured
		return err_code;
	return err_code;
}

int wrap_mv_source_fill(camera_preview_data_s *frame, void *user_data){
	int err_code;

	// fill preview image into mv_source in facedata structure
	err_code = mv_source_fill_by_buffer(&facedata.g_source, frame->data.double_plane.y
			, frame->data.double_plane.y_size + frame->data.double_plane.uv_size
			// assume preview format as NV12
			, (unsigned int) frame->width, (unsigned int) frame->height, MEDIA_VISION_COLORSPACE_NV12);
	if (err_code != MEDIA_VISION_ERROR_NONE)
		return err_code; // error occured

	// create new mv_engine_config in facedata structure
	err_code = mv_create_engine_config(&facedata.g_engine_config);
	if (err_code != MEDIA_VISION_ERROR_NONE)
		return err_code;

	err_code = mv_engine_config_set_string_attribute(&facedata.g_engine_config,
	                                                   MV_FACE_DETECTION_MODEL_FILE_PATH,
	                                                   "/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");
	if (err_code != MEDIA_VISION_ERROR_NONE)
			return err_code;

	err_code = mv_engine_config_foreach_supported_attribute(_mv_supported_attribute_cb, NULL);
	if (err_code != MEDIA_VISION_ERROR_NONE)
				return err_code;

}

bool _mv_supported_attribute_cb(mv_config_attribute_type_e attribute_type,
		const char *attribute_name, void *user_data){
	dlog_print(DLOG_DEBUG, "FACE_C", "%s", attribute_name);
	return true;
	/* @return @c true to continue with the next iteration of the loop, \n
	 * otherwise @c false to break out of the loop
	 */
}




