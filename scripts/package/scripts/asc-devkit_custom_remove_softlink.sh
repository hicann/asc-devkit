#!/bin/sh
# Perform custom remove softlink script for asc-devkit package
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

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
    grep '^arch=' "$scene_file" | cut -d"=" -f2
}

remove_stub_softlink() {
    local ref_dir="$1"
    if [ ! -d "$ref_dir" ]; then
        return
    fi
    local stub_dir="$2"
    if [ ! -d "$stub_dir" ]; then
        return
    fi
    local pwdbak="$(pwd)"
    cd "$stub_dir" && chmod u+w .
    find "$ref_dir" -mindepth 1 -maxdepth 1 -exec basename {} \; | while IFS= read -r item; do
        rm -rf -- "$item"
    done
    [ -L "x86_64" ] && rm -rf -- "x86_64"
    [ -L "aarch64" ] && rm -rf -- "aarch64"
    cd "$pwdbak"
}

do_remove_stub_softlink() {
    local arch_name="$(get_arch_name "$install_path/$version_dir/share/info/asc-devkit")"
    local arch_linux_path="$install_path/$latest_dir/$arch_name-linux"
    if [ ! -e "$arch_linux_path" ] || [ -L "$arch_linux_path" ]; then
        return
    fi
    local ref_dir="$install_path/$version_dir/share/info/asc-devkit/lib64/stub/linux/$arch_name"
    remove_stub_softlink "$ref_dir" "$arch_linux_path/devlib"
    remove_stub_softlink "$ref_dir" "$arch_linux_path/lib64/stub"
    if [ -d "$install_path/$latest_dir/tools/ascendc_tools" ]; then
        if [ -z "$(ls -A "$install_path/$latest_dir/tools/ascendc_tools")" ]; then
            rm -f "$install_path/$latest_dir/tools/ascendc_tools/"
        fi
    fi

    if [ -L "$install_path/$version_dir/compiler/bin/asc_opc" ]; then
        rm "$install_path/$version_dir/compiler/bin/asc_opc"
    fi
    if [ -z "$(ls -A "$install_path/$version_dir/compiler/bin")" ]; then
        rm -r "$install_path/$version_dir/compiler/bin/"
    fi
    if [ -z "$(ls -A "$install_path/$version_dir/compiler")" ]; then
        rm -r "$install_path/$version_dir/compiler/"
    fi

    if [ -L "$install_path/$latest_dir/compiler/bin/asc_opc" ]; then
        rm "$install_path/$latest_dir/compiler/bin/asc_opc"
    fi
    if [ -z "$(ls -A "$install_path/$latest_dir/compiler/bin")" ]; then
        rm -r "$install_path/$latest_dir/compiler/bin/"
    fi
    if [ -z "$(ls -A "$install_path/$latest_dir/compiler")" ]; then
        rm -r "$install_path/$latest_dir/compiler/"
    fi
    if [ -d "$install_path/$latest_dir/tools/opbuild" ]; then
        rm -f "$install_path/$latest_dir/tools/opbuild"
    fi

    if [ -d "$arch_linux_path/pkg_inc/asc/hccl" ]; then
        rm -rf "$arch_linux_path/pkg_inc/asc"
    fi
}

do_remove_stub_softlink

python_dir_chmod_set() {
    local dir="$1"
    if [ ! -d "$dir" ]; then
        return
    fi
    chmod u+w "$dir" > /dev/null 2>&1
}

remove_softlink() {
    local pattern="$1"
    local _path
    for _path in "$WHL_SOFTLINK_INSTALL_DIR_PATH"/$pattern; do
        if [ -e "$_path" ] || [ -L "$_path" ]; then
            rm -rf -- "$_path" > /dev/null 2>&1
        fi
    done
}

remove_empty_dir() {
    local _path="$1"
    if [ -d "${_path}" ]; then
        local is_empty=$(ls "${_path}" | wc -l)
        if [ "$is_empty" -eq 0 ]; then
            prev_path=$(dirname "${_path}")
            chmod +w "${prev_path}" > /dev/null 2>&1
            rm -rf "${_path}" > /dev/null 2>&1
        fi
    fi
}

WHL_SOFTLINK_INSTALL_DIR_PATH="$install_path/$latest_dir/python/site-packages"

python_dir_chmod_set "$WHL_SOFTLINK_INSTALL_DIR_PATH"

remove_softlink "asc_op_compile_base"
remove_softlink "asc_op_compile_base-*.dist-info"
remove_softlink "asc_opc_tool"
remove_softlink "asc_opc_tool-*.dist-info"
# hccl python whl 软链接
remove_softlink "hccl"
remove_softlink "hccl-*.dist-info"

remove_empty_dir "$WHL_SOFTLINK_INSTALL_DIR_PATH"
remove_empty_dir "$install_path/$latest_dir/python"
remove_empty_dir "$install_path/$latest_dir"
