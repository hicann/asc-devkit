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

shopt -s extglob
WORKSPACE=/home/taskspace
cd /home/taskspace

log() {
  local dt
  dt=$(date '+%Y%m%d.%H%M%S')
  echo "===================================================================="
  echo "$dt : $*"
  echo "===================================================================="
}

log "init test case, please wait ..."
rm -rf /root/ascend/log

# ==============================
# 确定要测试的 ops 列表
# ==============================
declare -a ops
ops=("is_finite")

export ASCEND_GLOBAL_LOG_LEVEL=2
export ASCEND_SLOG_PRINT_TO_STDOUT=0

# ==============================
# 运行 asc-devkit 测试主流程
# ==============================
log "start run test case, please wait ..."

for op in "${ops[@]}"; do
  echo "Processing: $op"
  mode="eager"
  [ "$op" = "crop_and_resize" ] && mode="graph"
  source /usr/local/Ascend/cann/set_env.sh

  cd ${WORKSPACE}
  arm_package="cann-asc-devkit_linux-aarch64.run"
  (cd ./scripts && bash devkit_dir_check.sh 2>&1 | tee -a ./run_test.log)

  wget -nv -O "${arm_package}" \
    "https://ascend-ci.obs.cn-north-4.myhuaweicloud.com/${obs_path}/${arm_package}" 2>/dev/null

  if [ ! -f "${arm_package}" ] || [ ! -s "${arm_package}" ]; then
    echo "No custom package found, This PR no need execute smoke."
    rm -f "${arm_package}"
    exit 0
  fi

  chmod +x ${arm_package}
  yes "y" | bash "${arm_package}" --full --install-path=/usr/local/Ascend --quiet
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 10; update-alternatives --config gcc <<< '1'
  update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 10; update-alternatives --config g++ <<< '1'
  source /usr/local/Ascend/cann/set_env.sh
  python3 -m pip install -r .devcontainer/requirements.txt
  (cd ./scripts && bash run_presmoke.sh  2>&1 | tee -a ./run_test.log)
  if [ -d "presmoke_reports" ];then
    mv presmoke_reports /root/ascend/log
  fi
  cd ./scripts
done

# 打包plog
mkdir -p /root/ascend
slog_name="slog.tar.gz"
tar -zcf slog.tar.gz -C /root/ascend log
OBS_KEY="${obs_smoke_path}/plog/${slog_name}"
# upload plog
if python3 /home/upload.py --bucket-name "ascend-ci" --action upload  --local-file "slog.tar.gz" --obs-object-key "${OBS_KEY}"; then
  echo "::set-output var=plog_url:https://ascend-ci.obs.cn-north-4.myhuaweicloud.com/${OBS_KEY}"
fi

# ==============================
# 检查 NPU 状态
# ==============================
log "checking NPU status ..."
mkdir -p ./npu_log
npu-smi info 2>&1 | tee ./npu_log/npu_info.log
if grep "dcmi module initialize failed" "./npu_log/npu_info.log"; then
  date_time=$(date '+%Y%m%d.%H%M%S')
  echo "$date_time : ${repo_name}_${pr_id} dcmi module initialize failed" >> ./npu_log/`date +%Y%m%d`.log
  python3 /home/aa/check_npu.py $repo_name $pr_id `cat /home/aa/hostname`
  exit 1
fi

# ==============================
# 检查测试结果
# ==============================
log "checking test results ..."

date_time=`date +%Y%m%d`"."`date +%H%M%S`
if grep -w -e "execute samples success" "./run_test.log"; then
  echo "$date_time : run test case success"
else
  echo "$date_time : run test case failed"
  exit 1
fi
