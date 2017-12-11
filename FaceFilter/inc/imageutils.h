/*
 * imageutils.h
 *
 *  Created on: Dec 10, 2017
 *      Author: tazo
 */

#ifndef IMAGEUTILS_H_
#define IMAGEUTILS_H_

#include <image_util.h>
#include <storage.h>

typedef struct _imageinfo{
	unsigned char* data;
	int size;
}imageinfo;

const char *_map_colorspace(image_util_colorspace_e color_space);

void _image_util_start_cb(void* user_data);

void _image_util_completed_cb(media_packet_h *dst, int error_code, void *user_data);

#endif /* IMAGEUTILS_H_ */
