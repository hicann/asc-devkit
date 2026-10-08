# 自定义样例工程+TensorFlow内置样例

## 概述

本样例基于Add计算展示如何将Ascend C自定义算子映射到TensorFlow内置算子，并通过TensorFlow调用。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── tensorflow_builtin
│   ├── run_add_custom.py       // TensorFlow调用脚本（映射到内置AddV2算子）
│   └── README.md               // 样例说明文档
```

## 代码实现介绍

完成样例工程创建后，会在样例工程目录下生成framework/tf_plugin目录，用于存放TensorFlow框架适配插件实现文件。

本样例展示将Add计算映射到TensorFlow内置算子AddV2上，核心流程如下：
1. 使用`np.random.uniform`生成随机输入数据，并通过`tf.compat.v1.placeholder`定义输入节点。
2. 构建计算图：使用`tf.math.add`实现张量加法运算。
3. 分别创建CPU和NPU会话，通过`session.run`执行计算图并传入输入数据。
4. 使用`np.allclose`对比NPU和CPU计算结果，验证计算正确性。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 编译、打包和部署自定义样例工程

  运行此样例前，需先进入[自定义样例工程样例](../../../99_acl_based/00_acl_compilation/custom_op)目录完成编译、打包和部署。

  > [!NOTE]注意
  > 需适配插件代码，路径为： `examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/framework/tf_plugin/tensorflow_add_custom_plugin.cc`，需修改插件代码中的TensorFlow调用算子名称OriginOpType为"AddV2"，如下所示：
  >
  > ```cpp
  > REGISTER_CUSTOM_OP("AddCustom")
  >   .FrameworkType(TENSORFLOW)      // type: TENSORFLOW
  >   .OriginOpType("AddV2")      // name in tf module
  >   .ParseParamsByOperatorFn(AutoMappingByOpFn);
  > ```

- 安装TensorFlow插件包

  请参考[《TensorFlow 2.6.5模型迁移》](https://www.hiascend.com/document/redirect/canncommercial-tfmigr26)中的“安装框架插件包”章节，获取详细的安装指南和步骤。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  python3 run_add_custom.py
  ```

  执行结果如下，说明执行成功。

  ```text
  test pass
  ```
