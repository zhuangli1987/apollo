/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBERTRON_CROUTINE_SYSTEM_SYSTEM_HOOK_H_
#define CYBERTRON_CROUTINE_SYSTEM_SYSTEM_HOOK_H_

#include <unistd.h>
#include <iostream>

extern "C" {
typedef unsigned int (*usleep_t)(useconds_t);
extern usleep_t usleep_f;
}

void system_hook_init();
int usleep(useconds_t usec);

#endif  // CYBERTRON_CROUTINE_SYSTEM_SYSTEM_HOOK_H_
