# 自定义算子工程+aclop单算子模型调用样例

## 概述

本样例基于示例自定义算子工程，介绍了`aclopExecuteV2`单算子模型的方式执行固定shape算子。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |
| Atlas 200I/500 A2推理产品 | >= CANN 9.0.0 |
| Atlas推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── aclop_invocation
│   ├── add_custom.json         // 单算子描述文件（atc模型转换输入）
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── main.cpp                // 算子调用主程序
│   └── README.md               // 样例说明文档
```

## 代码实现简介

完成自定义算子的开发部署后，可以通过单算子模型调用的方式来验证单算子的功能。具体可参考[单算子调用](https://hiascend.com/document/redirect/CannCommunityCppOpcall)中“单算子模式执行”相关章节。

单算子模型调用，在离线模式下需要提前通过`atc --singleop`生成单算子离线模型，并在应用中配置模型目录。

本样例中通过`aclopExecuteV2`接口执行单算子模型，核心流程如下：
1. 通过`aclCreateTensorDesc`创建输入/输出Tensor描述，并使用`aclCreateDataBuffer`准备Device侧数据Buffer。
2. 调用`aclopSetModelDir`指定模型目录后，使用`aclopExecuteV2`执行算子计算。
3. 同步流`aclrtSynchronizeStream`，并将Device侧结果拷回Host侧进行结果验证。

## 编译运行

- 编译、打包和部署自定义算子工程

  运行此样例前，需先进入[自定义算子工程样例](../../00_acl_compilation/custom_op)目录完成编译、打包和部署。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  mkdir -p build && cd build
  python3 ../scripts/gen_data.py
  # 使用atc模型转换工具生成单算子离线模型
  atc --singleop=../add_custom.json --output=. --soc_version=${soc_version}
  cmake .. && make -j
  ./execute_add_op
  ```

  > AI处理器的型号`${soc_version}`请通过如下方式获取：
  > - 针对如下产品型号：在安装昇腾AI处理器的服务器执行`npu-smi info`命令进行查询，获取**Name**信息。实际配置值为AscendName，例如**Name**取值为xxxyy，实际配置值为Ascendxxxyy。
  >   - Atlas A2系列产品
  >   - Atlas 200I/500 A2推理产品
  >   - Atlas推理系列产品
  >   - Atlas训练系列产品
  >
  > - 针对如下产品型号，在安装昇腾AI处理器的服务器执行`npu-smi info -t board -i <id> -c <chip_id>`命令进行查询，获取**Chip Name**和**NPU Name**信息，实际配置值为Chip Name_NPU Name。例如**Chip Name**取值为Ascendxxx，**NPU Name**取值为1234，实际配置值为Ascendxxx_1234。其中：
  >
  >   id：设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id
  >
  >   chip_id：芯片id，通过`npu-smi info -m`命令查出的Chip ID即为芯片id
  >   - Ascend 950PR&950DT系列产品
  >   - Atlas A3系列产品
  >
  >   基于同系列的AI处理器型号创建的算子工程，其基础功能（基于该工程进行算子开发、编译和部署）通用。

  执行结果如下，说明执行成功。

  ```text
  test pass
  ```
