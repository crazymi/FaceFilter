#include "imageutils.h"
#include "main.h"
#include <image_util.h>
#include <storage.h>


#define BUFLEN 256

static transformation_h handle = NULL;
static media_packet_h media_packet = NULL;
static char *images_directory = NULL;
static char sample_file_path[BUFLEN];
static char image_util_filename_encoded[BUFLEN];
static const char *image_util_source_filename = "glasses01.jpg";
static const char *image_util_filename_color = "colorspace.jpg";
static media_packet_h packet_h;

const char *_map_colorspace(image_util_colorspace_e color_space)
{
    switch (color_space) {
    case IMAGE_UTIL_COLORSPACE_YV12:
        return "IMAGE_UTIL_COLORSPACE_YV12";

    case IMAGE_UTIL_COLORSPACE_YUV422:
        return "IMAGE_UTIL_COLORSPACE_YUV422";

    case IMAGE_UTIL_COLORSPACE_I420:
        return "IMAGE_UTIL_COLORSPACE_I420";

    case IMAGE_UTIL_COLORSPACE_NV12:
        return "IMAGE_UTIL_COLORSPACE_NV12";

    case IMAGE_UTIL_COLORSPACE_UYVY:
        return "IMAGE_UTIL_COLORSPACE_UYVY";

    case IMAGE_UTIL_COLORSPACE_YUYV:
        return "IMAGE_UTIL_COLORSPACE_YUYV";

    case IMAGE_UTIL_COLORSPACE_RGB565:
        return "IMAGE_UTIL_COLORSPACE_RGB565";

    case IMAGE_UTIL_COLORSPACE_RGB888:
        return "IMAGE_UTIL_COLORSPACE_RGB888";

    case IMAGE_UTIL_COLORSPACE_ARGB8888:
        return "IMAGE_UTIL_COLORSPACE_ARGB8888";

    case IMAGE_UTIL_COLORSPACE_BGRA8888:
        return "IMAGE_UTIL_COLORSPACE_BGRA8888";

    case IMAGE_UTIL_COLORSPACE_RGBA8888:
        return "IMAGE_UTIL_COLORSPACE_RGBA8888";

    case IMAGE_UTIL_COLORSPACE_BGRX8888:
        return "IMAGE_UTIL_COLORSPACE_BGRX8888";

    case IMAGE_UTIL_COLORSPACE_NV21:
        return "IMAGE_UTIL_COLORSPACE_NV21";

    case IMAGE_UTIL_COLORSPACE_NV16:
        return "IMAGE_UTIL_COLORSPACE_NV16";

    case IMAGE_UTIL_COLORSPACE_NV61:
        return "IMAGE_UTIL_COLORSPACE_NV61";
    }
}

