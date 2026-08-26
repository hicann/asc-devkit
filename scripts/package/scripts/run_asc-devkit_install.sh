#!/bin/sh
# Perform install for asc-devkit package
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
#
# The code snippet comes from Huawei's open-source Ascend project.
# Copyright 2019-2020 Huawei Technologies Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
# ----------------------------------------------------------------------------------------------------------

username="$(id -un)"
usergroup="$(id -gn)"
is_quiet=n
in_install_for_all=n
setenv_flag=n
docker_root=""
curpath=$(dirname $(readlink -f "$0"))
common_func_path="${curpath}/common_func.inc"
pkg_version_path="${curpath}/../version.info"
chip_type="all"
feature_type="all"

. "${common_func_path}"

if [ "$1" ]; then
    input_install_dir="${2}"
    common_parse_type="${3}"
    is_quiet="${4}"
    setenv_flag="${5}"
    docker_root="${6}"
    in_install_for_all="${7}"
    pkg_version_dir="${8}"
fi

if [ "x${docker_root}" != "x" ]; then
    common_parse_dir="${docker_root}${input_install_dir}"
else
    common_parse_dir="${input_install_dir}"
fi

get_version "pkg_version" "$pkg_version_path"
is_multi_version_pkg "pkg_is_multi_version" "$pkg_version_path"
if [ "$pkg_is_multi_version" = "true" ] && [ "$hetero_arch" != "y" ]; then
    common_parse_dir="$common_parse_dir/$pkg_version_dir"
fi

if [ $(id -u) -ne 0 ]; then
    log_dir="${HOME}/var/log/ascend_seclog"
else
    log_dir="/var/log/ascend_seclog"
fi
logfile="${log_dir}/ascend_install.log"

get_install_param() {
    local _key="$1"
    local _file="$2"
    local _param=""

    if [ ! -f "${_file}" ]; then
        exit 1
    fi
    local install_info_key_array="Asc-Devkit_Install_Type Asc-Devkit_Chip_Type Asc-Devkit_Feature_Type Asc-Devkit_UserName Asc-Devkit_UserGroup Asc-Devkit_Install_Path_Param Asc-Devkit_Arch_Linux_Path Asc-Devkit_Hetero_Arch_Flag"
    for key_param in ${install_info_key_array}; do
        if [ "${key_param}" = "${_key}" ]; then
            _param=$(grep -i "${_key}=" "${_file}" | cut -d"=" -f2-)
            break
        fi
    done
    echo "${_param}"
}

install_info="${common_parse_dir}/share/info/asc-devkit/ascend_install.info"
if [ -f "$install_info" ]; then
    chip_type=$(get_install_param "Asc-Devkit_Chip_Type" "${install_info}")
    feature_type=$(get_install_param "Asc-Devkit_Feature_Type" "${install_info}")
fi

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

# 静默模式日志打印
new_echo() {
    local log_type="$1"
    local log_msg="$2"
    if [ "${is_quiet}" = "n" ]; then
        echo "${log_type}" "${log_msg}" 1> /dev/null
    fi
}

output_progress() {
    new_echo "INFO" "asc-devkit install upgradePercentage:$1%"
    log "INFO" "asc-devkit install upgradePercentage:$1%"
}

create_latest_linux_softlink() {
    if [ "$pkg_is_multi_version" = "true" ] && [ "$hetero_arch" = "y" ]; then
        local linux_path="$(realpath $common_parse_dir/..)"
        local arch_path="$(basename $linux_path)"
        local latest_path="$(realpath $linux_path/../..)/latest"
        if [ -d "$latest_path" ]; then
            if [ ! -e "$latest_path/$arch_path" ] || [ -L "$latest_path/$arch_path" ]; then
                ln -srfn "$linux_path" "$latest_path"
            fi
        fi
    fi
}

##########################################################################
log "INFO" "step into run_asc-devkit_install.sh ......"
log "INFO" "install target dir $common_parse_dir, type $common_parse_type."

if [ ! -d "$common_parse_dir" ]; then
    log "ERROR" "ERR_NO:0x0001;ERR_DES:path $common_parse_dir is not exist."
    exit 1
fi

new_install() {
    # if [ ! -d "${sourcedir}" ]; then
    #     log "INFO" "no need to install asc-devkit files."
    #     return 0
    # fi
    output_progress 10

    local setenv_option=""
    if [ "${setenv_flag}" = y ]; then
        setenv_option="--setenv"
    fi

    if [ -d "$common_parse_dir/tools/ascendc_tools" ];then
        chmod 755 "$common_parse_dir/tools/ascendc_tools"
    fi

    # 执行安装
    custom_options="--custom-options=--common-parse-dir=$common_parse_dir,--logfile=$logfile,--stage=install,--quiet=$is_quiet,--hetero-arch=$hetero_arch"
    sh "$curpath/install_common_parser.sh" --copy_all --package="asc-devkit" --install --username="$username" --usergroup="$usergroup" --set-cann-uninstall \
        --version=$pkg_version --version-dir=$pkg_version_dir --use-share-info \
        $setenv_option $in_install_for_all --docker-root="$docker_root" --chip="$chip_type" --feature="$feature_type" \
        $custom_options "$common_parse_type" "$input_install_dir" "$curpath/filelist.csv"
    if [ $? -ne 0 ]; then
        log "ERROR" "ERR_NO:0x0085;ERR_DES:failed to install package."
        return 1
    fi

    if [ -d "$common_parse_dir/tools/ascendc_tools" ];then
        chmod 555 "$common_parse_dir/tools/ascendc_tools"
    fi

    create_latest_linux_softlink
    return 0
}

new_install
if [ $? -ne 0 ]; then
    exit 1
fi

output_progress 100
exit 0
