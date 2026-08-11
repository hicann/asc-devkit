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

set -e

SUPPORTED_SHORT_OPTS=("h" "j" "t" "p" "f")
SUPPORTED_LONG_OPTS=(
    "help" "cov" "cache" "pkg" "pkg-type" "asan" "make_clean" "cann_3rd_lib_path" "test" "cann_path" "adv_test" "adv_test_two" "arm_test" "basic_test_one" "basic_test_two" "basic_test_three" "basic_test_four" "basic_test_five" "build-type" "extra-cmake-args" "changed_file" "enable-sign" "sign-script"
)

CURRENT_DIR=$(dirname $(readlink -f ${BASH_SOURCE[0]}))
BUILD_DIR=${CURRENT_DIR}/build
OUTPUT_DIR=${CURRENT_DIR}/build_out
CANN_3RD_LIB_PATH=${BUILD_DIR}
USER_ID=$(id -u)
CPU_NUM=$(($(cat /proc/cpuinfo | grep "^processor" | wc -l)))
THREAD_NUM=32
BUILD_TYPE="Release"
PACKAGE_TYPE="run"
PACKAGE_TYPE_SET="false"
ENABLE_BUILD_DEVICE=ON
USE_CXX11_ABI=0
CMAKE_TOOLCHAIN_FILE_VAL=""
CHANGED_FILES=""
CI_MODE=FALSE
ENABLE_SIGN="false"
CUSTOM_SIGN_SCRIPT=""

dotted_line="----------------------------------------------------------------"

function log() {
    local current_time=$(date +"%Y-%m-%d %H:%M:%S")
    echo "[$current_time] "$1
}

