#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

vendor_name=customize
opmaster_so_name=libcust_opmaster_rt2.0.so
opsproto_so_name=libcust_opsproto_rt2.0.so
opapi_so_name=libcust_opapi.so
targetdir=/usr/local/Ascend/opp
target_custom=0

sourcedir=$PWD/packages
vendordir=vendors/$vendor_name

QUIET="y"
INSTALL_FOR_ALL="n"
FORCE_INSTALL="n"

CURR_OPERATE_USER="$(id -nu 2>/dev/null)"
CURR_OPERATE_GROUP="$(id -ng 2>/dev/null)"

while true
do
    case $1 in
    --quiet)
        QUIET="y"
        shift
    ;;
    --install-path=*)
        INSTALL_PATH=$(echo $1 | cut -d"=" -f2-)
        INSTALL_PATH=${INSTALL_PATH%*/}
        shift
    ;;
    --install-for-all)
        INSTALL_FOR_ALL="y"
        shift
    ;;
    --force)
        FORCE_INSTALL="y"
        shift
    ;;
    --*)
        shift
    ;;
    *)
        break
    ;;
    esac
done

log() {
    cur_date=$(date +"%Y-%m-%d %H:%M:%S")
    echo "[ops_custom] [$cur_date] "$1
}

resolve_so_paths() {
    local so_name="$1"
    local search_dir=""
    local so_path=""
    local arch_search_dir=""
    local arch_dir=""

    # Return value is a newline-separated so path list (echo per line).
    case "${so_name}" in
        "${opmaster_so_name}")
            search_dir="${sourcedir}/${vendordir}/op_impl/ai_core/tbe/op_tiling/lib/linux"
            ;;
        "${opsproto_so_name}")
            search_dir="${sourcedir}/${vendordir}/op_proto/lib/linux"
            ;;
        "${opapi_so_name}")
            # op_api uses fixed lib path and does not participate in arch-subdir traversal.
            so_path="${sourcedir}/${vendordir}/op_api/lib/${so_name}"
            [ -f "${so_path}" ] && echo "${so_path}"
            return 0
            ;;
        *)
            return 1
            ;;
    esac

    [ -d "${search_dir}" ] || return 0

    # Only validate linux/<arch>/ directories recognized by map_arch() whitelist.
    for arch_search_dir in "${search_dir}"/*; do
        [ -d "${arch_search_dir}" ] || continue
        arch_dir="${arch_search_dir##*/}"
        map_arch "${arch_dir}" >/dev/null 2>&1 || continue

        so_path=$(find "${arch_search_dir}" -type f -name "${so_name}" -print -quit 2>/dev/null)
        [ -n "${so_path}" ] || continue
        echo "${so_path}"
    done

    # If no whitelist arch dir exists or target so is absent, return empty.
    return 0
}

map_arch() {
    local arch_raw="$1"
    # Normalize arch strings from both `uname -m` and `readelf -h | Machine`.
    case "${arch_raw}" in
        x86_64|X86_64|amd64|AMD64|Advanced\ Micro\ Devices\ X86-64)
            echo "x86_64"
            return 0
            ;;
        aarch64|AArch64|arm64|ARM64)
            echo "aarch64"
            return 0
            ;;
        *)
            # Keep a strict whitelist to avoid silent misclassification.
            log "[ERROR] Unsupported architecture '${arch_raw}'."
            return 1
            ;;
    esac
}

version_gt() {
    local version_a="$1"
    local version_b="$2"
    local max_version

    # Shell convention: return 0 means true -> version_a > version_b.
    max_version=$(printf "%s\n%s\n" "${version_a}" "${version_b}" | sort -V | tail -n 1)
    if [ "${version_a}" = "${max_version}" ] && [ "${version_a}" != "${version_b}" ]; then
        return 0
    fi
    return 1
}

