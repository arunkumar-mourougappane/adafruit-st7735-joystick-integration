/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _VAL_INTERNAL_TRUSTED_STORAGE_H_
#define _VAL_INTERNAL_TRUSTED_STORAGE_H_

#include "val.h"

#define UID_BASE_VALUE  0
#define BYTES_TO_BITS(byte)             (byte * 8)

enum its_function_code {
    VAL_ITS_SET                         = 0x1,
    VAL_ITS_GET                         = 0x2,
    VAL_ITS_GET_INFO                    = 0x3,
    VAL_ITS_REMOVE                      = 0x4,
};

uint32_t val_its_function(int type, ...);
#endif /* _VAL_INTERNAL_TRUSTED_STORAGE_H_ */