usage() {
  local specific_help="$1"

  if [[ -n "$specific_help" ]]; then
    case "$specific_help" in
      package)
        echo "Package Build Options:"
        echo $dotted_line
        echo "    --pkg                Compile package"
        echo "    --pkg-type=<TYPE>    Specify package type (TYPE options: run/rpm/deb/all or comma-separated values, eg: deb,rpm), Default: run"
        echo "                         all builds run, rpm and deb packages"
        echo "    -p, --cann_path      Set the cann package installation directory, eg: /usr/local/Ascend/cann
                         Default: /usr/local/Ascend/cann (root) or \$HOME/Ascend/cann (non-root)"
        echo "    -j                   Compile thread nums, default is 32, eg: -j 8
                         Adjusted to CPU core count if it exceeds the core count"
        echo "    --cann_3rd_lib_path  Set the path for third-party library dependencies, eg: ./build"
        echo "    --asan               Enable ASAN (address Sanitizer)"
        echo $dotted_line
        echo "Examples:"
        echo "    bash build.sh --pkg -j 8"
        echo "    bash build.sh --pkg --pkg-type=rpm -j 8"
        echo "    bash build.sh --pkg --pkg-type=deb -j 8"
        echo "    bash build.sh --pkg --pkg-type=deb,rpm -j 8"
        echo "    bash build.sh --pkg --pkg-type=all -j 8"
        echo "    bash build.sh --pkg --asan -j 32"
        return
        ;;
      test)
        echo "Test Options:"
        echo $dotted_line
        echo "    -t, --test           Build and run all unit tests"
        echo "    -p, --cann_path      Set the cann package installation directory, eg: /usr/local/Ascend/cann
                         Default: /usr/local/Ascend/cann (root) or \$HOME/Ascend/cann (non-root)"
        echo "    -j                   Compile thread nums, default is 32, eg: -j 8
                         Adjusted to CPU core count if it exceeds the core count"
        echo "    --adv_test            Build and run the adv part of unit tests"
        echo "    --adv_test_two        Build and run the adv_test_two part of unit tests"
        echo "    --arm_test            Build and run the arm part of unit tests"
        echo "    --basic_test_one      Build and run the basic_one part of unit tests"
        echo "    --basic_test_two      Build and run the basic_two part of unit tests"
        echo "    --basic_test_three    Build and run the basic_three part of unit tests"
        echo "    --basic_test_four     Build and run the basic_four part of unit tests"
        echo "    --basic_test_five     Build and run the basic_five part of unit tests"
        echo "    --cann_3rd_lib_path  Set the path for third-party library dependencies, eg: ./build"
        echo "    --cov                Enable code coverage for unit tests"
        echo "    --asan               Enable ASAN (address Sanitizer)"
        echo $dotted_line
        echo "Examples:"
        echo "    bash build.sh -t --cov"
        echo "    bash build.sh --test_part --asan -j 32"
        return
        ;;
      clean)
        echo "Clean Options:"
        echo $dotted_line
        echo "    --make_clean         Clean build artifacts"
        echo $dotted_line
        echo "Examples:"
        echo "    bash build.sh -t --make_clean"
        return
        ;;
    esac
  fi

  echo "build script for asc-devkit repository"
  echo "Usage: bash build.sh [OPTION]..."
  echo ""
  echo "    The following are all supported arguments:"
  echo $dotted_line
  echo "    -h, --help           Display help information"
  echo "    -j                   Compile thread nums, default is 32, eg: -j 8
                         Adjusted to CPU core count if it exceeds the core count"
  echo "    -t, --test           Build and run all unit tests"
  echo "    -p, --cann_path      Set the cann package installation directory, eg: /usr/local/Ascend/cann
                         Default: /usr/local/Ascend/cann (root) or \$HOME/Ascend/cann (non-root)"
  echo "    --adv_test            Build and run the adv part of unit tests"
  echo "    --adv_test_two        Build and run the adv_test_two part of unit tests"
  echo "    --arm_test            Build and run the arm part of unit tests"
  echo "    --basic_test_one      Build and run the basic_one part of unit tests"
  echo "    --basic_test_two      Build and run the basic_two part of unit tests"
  echo "    --basic_test_three    Build and run the basic_three part of unit tests"
  echo "    --basic_test_four     Build and run the basic_four part of unit tests"
  echo "    --basic_test_five     Build and run the basic_five part of unit tests"
  echo "    --pkg                Compile package"
  echo "    --pkg-type=<TYPE>    Specify package type (TYPE options: run/rpm/deb/all or comma-separated values, eg: deb,rpm), Default: run"
  echo "                         all builds run, rpm and deb packages"
  echo "    --cann_3rd_lib_path  Set the path for third-party library dependencies, eg: ./build"
  echo "    --cov                Enable code coverage for unit tests"
  echo "    --asan               Enable ASAN (address Sanitizer)"
  echo "    --cache              Use compiler launcher cache program, eg: --cache ccache"
  echo "    --make_clean         Clean build artifacts"
  echo "    --build-type=<TYPE>"
  echo "                         Specify build type (TYPE options: Release/Debug), Default:Release"
  echo "    -f, --changed_file   Set the changed files for CI mode, eg: -f change_file.txt"
}

parse_cmake_extra_args() {
    echo "Parse cmake extra args."
    # para check
    local args_str="$1"
    if [[ -z "$args_str" ]]; then
        echo "The parsed parameter string is empty."
        return 0
    fi

    IFS=',' read -ra kv_pairs <<< "$args_str"

    for kv_pair in "${kv_pairs[@]}"; do
        if [[ -z "$kv_pair" ]]; then
            continue
        fi

        local key="${kv_pair%%=*}"
        local value="${kv_pair#*=}"

        case "$key" in
            "ENABLE_BUILD_DEVICE")
                ENABLE_BUILD_DEVICE="$value"
                echo "Set ENABLE_BUILD_DEVICE to ${ENABLE_BUILD_DEVICE}."
                ;;
            "USE_CXX11_ABI")
                USE_CXX11_ABI="$value"
                echo "Set USE_CXX11_ABI to ${USE_CXX11_ABI}."
                ;;
            "CMAKE_TOOLCHAIN_FILE")
                CMAKE_TOOLCHAIN_FILE_VAL=$(realpath -s "$value")
                echo "Set CMAKE_TOOLCHAIN_FILE_VAL to ${CMAKE_TOOLCHAIN_FILE_VAL}."
                ;;
            *)
                echo "invalid parameter key: $key"
                ;;
        esac
    done

    if [[ "X$(echo "$USE_CXX11_ABI" | tr '[:upper:]' '[:lower:]')" == "xon" || "$USE_CXX11_ABI" == "1" ]]; then
      USE_CXX11_ABI=1
    elif [[ "X$(echo "$USE_CXX11_ABI" | tr '[:upper:]' '[:lower:]')" == "xoff" || "$USE_CXX11_ABI" == "0" ]]; then
      USE_CXX11_ABI=0
    fi
}

