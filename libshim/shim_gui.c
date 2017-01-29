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

// android::status_t android::SurfaceControl::setLayer(int layer)
extern void _ZN7android14SurfaceControl8setLayerEj(void ** ret, void ** v, int layer);

// android::status_t android::SurfaceControl::setLayer(unsigned int layer)
void _ZN7android14SurfaceControl8setLayerEi(void ** ret, void ** v, unsigned int layer) {
  _ZN7android14SurfaceControl8setLayerEj(ret, v, (int)layer);
}
