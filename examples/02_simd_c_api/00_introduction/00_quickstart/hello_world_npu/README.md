# HelloWorld算子直调样例

## 概述

本样例通过使用<<<>>>内核调用符来完成算子核函数在NPU侧运行验证的基础流程，核函数内通过printf打印输出结果。

## 支持的产品

- Ascend 950PR&950DT系列产品
- Atlas A3系列产品
- Atlas A2系列产品

## 目录结构介绍

```
├── hello_world_npu
│   ├── CMakeLists.txt      // 编译工程文件
│   └── hello_world.asc     // Ascend C算子实现 & 调用样例
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
  - 默认路径，root用户安装CANN软件包
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - 默认路径，非root用户安装CANN软件包
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - 指定路径install_path，安装CANN软件包
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- 样例执行
  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake ..;make -j;             # 编译工程
  ./demo                        # 执行样例
  ```
  执行结果如下，说明执行成功。
  ```bash
  [Block (0/8)]: Hello World!!!
  [Block (1/8)]: Hello World!!!
  [Block (2/8)]: Hello World!!!
  [Block (3/8)]: Hello World!!!
  [Block (4/8)]: Hello World!!!
  [Block (5/8)]: Hello World!!!
  [Block (6/8)]: Hello World!!!
  [Block (7/8)]: Hello World!!!
  ```