build_dlopen_lib_path() {
    local so_dir="$1"
    local dlopen_lib_path=""

    # Keep target so directory first to avoid resolving same-name so from other paths.
    dlopen_lib_path="${so_dir}:${sourcedir}/${vendordir}/op_api/lib"
    echo "${dlopen_lib_path}"
}

validate_so_dlopen() {
    local so_path="$1"
    local so_name="$2"
    local err_msg=""
    local so_dir=""
    local dlopen_lib_path=""

    so_dir=$(dirname "${so_path}")
    dlopen_lib_path=$(build_dlopen_lib_path "${so_dir}")

    # Use RTLD_NOW so unresolved dependency symbols fail immediately at load time.
err_msg=$(LD_LIBRARY_PATH="${dlopen_lib_path}:${LD_LIBRARY_PATH}" python3 - "${so_path}" << 'PY'
import ctypes
import os
import sys

path = sys.argv[1]
try:
    ctypes.CDLL(path, mode=os.RTLD_NOW)
except Exception as exc:
    print(str(exc))
    sys.exit(1)
PY
    )
    if [ $? -ne 0 ]; then
        err_msg="${err_msg//${so_path}/${so_name}}"
        log "[ERROR] Failed to load ${so_name} wish env ASCEND_HOME_PATH(${ASCEND_HOME_PATH}),error message is ${err_msg}."
        return 1
    fi
    return 0
}

validate_so_arch() {
    local so_path="$1"
    local so_name="$2"
    local env_arch="$3"
    local so_machine_raw=""
    local so_arch=""

    so_machine_raw=$(readelf -h "${so_path}" 2>/dev/null | awk -F: '/Machine:/{gsub(/^ +/, "", $2); print $2; exit}')
    if [ -z "${so_machine_raw}" ]; then
        log "[ERROR] Failed to parse architecture from '${so_name}'."
        return 1
    fi

    so_arch=$(map_arch "${so_machine_raw}")
    if [ $? -ne 0 ] || [ -z "${so_arch}" ]; then
        return 1
    fi

    if [ "${so_arch}" != "${env_arch}" ]; then
        log "[ERROR] The architecture of the shared library '${so_name}' (${so_arch}) is incompatible with the current running environment (${env_arch})."
        return 1
    fi
    return 0
}

validate_so_glibc() {
    local so_path="$1"
    local so_name="$2"
    local env_glibc="$3"
    local required_glibc=""

    # Read real ELF version requirements instead of free-form strings to avoid false positives.
    required_glibc=$(readelf -V "${so_path}" 2>/dev/null | grep -o 'GLIBC_[0-9]\+\(\.[0-9]\+\)\+' | sed 's/GLIBC_//' | sort -V | tail -n 1)
    if [ -z "${required_glibc}" ]; then
        return 0
    fi

    if [ -z "${env_glibc}" ]; then
        log "[ERROR] Failed to get glibc version from current runtime environment."
        return 1
    fi

    if version_gt "${required_glibc}" "${env_glibc}"; then
        log "[ERROR] The shared library '${so_name}' was linked against a newer version of the GNU C Library (glibc). The current runtime environment has an older version of glibs, which does not provide the required symbols."
        return 1
    fi
    return 0
}

validate_so_list() {
    local mode="$1"
    local env_arch="$2"
    local env_glibc="$3"
    local so_name=""
    local so_path=""
    local so_paths=""

    # mode=dlopen: ASCEND_HOME_PATH branch; mode=compat: arch+glibc branch.
    for so_name in "${opmaster_so_name}" "${opsproto_so_name}" "${opapi_so_name}"; do
        so_paths=$(resolve_so_paths "${so_name}")
        if [ $? -ne 0 ]; then
            log "[ERROR] Failed to resolve shared library '${so_name}' path."
            return 1
        fi
        if [ -z "${so_paths}" ]; then
            if [ "${so_name}" = "${opmaster_so_name}" ]; then
                log "[ERROR] Required shared library '${so_name}' was not found in package path."
                return 1
            fi
            continue
        fi
        # Read newline-separated so paths produced by resolve_so_paths().
        while IFS= read -r so_path; do
            [ -z "${so_path}" ] && continue
            if [ "${mode}" = "dlopen" ]; then
                validate_so_dlopen "${so_path}" "${so_name}" || return 1
            else
                validate_so_arch "${so_path}" "${so_name}" "${env_arch}" || return 1
                validate_so_glibc "${so_path}" "${so_name}" "${env_glibc}" || return 1
            fi
        done <<< "${so_paths}"
    done
    return 0
}

