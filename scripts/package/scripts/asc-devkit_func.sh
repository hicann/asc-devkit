#!/bin/sh
# Perform common functions for asc-devkit package
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

# Release 9.1.0 incorrectly claimed HCCL's header in the installed filelist.
# Run in a subshell so cleanup traps cannot affect the installer calling us.
# Keep cleanup state in subshell variables: some shells unwind locals before EXIT.
protect_hccl_header_from_legacy_filelist() (
    info_dir="$1"
    filelist="$info_dir/script/filelist.csv"
    script_dir="$info_dir/script"
    tmp_file=""
    dir_mode=""
    file_mode=""
    removed=""
    header_record='($2 == "copy" || $2 == "copy_entity" || $2 == "move") && $4 ~ /(^|\/)include\/hccl\/hccl\.h$/'

    [ -d "$info_dir/../hccl" ] && [ -f "$filelist" ] || return 0
    removed=$(awk -F, "$header_record { n++ } END { print n+0 }" "$filelist") || {
        log "ERROR" "Failed to read legacy filelist: $filelist"
        echo "Failed to read legacy filelist: $filelist" >&2
        return 1
    }
    [ "$removed" -ne 0 ] || return 0

    # Restore the directory mode and discard incomplete output on every exit.
    trap '
        ret=$?
        if [ -n "$tmp_file" ]; then
            rm -f "$tmp_file" || ret=1
        fi
        if [ -n "$dir_mode" ]; then
            chmod "$dir_mode" "$script_dir" || ret=1
        fi
        if [ "$ret" -ne 0 ]; then
            log "ERROR" "Failed to protect HCCL header in legacy filelist: $filelist"
            echo "Failed to protect HCCL header in legacy filelist: $filelist" >&2
        fi
        exit "$ret"
    ' 0
    trap 'exit 1' 1 2 15

    dir_mode=$(stat -c %a "$script_dir") || exit 1
    file_mode=$(stat -c %a "$filelist") || exit 1
    chmod u+w "$script_dir" || exit 1
    tmp_file=$(mktemp "$script_dir/.filelist.csv.XXXXXX") || exit 1
    # cp -p preserves the installed manifest's ownership as well as its mode.
    cp -p "$filelist" "$tmp_file" || exit 1
    chmod u+w "$tmp_file" || exit 1
    awk -F, -v expected="$removed" "$header_record { n++; next }
        { print }
        END { if (n != expected) exit 1 }" "$filelist" > "$tmp_file" || exit 1
    awk -F, "$header_record { exit 1 }" "$tmp_file" || exit 1
    chmod "$file_mode" "$tmp_file" || exit 1
    mv -f "$tmp_file" "$filelist" || exit 1
    tmp_file=""
    chmod "$dir_mode" "$script_dir" || exit 1
    dir_mode=""
    log "INFO" "Protected HCCL header: removed $removed record(s) from $filelist"
    exit 0
)

# Repair only the current package architecture in the already resolved install root.
# Heterogeneous manifests use ../../<arch>-linux relative to their metadata root.
hccl_header_relative_path() {
    if [ "$2" = "y" ]; then
        echo "../../$1/include/hccl/hccl.h"
    else
        echo "$1/include/hccl/hccl.h"
    fi
}

# Read metadata as data, never source an installed package's configuration.
hccl_install_value() {
    awk -F= -v key="$2" '$1 == key { sub(/^[^=]*=/, ""); gsub(/\r$/, ""); print; exit }' "$1"
}

