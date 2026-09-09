#!/bin/sh
# Perform custom create softlink script for asc-devkit package
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
common_func_path="${curpath}/common_func.inc"

. "${common_func_path}"

while true; do
    case "$1" in
    --install-path=*)
        install_path=$(echo "$1" | cut -d"=" -f2-)
        shift
        ;;
    --version-dir=*)
        version_dir=$(echo "$1" | cut -d"=" -f2)
        shift
        ;;
    --latest-dir=*)
        latest_dir=$(echo "$1" | cut -d"=" -f2)
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

get_arch_name() {
    local pkg_dir="$1"
    local scene_file="$pkg_dir/scene.info"
    grep '^arch=' $scene_file | cut -d"=" -f2
}

create_stub_softlink() {
    local stub_dir="$1"
    if [ ! -d "$stub_dir" ]; then
        return
    fi
    local arch_name="$2"
    local pwdbak="$(pwd)"
    cd $stub_dir && [ -d "$arch_name" ] && for so_file in $(find "$arch_name" -type f -o -type l); do
        ln -sf "$so_file" "$(basename $so_file)"
    done
    [ -d "linux/x86_64" ] && ln -snf "linux/x86_64" "x86_64"
    [ -d "linux/aarch64" ] && ln -snf "linux/aarch64" "aarch64"
    cd $pwdbak
}

do_create_stub_softlink() {
    local arch_name="$(get_arch_name $install_path/$version_dir/share/info/asc-devkit)"
    local arch_linux_path="$install_path/$latest_dir/$arch_name-linux"
    if [ ! -e "$arch_linux_path" ] || [ -L "$arch_linux_path" ]; then
        return
    fi
    create_stub_softlink "$arch_linux_path/devlib" "linux/$arch_name"
    create_stub_softlink "$arch_linux_path/lib64/stub" "linux/$arch_name"
    if [ -d "$install_path/$version_dir/tools" ]; then
        if [ ! -d "$install_path/$latest_dir/tools/ascendc_tools" ]; then
            mkdir -p "$install_path/$latest_dir/tools/ascendc_tools"
        fi
    fi
    if [ ! -d "$install_path/$version_dir/compiler/bin" ]; then
        mkdir -p "$install_path/$version_dir/compiler/bin"
    fi
    if [ ! -d "$install_path/$latest_dir/compiler/bin" ]; then
        mkdir -p "$install_path/$latest_dir/compiler/bin"
    fi
    if [ ! -d "$install_path/$latest_dir/tools/opbuild" ]; then
        ln -sr "$install_path/$version_dir/tools/opbuild" "$install_path/$latest_dir/tools/opbuild"
    fi
    if [ -e "$install_path/$version_dir/${arch_name}-linux/bin/asc_opc" ]; then
        if [ ! -L "$install_path/$version_dir/compiler/bin/asc_opc" ]; then
            ln -sr "$install_path/$version_dir/${arch_name}-linux/bin/asc_opc" "$install_path/$version_dir/compiler/bin/asc_opc"
        fi
        if [ ! -L "$install_path/$latest_dir/compiler/bin/asc_opc" ]; then
            ln -sr "$install_path/$version_dir/${arch_name}-linux/bin/asc_opc" "$install_path/$latest_dir/compiler/bin/asc_opc"
        fi
    fi

    if [ -d "$arch_linux_path/pkg_inc/asc/hccl" ]; then
        chmod 750 "$arch_linux_path/pkg_inc/asc/hccl"
        ln -sr "$arch_linux_path/asc/include/adv_api/hccl/internal" "$arch_linux_path/pkg_inc/asc/hccl/internal"
        chmod -R 550 "$arch_linux_path/pkg_inc/asc/hccl"
    fi
}

do_create_stub_softlink

python_dir_chmod_reset() {
    local dir="$1"
    if [ ! -d "$dir" ]; then
        return
    fi
    chmod u+w "$dir" > /dev/null 2>&1
}

WHL_INSTALL_DIR_PATH="$install_path/$version_dir/python/site-packages"
WHL_SOFTLINK_INSTALL_DIR_PATH="$install_path/$latest_dir/python/site-packages"

mkdir -p "$WHL_SOFTLINK_INSTALL_DIR_PATH"
python_dir_chmod_reset "$WHL_SOFTLINK_INSTALL_DIR_PATH"

create_softlink_if_exists "$WHL_INSTALL_DIR_PATH" "$WHL_SOFTLINK_INSTALL_DIR_PATH" "asc_op_compile_base"
create_softlink_if_exists "$WHL_INSTALL_DIR_PATH" "$WHL_SOFTLINK_INSTALL_DIR_PATH" "asc_op_compile_base-*.dist-info"
create_softlink_if_exists "$WHL_INSTALL_DIR_PATH" "$WHL_SOFTLINK_INSTALL_DIR_PATH" "asc_opc_tool"
create_softlink_if_exists "$WHL_INSTALL_DIR_PATH" "$WHL_SOFTLINK_INSTALL_DIR_PATH" "asc_opc_tool-*.dist-info"
# hccl python whl 软链接
create_softlink_if_exists "${WHL_INSTALL_DIR_PATH}" "$WHL_SOFTLINK_INSTALL_DIR_PATH" "hccl"
create_softlink_if_exists "${WHL_INSTALL_DIR_PATH}" "$WHL_SOFTLINK_INSTALL_DIR_PATH" "hccl-*.dist-info"

python_dir_chmod_set() {
    local dir="$1"
    if [ ! -d "$dir" ]; then
        return
    fi
    if [ $(id -u) -eq 0 ]; then
        chmod 755 "$dir" > /dev/null 2>&1
    else
        chmod 750 "$dir" > /dev/null 2>&1
    fi
}

python_dir_chmod_set "$WHL_SOFTLINK_INSTALL_DIR_PATH"
python_dir_chmod_set "$(dirname $WHL_SOFTLINK_INSTALL_DIR_PATH)"
