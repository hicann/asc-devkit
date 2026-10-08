# 自定义算子工程+ONNX模型调用样例

## 概述

本样例以LeakyRelu计算为例介绍通过onnx网络调用的方式调用自定义算子。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── onnx_plugin
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── leaky_relu.py            // 生成LeakyRelu ONNX模型的脚本
│   ├── main.cpp                 // 算子调用主程序（加载om模型执行LeakyRelu推理）
│   └── README.md                // 样例说明文档
```

## 样例实现

完成自定义算子的开发部署后，可以通过onnx模型调用方式验证样例功能。将LeakyRelu onnx模型转换为om模型后，可在应用中加载模型并执行推理。

本样例中通过`aclmdlExecute`接口执行模型推理，核心流程如下：
1. 通过`aclmdlLoadFromFile`加载om模型并获取模型ID，调用`aclmdlGetDesc`获取模型描述信息。
2. 调用`aclmdlCreateDataset`创建输入/输出dataset，并基于`aclmdlGetInputSizeByIndex`/`aclmdlGetOutputSizeByIndex`申请Device内存并创建DataBuffer。
3. 构造Host侧输入数据，`aclrtMemcpy`拷贝至Device侧，调用`aclmdlExecute`执行推理，并将结果拷回Host侧进行结果验证。

## 编译运行

  在本样例根目录下执行如下步骤，编译并执行样例。
  - 编译、打包和部署自定义样例工程

    运行此样例前，需先进入[自定义算子工程样例](../../../99_acl_based/00_acl_compilation/custom_op)目录完成编译、打包和部署。

- 安装onnx

  ```bash
  pip3 install onnx==1.16.0
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build; cd build
  python3 ../leaky_relu.py
  # 使用atc模型转换工具将*.onnx格式的模型转换成*.om格式的模型
  atc --model=./leaky_relu.onnx --framework=5 --soc_version=${soc_version} --output=./leaky_relu_custom --input_shape="X:8,16,1024" --input_format=ND
  cmake .. && make -j
  ./execute_leaky_relu_op
  ```

  > AI处理器的型号<soc_version>请通过如下方式获取：
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
  >   基于同系列的AI处理器型号创建的样例工程，其基础功能（基于该工程进行样例开发、编译和部署）通用。

  执行结果如下，说明执行成功。

  ```text
  test pass
  ```
