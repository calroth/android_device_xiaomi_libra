/*
 * Copyright (C) 2016 The CyanogenMod Project <http://www.cyanogenmod.org>
 * Copyright (C) 2017 The LineageOS Project <http://www.lineageos.org>
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

// libra: gps.libra shim
//
// The default Android L proprietary blob supplied by Xiaomi (which we have
// at "/system/lib64/hw/gps.vendor.default.so") returns a NULL for
// "data_conn_open_with_apn_ip_type" when we request an AGpsInterface_v2.
// This NULL gets dereferenced by system_server, crashing it.
//
// This shim intercepts the NULL and downgrades the AGpsInterface_v2 into
// an AGpsInterface_v1, which doesn't have "data_conn_open_with_apn_ip_type".
//
// Kanged from Galaxy S2. Props to them!

#define LOG_TAG "gps.libra"

#include <utils/Log.h>
#include <hardware/gps.h>

#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REAL_GPS_PATH "/system/lib64/hw/gps.vendor.default.so"

const GpsInterface * (*vendor_get_gps_interface)(struct gps_device_t * dev);
const void * (*vendor_get_extension)(const char * name);

static AGpsInterface_v1 shim_AGpsInterface;
static GpsInterface shim_GpsInterface;

const void * shim_get_extension(const char * name) {
	ALOGD("%s(%s)", __func__, name);
	const void * ret_val = vendor_get_extension(name);
	if (strcmp(name, AGPS_INTERFACE) == 0) {
		const AGpsInterface * vendor_AGpsInterface = (const AGpsInterface *)ret_val;
		if (vendor_AGpsInterface->size == sizeof(AGpsInterface_v2) &&
				vendor_AGpsInterface->data_conn_open_with_apn_ip_type == NULL) {
			ALOGW("data_conn_open_with_apn_ip_type == NULL. This would "
					"usually cause a crash. Removing it.");
			// Downgrade the vendor's AGpsInterface_v2 by copying its
			// members into our own AGpsInterface_v1.
			shim_AGpsInterface.size = sizeof(AGpsInterface_v1);
			shim_AGpsInterface.init = vendor_AGpsInterface->init;
			shim_AGpsInterface.data_conn_open = vendor_AGpsInterface->data_conn_open;
			shim_AGpsInterface.data_conn_closed = vendor_AGpsInterface->data_conn_closed;
			shim_AGpsInterface.data_conn_failed = vendor_AGpsInterface->data_conn_failed;
			shim_AGpsInterface.set_server = vendor_AGpsInterface->set_server;
			// And return our own AGpsInterface_v1.
			ret_val = &shim_AGpsInterface;
		}
	}
	return ret_val;
}

const GpsInterface * shim_get_gps_interface(struct gps_device_t * dev) {
	const GpsInterface * vendor_GpsInterface = vendor_get_gps_interface(dev);
	// Originally, the code altered the vendor's GpsInterface in-place.
	// With modern security measures, this causes SEGV_ACCERR...
	// So, copy the vendor's GpsInterface into our own and return that.
	memcpy(&shim_GpsInterface, vendor_GpsInterface, sizeof(GpsInterface));
	ALOGD("%s: shimming vendor get_extension", __func__);
	
	vendor_get_extension = vendor_GpsInterface->get_extension;
	shim_GpsInterface.get_extension = &shim_get_extension;
	
	return &shim_GpsInterface;
}

static int open_gps(const struct hw_module_t * module __attribute__ ((unused)),
		char const * name, struct hw_device_t ** device) {
	void * realGpsLib;
	int gpsHalResult;
	struct hw_module_t * realHalSym;

	struct gps_device_t ** gps = (struct gps_device_t **)device;

	realGpsLib = dlopen(REAL_GPS_PATH, RTLD_LOCAL);
	if (!realGpsLib) {
		ALOGE("Failed to load real GPS HAL '" REAL_GPS_PATH "': %s", dlerror());
		return -EINVAL;
	}

	realHalSym = (struct hw_module_t *)dlsym(realGpsLib, HAL_MODULE_INFO_SYM_AS_STR);
	if (!realHalSym) {
		ALOGE("Failed to locate the GPS HAL module sym: '" HAL_MODULE_INFO_SYM_AS_STR "': %s", dlerror());
		goto dl_err;
	}

	int result = realHalSym->methods->open(realHalSym, name, device);
	if (result < 0) {
		ALOGE("Real GPS open method failed: %d", result);
		goto dl_err;
	}
	ALOGD("Successfully loaded real GPS hal, shimming get_gps_interface...");
	// now, we shim hw_device_t
	vendor_get_gps_interface = (*gps)->get_gps_interface;
	(*gps)->get_gps_interface = &shim_get_gps_interface;

	return 0;
dl_err:
	dlclose(realGpsLib);
	return -EINVAL;
}

static struct hw_module_methods_t gps_module_methods = {
	.open = open_gps
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.module_api_version = 1,
	.hal_api_version = 0,
	.id = GPS_HARDWARE_MODULE_ID,
	.name = "Xiaomi Mi 4c (libra) GPS shim",
	.author = "The LineageOS Project",
	.methods = &gps_module_methods
};
