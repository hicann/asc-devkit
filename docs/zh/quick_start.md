# 快速开始

## 🛠️ 环境准备<a name="prepare&install"></a>

根据**本地是否有NPU设备**和**使用目标**选择对应的环境准备方式：

| 环境准备 | 社区体验 / 算子开发（CANN商用/社区版） | 生态开发者贡献（CANN master） |
| :---: | :---: | :---: |
| **无NPU设备** | [云开发环境](#cloud-dev-env) | [手动下载安装CANN master](#cann-install) + [云开发环境](#cloud-dev-env) |
| **有NPU设备** | [CANN官方Docker镜像](#cann-docker-image) | [手动下载安装CANN master](#cann-install) + [Dev Container](#devcontainer) |

> [!TIP] 选择建议
>
> - 为了保障开发体验环境的质量，推荐用户基于**容器化技术**完成**环境准备**。
> - 如不希望使用容器，也可在带NPU设备的主机上完成**环境准备**，请参考[CANN软件安装指南 - 在物理机上安装](https://www.hiascend.com/cann/download)。
> - 针对仅体验"编译安装本开源仓 + 仿真环境运行算子"的用户，不要求主机带NPU设备，可跳过安装NPU驱动和固件，直接安装CANN包，请参考[下载安装CANN包](#cann-install)。

<!-- 待加入AI Agent使能的逻辑 -->

### 1️⃣ 云开发环境<a name="cloud-dev-env"></a>

对于无NPU设备的用户，可直接使用**CANNLab云开发环境**，即"**一站式开发平台**"，该平台提供了在线可直接运行的昇腾ARM架构环境，环境中已安装必备的驱动固件、软件包和依赖，无需手动安装。该平台目前仅适用于Atlas A2系列产品，提供两种接入方式：

- **WebIDE**，提供网页版的便携开发体验。
- **VSCode IDE**，支持远程连接**云开发环境**，提供VSCode强大插件市场的支持。

1. 进入开源仓Gitcode页面，单击"`CANNLab > 云开发`"按钮，使用已认证过的华为云账号登录。若未注册或认证，请根据页面提示进行注册和认证。

   <p align="center"><img src="./figures/cloudIDE.png" alt="云平台" width="750px" height="90px"></p>

2. 根据页面提示创建NPU环境并配置规格，启动云开发环境后，单击"`连接 > WebIDE 或 Visual Studio Code`"进入一站式开发平台。开源项目的资源默认在`/mnt/workspace`目录下。

   <p align="center"><img src="./figures/webIDE.png" alt="云平台" width="1000px" height="150px"></p>

> [!NOTE] 使用说明
>
> - 环境默认安装了最新的商用版NPU驱动和固件、CANN包，源码下载时注意与软件配套。
> - 如需下载特定版本的CANN包，请参考[下载安装CANN包](#cann-install)。
> - 更多关于**CANNLab云开发环境**的介绍，请参考[CANNLab指导](https://gitcode.com/org/cann/discussions/54)。
> - [Huawei Developer Space插件](https://marketplace.visualstudio.com/items?itemName=HuaweiCloud.developerspace)为VSCode IDE接入**云开发环境**提供技术支持。

### 2️⃣ CANN官方Docker镜像<a name="cann-docker-image"></a>

对于有NPU设备的用户，可使用CANN官方Docker镜像进行开发体验。

1. 确认主机环境

   - 是否已安装NPU驱动和固件，使用`npu-smi info`能够输出NPU相关信息，如没有安装，请参考《[CANN软件安装指南](https://www.hiascend.com/document/redirect/CannCommunityInstWizard)》中“准备软件包”和“安装NPU驱动和固件”章节。驱动与固件是运行态依赖，若仅编译本项目源码，可以不安装。
   - 是否已安装Docker，使用`docker --version`能够输出Docker版本信息，如没有安装，请参考[Docker官方安装指南](https://docs.docker.com/engine/install/)。

2. 下载CANN镜像

    从[昇腾镜像仓库](https://www.hiascend.com/developer/ascendhub/detail/17da20d1c2b6493cb38765adeba85884)拉取已预集成CANN镜像：

    ```bash
    # 示例：ascend/cann:tag为9.0.0-beta.2的CANN社区包
    # docker pull swr.cn-south-1.myhuaweicloud.com/ascendhub/cann:9.0.0-beta.2-910b-ubuntu22.04-py3.11
    docker pull <ascend/cann:tag>
    ```

    > [!NOTE] 使用说明
    > - 镜像默认安装了对应版本的CANN包，源码下载时注意与软件配套。
    > - 镜像文件比较大，正常网速下，下载时间约为5～10分钟，请您耐心等待。

3. 运行Docker

    拉取镜像后，需要以特定参数启动，以便容器内能访问宿主机的NPU设备。

    ```bash
    docker run --name <cann_container> \
        --ipc=host --net=host --privileged \
        --device /dev/davinci0 \
        --device /dev/davinci_manager \
        --device /dev/devmm_svm \
        --device /dev/hisi_hdc \
        -v /usr/local/dcmi:/usr/local/dcmi \
        -v /usr/local/bin/npu-smi:/usr/local/bin/npu-smi \
        -v /usr/local/Ascend/driver/lib64/:/usr/local/Ascend/driver/lib64/ \
        -v /usr/local/Ascend/driver/version.info:/usr/local/Ascend/driver/version.info \
        -v /etc/ascend_install.info:/etc/ascend_install.info \
        -v </home/your_host_dir>:</home/your_container_dir> \
        -it <ascend/cann:tag> bash
    ```

    | 参数 | 说明 | 注意事项 |
    | :--- | :--- | :--- |
    | `--name <cann_container>` | 为容器指定名称，便于管理 | 自定义 |
    | `--ipc=host` | 与宿主机共享IPC命名空间，NPU进程间通信（共享内存、信号量）所需 | - |
    | `--net=host` | 使用宿主机网络栈，避免容器网络转发带来的通信延迟 | - |
    | `--privileged` | 赋予容器完整设备访问权限，NPU驱动正常工作所需 | - |
    | `--device /dev/davinci0` | 将宿主机的NPU设备卡映射到容器内，可指定映射多张NPU设备卡 | 必须根据实际情况调整：`davinci0`对应系统中的第0张NPU卡。请先在宿主机执行`npu-smi info`命令，根据输出显示的设备号（如`NPU 0`, `NPU 1`）来修改此编号 |
    | `--device /dev/davinci_manager` | 映射NPU设备管理接口 | - |
    | `--device /dev/devmm_svm` | 映射设备内存管理接口 | - |
    | `--device /dev/hisi_hdc` | 映射主机与设备间的通信接口 | - |
    | `-v /usr/local/dcmi:/usr/local/dcmi` | 挂载设备容器管理接口（DCMI）相关工具和库 | - |
    | `-v /usr/local/bin/npu-smi:/usr/local/bin/npu-smi` | 挂载`npu-smi`工具 | 使容器内可以直接运行此命令来查询NPU状态和性能信息 |
    | `-v /usr/local/Ascend/driver/lib64/:/usr/local/Ascend/driver/lib64/` | 将宿主机的NPU驱动库映射到容器内 | - |
    | `-v /usr/local/Ascend/driver/version.info:/usr/local/Ascend/driver/version.info` | 挂载驱动版本信息文件 | - |
    | `-v /etc/ascend_install.info:/etc/ascend_install.info` | 挂载CANN软件安装信息文件 | - |
    | `-v </home/your_host_dir>:</home/your_container_dir>` | 挂载宿主机的一个路径到容器中 | 自定义 |
    | `-it` | `-i`（交互式）和`-t`（分配伪终端）的组合参数 | - |
    | `<ascend/cann:tag>` | 指定要运行的Docker镜像 | 请确保此镜像名和标签（tag）与您通过`docker pull`拉取的镜像完全一致 |
    | `bash` | 容器启动后立即执行的命令 | - |

### 3️⃣ DevContainer<a name="devcontainer"></a>

对于有NPU设备的用户，推荐使用DevContainer进行生态开发者贡献。

DevContainer基于VS Code Dev Containers，通过仓库内`.devcontainer`配置自动构建一致的容器化开发环境，内置`conda`、`Python`等开发工具链。与宿主机的NPU驱动共享设备访问，适合需要编译源码、运行UT、向本仓贡献代码的场景。详细说明请参考[`.devcontainer/README.md`](../../.devcontainer/README.md)。

> [!NOTE] 使用说明
> DevContainer仅挂载宿主机的NPU驱动（只读），**CANN toolkit和ops包需在容器启动后手动安装**，请参考[下载安装CANN包](#cann-install)。

### 📥 下载安装CANN包<a name="cann-install"></a>
<!-- 待加入AI Agent下载 & 安装CANN包的skill -->
CANN包分为CANN toolkit包和CANN ops包。

#### 下载CANN包

1. <a name="下载-cann-商用社区版"></a>下载CANN商用/社区版

    如果您想体验**官网正式发布的CANN包**，请访问[CANN安装部署-昇腾社区](https://www.hiascend.com/cann/download)获取对应版本CANN包。

2. <a name="下载-cann-master"></a>下载CANN master

    如果您想体验**CANN master**，请访问[CANN master obs镜像网站](https://ascend.devcloud.huaweicloud.com/artifactory/cann-run-mirror/software/master)，下载**日期最新**的CANN包。

#### 安装CANN包

1. 安装CANN toolkit包 (必选)

    ```bash
    chmod +x Ascend-cann-toolkit_${cann_version}_linux-$(uname -m).run
    ./Ascend-cann-toolkit_${cann_version}_linux-$(uname -m).run --install --install-path=${install_path}
    ```

2. 安装CANN ops包 (可选)

    ```bash
    chmod +x Ascend-cann-${soc_name}-ops_${cann_version}_linux-$(uname -m).run
    ./Ascend-cann-${soc_name}-ops_${cann_version}_linux-$(uname -m).run --install --install-path=${install_path}
    ```

    > [!IMPORTANT] 安装说明
    > [examples](../../examples)中部分算子样例的编译运行依赖本包，若想完整体验样例编译运行流程，建议安装此包。

| 参数 | 说明 |
| :--- | :--- |
| `${cann_version}` | CANN包版本号 |
| `${soc_name}` | NPU型号，如`910b` |
| `${install_path}` | 安装路径，toolkit包和ops包需相同。默认：root用户`/usr/local/Ascend`，非root用户`$HOME/Ascend` |

## ✅ 环境验证<a name="cann-verify"></a>

> [!NOTE] 使用前须知
> 云开发环境和CANN官方Docker镜像已预装CANN包，可直接执行以下命令验证；DevContainer和手动安装用户请在安装CANN包后执行。

验证环境和驱动是否正常：

- **检查NPU设备**：

    ```bash
    # 运行npu-smi，若能正常显示设备信息，则驱动正常
    npu-smi info
    ```

- **检查CANN包安装**：
  
    ```bash
    # 查看CANN Toolkit的version字段提供的版本信息（默认路径安装）。CANNLab场景下，请将/usr/local替换为/home/developer
    cat /usr/local/Ascend/cann/$(uname -m)-linux/ascend_toolkit_install.info
    # 查看CANN ops的version字段提供的版本信息（默认路径安装）。CANNLab场景下，请将/usr/local替换为/home/developer
    cat /usr/local/Ascend/cann/$(uname -m)-linux/ascend_ops_install.info
    ```

## ⚙️ 环境变量配置<a name="cann-env-setup"></a>

> [!NOTE] 使用前须知
> 云开发环境和CANN官方Docker镜像已自动配置环境变量，可跳过此步骤。

按需选择合适的命令使环境变量生效：

```bash
# 默认路径安装，以root用户为例（非root用户，将/usr/local替换为${HOME}）
source /usr/local/Ascend/cann/set_env.sh
# 指定路径安装
# source ${install_path}/cann/set_env.sh
```

## 🔨 源码编译步骤<a name="source-build"></a>

### 📥 下载源码<a name="source-download"></a>

开发者可通过如下命令下载本仓源码：

```bash
# 下载项目源码，以master分支为例
git clone https://gitcode.com/cann/asc-devkit.git
```

### 📦 依赖检查<a name="dependency-check"></a>

> [!NOTE] 使用前须知
> 如您使用**容器化技术**，容器中已为您安装好依赖，可跳过此步骤。

以下为本开源仓源码编译和examples算子样例编译运行的基础依赖条件，您可参考仓库内[DevContainer Python依赖（以 Python 3.12 为例）](../../.devcontainer/requirements.txt)：

- python >= 3.9.0
- gcc >= 7.3.0 / g++ >= 7.3.0 (注意：要求gcc与g++版本一致)
- cmake >= 3.16.0
- pkg-config >= 0.29.0

### ⚡ 编译安装<a name="compile&install"></a>

1. 编译

   本开源仓提供一键式编译安装能力。

   方式一：进入本开源仓代码根目录，执行如下命令：

   ```bash
   bash build.sh --pkg
   ```

   方式二：用户也可使用离线下载功能，手动下载[makeself源码包](https://gitcode.com/cann-src-third-party/makeself/releases/download/release-2.5.0-patch1.0/makeself-release-2.5.0-patch1.tar.gz)存放至自定义目录PATH_TO_DOWNLOAD，进入本开源仓代码根目录，执行如下命令：

   ```bash
   bash build.sh --pkg --cann_3rd_lib_path={PATH_TO_DOWNLOAD} #PATH_TO_DOWNLOAD为自定义下载目录
   ```

   编译完成后会在`build_out`目录下生成`cann-asc-devkit_${cann_version}_linux-$(uname -m).run`软件包。

   > [!CAUTION] 编译报错可能
   > 本仓依赖其他CANN开源仓，**暂不支持独立升级**，须搭配对应版本的CANN包进行编译：
   > - master分支 -- 使用**最新的**[CANN master包](#下载-cann-master)
   > - 特定Tag -- 使用对应版本的[官网正式发布的CANN包](#下载-cann-商用社区版)

2. 安装

   在开源仓根目录下执行下列命令，将编译生成的run包安装到默认路径`/usr/local/Ascend`；或安装到指定的CANN包的装包路径`${install_path}`，同时会覆盖原CANN包中的Ascend C内容。

   ```bash
   cd build_out
   # 默认路径安装run包
   ./cann-asc-devkit_${cann_version}_linux-$(uname -m).run --full
   # 指定路径安装run包
   # ./cann-asc-devkit_${cann_version}_linux-$(uname -m).run --full --install-path=${install_path}
   ```

### 🧪 UT验证<a name="ut-verify"></a>

#### 安装依赖

- pytest >= 8.0.0

    执行以下命令安装：

    ```bash
    pip3 install "pytest>=8.0.0"
    ```

- coverage >= 4.5.4

    执行以下命令安装：

    ```bash
    pip3 install "coverage>=4.5.4"
    ```

- lcov >= 1.16 (仅在执行覆盖率统计场景需要)

    下载[lcov源码](https://gitcode.com/cann-src-third-party/lcov/releases/download/v1.16/lcov-1.16.tar.gz)后，执行以下命令安装：

    ```bash
    tar -xf lcov-1.16.tar.gz
    cd lcov-1.16
    make install         # root用户安装
    # sudo make install  # 非root用户安装
    ```

#### UT执行

方式一：在开源仓根目录执行下列命令，将按各模块依次批跑tests目录下的用例，得到结果日志，用于看护编译是否正常。

```bash
bash build.sh --adv_test                         # 批跑tests目录下adv_api里的用例
bash build.sh --basic_test_one                   # 批跑tests目录下basic_api part-one里的用例
bash build.sh --basic_test_two                   # 批跑tests目录下basic_api part-two里的用例
bash build.sh --basic_test_three                 # 批跑tests目录下basic_api part-three里的用例
```

方式二：用户也可使用离线下载功能，手动下载[三方库源码包](#开源第三方软件依赖)存放至自定义目录PATH_TO_DOWNLOAD，在开源仓根目录执行下列命令，同时批跑执行各模块的用例。

```bash
# 以PATH_TO_DOWNLOAD为自定义下载目录为例
bash build.sh --adv_test --cann_3rd_lib_path={PATH_TO_DOWNLOAD}          # 批跑tests目录下adv_api里的用例
bash build.sh --basic_test_one --cann_3rd_lib_path={PATH_TO_DOWNLOAD}    # 批跑tests目录下basic_api part-one里的用例
bash build.sh --basic_test_two --cann_3rd_lib_path={PATH_TO_DOWNLOAD}    # 批跑tests目录下basic_api part-two里的用例
bash build.sh --basic_test_three --cann_3rd_lib_path={PATH_TO_DOWNLOAD}  # 批跑tests目录下basic_api part-three里的用例
```

#### 开源第三方软件依赖

在执行UT时，依赖的第三方开源软件列表如下：

| 开源软件 | 版本 | 下载地址 |
| :---: | :---: | :---: |
| googletest | 1.14.0 | [googletest-1.14.0.tar.gz](https://gitcode.com/cann-src-third-party/googletest/releases/download/v1.14.0/googletest-1.14.0.tar.gz) |
| boost | 1.87.0 | [boost_1_87_0.tar.gz](https://gitcode.com/cann-src-third-party/boost/releases/download/v1.87.0/boost_1_87_0.tar.gz) |
| mockcpp | 2.7 | [mockcpp-2.7.tar.gz](https://gitcode.com/cann-src-third-party/mockcpp/releases/download/v2.7-h3/mockcpp-2.7.tar.gz) |
| mockcpp_patch | 2.7 | [mockcpp-2.7_py3-h3.patch](https://gitcode.com/cann-src-third-party/mockcpp/releases/download/v2.7-h3/mockcpp-2.7_py3-h3.patch) |