validate_shared_libraries() {
    local env_arch=""
    local env_glibc=""

    if [ "${FORCE_INSTALL}" = "y" ]; then
        log "[WARNING] --force is enabled, skip shared library validation."
        return 0
    fi

    # ASCEND_HOME_PATH exists: verify by dlopen in current runtime env.
    if [ -n "${ASCEND_HOME_PATH}" ]; then
        validate_so_list "dlopen" "" "" || return 1
    else
        # No ASCEND_HOME_PATH: enforce arch + glibc compatibility checks.
        env_arch=$(map_arch "$(uname -m)")
        if [ $? -ne 0 ] || [ -z "${env_arch}" ]; then
            return 1
        fi
        env_glibc=$(ldd --version 2>/dev/null | head -n 1 | grep -o '[0-9]\+\.[0-9]\+\(\.[0-9]\+\)\?' | head -n 1)
        validate_so_list "compat" "${env_arch}" "${env_glibc}" || return 1
    fi
    return 0
}

set_install_for_all_mod() {
    local _outvar="$1"
    local _mod="$2"
    local _new_mod

    local _new_mod="${_mod%?}"
    local _new_mod="${_new_mod}${_new_mod#${_new_mod%?}}"

    eval "${_outvar}=\"${_new_mod}\""
}

apply_chmod() {
    local path="$1"
    local mod="$2"
    local install_for_all="$3"

    local own="${CURR_OPERATE_USER}:${CURR_OPERATE_GROUP}"

    if [ "$install_for_all" = "true" ] || [ "$install_for_all" = "y" ]; then
        set_install_for_all_mod "mod" "$mod"
    fi
    chmod -R "$mod" "$path"
    if [ $? -ne 0 ]; then
        log "[ERROR] chmod $path $mod failed."
        return 1
    fi

    chown -f "$own" "$path"
    if [ $? -ne 0 ]; then
        log "[ERROR] chown $path $own failed."
        return 1
    fi

    return 0
}

create_dir() {
    local path="$1"
    local mod="$2"
    local install_for_all="$3"

    if [ "$path" = "" ]; then
        log "[ERROR] dir path is empty"
        return 1
    fi

    if [ ! -d "$path" ]; then
        mkdir -p "$path"
        if [ $? -ne 0 ]; then
            log "[ERROR] create dir $path failed."
            return 1
        fi
    fi

    apply_chmod "$path" "$mod" "$install_for_all"
    if [ $? -ne 0 ]; then
        return 1
    fi

    return 0
}