check_option_validity() {
  local arg="$1"

  if [[ "$arg" =~ "=" ]]; then
    arg="${arg%%=*}"
  fi

  if [[ "$arg" =~ ^-[^-] ]]; then
    if [[ $arg =~ ^-j[0-9]+$ ]]; then
      local jobs="${arg#-j}"
      if [[ "$jobs" -le 0 ]]; then
        log "[ERROR] -j only support positive integers."
        return 1
      fi
      return 0
    fi

    if [[ ! " ${SUPPORTED_SHORT_OPTS[@]} " =~ " ${arg:1} " ]]; then
      log "[ERROR] Invalid short option: ${arg}"
      return 1
    fi
  fi

  if [[ "$arg" =~ ^-- ]]; then
    if [[ ! " ${SUPPORTED_LONG_OPTS[@]} " =~ " ${arg:2} " ]]; then
      log "[ERROR] Invalid long option: ${arg}"
      return 1
    fi
  fi
  return 0
}

require_option_value() {
  local opt_name="$1"
  local opt_value="$2"

  if [[ -z "$opt_value" || "$opt_value" == -* ]]; then
    log "[ERROR] ${opt_name} requires a value."
    exit 1
  fi
}

check_help_combinations() {
  local args=("$@")
  local has_test=false
  local test_part=''
  local has_cov=false
  local has_pkg=false

  for arg in "${args[@]}"; do
    case "$arg" in
      -t|--test) has_test=true ;;
      --adv_test) test_part="adv_test" ;;
      --adv_test_two) test_part="adv_test_two" ;;
      --arm_test) test_part="arm_test" ;;
      --basic_test_one) test_part="basic_test_one" ;;
      --basic_test_two) test_part="basic_test_two" ;;
      --basic_test_three) test_part="basic_test_three" ;;
      --basic_test_four) test_part="basic_test_four" ;;
      --basic_test_five) test_part="basic_test_five" ;;
      --cov) has_cov=true ;;
      --pkg) has_pkg=true ;;
      -h|--help) ;;
    esac
  done

  if [[ "$has_test" == "true" && -n "$test_part" ]]; then
    log "[ERROR] --$test_part cannot be used with test(-t, --test)."
    return 1
  fi
  if [[ ("$has_test" == "true" || -n "$test_part") && "$has_pkg" == "true" ]]; then
    log "[ERROR] --pkg cannot be used with test(-t, --test, --$test_part)."
    return 1
  fi
  if [[ "$has_cov" == "true" && "$has_test" != "true" && -z "$test_part" ]]; then
    log "[ERROR] --cov must be used with test(-t, --test) or a test part option."
    return 1
  fi
  return 0
}

check_param_with_help() {
  for arg in "$@"; do
    if [[ "$arg" =~ ^- ]]; then
      if ! check_option_validity "$arg"; then
        log "[INFO] Use 'bash build.sh --help' for more information."
        exit 1
      fi
    fi
  done

  seen=()
  for arg in "$@"; do
    arg="${arg%%=*}"
    if [[ "$arg" =~ ^-j[0-9]+$ ]]; then
      arg="-j"
    fi
    if [[ " ${seen[@]} " =~ " $arg " ]]; then
      log "[ERROR] $arg can only be input one."
      exit 1
    fi
    seen+=("$arg")
  done

  for arg in "$@"; do
    if [[ "$arg" == "--help" || "$arg" == "-h" ]]; then
      # 检查帮助信息中的组合参数
      check_help_combinations "$@"
      local comb_result=$?
      if [ $comb_result -eq 1 ]; then
        exit 1
      fi
      SHOW_HELP="general"

      for prev_arg in "$@"; do
        case "$prev_arg" in
          --pkg) SHOW_HELP="package" ;;
          -t|--test) SHOW_HELP="test" ;;
          --adv_test) SHOW_HELP="test" ;;
          --adv_test_two) SHOW_HELP="test" ;;
          --arm_test) SHOW_HELP="test" ;;
          --basic_test_one) SHOW_HELP="test" ;;
          --basic_test_two) SHOW_HELP="test" ;;
          --basic_test_three) SHOW_HELP="test" ;;
          --basic_test_four) SHOW_HELP="test" ;;
          --basic_test_five) SHOW_HELP="test" ;;
          --make_clean) SHOW_HELP="clean" ;;
        esac
      done

      usage "$SHOW_HELP"
      exit 0
    fi
  done
}

