#!/bin/bash
# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------
set +e

log() {
  echo "===================================================================="
  echo "$(date '+%Y%m%d.%H%M%S') : $*"
  echo "===================================================================="
}

# ==============================
# Prepare the workspace
# ==============================
# The workspace is reused across runs: a leftover run_test.log or slog_*.tar.gz
# from the previous run would be mistaken for this one.
RUN_TEST_LOG="${WORKSPACE}/run_test.log"
ENV_LOG="/root/ascend/log/ci_env_report.log"

log "init test case, please wait ..."
rm -f "$RUN_TEST_LOG" "${WORKSPACE}"/slog_*.tar.gz
rm -rf /root/ascend/log
mkdir -p /root/ascend/log

export ASCEND_GLOBAL_LOG_LEVEL=2
export ASCEND_SLOG_PRINT_TO_STDOUT=0

cd "${WORKSPACE}" || exit 1

# ==============================
# Install the asc-devkit package under test
# ==============================
log "installing asc-devkit package ..."
# An empty run_url means broken plumbing, not "no package for this PR", so it must
# not share the exit path below.
if [ -z "${run_url:-}" ]; then
  echo "ERROR: run_url is empty, presmoke cannot pick a devkit package" >&2
  exit 1
fi
devkit_package=$(basename "${run_url}")
echo "devkit package for this smoke image: ${devkit_package}"
(cd ./scripts && bash devkit_dir_check.sh 2>&1 | tee -a "$RUN_TEST_LOG")

wget -nv -O "${devkit_package}" "${run_url}"
if [ ! -f "${devkit_package}" ] || [ ! -s "${devkit_package}" ]; then
  echo "No custom package found, This PR no need execute smoke."
  rm -f "${devkit_package}"
  exit 0
fi

# The open-source CI build of mc2_server.tar.gz carries no CMS signature. Once it is
# installed, device-side verification fails and not a single card can be opened.
mc2_path=$(ls -1 /usr/local/Ascend/*/opp/built-in/op_impl/aicpu/kernel/mc2_server.tar.gz 2>/dev/null | head -n 1)
mc2_saved=""
if [ -n "${mc2_path}" ]; then
  mc2_saved="$(mktemp -d)/mc2_server.tar.gz"
  cp -a "${mc2_path}" "${mc2_saved}"
  mc2_md5=$(md5sum < "${mc2_saved}" | cut -d' ' -f1)
  echo "mc2_server.tar.gz saved: ${mc2_path} md5=${mc2_md5}" | tee -a "$ENV_LOG"
else
  echo "WARNING: mc2_server.tar.gz not found before devkit install" | tee -a "$ENV_LOG" >&2
fi

chmod +x "${devkit_package}"
yes "y" | bash "${devkit_package}" --full --install-path=/usr/local/Ascend --quiet > /tmp/devkit_install.log 2>&1
tail -40 /tmp/devkit_install.log | sed 's/^/[devkit-install] /' | tee -a "$ENV_LOG"

if [ -n "${mc2_saved}" ]; then
  # The installer may move to a different version directory, so resolve the path again.
  mc2_path=$(ls -1 /usr/local/Ascend/*/opp/built-in/op_impl/aicpu/kernel/mc2_server.tar.gz 2>/dev/null | head -n 1)
  if [ -z "${mc2_path}" ]; then
    echo "ERROR: mc2_server.tar.gz missing after devkit install" | tee -a "$ENV_LOG" >&2
    exit 1
  fi
  echo "mc2_server.tar.gz after install: md5=$(md5sum < "${mc2_path}" | cut -d' ' -f1)" | tee -a "$ENV_LOG"
  if ! cp -a "${mc2_saved}" "${mc2_path}"; then
    echo "ERROR: failed to restore mc2_server.tar.gz to ${mc2_path}" | tee -a "$ENV_LOG" >&2
    exit 1
  fi
  mc2_now=$(md5sum < "${mc2_path}" | cut -d' ' -f1)
  echo "mc2_server.tar.gz restored: ${mc2_path} md5=${mc2_now}" | tee -a "$ENV_LOG"
  if [ "${mc2_now}" != "${mc2_md5}" ]; then
    echo "ERROR: mc2_server.tar.gz md5 mismatch (want ${mc2_md5}, got ${mc2_now})" | tee -a "$ENV_LOG" >&2
    exit 1
  fi
fi

# ==============================
# Run presmoke
# ==============================
log "start run test case, please wait ..."
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 10; update-alternatives --config gcc <<< '1'
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 10; update-alternatives --config g++ <<< '1'
source /usr/local/Ascend/cann/set_env.sh
python3 -m pip install -r .devcontainer/requirements.txt
(cd ./scripts && bash run_presmoke.sh 2>&1 | tee -a "$RUN_TEST_LOG")
if [ -d "presmoke_reports" ]; then
  mv presmoke_reports /root/ascend/log
fi

# ==============================
# Collect NPU status
# ==============================
log "checking NPU status ..."
# Diagnostics have to be written before the tar, or they never make it into the slog.
npu-smi info 2>&1 | sed 's/^/[npu-smi] /' | tee -a "$ENV_LOG"

# ==============================
# Pack the log for upload
# ==============================
# Without the arch, A2 and A5 would both produce slog.tar.gz and overwrite each other.
slog_arch="${ARCH:-}"
if [ -z "${slog_arch}" ]; then
  slog_arch=$(ls -d /root/ascend/log/presmoke_reports/presmoke_*/ 2>/dev/null \
    | head -n 1 | sed -E 's#.*/presmoke_([^_]+)_[0-9].*#\1#')
fi
slog_name="slog_${slog_arch:-unknown}.tar.gz"
tar -zcf "${slog_name}" -C /root/ascend log

# The upload is handled by the obs-upload step in the job, which carries if: always().
echo "plog_url: ${obs_smoke_path}/plog/${slog_name}"

# ==============================
# Check the test result
# ==============================
log "checking test results ..."
# tee swallows the exit code of run_presmoke.sh, so the verdict can only come from the
# string it prints when rc=0.
if grep -w -e "execute samples success" "$RUN_TEST_LOG"; then
  echo "$(date '+%Y%m%d.%H%M%S') : run test case success"
else
  echo "$(date '+%Y%m%d.%H%M%S') : run test case failed"
  exit 1
fi