if [ -n "${INSTALL_PATH}" ]; then
    if [[ ! "${INSTALL_PATH}" = /* ]]; then
        log "[ERROR] use absolute path for --install-path argument"
        exit 1
    fi
    if [ ! -d ${INSTALL_PATH} ]; then
        create_dir "${INSTALL_PATH}" "750" "${INSTALL_FOR_ALL}"
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi
    targetdir=${INSTALL_PATH}
else
    if [ "x${ASCEND_OPP_PATH}" == "x" ]; then
        log "[ERROR] env ASCEND_OPP_PATH no exist"
        exit 1
    fi
    targetdir="${ASCEND_OPP_PATH}"
    if [ -n "${ASCEND_CUSTOM_OPP_PATH}" ]; then
        log "[NOTE] ASCEND_CUSTOM_OPP_PATH is a runtime search path and does not control the installation destination. \
        The package will be installed to ${targetdir}."
        log "[NOTE] Operators in ASCEND_CUSTOM_OPP_PATH take precedence over operators installed in the default path. \
        To give this package higher priority, use --install-path=<path> and source the generated set_env.bash."
    fi
fi

if [ ! -d $targetdir ];then
    log "[ERROR] $targetdir no exist"
    exit 1
fi

if [ ! -x $targetdir ] || [ ! -w $targetdir ] || [ ! -r $targetdir ];then
    log "[WARNING] The directory $targetdir does not have sufficient permissions. \
    Please check and modify the folder permissions (e.g., using chmod), \
    or use the --install-path option to specify an installation path and source \
    the generated set_env.bash."
fi

upgrade()
{
    if [ ! -d ${sourcedir}/$vendordir/$1 ]; then
        log "[INFO] no need to upgrade ops $1 files"
        return 0
    fi

    if [ ! -d ${targetdir}/$vendordir/$1 ];then
        log "[INFO] create ${targetdir}/$vendordir/$1."
        create_dir "${targetdir}/$vendordir/$1" "750" "${INSTALL_FOR_ALL}"
        if [ $? -ne 0 ];then
            return 1
        fi
    else
        has_same_file=-1
        for file_a in ${sourcedir}/$vendordir/$1/*; do
            file_b=${file_a##*/};
            if [ "ls ${targetdir}/$vendordir/$1" = "" ]; then
                log "[INFO] ${targetdir}/$vendordir/$1 is empty !!"
		        return 1
	          fi
            grep -q $file_b <<<`ls ${targetdir}/$vendordir/$1`;
            if [[ $? -eq 0 ]]; then
                echo -n "${file_b} "
                has_same_file=0
            fi
        done
        if [ 0 -eq $has_same_file ]; then
            echo
            if test $QUIET = "n"; then
                echo "[INFO]: has old version in ${targetdir}/$vendordir/$1, \
                you want to Overlay Installation , please enter:[o]; \
                or replace directory installation , please enter: [r]; \
                or not install , please enter:[n]."

                while true
                do
                    read orn
                    if [ "$orn" = n ]; then
                        return 0
                    elif [ "$orn" = o ]; then
                        break;
                    elif [ "$orn" = r ]; then
                        [ -n "${targetdir}/$vendordir/$1/" ] && rm -rf "${targetdir}/$vendordir/$1"/*
                        break;
                    else
                        log "[ERROR] input error, please input again!"
                    fi
                done
            else
                [ -n "${targetdir}/$vendordir/$1/" ] && rm -rf "${targetdir}/$vendordir/$1"/*
            fi
        fi
        log "[INFO] replace or merge old ops $1 files ......"
    fi

    log "[INFO] copy new ops $1 files ......"
    if [ -d ${targetdir}/$vendordir/$1/ ]; then
        chmod -R +w "$targetdir/$vendordir/$1/" >/dev/null 2>&1
    fi
    cp -rf ${sourcedir}/$vendordir/$1/* $targetdir/$vendordir/$1/
    if [ $? -ne 0 ];then
        log "[ERROR] copy new $1 files failed"
        return 1
    fi

    apply_chmod "$targetdir/$vendordir/$1/" "750" "${INSTALL_FOR_ALL}"
    return 0
}
upgrade_proto()
{
    if [ ! -f ${sourcedir}/$vendordir/custom.proto ]; then
        log "[INFO] no need to upgrade custom.proto files"
        return 0
    fi
    if [ ! -d ${targetdir}/$vendordir/framework/caffe ];then
        log "[INFO] create ${targetdir}/$vendordir/framework/caffe."
        create_dir "${targetdir}/$vendordir/framework/caffe" "750" "${INSTALL_FOR_ALL}"
        if [ $? -ne 0 ];then
            return 1
        fi
    else
        if [ -f ${targetdir}/$vendordir/framework/caffe/custom.proto ]; then
            # 有老版本,判断是否要覆盖式安装
            if test $QUIET = "n"; then
                  echo "[INFO] ${targetdir}/$vendordir/framework/caffe has old version"\
                "custom.proto file. Do you want to replace? [y/n] "

                while true
                do
                    read yn
                    if [ "$yn" = n ]; then
                        return 0
                    elif [ "$yn" = y ]; then
                        break;
                    else
                        log "[ERROR] input error, please input again!"
                    fi
                done
            fi
        fi
        log "[INFO] replace old caffe.proto files ......"
    fi
    apply_chmod "$targetdir/$vendordir/framework/caffe/" "750" "${INSTALL_FOR_ALL}"
    cp -rf ${sourcedir}/$vendordir/custom.proto ${targetdir}/$vendordir/framework/caffe/
    if [ $? -ne 0 ];then
        log "[ERROR] copy new custom.proto failed"
        return 1
    fi
	log "[INFO] copy custom.proto success"

    return 0
}

upgrade_file()
{
    if [ ! -e ${sourcedir}/$vendordir/$1 ]; then
        log "[INFO] no need to upgrade ops $1 file"
        return 0
    fi

    log "[INFO] copy new $1 files ......"
    cp -f ${sourcedir}/$vendordir/$1 $targetdir/$vendordir/$1
    if [ $? -ne 0 ];then
        log "[ERROR] copy new $1 file failed"
        return 1
    fi
    apply_chmod "$targetdir/$vendordir/$1" "750" "${INSTALL_FOR_ALL}"
    return 0
}

upgrade_uninstall()
{
    uninstall_file=$(realpath ${sourcedir}/../uninstall.sh)
    dst_path=$targetdir/$vendordir/scripts
    if [ ! -e ${uninstall_file} ]; then
        log "[INFO] no need to upgrade uninstall.sh file"
        return 0
    fi

    log "[INFO] copy new uninstall.sh files ......"
    if [ ! -e ${dst_path} ]; then
        create_dir "${dst_path}" "750" "${INSTALL_FOR_ALL}"
    fi
    if [ ! -w ${dst_path} ]; then
        apply_chmod "$dst_path" "750" "${INSTALL_FOR_ALL}"
    fi
    cp -f ${uninstall_file} $dst_path
    if [ $? -ne 0 ];then
        log "[ERROR] copy new uninstall.sh file failed"
        return 1
    fi
    apply_chmod "$dst_path" "550" "${INSTALL_FOR_ALL}"
    return 0
}

delete_optiling_file()
{
  if [ ! -d ${targetdir}/vendors ];then
    log "[INFO] $1 not exist, no need to uninstall"
    return 0
  fi
  sys_info=$(uname -m)
  if [ ! -d ${sourcedir}/$vendordir/$1/ai_core/tbe/op_tiling/lib/linux/${sys_info} ];then
    rm -rf ${sourcedir}/$vendordir/$1/ai_core/tbe/op_tiling/liboptiling.so
  fi
  return 0
}

if [ ! -d "${targetdir}/vendors" ];then
    log "[INFO] create ${targetdir}/vendors."
    create_dir "${targetdir}/vendors" "750" "${INSTALL_FOR_ALL}"
    if [ $? -ne 0 ];then
        exit 1
    fi
else
    apply_chmod "${targetdir}/vendors" "750" "${INSTALL_FOR_ALL}"
fi

if [ ! -d "${targetdir}/$vendordir" ];then
    log "[INFO] create ${targetdir}/$vendordir."
    create_dir "${targetdir}/$vendordir" "750" "${INSTALL_FOR_ALL}"
    if [ $? -ne 0 ];then
        exit 1
    fi
else
    apply_chmod "${targetdir}/$vendordir" "750" "${INSTALL_FOR_ALL}"
fi

log "[INFO] validate shared libraries before installation"
validate_shared_libraries
if [ $? -ne 0 ]; then
    log "[ERROR] Shared library validation failed. To skip this validation and continue installation, run this package again with --force."
    exit 1
fi

log "[INFO] upgrade framework"
upgrade framework
if [ $? -ne 0 ];then
    exit 1
fi

log "[INFO] upgrade op proto"
upgrade op_proto
if [ $? -ne 0 ];then
    exit 1
fi

log "[INFO] upgrade op impl"
delete_optiling_file op_impl
upgrade op_impl
if [ $? -ne 0 ];then
    exit 1
fi

log "[INFO] upgrade op api"
upgrade op_api
if [ $? -ne 0 ];then
    exit 1
fi

log "[INFO] upgrade uninstall"
upgrade_uninstall
if [ $? -ne 0 ];then
    exit 1
fi

log "[INFO] upgrade version.info"
upgrade_file version.info
if [ $? -ne 0 ];then
    exit 1
fi

upgrade_proto
if [ $? -ne 0 ];then
    exit 1
fi

# set the set_env.bash
if [ -n "${INSTALL_PATH}" ] && [ -d ${INSTALL_PATH} ]; then
    _ASCEND_CUSTOM_OPP_PATH=${targetdir}/${vendordir}
    bin_path="${_ASCEND_CUSTOM_OPP_PATH}/bin"
    set_env_variable="#!/bin/bash\nexport ASCEND_CUSTOM_OPP_PATH=${_ASCEND_CUSTOM_OPP_PATH}:\${ASCEND_CUSTOM_OPP_PATH}\nexport LD_LIBRARY_PATH=${_ASCEND_CUSTOM_OPP_PATH}/op_api/lib/:\${LD_LIBRARY_PATH}"
    if [ ! -d ${bin_path} ]; then
        create_dir "${bin_path}" "750" "${INSTALL_FOR_ALL}"
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi
    echo -e ${set_env_variable} > ${bin_path}/set_env.bash
    if [ $? -ne 0 ]; then
        log "[ERROR] write ASCEND_CUSTOM_OPP_PATH to set_env.bash failed"
        exit 1
    else
        log "[INFO] using requirements: when custom module install finished or before you run the custom module, \
        execute the command [ source ${bin_path}/set_env.bash ] to set the environment path"
    fi
else
    _ASCEND_CUSTOM_OPP_PATH=${targetdir}/${vendordir}
    config_file=${targetdir}/vendors/config.ini
    if [ ! -f ${config_file} ]; then
        touch ${config_file}
        apply_chmod "${config_file}" "640" "${INSTALL_FOR_ALL}"
        echo "load_priority=$vendor_name" > ${config_file}
        if [ $? -ne 0 ];then
            log "[ERROR] echo load_priority failed"
            exit 1
        fi
    else
        found_vendors="$(grep -w "load_priority" "$config_file" | cut --only-delimited -d"=" -f2-)"
        found_vendor=$(echo $found_vendors | sed "s/\<$vendor_name\>//g" | tr ',' ' ')
        vendor=$(echo $found_vendor | tr -s ' ' ',')
        if [ "$vendor" != "" ]; then
            sed -i "/load_priority=$found_vendors/s@load_priority=$found_vendors@load_priority=$vendor_name,$vendor@g" "$config_file"
        fi
    fi
    apply_chmod "${config_file}" "750" "${INSTALL_FOR_ALL}"
    log "[INFO] using requirements: when custom module install finished or before you run the custom module, \
        execute the command [ export LD_LIBRARY_PATH=${_ASCEND_CUSTOM_OPP_PATH}/op_api/lib/:\${LD_LIBRARY_PATH} ] to set the environment path"
fi

if [ -d ${targetdir}/$vendordir/op_impl/cpu/aicpu_kernel/impl/ ]; then
    chmod -R 444 ${targetdir}/$vendordir/op_impl/cpu/aicpu_kernel/impl/* >/dev/null 2>&1
fi

echo "SUCCESS"
exit 0