check_param_j() {
  if [[ ! $THREAD_NUM =~ ^[0-9]+$ || "$THREAD_NUM" -le 0 ]]; then
   log "[ERROR] -j only support positive integers."
   exit 1
  fi

  if [[ "$THREAD_NUM" -gt "$CPU_NUM" ]]; then
    log "[WARNING] compile thread num:$THREAD_NUM over core num:$CPU_NUM, adjust to core num."
    THREAD_NUM=$CPU_NUM
  fi
}

check_param_clean() {
  if [[ "$#" -gt 1 || ( "$#" -eq 2 && $has_h == "true" ) ]]; then
    log "[ERROR] --make_clean must be used separately."
    exit 1
  fi
}

check_param_test_pkg() {
  if [[ "$TEST" == "all" && "$PKG" == "true" ]]; then
    log "[ERROR] --pkg cannot be used with test(-t, --test)."
    exit 1
  fi
}

check_param_test_part() {
  if [[ "$TEST" == "all" && -n "$TEST_PART" ]]; then
    log "[ERROR] --$TEST_PART cannot be used with test(-t, --test)."
    exit 1
  fi
}

check_param_test_build_type() {
  if [[ "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "Debug" ]]; then
    log "[ERROR] --build-type must be Release or Debug."
    exit 1
  fi
  if [[ "$TEST" == "all" || -n "$TEST_PART" ]]; then
    log "[ERROR] --build-type cannot be used with test(-t, --test) or --$TEST_PART."
    exit 1
  fi
}

check_param_pkg_type() {
  case "${PACKAGE_TYPE}" in
    run|rpm|deb|all)
      ;;
    deb,rpm|rpm,deb)
      PACKAGE_TYPE="deb,rpm"
      ;;
    *)
      log "[ERROR] --pkg-type must be run, rpm, deb, all or the combination deb,rpm."
      exit 1
      ;;
  esac
}

check_param_pkg_type_usage() {
  if [[ "$PACKAGE_TYPE_SET" == "true" && "$PKG" != "true" ]]; then
    log "[ERROR] --pkg-type must be used with --pkg."
    exit 1
  fi
}

check_param_cov() {
  if [[ "$COV" == "true" && "$TEST" != "all" ]]; then
    log "[ERROR] --cov must be used with test(-t, --test)."
    exit 1
  fi
}

