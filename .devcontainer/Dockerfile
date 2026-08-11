# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

# Build:
#   docker buildx build --network host -t ascendc:ubuntu24.04 .devcontainer/

# Stage 0 — used only to copy /opt/conda into the main image
FROM continuumio/miniconda3:latest AS conda-base

FROM ubuntu:24.04

# --- Build arguments (override with --build-arg) ---
ARG GENERAL_MIRROR=mirrors.huaweicloud.com
ARG CONDA_MIRROR=https://mirrors.tuna.tsinghua.edu.cn/anaconda
ARG REPO_SCRIPT_URL=https://mirrors.tuna.tsinghua.edu.cn/git/git-repo
ARG REPO_GIT_URL=https://mirrors.ustc.edu.cn/aosp/git-repo

# --- Base environment ---
ENV TZ=Asia/Shanghai \
    LANG=C.UTF-8 \
    SHELL=/bin/zsh \
    DEBIAN_FRONTEND=noninteractive \
    PATH=/opt/conda/envs/py312/bin:/opt/conda/bin:$PATH \
    REPO_URL=${REPO_GIT_URL}

USER root
WORKDIR /tmp

SHELL ["/bin/bash", "-c"]

# --- APT mirror & system packages ---
# apt cache lives in BuildKit mount, not the image layer — no apt-get clean.
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    set -eu && \
    # Ubuntu's default config wipes /var/cache/apt after install,
    # which would defeat the cache mount. Disable that hook.
    rm -f /etc/apt/apt.conf.d/docker-clean && \
    arch="$(dpkg --print-architecture)" && \
    if [ "${arch}" = "arm64" ]; then \
        sed -i "s@//ports.ubuntu.com@//${GENERAL_MIRROR}@g" /etc/apt/sources.list.d/ubuntu.sources; \
    else \
        sed -i "s@//archive.ubuntu.com@//${GENERAL_MIRROR}@g; s@//security.ubuntu.com@//${GENERAL_MIRROR}@g" /etc/apt/sources.list.d/ubuntu.sources; \
    fi && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        tzdata lsb-release ca-certificates openssl gnupg2 \
        curl wget openssh-client iputils-ping net-tools lsof \
        zip unzip \
        build-essential cmake ninja-build ccache pkg-config \
        autoconf automake libtool \
        gdb gdbserver lcov \
        git git-lfs \
        vim htop tree zsh clangd whiptail \
        pciutils && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone

# --- repo tool + git global config ---
RUN set -eu && \
    curl -fsSL ${REPO_SCRIPT_URL} -o /usr/local/bin/repo && \
    chmod a+x /usr/local/bin/repo && \
    git config --global core.autocrlf input && \
    git config --global core.editor vim && \
    git config --global color.ui auto && \
    git config --global credential.helper "cache --timeout=3600" && \
    git config --global alias.lg \
        "log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit"

# --- oh-my-zsh + plugins (via gitcode mirror) ---
RUN set -eu && \
    git clone --depth=1 https://gitcode.com/ohmyzsh/ohmyzsh.git /root/.oh-my-zsh && \
    cp /root/.oh-my-zsh/templates/zshrc.zsh-template /root/.zshrc && \
    git clone --depth=1 https://gitcode.com/zsh-users/zsh-autosuggestions \
        /root/.oh-my-zsh/custom/plugins/zsh-autosuggestions && \
    git clone --depth=1 https://gitcode.com/zsh-users/zsh-syntax-highlighting \
        /root/.oh-my-zsh/custom/plugins/zsh-syntax-highlighting && \
    sed -i 's/plugins=(git)/plugins=(git z zsh-autosuggestions zsh-syntax-highlighting)/' /root/.zshrc && \
    sed -i 's/ZSH_THEME="robbyrussell"/ZSH_THEME="agnoster"/' /root/.zshrc && \
    chsh -s "$(which zsh)" root

# --- conda + condarc / pip.conf ---
COPY --from=conda-base /opt/conda /opt/conda

RUN set -eu && mkdir -p /etc/conda && cat > /etc/conda/.condarc <<EOF
channels:
  - defaults
show_channel_urls: true
default_channels:
  - ${CONDA_MIRROR}/pkgs/main
  - ${CONDA_MIRROR}/pkgs/r
  - ${CONDA_MIRROR}/pkgs/msys2
custom_channels:
  conda-forge: ${CONDA_MIRROR}/cloud
  pytorch: ${CONDA_MIRROR}/cloud
EOF

RUN set -eu && cat > /etc/pip.conf <<EOF
[global]
index-url = https://${GENERAL_MIRROR}/repository/pypi/simple
trusted-host = ${GENERAL_MIRROR}
default-timeout = 120
EOF

# --- Python env ---
COPY requirements.txt /tmp/requirements.txt
RUN --mount=type=cache,target=/root/.cache/pip,sharing=locked \
    set -eu && \
    conda create -y -n py312 python=3.12 pip && \
    conda run -n py312 pip install -r /tmp/requirements.txt && \
    conda clean -afy && \
    find /opt/conda -follow -type d -name '__pycache__' -exec rm -rf {} + 2>/dev/null || true && \
    rm -f /tmp/requirements.txt

# --- Shell customization ---
RUN set -eu && \
    conda init zsh && \
    cat >> /root/.zshrc <<EOF && rm -rf /tmp/*

conda activate py312

# AscendC banner
echo "\\033[1;36m"
echo " █████╗ ███████╗ ██████╗███████╗███╗   ██╗██████╗  ██████╗"
echo "██╔══██╗██╔════╝██╔════╝██╔════╝████╗  ██║██╔══██╗██╔════╝"
echo "███████║███████╗██║     █████╗  ██╔██╗ ██║██║  ██║██║     "
echo "██╔══██║╚════██║██║     ██╔══╝  ██║╚██╗██║██║  ██║██║     "
echo "██║  ██║███████║╚██████╗███████╗██║ ╚████║██████╔╝╚██████╗"
echo "╚═╝  ╚═╝╚══════╝ ╚═════╝╚══════╝╚═╝  ╚═══╝╚═════╝  ╚═════╝"
echo "\\033[0m"

# Ascend driver environment
[ -f /usr/local/Ascend/driver/bin/setenv.bash ] && source /usr/local/Ascend/driver/bin/setenv.bash
EOF

WORKDIR /root
ENTRYPOINT ["/bin/zsh"]