repair_missing_hccl_header() (
    root="$1"
    payload="$2"
    relative=$(hccl_header_relative_path "$3" "$4")
    target="$root/$relative"
    hccl_info="$root/share/info/hccl"
    [ -d "$hccl_info" ] || exit 0
    # Existing headers are never replaced, regardless of their content.
    [ -f "$target" ] && exit 0
    if [ -e "$target" ] || [ -L "$target" ]; then
        log "ERROR" "Invalid HCCL header target: $target"
        exit 1
    fi
    metadata="$hccl_info/ascend_install.info"
    manifest="$hccl_info/script/filelist.csv"
    [ -r "$metadata" ] && [ -r "$manifest" ] || {
        log "ERROR" "Cannot read HCCL installation metadata: $hccl_info"
        exit 1
    }
    install_type=$(hccl_install_value "$metadata" Hccl_Install_Type) || exit 1
    case "$install_type" in
        full|debug) types='^(all|docker|devel|run)$' ;;
        devel|run) types="^(all|$install_type)$" ;;
        *) log "ERROR" "Unknown HCCL installation type: $install_type"; exit 1 ;;
    esac
    # Match the destination and install-type tokens, not a filename substring.
    record=$(awk -F, -v relative="$relative" -v types="$types" '
        ($2 == "copy" || $2 == "copy_entity" || $2 == "move") &&
        ($4 == relative || $4 == "include/hccl/hccl.h") {
            count = split($8, entries, /[;|]/)
            for (i = 1; i <= count; i++) {
                if (entries[i] ~ types) { print $6 ":" $4; exit }
            }
        }' "$manifest") || exit 1
    [ -n "$record" ] || exit 0
    mode=${record%%:*}
    target="$root/${record#*:}"
    [ -f "$target" ] && exit 0
    if [ -e "$target" ] || [ -L "$target" ]; then
        log "ERROR" "Invalid HCCL header target: $target"
        exit 1
    fi
    # Resolve directory links before writing: shared include may select another arch.
    layout_root="$root"
    [ "$4" != "y" ] || layout_root="$root/../.."
    layout_root=$(cd "$layout_root" && pwd -P) || exit 1
    resolved=$(readlink -m "$target") || exit 1
    expected="$layout_root/$3/include/hccl/hccl.h"
    if [ "${record#*:}" = "include/hccl/hccl.h" ] && [ "$4" != "y" ] &&
        [ ! -L "$root/include" ]; then
        expected="$layout_root/include/hccl/hccl.h"
    fi
    if [ "$resolved" != "$expected" ]; then
        log "ERROR" "HCCL header target escapes its installation architecture: $target -> $resolved"
        exit 1
    fi
    case "$mode" in
        [0-7][0-7][0-7]|[0-7][0-7][0-7][0-7]) ;;
        *) log "ERROR" "Invalid HCCL header permissions: $mode"; exit 1 ;;
    esac
    [ -f "$payload" ] || { log "ERROR" "Missing HCCL repair payload: $payload"; exit 1; }
    target_dir=$(dirname "$target")
    parent_dir=$(dirname "$target_dir")
    dir_mode=""
    parent_mode=""
    temp=""
    trap '
        ret=$?
        [ -z "$temp" ] || rm -f "$temp" || ret=1
        [ -z "$dir_mode" ] || chmod "$dir_mode" "$target_dir" || ret=1
        [ -z "$parent_mode" ] || chmod "$parent_mode" "$parent_dir" || ret=1
        if [ "$ret" -ne 0 ]; then
            log "ERROR" "Failed to repair HCCL header: $target"
        fi
        exit "$ret"
    ' 0
    trap 'exit 1' 1 2 15
    if [ ! -d "$target_dir" ]; then
        parent_mode=$(stat -c %a "$parent_dir") || exit 1
        chmod u+w "$parent_dir" || exit 1
        # Use HCCL's directory record when available; 550 is its legacy default.
        new_dir_mode=$(awk -F, -v path="${record#*:}" -v types="$types" '
            BEGIN { sub(/\/hccl\.h$/, "", path) }
            $2 == "mkdir" && $4 == path {
                count = split($8, entries, /[;|]/)
                for (i = 1; i <= count; i++) {
                    if (entries[i] ~ types) { print $6; exit }
                }
            }' "$manifest") || exit 1
        new_dir_mode=${new_dir_mode:-550}
        case "$new_dir_mode" in
            [0-7][0-7][0-7]|[0-7][0-7][0-7][0-7]) ;;
            *) log "ERROR" "Invalid HCCL directory permissions: $new_dir_mode"; exit 1 ;;
        esac
        mkdir -m "$new_dir_mode" "$target_dir" || exit 1
    fi
    dir_mode=$(stat -c %a "$target_dir") || exit 1
    chmod u+w "$target_dir" || exit 1
    temp=$(mktemp "$target_dir/.hccl.h.XXXXXX") || exit 1
    cp "$payload" "$temp" || exit 1
    chown --reference="$metadata" "$temp" || exit 1
    chmod "$mode" "$temp" || exit 1
    cmp -s "$payload" "$temp" || exit 1
    # Atomic no-replace publication: do not overwrite a concurrently created file.
    if ! ln "$temp" "$target"; then
        [ -f "$target" ] || exit 1
    fi
    log "INFO" "Repaired missing HCCL header: $target"
)