set_options() {
  while [[ $# -gt 0 ]]; do
    case $1 in
    -h|--help)
      has_h="true"
      usage
      exit 0
      ;;
    --cache=*)
      CCACHE_PROGRAM="${1#*=}"
      require_option_value "${1%%=*}" "${CCACHE_PROGRAM}"
      if ! command -v "${CCACHE_PROGRAM}" >/dev/null 2>&1; then
        log "[ERROR] cache program not found: ${CCACHE_PROGRAM}"
        exit 1
      fi
      shift
      ;;
    --cache)
      require_option_value "$1" "$2"
      CCACHE_PROGRAM="$2"
      if ! command -v "${CCACHE_PROGRAM}" >/dev/null 2>&1; then
        log "[ERROR] cache program not found: ${CCACHE_PROGRAM}"
        exit 1
      fi
      shift 2
      ;;
    -p=*|--cann_path=*)
      cann_path="${1#*=}"
      shift
      ;;
    -p|--cann_path)
      cann_path="$2"
      shift 2
      ;;
    -t|--test)
      TEST="all"
      check_param_test_pkg
      shift
      ;;
    --adv_test)
      TEST_PART="adv_test"
      check_param_test_part
      shift
      ;;
    --adv_test_two)
      TEST_PART="adv_test_two"
      check_param_test_part
      shift
      ;;
    --arm_test)
      TEST_PART="arm_test"
      check_param_test_part
      shift
      ;;
    --basic_test_one)
      TEST_PART="basic_test_one"
      check_param_test_part
      shift
      ;;
    --basic_test_two)
      TEST_PART="basic_test_two"
      check_param_test_part
      shift
      ;;
    --basic_test_three)
      TEST_PART="basic_test_three"
      check_param_test_part
      shift
      ;;
    --basic_test_four)
      TEST_PART="basic_test_four"
      check_param_test_part
      shift
      ;;
    --basic_test_five)
      TEST_PART="basic_test_five"
      check_param_test_part
      shift
      ;;
    --asan)
      ASAN="true"
      shift
      ;;
    --cov)
      COV="true"
      shift
      ;;
    --pkg)
      PKG="true"
      check_param_test_pkg
      shift
      ;;
    --pkg-type=*)
      PACKAGE_TYPE="${1#*=}"
      PACKAGE_TYPE_SET="true"
      check_param_pkg_type
      shift
      ;;
    --pkg-type)
      require_option_value "$1" "$2"
      PACKAGE_TYPE="$2"
      PACKAGE_TYPE_SET="true"
      check_param_pkg_type
      shift 2
      ;;
    --extra-cmake-args=*)
      local cmake_args="${1#*=}"
      parse_cmake_extra_args "${cmake_args}"
      shift
      ;;
    --cann_3rd_lib_path=*)
      CANN_3RD_LIB_PATH="${1#*=}"
      shift
      ;;
    --cann_3rd_lib_path)
      CANN_3RD_LIB_PATH="$2"
      shift 2
      ;;
    --make_clean)
      MAKE_CLEAN="true"
      check_param_clean
      clean
      exit 0
      ;;
    -j)
      require_option_value "$1" "$2"
      THREAD_NUM="$2"
      check_param_j
      shift 2
      ;;
    -j=*)
      THREAD_NUM="${1#*=}"
      check_param_j
      shift
      ;;
    -j*)
      THREAD_NUM="${1#-j}"
      check_param_j
      shift
      ;;
    -f)
      require_option_value "$1" "$2"
      CHANGED_FILES="$2"
      CI_MODE=TRUE
      shift 2
      ;;
    --changed_file=*)
      CHANGED_FILES="${1#*=}"
      CI_MODE=TRUE
      shift
      ;;
    --changed_file)
      require_option_value "$1" "$2"
      CHANGED_FILES="$2"
      CI_MODE=TRUE
      shift 2
      ;;
    --build-type=*)
      BUILD_TYPE="${1#*=}"
      check_param_test_build_type
      shift
      ;;
    --enable-sign)
        ENABLE_SIGN="true"
        shift
        ;;
    --sign-script)
        CUSTOM_SIGN_SCRIPT="$(realpath $2)"
        shift 2
        ;;
    --build-type)
      BUILD_TYPE="$2"
      check_param_test_build_type
      shift 2
      ;;
    *)
      log "[ERROR] Undefined option: $1"
      usage
      break
      ;;
    esac
  done
}

set_env() {
  if [ "${USER_ID}" != "0" ]; then
    DEFAULT_TOOLKIT_INSTALL_DIR="${HOME}/Ascend/cann"
    DEFAULT_INSTALL_DIR="${HOME}/Ascend/cann"
  else
    DEFAULT_TOOLKIT_INSTALL_DIR="/usr/local/Ascend/cann"
    DEFAULT_INSTALL_DIR="/usr/local/Ascend/cann"
  fi

  if [ -n "${cann_path}" ];then
    ASCEND_CANN_PACKAGE_PATH=${cann_path}
  elif [ -n "${ASCEND_HOME_PATH}" ];then
    ASCEND_CANN_PACKAGE_PATH=${ASCEND_HOME_PATH}
  elif [ -n "${ASCEND_OPP_PATH}" ];then
    ASCEND_CANN_PACKAGE_PATH=$(dirname ${ASCEND_OPP_PATH})
  elif [ -d "${DEFAULT_TOOLKIT_INSTALL_DIR}" ];then
    ASCEND_CANN_PACKAGE_PATH=${DEFAULT_TOOLKIT_INSTALL_DIR}
  elif [ -d "${DEFAULT_INSTALL_DIR}" ];then
    ASCEND_CANN_PACKAGE_PATH=${DEFAULT_INSTALL_DIR}
  else
    log "Error: Please set the cann package installation directory through parameter -p|--cann_path."
    exit 1
  fi

  source $ASCEND_CANN_PACKAGE_PATH/set_env.sh || echo "0"
}

