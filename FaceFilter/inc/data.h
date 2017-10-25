/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_DATA_H)
#define _DATA_H

#include "view.h"

void create_buttons_in_main_window(void);

static int camera_attr_get_filter_range(int *min,
        int *max);

static int camera_attr_get_filter(int* filter);
static int camera_attr_set_filter(int filter);
static int camera_attr_get_sticker_range(int* min,
        int* max);
static int camera_attr_get_sticker(int* sticker);
static int camera_attr_set_sticker(int sticker);
#endif