remove_hccl_header_without_hccl() (
    root="$1"
    [ -d "$root/share/info/hccl" ] && exit 0
    relative=$(hccl_header_relative_path "$2" "$3")
    set -- "$root/$relative"
    # A non-prefixed manifest may use a real include directory. Do not follow
    # the shared include symlink into a different installed architecture.
    if [ "$relative" = "${relative#../../}" ] && [ ! -L "$root/include" ]; then
        set -- "$@" "$root/include/hccl/hccl.h"
    fi
    dir_mode=""
    trap 'ret=$?; [ -z "$dir_mode" ] || chmod "$dir_mode" "$target_dir" || ret=1; exit "$ret"' 0
    trap 'exit 1' 1 2 15
    for target in "$@"; do
        [ -e "$target" ] || [ -L "$target" ] || continue
        if [ ! -f "$target" ] && [ ! -L "$target" ]; then
            log "ERROR" "Refusing to remove non-file HCCL header: $target"
            exit 1
        fi
        target_dir=$(dirname "$target")
        dir_mode=$(stat -c %a "$target_dir") || exit 1
        chmod u+w "$target_dir" || exit 1
        rm -f "$target" || exit 1
        chmod "$dir_mode" "$target_dir" || exit 1
        dir_mode=""
        log "INFO" "Removed HCCL header without installed HCCL: $target"
    done
)

get_pkg_arch_name() {
    local scene_info="$curpath/../scene.info"
    if [ ! -f "$scene_info" ]; then
        local cur_date="$(date +'%Y-%m-%d %H:%M:%S')"
        echo "[AscDevkit] [$cur_date] [ERROR]: $scene_info file cannot be found!"
        exit 1
    fi
    local arch="$(grep -iw arch "$scene_info" | cut -d"=" -f2- | awk '{print tolower($0)}')"
    if [ -z "$arch" ]; then
        local cur_date="$(date +'%Y-%m-%d %H:%M:%S')"
        echo "[AscDevkit] [$cur_date] [ERROR]: var arch cannot be found in file $scene_info!"
        exit 1
    fi
    echo $arch
}

get_installed_pkg_arch_name() {
    if [ "$pkg_is_multi_version" = "true" ]; then
        local scene_info="$docker_root$input_install_path/$pkg_version_dir/share/info/asc-devkit/scene.info"
    else
        local scene_info="$docker_root$input_install_path/share/info/asc-devkit/scene.info"
    fi
    if [ ! -f "$scene_info" ]; then
        return
    fi
    local arch="$(grep -iw arch "$scene_info" | cut -d"=" -f2- | awk '{print tolower($0)}')"
    echo $arch
}

get_os_arch_name() {
    uname -m 2>&1 | awk '{print tolower($0)}'
}

get_hetero_arch_name() {
    if [ "x$(get_pkg_arch_name)" = "xx86_64" ]; then
        echo aarch64
    else
        echo x86_64
    fi
}

if [ "$(get_pkg_arch_name)" = "$(get_os_arch_name)" ]; then
    is_pkg_hetero_arch="n"
