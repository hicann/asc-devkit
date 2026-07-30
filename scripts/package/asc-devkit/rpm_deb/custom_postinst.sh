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
unset PYTHONPATH
export PIP_BREAK_SYSTEM_PACKAGES=1
pip_command=""

detect_pip() {
    if command -v python3 >/dev/null 2>&1 && python3 -m pip --version >/dev/null 2>&1; then
        pip_command="python3"
    elif command -v pip3 >/dev/null 2>&1 && pip3 --version >/dev/null 2>&1; then
        pip_command="pip3"
    fi
}

run_pip() {
    if [ "${pip_command}" = "python3" ]; then
        python3 -m pip "$@"
    else
        pip3 "$@"
    fi
}

install_purelib_wheel() {
    local wheel="$1"
    if ! command -v python3 >/dev/null 2>&1; then
        echo "[asc-devkit] python3 is required to install ${wheel}." >&2
        return 1
    fi

    python3 - "${wheel}" "${whl_install_dir}" <<'PY'
import os
import sys
import zipfile

wheel_path = sys.argv[1]
target_dir = os.path.realpath(sys.argv[2])

with zipfile.ZipFile(wheel_path) as wheel:
    members = wheel.infolist()
    metadata_files = [item.filename for item in members if item.filename.endswith(".dist-info/WHEEL")]
    if len(metadata_files) != 1:
        raise RuntimeError(f"invalid wheel metadata: {wheel_path}")

    metadata = wheel.read(metadata_files[0]).decode("utf-8")
    if "Root-Is-Purelib: true" not in metadata:
        raise RuntimeError(f"pip is required to install non-purelib wheel: {wheel_path}")

    for item in members:
        parts = item.filename.split("/")
        if any(part.endswith(".data") for part in parts):
            raise RuntimeError(f"pip is required to install wheel with .data content: {wheel_path}")
        destination = os.path.realpath(os.path.join(target_dir, item.filename))
        if os.path.commonpath((target_dir, destination)) != target_dir:
            raise RuntimeError(f"unsafe wheel member: {item.filename}")

    os.makedirs(target_dir, exist_ok=True)
    wheel.extractall(target_dir)
PY
}

install_wheel() {
    local wheel="$1"
    if [ ! -f "${wheel}" ]; then
        echo "[asc-devkit] wheel not found: ${wheel}" >&2
        return 1
    fi
    echo "[asc-devkit] installing ${wheel}"
    if [ -n "${pip_command}" ]; then
        run_pip install --disable-pip-version-check --upgrade --no-deps --force-reinstall \
            -t "${whl_install_dir}" "${wheel}"
    else
        echo "[asc-devkit] pip is unavailable, extracting purelib wheel with python3"
        install_purelib_wheel "${wheel}"
    fi
}

create_package_directories() {
    local path=""
    local mode=""
    local parent_dir=""
    local parent_mode=""
    local tab=""
    [ -f "${filelist}" ] || return 0

    tab=$(printf '\t')
    awk -F, 'NR > 1 && $2 == "mkdir" && $4 != "NA" { print $4 "\t" $6 }' "${filelist}" |
    while IFS="${tab}" read -r path mode; do
        [ -n "${path}" ] || continue
        parent_dir="${sourcedir}/$(dirname "${path}")"
        parent_mode=$(stat -c %a "${parent_dir}")
        chmod u+w "${parent_dir}"
        mkdir -p "${sourcedir}/${path}"
        chmod "${parent_mode}" "${parent_dir}"
        [ "${mode}" = "NA" ] || chmod "${mode}" "${sourcedir}/${path}"
    done
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

create_stub_softlinks() {
    local devlib_dir="${sourcedir}/${pkg_arch_name}-linux/devlib"
    local lib=""
    [ -d "${devlib_dir}" ] || return 0

    chmod u+w "${devlib_dir}"
    get_stub_libs_from_filelist | while IFS= read -r lib; do
        [ -n "${lib}" ] || continue
        if [ -f "${devlib_dir}/${lib}" ]; then
            ln -sfn "${lib}" "${devlib_dir}/$(basename "${lib}")"
        fi
    done
    chmod u-w "${devlib_dir}"
}

set_python_permissions() {
    local path=""
    for path in \
        "${whl_install_dir}/asc_op_compile_base" \
        "${whl_install_dir}/asc_op_compile_base-0.1.0.dist-info" \
        "${whl_install_dir}/asc_opc_tool" \
        "${whl_install_dir}/asc_opc_tool-0.1.0.dist-info"; do
        [ -e "${path}" ] || continue
        find "${path}" -type d -exec chmod 550 {} +
        find "${path}" -type f -exec chmod 550 {} +
    done
}

clear_kernel_cache_dir() {
    local cache_dir=""
    local atc_data_dir="${HOME}/atc_data"
    [ -w "${atc_data_dir}" ] || return 0
    for cache_dir in "${atc_data_dir}"/kernel_cache*; do
        [ -d "${cache_dir}" ] || continue
        rm -rf "${cache_dir}" || true
    done
}

remove_wheel_payload() {
    case "${1:-}" in
        *[!0-9]*|'')
            rm -f \
                "${sourcedir}/lib/asc_op_compile_base-0.1.0-py3-none-any.whl" \
                "${sourcedir}/lib/asc_opc_tool-0.1.0-py3-none-any.whl"
            rmdir "${sourcedir}/lib" 2>/dev/null || true
            ;;
    esac
}

mkdir -p "${whl_install_dir}"
detect_pip
install_wheel "${sourcedir}/lib/asc_op_compile_base-0.1.0-py3-none-any.whl"
install_wheel "${sourcedir}/lib/asc_opc_tool-0.1.0-py3-none-any.whl"
set_python_permissions
create_package_directories
create_stub_softlinks

remove_wheel_payload "${1:-}"
clear_kernel_cache_dir