function clean()
{
  if [ -n "${BUILD_DIR}" ];then
    rm -rf ${BUILD_DIR}
  fi

  if [ -n "${OUTPUT_DIR}" ];then
    rm -rf ${OUTPUT_DIR}
  fi

  mkdir -p ${BUILD_DIR} ${OUTPUT_DIR}
}

function cmake_config()
{
  local extra_option="$1"
  log "Info: cmake config ${CUSTOM_OPTION} ${extra_option} ."
  cmake ..  ${CUSTOM_OPTION} ${extra_option}
}

function build()
{
  log "Info: build target:$@ JOB_NUM:${JOB_NUM}"
  cmake --build . --target "$@" -j ${THREAD_NUM}
}

function build_package(){
  CUSTOM_OPTION="${CUSTOM_OPTION} -DENABLE_TEST=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_BUILD_DEVICE=${ENABLE_BUILD_DEVICE} -DPACKAGE_TYPE=${PACKAGE_TYPE}"
  cmake_config
  build package
  collect_package_artifacts
}

function collect_package_artifacts()
{
  local package_types="${PACKAGE_TYPE}"
  local package_ext=""
  local copied=false
  local found=false
  local pkg_file=""

  if [ "${package_types}" == "all" ]; then
    package_types="run,rpm,deb"
  fi

  for package_ext in ${package_types//,/ }; do
    found=false
    while IFS= read -r pkg_file; do
      if [ -f "${pkg_file}" ]; then
        if [ "$(realpath "${pkg_file}")" = "$(realpath -m "${OUTPUT_DIR}/$(basename "${pkg_file}")")" ]; then
          copied=true
          found=true
          continue
        fi
        cp -f "${pkg_file}" "${OUTPUT_DIR}/"
        copied=true
        found=true
      fi
    done < <(find "${BUILD_DIR}" "${OUTPUT_DIR}" -type f -name "*.${package_ext}" 2>/dev/null)

    if [ "${found}" != "true" ]; then
      log "[ERROR] No ${package_ext} package was found after build."
      exit 1
    fi
  done

  if [ "${copied}" != "true" ]; then
    log "[ERROR] No ${package_types} package was found after build."
    exit 1
  fi
}

function build_test() {
  cmake_config
  build all
}

function build_test_part() {
  source ${CURRENT_DIR}/tests/test_parts.sh

  if [ "$TEST_PART" == "adv_test" ]; then
    TEST_TARGET_LIST=("${adv_test_targets[@]}")
  elif [ "$TEST_PART" == "adv_test_two" ]; then
    TEST_TARGET_LIST=("${adv_test_two_targets[@]}")
  elif [ "$TEST_PART" == "arm_test" ]; then
    TEST_TARGET_LIST=("${arm_test_targets[@]}")
  elif [ "$TEST_PART" == "basic_test_one" ]; then
    TEST_TARGET_LIST=("${basic_test_one_targets[@]}")
  elif [ "$TEST_PART" == "basic_test_two" ]; then
    TEST_TARGET_LIST=("${basic_test_two_targets[@]}")
  elif [ "$TEST_PART" == "basic_test_three" ]; then
    TEST_TARGET_LIST=("${basic_test_three_targets[@]}")
  elif [ "$TEST_PART" == "basic_test_four" ]; then
    TEST_TARGET_LIST=("${basic_test_four_targets[@]}")
  elif [ "$TEST_PART" == "basic_test_five" ]; then
    TEST_TARGET_LIST=("${basic_test_five_targets[@]}")
  fi

  for tag in "${TEST_TARGET_LIST[@]}"; do
    if [ "${tag}" == "hccl_checker_ops_stest" ]; then
      local hccl_st_dir="${CURRENT_DIR}/tests/api/adv_api/hccl/cc/st/algorithm"
      bash ${hccl_st_dir}/build.sh
    else
      TARGETS="${TARGETS} --target ${tag}"
      TEST_MOD="${tag},${TEST_MOD}"
    fi
  done

  CUSTOM_OPTION="${CUSTOM_OPTION} -DTEST_MOD=${TEST_MOD}"
  if [ "${COV}" == "true" ]; then
    TARGETS="${TARGETS} --target collect_coverage_data"
  fi

  cmake_config
  cmake --build . ${TARGETS} -j ${THREAD_NUM}
  return 0
}

set_ci_mode() {
  if [[ -z "$CHANGED_FILES" ]]; then
    log "[INFO] CI mode: no changed files, skip all (no compile and test, no package)."
    exit 200
  fi

  if [[ "$CHANGED_FILES" != /* ]]; then
    CHANGED_FILES=${CURRENT_DIR}/$CHANGED_FILES
  fi
  log "[INFO] CI mode: changed file path is $CHANGED_FILES"
  log "[INFO] CI mode: context in changed file: "
  cat "$CHANGED_FILES"

  if [[ -n "$CHANGED_FILES" ]]; then
    log "[INFO] CI mode: processing changed files for CI mode."
    local parse_script="${CURRENT_DIR}/scripts/util/parse_changed_files.py"
    if [[ ! -f "$parse_script" ]]; then
      log "[ERROR] CI mode: parse_changed_files.py not found at $parse_script"
      exit 1
    fi

    CI_ACTION=$(python3 "$parse_script" "$CHANGED_FILES")
    log "[INFO] CI mode: action = $CI_ACTION"

    if [[ "${CI_ACTION}" == "SKIP" ]]; then
      log "[INFO] CI mode: Skip all (no compile and test, no package)."
      exit 200
    fi

    if [[ "${CI_ACTION}" == "PKG" && "${PKG}" == "true" ]]; then
      log "[INFO] CI mode: Trigger package build for examples changes."
      CUSTOM_OPTION="${CUSTOM_OPTION} -DPACKAGE_OPEN_PROJECT=ON -DKERNEL_MODE=OFF"
      build_package
      exit 0
    fi
  fi
}

main() {
  check_param_with_help "$@"
  set_options "$@"
  check_param_pkg_type_usage

  set_env

  CUSTOM_OPTION="${CUSTOM_OPTION} -DASCEND_CANN_PACKAGE_PATH=${ASCEND_CANN_PACKAGE_PATH}"
  if [ -n "${CCACHE_PROGRAM}" ];then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DCMAKE_C_COMPILER_LAUNCHER=${CCACHE_PROGRAM} -DCMAKE_CXX_COMPILER_LAUNCHER=${CCACHE_PROGRAM}"
  fi

  if [ -n "${TEST}" ];then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DENABLE_TEST=ON -DTEST_MOD=all"
  fi

  if [ -n "${TEST_PART}" ]; then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DENABLE_TEST=ON"
  fi

  if [ "${ASAN}" == "true" ];then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DENABLE_ASAN=true"
  fi

  if [ "${COV}" == "true" ];then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DENABLE_GCOV=true"
  fi

  if [ "${PKG}" == "true" ];then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DPACKAGE_OPEN_PROJECT=ON"
  fi

  if [ -n "${CANN_3RD_LIB_PATH}" ];then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DCANN_3RD_LIB_PATH=${CANN_3RD_LIB_PATH}"
  fi

  if [[ ! -d "${BUILD_DIR}" ]]; then
    mkdir -p "${BUILD_DIR}"
  fi
  if [[ ! -d "${OUTPUT_DIR}" ]]; then
    mkdir -p "${OUTPUT_DIR}"
  fi

  cd ${BUILD_DIR}

  if [[ "${CI_MODE}" == "TRUE" ]]; then
    set_ci_mode
  fi

  if [ -n "${TEST}" ]; then
    build_test
  elif [ -n "$TEST_PART" ]; then
    build_test_part
  elif [ -n "${PKG}" ]; then
    CUSTOM_OPTION="${CUSTOM_OPTION} -DCUSTOM_SIGN_SCRIPT=${CUSTOM_SIGN_SCRIPT} -DENABLE_SIGN=${ENABLE_SIGN} -DPACKAGE_OPEN_PROJECT=ON"
    build_package
  else
    cmake_config
    build all
  fi
}

set -o pipefail
if [[ $# -eq 0 ]]; then
  usage
fi

main "$@"