else
    is_pkg_hetero_arch="y"
fi

arch_linux_path="$(get_pkg_arch_name)-linux"
arch_scripts_path="$arch_linux_path/hetero-arch-scripts"
arch_linux_path_os="$(get_os_arch_name)-linux"
arch_linux_path_hetero="$(get_hetero_arch_name)-linux"
arch_scripts_path_hetero="$arch_linux_path_hetero/hetero-arch-scripts"

is_same_arch_pkg_installed() {
    if [ "$pkg_is_multi_version" = "true" ]; then
        local linux_path="$docker_root$input_install_path/$pkg_version_dir/$arch_linux_path_os"
    else
        local linux_path="$docker_root$input_install_path/$arch_linux_path_os"
    fi
    if [ "$is_pkg_hetero_arch" = "y" ] && [ -d "$linux_path" ]; then
        echo "y"
    else
        echo "n"
    fi
}

get_package_upgrade_install_info_hetero() {
    local _outvar="$1"
    local path_latest="$docker_root$input_install_path/latest"
    local path_hetero="$docker_root$input_install_path/latest/$arch_scripts_path"
    if [ -d "$path_latest" ] && [ -d "$path_hetero" ]; then
        local install_info=$(find "$path_hetero" -type f -name 'ascend_install.info' | grep 'asc-devkit/ascend_install.info')
        if [ -n "$install_info" ]; then
            install_info="$(realpath $install_info)"
            eval "${_outvar}=\"$install_info\""
            return
        fi
    fi
    eval "${_outvar}=\"\""
}

is_hetero_arch_pkg_installed() {
    if [ "$is_pkg_hetero_arch" = "y" ]; then
        if [ "$pkg_is_multi_version" = "true" ]; then
            local path_version="$docker_root$input_install_path/$pkg_version_dir/$arch_scripts_path/share/info/asc-devkit/version.info"
            local path_install="$docker_root$input_install_path/$pkg_version_dir/$arch_scripts_path/share/info/asc-devkit/ascend_install.info"
        else
            local path_version="$docker_root$input_install_path/$arch_scripts_path/share/info/asc-devkit/version.info"
            local path_install="$docker_root$input_install_path/$arch_scripts_path/share/info/asc-devkit/ascend_install.info"
        fi
        if [ -f "$path_version" ] && [ -f "$path_install" ]; then
            echo "installed-hetero"
            return
        fi

        if [ "$pkg_is_multi_version" = "true" ]; then
            local path_version="$docker_root$input_install_path/$pkg_version_dir/share/info/asc-devkit/version.info"
            local path_install="$docker_root$input_install_path/$pkg_version_dir/share/info/asc-devkit/ascend_install.info"
        else
            local path_version="$docker_root$input_install_path/share/info/asc-devkit/version.info"
            local path_install="$docker_root$input_install_path/share/info/asc-devkit/ascend_install.info"
        fi
        if [ -f "$path_version" ] && [ -f "$path_install" ] && [ "$(get_installed_pkg_arch_name)" = "$(get_pkg_arch_name)" ]; then
            echo "installed-normal"
            return
        fi

        if [ "$pkg_is_multi_version" = "true" ]; then
            get_package_upgrade_install_info_hetero "ascend_install_info"
            if [ -f "${ascend_install_info}" ]; then
                echo "installed-hetero-to-be-upgraded"
                return
            else
                get_package_upgrade_install_info "ascend_install_info" "$docker_root$input_install_path" "asc-devkit"
                if [ -f "${ascend_install_info}" ]; then
                    echo "installed-normal-to-be-upgraded"
                    return
                fi
            fi
        fi

        echo "no"
    else
        echo "NA"
    fi
}

