#include "facefilter.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Evas_Object *layout;
	Evas_Object *camera_rect;
	Evas_Object *image;
	Evas_Object *box;

	camera_h camera;
	char* image_path;
} appdata_s;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void
my_box_pack(Evas_Object *box, Evas_Object *child, double h_weight, double v_weight, double h_align, double v_align)
{
	Evas_Object *frame = elm_frame_add(box);
	elm_object_style_set(frame, "pad_medium");
	evas_object_size_hint_weight_set(frame, h_weight, v_weight);
	evas_object_size_hint_align_set(frame, h_align, v_align);

	evas_object_size_hint_weight_set(child, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(child, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(child);
	elm_object_content_set(frame, child);

	elm_box_pack_end(box, frame);
	evas_object_show(frame);
}

static inline const char*
get_resource_path(const char* file_path)
{
	static char absolute_path[PATH_MAX] = "";
	static char *res_path_buff = NULL;
	if(res_path_buff == NULL)
		res_path_buff = app_get_resource_path();
	snprintf(absolute_path, sizeof(absolute_path), "%s%s", res_path_buff, file_path);
	return absolute_path;
}

static Evas_Object*
_main_layout_add(appdata_s *ad, Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	ad->layout = layout;

	char* edje_path = get_resource_path("edje/camera_capture.edj");
	elm_layout_file_set(ad->layout, edje_path, "camera_capture");

	Evas *evas = evas_object_evas_get(parent);
	ad->camera_rect = evas_object_image_filled_add(evas);
	elm_object_part_content_set(layout, "render", ad->camera_rect);

	ad->image = elm_image_add(parent);
	elm_object_part_content_set(layout, "gallery", ad->image);

	return layout;
}

static void _destroy_camera(appdata_s *ad)
{
	if(ad->camera)
	{
		camera_stop_preview(ad->camera);
		camera_destroy(ad->camera);
		ad->camera = NULL;
	}
}

static void
_create_camera(appdata_s *ad)
{
	if(ad->camera)
		_destroy_camera(ad);

	if(camera_create(CAMERA_DEVICE_CAMERA0, &ad->camera) == CAMERA_ERROR_NONE)
	{
		camera_set_capture_format(ad->camera, CAMERA_PIXEL_FORMAT_JPEG);
		camera_set_display(ad->camera, CAMERA_DISPLAY_TYPE_EVAS, GET_DISPLAY(ad->camera_rect));
		camera_set_display_mode(ad->camera, CAMERA_DISPLAY_MODE_FULL);

		camera_set_display_rotation(ad->camera, CAMERA_ROTATION_270);
		camera_set_display_flip(ad->camera, CAMERA_FLIP_NONE);
	}
	else {
		ad->camera = NULL;
	}

}

static inline char*
gen_data_path(const char *file_name)
{
	static char* absolute_path = NULL;
	char result[PATH_MAX] = "";
	if(absolute_path == NULL)
		absolute_path = app_get_data_path();
	snprintf(result, sizeof(result), "%s/%s", absolute_path, file_name);
	return strdup(result);
}

static char*
_save_file(appdata_s *ad, camera_image_data_s* image)
{
	char buf[PATH_MAX] = "";
	snprintf(buf, PATH_MAX, "camera_capture.jpg");
	char *file_name = gen_data_path(buf);

	FILE *f = fopen(file_name, "w");

	if(f)
	{
		fwrite(image->data, image->size, 1, f);
		fclose(f);
	}
	else
	{
		free(file_name);
		file_name = NULL;
	}
	return file_name;
}

static void
_on_camera_capture_cb(camera_image_data_s *image, camera_image_data_s *postview, camera_image_data_s *thumbnail, void *user_data)
{
	appdata_s *ad = user_data;
	free(ad->image_path);
	ad->image_path = _save_file(ad, image);
}

static void _on_camera_capture_completed_cb(void *user_data)
{
	appdata_s *ad = user_data;
	camera_start_preview(ad->camera);
}

static void
btn_capture_cb(void* data, Evas_Object *obj, void* event_info)
{
	appdata_s * ad = (appdata_s *)data;
	camera_start_capture(ad->camera, _on_camera_capture_cb, _on_camera_capture_completed_cb, ad);
}

static void
create_base_gui(appdata_s *ad)
{
	elm_config_accel_preference_set("opengl");

	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	/*ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>Hello Tizen</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);
*/

	ad->box = elm_box_add(ad->win);
	evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->box);
	evas_object_show(ad->box);

	Evas_Object *layout = _main_layout_add(ad, ad->win);
	my_box_pack(ad->box, layout, 0.9, 1.0, -1.0, -1.0);

	Evas_Object *btn = elm_button_add(ad->win);
	elm_object_text_set(btn, "#");
	evas_object_smart_callback_add(btn, "clicked", btn_capture_cb, ad);
	my_box_pack(ad->box, btn, 0.1, 0.0, -1.0, 0.5);

	_create_camera(ad);
	camera_start_preview(ad->camera);
	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	_destroy_camera(data);
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/

	int ret;
	char *language;

	ret = app_event_get_language(event_info, &language);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_event_get_language() failed. Err = %d.", ret);
		return;
	}

	if (language != NULL) {
		elm_language_set(language);
		free(language);
	}
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	appdata_s *ad = user_data;
	app_device_orientation_e screen_rot = 0;
	camera_rotation_e camera_rot = CAMERA_ROTATION_270;
	bool horizontal_box = false;

	app_event_get_device_orientation(event_info, &screen_rot);
	switch(screen_rot)
	{
	case APP_DEVICE_ORIENTATION_0:
		camera_rot = CAMERA_ROTATION_270;
		break;
	case APP_DEVICE_ORIENTATION_90:
		camera_rot = CAMERA_ROTATION_180;
		horizontal_box = true;
		break;
	case APP_DEVICE_ORIENTATION_180:
		camera_rot = CAMERA_ROTATION_90;
		break;
	case APP_DEVICE_ORIENTATION_270:
		camera_rot = CAMERA_ROTATION_NONE;
		horizontal_box = true;
		break;
	}

	camera_set_display_rotation(ad->camera, camera_rot);
	elm_win_rotation_with_resize_set(ad->win, screen_rot);
	elm_box_horizontal_set(ad->box, horizontal_box);

}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
