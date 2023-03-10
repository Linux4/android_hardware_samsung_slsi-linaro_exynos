/*
 * Copyright (C) 2014 ARM Limited. All rights reserved.
 *
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FORMAT_CHOOSER_H_
#define FORMAT_CHOOSER_H_

#include "gralloc_helper.h"

/* Internal format masks */
#define	    GRALLOC_ARM_INTFMT_FMT_MASK             0x00000000ffffffffULL
#define	    GRALLOC_ARM_INTFMT_EXT_MASK             0xffffffff00000000ULL

int check_for_compression(int w, int h, int format, uint64_t consumer_usage, uint64_t producer_usage);
uint64_t gralloc_select_format(int req_format, int usage, int is_compressible);

#endif /* FORMAT_CHOOSER_H_ */
