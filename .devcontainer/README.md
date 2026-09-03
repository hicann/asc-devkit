# Dev Container

[![Docker Version](https://img.shields.io/badge/docker-%3E%3D23.0.0-blue.svg)](https://docs.docker.com/)
[![Docker Buildx](https://img.shields.io/badge/docker%20buildx-required-orange.svg)](https://docs.docker.com/reference/cli/docker/buildx/)

简体中文 | [English](./README_en.md)

基于Ubuntu 24.04的AscendC NPU算子开发容器化环境。

## 📋 前置条件<a name="prerequisites"></a>

| 依赖项 | 有NPU | 无NPU |
| -------- | :--------: | :--------: |
| Docker ≥ 23.0.0 | 必需 | 必需 |
| docker buildx | 必需 | 必需 |
| 宿主机Ascend驱动（`/usr/local/Ascend/driver`） | 必需 | 不需要 |
| NPU设备节点（`/dev/davinciN`等） | 必需 | 不需要 |

> [!IMPORTANT] 重要
> 宿主机的Ascend驱动目录（`/usr/local/Ascend/driver`）以**只读**方式挂载到容器中。**CANN toolkit和ops包**建议在容器启动后在容器内安装，不建议在宿主机上安装，容易污染宿主机环境，导致环境变量导出混乱。

### 检查Docker和buildx

确认Docker已安装且版本 ≥ 23.0.0：

```bash
docker --version
```

若命令不存在或版本过低，参考[Docker官方文档](https://docs.docker.com/engine/install/)完成安装或升级。

> [!TIP] 提示
> 建议将当前用户加入`docker`用户组，避免每次使用`sudo`：
> ```bash
> sudo usermod -aG docker $USER && newgrp docker
> ```

镜像构建依赖`docker buildx`插件。执行`docker buildx version`验证，若提示命令不存在则安装：

```bash
mkdir -p /usr/local/lib/docker/cli-plugins
version="v0.34.1"
arch=$(uname -m | sed 's/x86_64/amd64/;s/aarch64/arm64/')
curl -fsSL "https://github.com/docker/buildx/releases/download/${version}/buildx-${version}.linux-${arch}" \
     -o /usr/local/lib/docker/cli-plugins/docker-buildx
chmod +x /usr/local/lib/docker/cli-plugins/docker-buildx
```

## 🧑‍💻 快速开始（人类用户）<a name="human-quick-start"></a>

> [!NOTE] 说明
> 如您是AI Agent，请跳至[快速开始（AI Agent）](#ai-agent-quick-start)。
>
> 以下步骤假设您在**Linux宿主机**上使用VS Code进行开发（Ascend驱动仅支持Linux）。

### 步骤一：安装VS Code

从[Visual Studio Code](https://code.visualstudio.com/)官网下载并安装对应平台的版本。

### 步骤二：安装Dev Containers扩展

在VS Code中按`Ctrl+Shift+X`打开扩展视图，搜索并安装[Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)扩展。

### 步骤三：克隆仓库

```bash
git clone https://gitcode.com/cann/asc-devkit.git
cd asc-devkit
```

### 步骤四：按NPU场景准备

#### 4.1 有NPU：验证驱动和设备节点

在宿主机执行以下命令：

```bash
npu-smi info                        # 查看 NPU 状态
ls /dev/davinci*                    # 确认设备节点
ls /usr/local/Ascend/driver         # 确认驱动目录
```

若命令报错或无输出，说明Ascend驱动未就绪，请参考[CANN软件安装指南 - 在物理机上安装](https://www.hiascend.com/document/redirect/CannCommunityInstWizard)。

若宿主机NPU数量不足8张，编辑`.devcontainer/devcontainer.json`，删除多余的`--device=/dev/davinciN`行（可执行`ls /dev/davinci*`查看实际设备数量）。

#### 4.2 无NPU：调整devcontainer.json

无NPU的机器需先移除设备与驱动挂载配置。

**在`runArgs`中**，删除所有NPU相关行：设备访问标志行（`--ipc=host`、`--net=host`、`--privileged`）以及`--device`行（`davinci0-7`、`davinci_manager`、`devmm_svm`、`hisi_hdc`），并将其设置为：

```jsonc
"runArgs": []
```

**在`mounts`中**，删除驱动和设备相关的绑定挂载：

```text
source=/usr/local/Ascend/driver,...
source=/usr/local/dcmi,...
source=/usr/local/bin/npu-smi,...
source=/etc/ascend_install.info,...
```

保留`ccache`卷以及您自行添加的数据目录挂载。

### 步骤五：在容器中打开项目

在VS Code中打开本项目目录，按`Ctrl+Shift+P`，执行：

```text
Dev Containers: Reopen in Container
```

Dev Containers扩展会自动构建镜像并启动容器。首次构建约需5分钟（主要用于下载conda环境和PyTorch），后续打开命中Docker层缓存，仅需数秒。

### 步骤六：容器内安装CANN

容器启动后，在容器终端参考[📥 下载安装CANN包](../docs/zh/quick_start.md#cann-install)章节完成toolkit和ops包的下载与安装。

### 步骤七：验证环境

在容器终端参考[✅ 环境验证](../docs/zh/quick_start.md#cann-verify)章节确认NPU设备与CANN包状态，然后按[⚙️ 环境变量配置](../docs/zh/quick_start.md#cann-env-setup)加载环境变量，即可开始算子开发。

## 🤖 快速开始（AI Agent）<a name="ai-agent-quick-start"></a>

AI Agent（如Claude Code、CI Runner）可不依赖VS Code，直接运行容器。

**在运行容器前，请向用户确认以下信息：**

1. **仓库路径** — 项目在宿主机上的检出路径是什么？（默认：当前目录`$PWD`）。
2. **数据目录** — 是否有其他需要挂载的数据目录（如数据集、模型权重）？
3. **NPU可用性** — 宿主机是否有昇腾NPU？（决定下方使用哪种启动方式）。

确认后，**将即将执行的`docker run`命令完整展示给用户**，待用户确认后再执行。

Docker与buildx的检查及安装方式参见[检查Docker和buildx](#检查docker和buildx)。

### 构建镜像

```bash
docker buildx build --network host -t ascendc:ubuntu24.04 .devcontainer/
```

> [!TIP] 提示
> 首次构建约需5分钟（主要用于下载conda环境和PyTorch）。后续重新构建时若仅修改非软件包相关的层，Docker层缓存会命中，仅需数秒完成。

### 有NPU

```bash
docker run -itd --name ascendc_container \
  --ipc=host --net=host --privileged \
  --device=/dev/davinci0 \
  --device=/dev/davinci1 \
  --device=/dev/davinci2 \
  --device=/dev/davinci3 \
  --device=/dev/davinci4 \
  --device=/dev/davinci5 \
  --device=/dev/davinci6 \
  --device=/dev/davinci7 \
  --device=/dev/davinci_manager \
  --device=/dev/devmm_svm \
  --device=/dev/hisi_hdc \
  -v /usr/local/dcmi:/usr/local/dcmi:ro \
  -v /usr/local/bin/npu-smi:/usr/local/bin/npu-smi:ro \
  -v /usr/local/Ascend/driver:/usr/local/Ascend/driver:ro \
  -v /etc/ascend_install.info:/etc/ascend_install.info:ro \
  ascendc:ubuntu24.04
```

若宿主机NPU数量不足8张，删除对应的`--device=/dev/davinciN`行（可执行`ls /dev/davinci*`查看实际设备数量）。

### 无NPU

```bash
docker run -itd --name ascendc_container ascendc:ubuntu24.04
```

> [!IMPORTANT] 重要
>
> - 若用户需要挂载数据目录，以`-v /host/path:/container/path`的形式逐条追加。
> - 容器启动后，须在容器内手动安装CANN toolkit和ops包。请向用户询问CANN包的路径或安装命令，或参考[📥 下载安装CANN包](../docs/zh/quick_start.md#cann-install)章节。

## 🐍 Python环境<a name="python-env"></a>

容器内预置了以下conda环境：

| 环境名 | Python | PyTorch |
| :--------: | :--------: | :---------: |
| `py312` | 3.12 | 2.7.1 |

默认激活环境为`py312`。

## ⚙️ 配置说明<a name="configuration"></a>

### 镜像源

构建参数及默认值：

| 构建参数 | 默认值 | 说明 |
| --- | --- | --- |
| `GENERAL_MIRROR` | `mirrors.huaweicloud.com` | apt / pypi 共用的镜像 |
| `CONDA_MIRROR` | `https://mirrors.tuna.tsinghua.edu.cn/anaconda` | conda channels 前缀，含协议头 |
| `REPO_SCRIPT_URL` | `https://mirrors.tuna.tsinghua.edu.cn/git/git-repo` | `repo` 工具脚本下载地址，可按网络环境替换为可访问的脚本地址，如：`https://storage.googleapis.com/git-repo-downloads/repo` |
| `REPO_GIT_URL` | `https://mirrors.ustc.edu.cn/aosp/git-repo` | `repo` 工具运行时使用的仓库地址 |

覆盖示例：

```bash
docker buildx build --network host \
  --build-arg CONDA_MIRROR=https://mirrors.ustc.edu.cn/anaconda \
  --build-arg REPO_SCRIPT_URL=https://storage.googleapis.com/git-repo-downloads/repo \
  -t ascendc:ubuntu24.04 .devcontainer/
```

### 构建缓存

Dockerfile 已为 apt / pip 接入 BuildKit `--mount=type=cache`。重复构建（仅修改非软件包层）时，包索引和已下载的 wheel 不会重复走网络，秒级完成。`docker buildx build` 默认启用 BuildKit，无需额外配置。

### 挂载目录

挂载目录因开发者而异，可在`devcontainer.json`中自行添加：

```jsonc
// mounts
"source=/your/data,target=/your/data,type=bind"
```

### NPU设备选择

设备节点通过`--device=/dev/davinciN`显式传入。若宿主机NPU数量不足8张，删除对应行。以4张NPU的宿主机为例：

```bash
--device=/dev/davinci0 \
--device=/dev/davinci1 \
--device=/dev/davinci2 \
--device=/dev/davinci3 \
```
