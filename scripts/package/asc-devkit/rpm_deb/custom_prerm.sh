#!/bin/bash
set -e
# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

sourcedir="${INSTALL_PATH}"
pkg_arch_name="${PKG_ARCH_NAME}"
whl_install_dir="${sourcedir}/python/site-packages"
filelist="${sourcedir}/share/info/asc-devkit/script/filelist.csv"
export PYTHONPATH="${whl_install_dir}"
export PIP_BREAK_SYSTEM_PACKAGES=1

run_pip() {
    if command -v python3 >/dev/null 2>&1 && python3 -m pip --version >/dev/null 2>&1; then
        python3 -m pip "$@"
    elif command -v pip3 >/dev/null 2>&1; then
        pip3 "$@"
    else
        return 1
    fi
}

remove_python_package() {
    local package_name="$1"
    local module_name="$2"
    local dist_info_prefix="$3"
    local dist_info=""

    [ -d "${whl_install_dir}" ] || return 0
    chmod -R u+w "${whl_install_dir}/${module_name}" 2>/dev/null || true
    find "${whl_install_dir}" -maxdepth 1 -type d -name "${dist_info_prefix}-*.dist-info" -print |
    while IFS= read -r dist_info; do
        chmod -R u+w "${dist_info}" 2>/dev/null || true
    done

    if [ -d "${whl_install_dir}/${module_name}" ] ||
       [ -n "$(find "${whl_install_dir}" -maxdepth 1 -type d -name "${dist_info_prefix}-*.dist-info" -print -quit)" ]; then
        run_pip uninstall -y "${package_name}" >/dev/null 2>&1 || true
    fi

    rm -rf "${whl_install_dir:?}/${module_name}"
    find "${whl_install_dir}" -maxdepth 1 -type d -name "${dist_info_prefix}-*.dist-info" -exec rm -rf {} +
}

get_stub_libs_from_filelist() {
    [ -f "${filelist}" ] || return 0
    awk -F, -v arch_name="${pkg_arch_name}" 'BEGIN {
        prefix = "^" arch_name "-linux/devlib/"
        pattern = "^" arch_name "-linux/devlib/linux/" arch_name "/[^/]+\\.(so|a)$"
    }
    $4 ~ pattern {
        path = $4
        sub(prefix, "", path)
        print path
    }' "${filelist}"
}

remove_stub_softlinks() {
    local devlib_dir="${sourcedir}/${pkg_arch_name}-linux/devlib"
    local lib=""
    [ -d "${devlib_dir}" ] || return 0

    chmod u+w "${devlib_dir}"
    get_stub_libs_from_filelist | while IFS= read -r lib; do
        [ -n "${lib}" ] || continue
        if [ -L "${devlib_dir}/$(basename "${lib}")" ]; then
            rm -f "${devlib_dir}/$(basename "${lib}")"
        fi
    done
    chmod u-w "${devlib_dir}"
}

remove_empty_dir() {
    local dir="$1"
    [ -d "${dir}" ] || return 0
    rmdir "${dir}" 2>/dev/null || true
}

remove_python_package "asc_op_compile_base" "asc_op_compile_base" "asc_op_compile_base"
remove_python_package "asc_opc_tool" "asc_opc_tool" "asc_opc_tool"
remove_empty_dir "${whl_install_dir}"
remove_empty_dir "${sourcedir}/python"

rm -rf "${sourcedir}/${pkg_arch_name}-linux/tikcpp/ascendc_kernel_cmake/legacy_modules/util/__pycache__"
remove_stub_softlinks
