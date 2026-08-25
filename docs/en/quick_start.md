# Quick Start

## 🛠️ Environment Preparation<a name="prepare&install"></a>

Choose the appropriate environment preparation method based on **whether you have an NPU device locally** and **your usage goal**:

| Environment Preparation | Community Experience / Operator Development (CANN Commercial/Community Edition) | Ecosystem Developer Contribution (CANN master) |
| :---: | :---: | :---: |
| **No NPU device** | [Cloud Development Environment](#1️⃣-cloud-development-environment) | [Manually Download and Install CANN master](#📥-download-and-install-cann-package) + [Cloud Development Environment](#1️⃣-cloud-development-environment) |
| **Have NPU device** | [CANN Official Docker Image](#2️⃣-cann-official-docker-image) | [Manually Download and Install CANN master](#📥-download-and-install-cann-package) + [Dev Container](#3️⃣-devcontainer) |

> [!TIP] Recommendations
>
> - To ensure development environment quality, we recommend users complete **environment preparation** based on **containerization technology**.
> - If you prefer not to use containers, you can also complete **environment preparation** on a host with NPU device. Refer to [CANN Software Installation Guide - Install on Physical Machine](https://www.hiascend.com/cann/download).
> - For users who only want to experience "compile and install this open source repository + run operators in simulation environment", the host does not require an NPU device. You can skip NPU driver and firmware installation and directly install the CANN package. Refer to [Download and Install CANN Package](#📥-download-and-install-cann-package).

<!-- AI Agent enablement logic to be added -->

### 1️⃣ Cloud Development Environment

For users without NPU devices, you can use the NPU computing resources provided by the **cloud development environment** for development experience. The **cloud development environment** provides an online directly runnable Ascend ARM architecture environment. Currently only applicable to Atlas A2 series products, it provides two access methods:

- **WebIDE Development Platform**, that is "**One-stop Development Platform**", provides web-based portable development experience.
- **VSCode IDE**, supports remote connection to **cloud development environment**, provides VSCode powerful plugin marketplace support.

1. Go to the open source repository Gitcode page and click the "`Cloud Development`" button. Log in with a verified Huawei Cloud account. If not registered or verified, follow the page prompts to register and verify.

   <p align="center"><img src="../zh/figures/cloudIDE.png" alt="Cloud Platform" width="750px" height="90px"></p>

2. Follow the page prompts to create and start the cloud development environment. Click "`Connect > WebIDE or Visual Studio Code`" to enter the cloud development environment. Open source project resources are in the `/mnt/workspace` directory by default.

   <p align="center"><img src="../zh/figures/webIDE.png" alt="Cloud Platform" width="1000px" height="150px"></p>

> [!NOTE] Usage Instructions
>
> - The environment has the latest commercial NPU driver and firmware, CANN package pre-installed. Pay attention to software compatibility when downloading source code.
> - To download a specific version of CANN package, refer to [Download and Install CANN Package](#📥-download-and-install-cann-package).
> - For more information about **WebIDE Development Platform**, refer to [Cloud Development Platform Introduction](https://gitcode.com/org/cann/discussions/54).
> - [Huawei Developer Space plugin](https://marketplace.visualstudio.com/items?itemName=HuaweiCloud.developerspace) provides technical support for VSCode IDE to access **cloud development environment**.

### 2️⃣ CANN Official Docker Image

For users with NPU devices, you can use the CANN official Docker image for development experience.

1. Verify host environment

   - Check if NPU driver and firmware are installed. Use `npu-smi info` to output NPU information. If not installed, refer to [CANN Software Installation Guide - Install on Physical Machine](https://www.hiascend.com/cann/download).
   - Check if Docker is installed. Use `docker --version` to output Docker version information. If not installed, refer to [Docker Official Installation Guide](https://docs.docker.com/engine/install/).

2. Download CANN image

    Pull the CANN pre-integrated image from [Ascend Image Repository](https://www.hiascend.com/developer/ascendhub/detail/17da20d1c2b6493cb38765adeba85884):

    ```bash
    # Example: ascend/cann:tag is 9.0.0-beta.2 CANN community package
    # docker pull swr.cn-south-1.myhuaweicloud.com/ascendhub/cann:9.0.0-beta.2-910b-ubuntu22.04-py3.11
    docker pull <ascend/cann:tag>
    ```

    > [!NOTE] Usage Instructions
    > - The image has the corresponding version of CANN package pre-installed. Pay attention to software compatibility when downloading source code.
    > - The image file is relatively large. Under normal network speed, download time is about 5-10 minutes. Please wait patiently.

3. Run Docker

    After pulling the image, you need to start with specific parameters so that the container can access the host's NPU device.

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

    | Parameter | Description | Notes |
    | :--- | :--- | :--- |
    | `--name <cann_container>` | Specify name for container for management | Custom |
    | `--ipc=host` | Share IPC namespace with host, required for NPU inter-process communication (shared memory, semaphore) | - |
    | `--net=host` | Use host network stack, avoid communication latency from container network forwarding | - |
    | `--privileged` | Grant container full device access permissions, required for NPU driver to work properly | - |
    | `--device /dev/davinci0` | Map host NPU device card to container, can specify mapping multiple NPU device cards | Must adjust according to actual situation: `davinci0` corresponds to the 0th NPU card in the system. Please execute `npu-smi info` command on the host first, and modify this number according to the device number shown in the output (such as `NPU 0`, `NPU 1`) |
    | `--device /dev/davinci_manager` | Map NPU device management interface | - |
    | `--device /dev/devmm_svm` | Map device memory management interface | - |
    | `--device /dev/hisi_hdc` | Map communication interface between host and device | - |
    | `-v /usr/local/dcmi:/usr/local/dcmi` | Mount device container management interface (DCMI) related tools and libraries | - |
    | `-v /usr/local/bin/npu-smi:/usr/local/bin/npu-smi` | Mount `npu-smi` tool | Enables running this command directly in the container to query NPU status and performance information |
    | `-v /usr/local/Ascend/driver/lib64/:/usr/local/Ascend/driver/lib64/` | Map host NPU driver libraries to container | - |
    | `-v /usr/local/Ascend/driver/version.info:/usr/local/Ascend/driver/version.info` | Mount driver version information file | - |
    | `-v /etc/ascend_install.info:/etc/ascend_install.info` | Mount CANN software installation information file | - |
    | `-v </home/your_host_dir>:</home/your_container_dir>` | Mount a host path to container | Custom |
    | `-it` | Combination of `-i` (interactive) and `-t` (allocate pseudo-terminal) | - |
    | `<ascend/cann:tag>` | Specify Docker image to run | Ensure this image name and tag match exactly the image you pulled via `docker pull` |
    | `bash` | Command to execute immediately after container starts | - |

### 3️⃣ DevContainer

For users with NPU devices, we recommend using DevContainer for ecosystem developer contribution.

DevContainer is based on VS Code Dev Containers. It automatically builds a consistent containerized development environment through `.devcontainer` configuration in the repository. It has built-in `conda`, `Python` and other development toolchains. It shares device access with the host's NPU driver, suitable for scenarios that require compiling source code, running UT, and contributing code to this repository. For detailed instructions, refer to [`.devcontainer/README_en.md`](../../.devcontainer/README_en.md).

> [!NOTE] Usage Instructions
> DevContainer only mounts the host's NPU driver (read-only). **CANN toolkit and ops packages need to be manually installed after container starts**. Refer to [Download and Install CANN Package](#📥-download-and-install-cann-package).

### 📥 Download and Install CANN Package<a name="cann-install"></a>
<!-- AI Agent download & install CANN package skill to be added -->
CANN packages are divided into CANN toolkit package and CANN ops package.

#### Download CANN Package

1. <a name="download-cann-commercial-community-edition"></a>Download CANN Commercial/Community Edition

    If you want to experience the **officially released CANN package**, visit [CANN Installation Deployment - Ascend Community](https://www.hiascend.com/cann/download) to get the corresponding version of CANN package.

2. <a name="download-cann-master"></a>Download CANN master

    If you want to experience **CANN master**, visit [CANN master obs mirror site](https://ascend.devcloud.huaweicloud.com/artifactory/cann-run-mirror/software/master) to download the **latest dated** CANN package.

#### Install CANN Package

1. Install CANN toolkit package (Required)

    ```bash
    chmod +x Ascend-cann-toolkit_${cann_version}_linux-$(uname -m).run
    ./Ascend-cann-toolkit_${cann_version}_linux-$(uname -m).run --install --install-path=${install_path}
    ```

2. Install CANN ops package (Optional)

    ```bash
    chmod +x Ascend-cann-${soc_name}-ops_${cann_version}_linux-$(uname -m).run
    ./Ascend-cann-${soc_name}-ops_${cann_version}_linux-$(uname -m).run --install --install-path=${install_path}
    ```

    > [!IMPORTANT] Installation Instructions
    > Some operator samples in [examples](../../examples) depend on this package for compilation and running. To fully experience the sample compilation and running process, we recommend installing this package.

| Parameter | Description |
| :--- | :--- |
| `${cann_version}` | CANN package version number |
| `${soc_name}` | NPU model, for example, `910b` |
| `${install_path}` | Installation path, must be the same for toolkit and ops packages. Default: `/usr/local/Ascend` for root user, `$HOME/Ascend` for non-root user |

## ✅ Environment Verification<a name="cann-verify"></a>

> [!NOTE] Before Use
> Cloud development environment and CANN official Docker image have CANN package pre-installed. You can directly execute the following commands to verify. DevContainer and manual installation users should execute after installing CANN package.

Verify that the environment and driver are working properly:

- **Check NPU device**:

    ```bash
    # Run npu-smi, if device information displays normally, driver is working
    npu-smi info
    ```

- **Check CANN package installation**:
  
    ```bash
    # View version information provided by CANN package's version field (default path installation). For WebIDE scenario, replace /usr/local with /home/developer
    cat /usr/local/Ascend/cann/$(uname -m)-linux/ascend_toolkit_install.info
    cat /usr/local/Ascend/cann/$(uname -m)-linux/ascend_ops_install.info
    ```

## ⚙️ Environment Variable Configuration<a name="cann-env-setup"></a>

> [!NOTE] Before Use
> Cloud development environment and CANN official Docker image have automatically configured environment variables. You can skip this step.

Choose the appropriate command to activate environment variables as needed:

```bash
# Default path installation, using root user as example (for non-root user, replace /usr/local with ${HOME})
source /usr/local/Ascend/cann/set_env.sh
# Specified path installation
# source ${install_path}/cann/set_env.sh
```

## 🔨 Source Code Compilation Steps

### 📥 Download Source Code

Developers can download this repository's source code with the following command:

```bash
# Download project source code, using master branch as example
git clone https://gitcode.com/cann/asc-devkit.git
```

### 📦 Dependency Check

> [!NOTE] Before Use
> If you use **containerization technology**, dependencies are already installed in the container. You can skip this step.

The following are basic dependencies for source code compilation and examples operator compilation and running in this open source repository:

- python >= 3.9.0
- gcc >= 7.3.0 / g++ >= 7.3.0 (Note: gcc and g++ versions must be consistent)
- cmake >= 3.16.0

### ⚡ Compile and Install<a name="compile&install"></a>

1. Compile

   This open source repository provides one-click compilation and installation capability.

   Method 1: Go to the open source repository code root directory and execute the following command:

   ```bash
   bash build.sh --pkg
   ```

   Method 2: Users can also use offline download function to manually download [makeself source package](https://gitcode.com/cann-src-third-party/makeself/releases/download/release-2.5.0-patch1.0/makeself-release-2.5.0-patch1.tar.gz) and save it to a custom directory PATH_TO_DOWNLOAD. Go to the open source repository code root directory and execute the following command:

   ```bash
   bash build.sh --pkg --cann_3rd_lib_path={PATH_TO_DOWNLOAD} #PATH_TO_DOWNLOAD is custom download directory
   ```

   After compilation completes, `cann-asc-devkit_${cann_version}_linux-$(uname -m).run` software package will be generated in the `build_out` directory.

   > [!CAUTION] Possible Compilation Errors
   > This repository depends on other CANN open source repositories. **Independent upgrade is not supported temporarily**. It must be compiled with the corresponding version of CANN package:
   > - master branch -- Use **latest** [CANN master package](#download-cann-master)
   > - Specific Tag -- Use corresponding version of [officially released CANN package](#download-cann-commercial-community-edition)

2. Install

   Execute the following command in the open source repository root directory to install the compiled run package to the default path `/usr/local/Ascend`. Or install to the specified CANN package installation path `${install_path}`, which will overwrite the original Ascend C content in the CANN package.

   ```bash
   cd build_out
   # Default path installation of run package
   ./cann-asc-devkit_${cann_version}_linux-$(uname -m).run --full
   # Specified path installation of run package
   # ./cann-asc-devkit_${cann_version}_linux-$(uname -m).run --full --install-path=${install_path}
   ```

### 🧪 UT Verification

#### Install Dependencies

- pytest >= 8.0.0

    Execute the following command to install:

    ```bash
    pip3 install "pytest>=8.0.0"
    ```

- coverage >= 4.5.4

    Execute the following command to install:

    ```bash
    pip3 install "coverage>=4.5.4"
    ```

- lcov >= 1.16 (Only required for coverage statistics scenario)

    Download [lcov source code](https://gitcode.com/cann-src-third-party/lcov/releases/download/v1.16/lcov-1.16.tar.gz), then execute the following command to install:

    ```bash
    tar -xf lcov-1.16.tar.gz
    cd lcov-1.16
    make install         # root user installation
    # sudo make install  # non-root user installation
    ```

#### UT Execution

Method 1: Execute the following command in the open source repository root directory. It will run test cases in the tests directory by module sequentially and generate result logs for monitoring compilation status.

```bash
bash build.sh --adv_test                         # Run test cases in tests/adv_api
bash build.sh --basic_test_one                   # Run test cases in tests/basic_api part-one
bash build.sh --basic_test_two                   # Run test cases in tests/basic_api part-two
bash build.sh --basic_test_three                 # Run test cases in tests/basic_api part-three
```

Method 2: Users can also use offline download function to manually download [third-party library source packages](#open-source-third-party-software-dependencies) and save them to a custom directory PATH_TO_DOWNLOAD. Execute the following command in the open source repository root directory to run test cases for each module.

```bash
# Using PATH_TO_DOWNLOAD as custom download directory as example
bash build.sh --adv_test --cann_3rd_lib_path={PATH_TO_DOWNLOAD}          # Run test cases in tests/adv_api
bash build.sh --basic_test_one --cann_3rd_lib_path={PATH_TO_DOWNLOAD}    # Run test cases in tests/basic_api part-one
bash build.sh --basic_test_two --cann_3rd_lib_path={PATH_TO_DOWNLOAD}    # Run test cases in tests/basic_api part-two
bash build.sh --basic_test_three --cann_3rd_lib_path={PATH_TO_DOWNLOAD}  # Run test cases in tests/basic_api part-three
```

#### Open Source Third-Party Software Dependencies

The following third-party open source software are required when executing UT:

| Open Source Software | Version | Download URL |
| :---: | :---: | :---: |
| googletest | 1.14.0 | [googletest-1.14.0.tar.gz](https://gitcode.com/cann-src-third-party/googletest/releases/download/v1.14.0/googletest-1.14.0.tar.gz) |
| boost | 1.87.0 | [boost_1_87_0.tar.gz](https://gitcode.com/cann-src-third-party/boost/releases/download/v1.87.0/boost_1_87_0.tar.gz) |
| mockcpp | 2.7 | [mockcpp-2.7.tar.gz](https://gitcode.com/cann-src-third-party/mockcpp/releases/download/v2.7-h3/mockcpp-2.7.tar.gz) |
| mockcpp_patch | 2.7 | [mockcpp-2.7_py3-h3.patch](https://gitcode.com/cann-src-third-party/mockcpp/releases/download/v2.7-h3/mockcpp-2.7_py3-h3.patch) |
