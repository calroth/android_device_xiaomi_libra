#!/bin/bash -e
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

export VENDOR=xiaomi
export DEVICE=libra

# Load extractutils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$MY_DIR" ]]; then MY_DIR="$PWD"; fi

REPO_ROOT="$MY_DIR"/../../..
CM_ROOT="$REPO_ROOT"
HELPER=
for x in "${REPO_ROOT}"/vendor/*; do
  if [ -f "$x/build/tools/extract_utils.sh" ]; then
    HELPER="$x/build/tools/extract_utils.sh"
    break;
  fi
done
if [ ! -f "$HELPER" ]; then
  echo "Unable to find helper script at $HELPER"
  exit 1
fi
. "$HELPER"

# Overridden version of oat2dex for libra below...

#
# oat2dex:
#
# $1: extracted apk|jar (to check if deodex is required)
# $2: odexed apk|jar to deodex
# $3: source of the odexed apk|jar
#
# Convert apk|jar .odex in the corresposing classes.dex
#
function oat2dex() {
    local CM_TARGET="$1"
    local OEM_TARGET="$2"
    local SRC="$3"
    local TARGET=
    local OAT=

    if [ -z "$BAKSMALIJAR" ] || [ -z "$SMALIJAR" ] || [ -z "$OAT2DEXJAR" ]; then
        export BAKSMALIJAR="$CM_ROOT"/vendor/cm/build/tools/smali/baksmali.jar
        export SMALIJAR="$CM_ROOT"/vendor/cm/build/tools/smali/smali.jar
        export OAT2DEXJAR="$CM_ROOT"/device/xiaomi/libra/oat2dex.jar
    fi

    # Extract existing boot.oats to the temp folder
    if [ -z "$ARCHES" ]; then
        echo "Checking if system is odexed and extracting boot.oats, if applicable. This may take a while..."
        for ARCH in "arm64" "arm" "x86_64" "x86"; do
            if get_file "system/framework/$ARCH/boot.oat" "$TMPDIR/boot_$ARCH.oat" "$SRC"; then
                ARCHES+="$ARCH "
            fi
        done
    fi

    if [ -z "$ARCHES" ]; then
        FULLY_DEODEXED=1 && return 0 # system is fully deodexed, return
    fi

    if [ ! -f "$CM_TARGET" ]; then
        return;
    fi

    if grep "classes.dex" "$CM_TARGET" >/dev/null; then
        return 0 # target apk|jar is already odexed, return
    fi

    for ARCH in $ARCHES; do
        BOOTOAT="$TMPDIR/boot_$ARCH.oat"

        local OAT="$(dirname "$OEM_TARGET")/oat/$ARCH/$(basename "$OEM_TARGET" ."${OEM_TARGET##*.}").odex"
        local OAT2="$(dirname "$OEM_TARGET")/$ARCH/$(basename "$OEM_TARGET" ."${OEM_TARGET##*.}").odex"

        if get_file "$OAT" "$TMPDIR" "$SRC"; then
            java -jar "$BAKSMALIJAR" -x -o "$TMPDIR/dexout" -c "$BOOTOAT" -d "$TMPDIR" "$TMPDIR/$(basename "$OAT")"
        elif get_file "$OAT2" "$TMPDIR" "$SRC"; then
		    # Special version which runs oat2dex to deodex the OAT file
			# Regular baksmali can't handle art version 45 files (from Android L)
            java -jar "$OAT2DEXJAR" boot "$BOOTOAT"
            java -jar "$OAT2DEXJAR" "$TMPDIR/$(basename "$OAT2")" "$TMPDIR/odex"
            java -jar "$BAKSMALIJAR" -x -o "$TMPDIR/dexout" -c "$BOOTOAT" -d "$TMPDIR" "$TMPDIR/$(basename "$OAT2" ".odex").dex"
        elif [[ "$CM_TARGET" =~ .jar$ ]]; then
            # try to extract classes.dex from boot.oat for framework jars
            echo "(2)" java -jar "$BAKSMALIJAR" -x -o "$TMPDIR/dexout" -c "$BOOTOAT" -d "$TMPDIR" -e "/$OEM_TARGET" "$BOOTOAT"
            java -jar "$BAKSMALIJAR" -x -o "$TMPDIR/dexout" -c "$BOOTOAT" -d "$TMPDIR" -e "/$OEM_TARGET" "$BOOTOAT"
        else
            continue
        fi

        java -jar "$SMALIJAR" "$TMPDIR/dexout" -o "$TMPDIR/classes.dex" && break
    done

    rm -rf "$TMPDIR/dexout"
}

# ...Overridden version of oat2dex for libra above

if [ $# -eq 0 ]; then
  SRC=adb
else
  if [ $# -eq 1 ]; then
     SRC=$1
  elif [ $# -eq 2 ]; then
      SRC=$1
      RADIO_SRC=$2
  else
     echo "$0: bad number of arguments"
     echo ""
     echo "usage: $0 [PATH_TO_EXPANDED_ROM] [PATH_TO_RADIO_FOLDER]"
     echo ""
     echo "If PATH_TO_EXPANDED_ROM is not specified, blobs will be extracted from"
     echo "the device using adb pull."
     exit 1
  fi
fi

setup_vendor "$DEVICE" "$VENDOR" "$REPO_ROOT"

# Extract the device specific files that always occur in /system
extract "$MY_DIR/proprietary-blobs.txt" "$SRC"
if [ -n "$RADIO_SRC" ]; then
    extract_firmware "$MY_DIR"/proprietary-firmware.txt "$RADIO_SRC"
fi

"$MY_DIR"/setup-makefiles.sh
