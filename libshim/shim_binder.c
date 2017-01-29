/*
 * Copyright (C) 2017 The LineageOS Project
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

#include <cutils/jstring.h>
#include <sys/types.h>

#ifdef __aarch64__
  // android::status_t android::Parcel::writeString16(const char16_t * str, size_t len)
  extern void _ZN7android6Parcel13writeString16EPKDsm(void ** ret, void ** v, const char16_t * str, size_t len);
  
  // android::status_t android::Parcel::writeString16(const unsigned short * str, size_t len)
  void _ZN7android6Parcel13writeString16EPKtm(void ** ret, void ** v, const unsigned short * str, size_t len) {
    _ZN7android6Parcel13writeString16EPKDsm(ret, v, (const char16_t *)str, len);
  }
#else
  // android::status_t android::Parcel::writeString16(const char16_t * str, size_t len)
  extern void _ZN7android6Parcel13writeString16EPKDsj(void ** ret, void ** v, const char16_t * str, size_t len);
  
  // android::status_t android::Parcel::writeString16(const unsigned short * str, size_t len)
  void _ZN7android6Parcel13writeString16EPKtj(void ** ret, void ** v, const unsigned short * str, size_t len) {
    _ZN7android6Parcel13writeString16EPKDsj(ret, v, (const char16_t *)str, len);
  }
#endif