update_install_info_hetero() {
    local install_info=$1
    local pkg_version_dir=$2
    if [ -f "$install_info" ]; then
        chmod u+w "$(dirname $install_info)" "$install_info"
        local new_install_path="$(grep -iw Asc-Devkit_Install_Path_Param $install_info)"
        local new_install_path="$(echo $new_install_path | awk -v version=$pkg_version_dir 'BEGIN{FS=OFS="/"} {$(NF-2)=version; print}')"
        sed -i -e "s:Asc-Devkit_Install_Path_Param=.*:$new_install_path:" "$install_info"
    fi
}

replace_filelist() {
    local curpath=$(dirname $(readlink -f "$0"))
    cp -p $curpath/filelist.csv $curpath/filelist.csv.bak && awk -v arch_prefix="$arch_linux_path" 'BEGIN {
        FS=OFS=","
        pat="^"arch_prefix
    }

    function in_pkg_black_list() {
        relative_install_path = $4
        if (relative_install_path ~ /^asc-devkit\/python/) { return 1 }
        return 0
    }

    function format_softlink(links) {
        split(links, arr, ";")
        ret = ""
        for (i in arr) {
            if (arr[i] ~ pat) { arr[i] = "../../" arr[i] }
            ret = ret ";" arr[i]
        }
        sub("^;", "", ret)
        return ret
    }

    {
        if (in_pkg_black_list()) { next }
        if ($4 ~ pat) {
            if ($4 ~ pat"/python") { $15 = "NA" }
            $4 = "../../" $4;
        }
        $9 = format_softlink($9)
        print
    }' $curpath/filelist.csv.bak > $curpath/filelist.csv
}

get_dir_mod() {
    local path="$1"
    stat -c %a "$path"
}

remove_dir_recursive() {
    local dir_start="$1"
    local dir_end="$2"
    if [ "$dir_end" = "$dir_start" ]; then
        return 0
    fi
    if [ ! -e "$dir_end" ]; then
        return 0
    fi
    if [ "x$(ls -A $dir_end 2>&1)" != "x" ]; then
        return 0
    fi
    local up_dir="$(dirname $dir_end)"
    local oldmod="$(get_dir_mod $up_dir)"
    chmod u+w "$up_dir"
    [ -n "$dir_end" ] && rm -rf "$dir_end"
    if [ $? -ne 0 ]; then
        chmod "$oldmod" "$up_dir"
        return 1
    fi
    chmod "$oldmod" "$up_dir"
    remove_dir_recursive "$dir_start" "$up_dir"
}

get_pkg_toolchain() {
    local scene_info="$curpath/../scene.info"
    grep -iw ^toolchain "$scene_info" 2> /dev/null | cut -d"=" -f2-
}

get_stub_libs_from_filelist() {
    awk -v arch_name="$arch_name" 'BEGIN {
        FS=","
        prefix=sprintf("^%s-linux/devlib/", arch_name)
        pat=sprintf("^%s-linux/devlib/(linux/%s/[^/]+\\.(so|a)$)", arch_name, arch_name)
    }
    {
        if (match($4, pat)) {
            sub(prefix, "", $4)
            print($4)
        }
    }' $curpath/filelist.csv
}

create_stub_softlink() {
    local install_path="$1"
    if [ ! -d "$install_path" ]; then
        return
    fi
    local arch_name="$pkg_arch_name"
    ([ -d "$install_path/${arch_name}-linux/devlib" ] && cd "$install_path/${arch_name}-linux/devlib" && {
        chmod u+w . && \
        for lib in $(get_stub_libs_from_filelist); do
            [ -f "$lib" ] && ln -sf "$lib" "$(basename $lib)"
        done
        chmod u-w .
    })
}

remove_stub_softlink() {
    local install_path="$1"
    if [ ! -d "$install_path" ]; then
        return
    fi
    local arch_name="$pkg_arch_name"
    ([ -d "$install_path/${arch_name}-linux/devlib" ] && cd "$install_path/${arch_name}-linux/devlib" && {
        basename --multiple $(get_stub_libs_from_filelist) | xargs --no-run-if-empty rm -rf
    })
}

pkg_arch_name="$(get_pkg_arch_name)"
