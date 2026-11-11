#!/bin/sh
# Perform custom uninstall script for asc-devkit package
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

curpath=$(dirname $(readlink -f "$0"))
devkit_func_path="${curpath}/asc-devkit_func.sh"

. "${devkit_func_path}"

common_parse_dir=""
logfile=""
stage=""
is_quiet="n"
hetero_arch="n"

while true; do
    case "$1" in
    --common-parse-dir=*)
        common_parse_dir=$(echo "$1" | cut -d"=" -f2-)
        shift
        ;;
    --version-dir=*)
        pkg_version_dir=$(echo "$1" | cut -d"=" -f2-)
        shift
        ;;
    --logfile=*)
        logfile=$(echo "$1" | cut -d"=" -f2)
        shift
        ;;
    --stage=*)
        stage=$(echo "$1" | cut -d"=" -f2)
        shift
        ;;
    --quiet=*)
        is_quiet=$(echo "$1" | cut -d"=" -f2)
        shift
        ;;
    --hetero-arch=*)
        hetero_arch=$(echo "$1" | cut -d"=" -f2)
        shift
        ;;
    -*)
        shift
        ;;
    *)
        break
        ;;
    esac
done

# 写日志
log() {
    local cur_date="$(date +'%Y-%m-%d %H:%M:%S')"
    local log_type="$1"
    local log_msg="$2"
    local log_format="[AscDevkit] [$cur_date] [$log_type]: $log_msg"
    if [ "$log_type" = "INFO" ]; then
        echo "$log_format"
    elif [ "$log_type" = "WARNING" ]; then
        echo "$log_format"
    elif [ "$log_type" = "ERROR" ]; then
        echo "$log_format"
    elif [ "$log_type" = "DEBUG" ]; then
        echo "$log_format" 1> /dev/null
    fi
    echo "$log_format" >> "$logfile"
}

whl_uninstall_package() {
    local _module="$1"
    local _module_path="$2"
    if ! command -v pip3 >/dev/null 2>&1; then
        log "ERROR" "uninstall ${_module} failed, pip3 is not installed."
        exit 1
    fi
    if [ ! -d "$WHL_INSTALL_DIR_PATH/${_module}" ]; then
        pip3 show "${_module}" > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            log "WARNING" "${_module} is not exist."
        else
            pip3 uninstall -y "${_module}" 1> /dev/null 2>&1
            local ret=$?
            if [ $ret -ne 0 ]; then
                log "WARNING" "uninstall ${_module} failed, error code: $ret."
                exit 1
            else
                log "INFO" "${_module} uninstalled successfully!"
            fi
        fi
    else
        export PYTHONPATH="${_module_path}"
        export PIP_BREAK_SYSTEM_PACKAGES=1  # 强制卸载系统包
        pip3 uninstall -y "${_module}" > /dev/null 2>&1
        local ret=$?
        if [ $ret -ne 0 ]; then
            log "WARNING" "uninstall ${_module} failed, error code: $ret."
            exit 1
        else
            log "INFO" "${_module} uninstalled successfully!"
        fi
    fi
}

remove_empty_dir() {
    local _path="$1"
    if [ -d "${_path}" ]; then
        local is_empty=$(ls "${_path}" | wc -l)
        if [ "$is_empty" -ne 0 ]; then
            log "INFO" "${_path} dir is not empty."
        else
            prev_path=$(dirname "${_path}")
            chmod +w "${prev_path}" > /dev/null 2>&1
            rm -rf "${_path}" > /dev/null 2>&1
        fi
    fi
}

remove_package_leftovers() {
    local _path="$1"
    if [ -d "${_path}" ]; then
        prev_path=$(dirname "${_path}")
        chmod +w "${prev_path}" > /dev/null 2>&1
        rm -rf "${_path}" > /dev/null 2>&1
    fi
}

WHL_SOFTLINK_INSTALL_DIR_PATH="${common_parse_dir}/share/info/asc-devkit/python/site-packages"
WHL_INSTALL_DIR_PATH="${common_parse_dir}/python/site-packages"
ASC_OP_COMPILE_BASE_NAME="asc_op_compile_base"
ASC_OPC_TOOL_NAME="asc_opc_tool"

custom_uninstall() {
    if [ -z "$common_parse_dir/share/info/asc-devkit" ]; then
        log "ERROR" "ERR_NO:0x0001;ERR_DES:asc-devkit directory is empty"
        exit 1
    fi

    remove_hccl_header_without_hccl "$common_parse_dir" "$arch_linux_path" "$hetero_arch" || return 1

    if [ "$hetero_arch" != "y" ]; then
        chmod +w -R "${WHL_INSTALL_DIR_PATH}/asc_op_compile_base" 2> /dev/null
        chmod +w -R "${WHL_INSTALL_DIR_PATH}/asc_op_compile_base-0.1.0.dist-info" 2> /dev/null
        whl_uninstall_package "${ASC_OP_COMPILE_BASE_NAME}" "${WHL_INSTALL_DIR_PATH}"
        remove_package_leftovers "${WHL_INSTALL_DIR_PATH}/${ASC_OP_COMPILE_BASE_NAME}"

        chmod +w -R "${WHL_INSTALL_DIR_PATH}/asc_opc_tool" 2> /dev/null
        chmod +w -R "${WHL_INSTALL_DIR_PATH}/asc_opc_tool-0.1.0.dist-info" 2> /dev/null
        whl_uninstall_package "${ASC_OPC_TOOL_NAME}" "${WHL_INSTALL_DIR_PATH}"
        remove_package_leftovers "${WHL_INSTALL_DIR_PATH}/${ASC_OPC_TOOL_NAME}"

        if [ -d "${WHL_INSTALL_DIR_PATH}" ]; then
            local python_path=$(dirname "${WHL_INSTALL_DIR_PATH}")
            chmod +w "${python_path}"
        fi

        remove_empty_dir "${WHL_INSTALL_DIR_PATH}"
        remove_empty_dir "${common_parse_dir}/python"
    fi

    # Python bytecode generated after installation is not in filelist.csv.
    local module module_dir
    for module in legacy_modules fwk_modules; do
        module_dir="$common_parse_dir/$arch_linux_path/tikcpp/ascendc_kernel_cmake/$module"
        [ -d "$module_dir" ] || continue
        if ! find "$module_dir" -type d -name '__pycache__' -prune -exec rm -rf -- {} + \
            -o -type f -name '*.pyc' -exec rm -f -- {} +; then
            log "WARNING" "Failed to remove Python bytecode from $module_dir."
        fi
    done

    # remove softlinks for stub libs in devlib/linux/${ARCH}
    remove_stub_softlink "$common_parse_dir"

    return 0
}

custom_uninstall
if [ $? -ne 0 ]; then
    exit 1
fi
exit 0