void _image_util_start_cb(void* user_data)
{
    /* Decode the given JPEG file to the img_source buffer. */
    unsigned char *img_source = NULL;
    int width, height;
    unsigned int size_decode;

    char *resource_path = app_get_resource_path();
    snprintf(sample_file_path, BUFLEN, "%s%s", resource_path, image_util_source_filename);

    int error_code = image_util_decode_jpeg(sample_file_path, IMAGE_UTIL_COLORSPACE_RGB888, &img_source, &width, &height, &size_decode);
    if (error_code != IMAGE_UTIL_ERROR_NONE) {
        DLOG_PRINT_ERROR("image_util_decode_jpeg", error_code);
        return;
    }

    DLOG_PRINT_DEBUG_MSG("Decoded image width: %d height: %d size %d", width, height, size_decode);

    /* Create a media format structure. */
    media_format_h fmt;
    error_code = media_format_create(&fmt);
    if (error_code != MEDIA_FORMAT_ERROR_NONE) {
        DLOG_PRINT_ERROR("media_format_create", error_code);
        free(img_source);
        return;
    }

    /* Set the MIME type of the created format. */
    error_code = media_format_set_video_mime(fmt, MEDIA_FORMAT_RGB888);
    if (error_code != MEDIA_FORMAT_ERROR_NONE) {
        DLOG_PRINT_ERROR("media_format_set_video_mime", error_code);
        media_format_unref(fmt);
        free(img_source);
        return;
    }

    /* Set the width of the created format. */
    error_code = media_format_set_video_width(fmt, width);
    if (error_code != MEDIA_FORMAT_ERROR_NONE) {
        DLOG_PRINT_ERROR("media_format_set_video_width", error_code);
        media_format_unref(fmt);
        free(img_source);
        return;
    }

    /* Set the height of the created format. */
    error_code = media_format_set_video_height(fmt, height);
    if (error_code != MEDIA_FORMAT_ERROR_NONE) {
        DLOG_PRINT_ERROR("media_format_set_video_height", error_code);
        media_format_unref(fmt);
        free(img_source);
        return;
    }

    /* Create a media packet with the image. */
    error_code = media_packet_create_alloc(fmt, NULL, NULL, &media_packet);
    if (error_code != MEDIA_PACKET_ERROR_NONE) {
        DLOG_PRINT_ERROR("media_packet_create_alloc", error_code);
        media_format_unref(fmt);
        free(img_source);
        return;
    }

    media_format_unref(fmt);

    /* Get the pointer to the internal media packet buffer, where the image will be stored. */
    void *packet_buffer = NULL;

    error_code = media_packet_get_buffer_data_ptr(media_packet, &packet_buffer);
    if (error_code != MEDIA_PACKET_ERROR_NONE || NULL == packet_buffer) {
        DLOG_PRINT_ERROR("media_packet_get_buffer_data_ptr", error_code);
        free(img_source);
        return;
    }

    /* Copy the image content to the media_packet internal buffer. */
    memcpy(packet_buffer, (void *) img_source, size_decode);
    free(img_source);

    /* Create a handle to the transformation. */
    error_code = image_util_transform_create(&handle);
    if (error_code != IMAGE_UTIL_ERROR_NONE) {
        DLOG_PRINT_ERROR("image_util_transform_create", error_code);
        return;
    }

    /* Disable the hardware acceleration for the created transformation. */
    error_code = image_util_transform_set_hardware_acceleration(handle, false);
    CHECK_ERROR("image_util_transform_set_hardware_acceleration", error_code);

	DLOG_PRINT_DEBUG_MSG("Converting the image color space.");

	image_util_colorspace_e colorspace = IMAGE_UTIL_COLORSPACE_NV12;

	/* Set the color space the image color space will be converted to. */
	error_code = image_util_transform_set_colorspace(handle, colorspace);
	if (error_code != IMAGE_UTIL_ERROR_NONE) {
		DLOG_PRINT_ERROR("image_util_transform_set_colorspace", error_code);
		return;
	}

    /* Execute the transformation. */
    error_code = image_util_transform_run(handle, media_packet, _image_util_completed_cb, NULL);
    if (error_code != IMAGE_UTIL_ERROR_NONE) {
        DLOG_PRINT_ERROR("image_util_transform_run", error_code);
    }
}

void _image_util_completed_cb(media_packet_h *dst, int error_code, void *user_data)
{
    packet_h = *dst;
    dlog_print(DLOG_DEBUG, LOG_TAG, "Transformation finished.");
    if (error_code != IMAGE_UTIL_ERROR_NONE || dst == NULL) {
        dlog_print(DLOG_ERROR, LOG_TAG, "An error occurred during transformation.<br>Error code: %d.", error_code);
    } else {
        /* Get the transformed image format. */
        media_format_h fmt = NULL;

        int error_code = media_packet_get_format(*dst, &fmt);
        if (error_code != MEDIA_PACKET_ERROR_NONE) {
            DLOG_PRINT_ERROR("media_packet_get_format", error_code);
            image_util_transform_destroy(handle);
            return;
        }

        /* Get the transformed image dimensions and MIME type. */
        media_format_mimetype_e mimetype;
        int width, height;

        error_code = media_format_get_video_info(fmt, &mimetype, &width, &height, NULL, NULL);
        if (error_code != MEDIA_FORMAT_ERROR_NONE) {
            DLOG_PRINT_ERROR("media_format_get_video_info", error_code);
            media_format_unref(fmt);
            image_util_transform_destroy(handle);
            return;
        }
        /* Release the memory allocated for the media format. */
        media_format_unref(fmt);

        /* Get the buffer where the transformed image is stored. */
        void *packet_buffer = NULL;

        error_code = media_packet_get_buffer_data_ptr(*dst, &packet_buffer);
        if (error_code != MEDIA_PACKET_ERROR_NONE) {
            DLOG_PRINT_ERROR("media_packet_get_buffer_data_ptr", error_code);
            image_util_transform_destroy(handle);
            return;
        }

        imageinfo* imginfo = (imageinfo*)user_data;
		imginfo->size = width*height;
		imginfo->data = (unsigned char*)malloc(sizeof(unsigned char)*imginfo->size);
		memcpy(imginfo->data, packet_buffer, sizeof(packet_buffer));

        DLOG_PRINT_DEBUG_MSG("sizeof(packet_buffer): %d", sizeof(packet_buffer));
    }

}
