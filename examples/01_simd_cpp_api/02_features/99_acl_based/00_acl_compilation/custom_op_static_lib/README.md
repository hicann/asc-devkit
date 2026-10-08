# 自定义算子静态库

## 概述

本样例以`AddCustom`为例，展示如何编译、打包并链接自定义算子静态库，通过aclnn的方式执行算子。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── custom_op_static_lib
│   ├── CMakeLists.txt            // 顶层编译工程文件
│   ├── app                       // 链接静态库并执行算子
│   ├── op                        // 编译打包生成自定义算子静态库
│   └── README.md                 // 样例说明文档
```

## 代码实现介绍

本样例中`op`目录负责编译打包生成自定义算子静态库，生成的静态库路径为`./build/customize-install/lib/lib${package_name}.a`；`app`目录通过`find_package(${package_name})`引入静态库，编译`main.cpp`生成`execute_add_op`，调用`aclnnAddCustom`并校验结果。

AddCustom算子介绍等内容可参考[算子描述](../custom_op/README.md)。

## 编译运行

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。需确保`ASCEND_HOME_PATH`指向CANN安装根路径，以便头文件与库路径生效。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  mkdir -p build && cd build
  python3 ../app/scripts/gen_data.py
  cmake .. && make -j
  ./execute_add_op
  ```

  执行结果如下，说明执行成功。

  ```text
  test pass
  ```
