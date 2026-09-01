# 使用C_API实现Add算子样例（RegBase场景）

## 概述

本样例采用C_API接口编写Add算子样例。基于同步搬运、计算接口实现。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```
├── c_api_simd_add
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── c_api_add.asc           // Ascend C 算子实现 & 调用样例
│   └── README.md
```

## 算子描述

- 算子功能：  
  Add算子实现了两个数据相加，返回相加结果的功能。对应的数学表达式为：  

  ```
  z = x + y
  ```

- 算子规格：
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">z</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- 算子实现：

  - kernel实现  

    C_API输入数据需要先搬运进片上存储，再加载到Reg矢量计算寄存器，然后使用计算接口完成两个输入参数相加，得到计算结果，搬出到Local Memory，再搬出到外部存储上。

    Add算子的实现流程分为3个步骤：

    第一步将Global Memory上的输入x和y搬运到Local Memory，分别存储在xLocal、yLocal。

    第二步从Unified Buffer加载数据到寄存器src0和src1，使用`asc_add`对寄存器数据执行加法操作，结果存储在寄存器dst中，计算完成后将结果搬回zLocal。

    第三步将输出数据从zLocal搬运至Global Memory上的输出z中。

  - 向量计算中的mask控制

    在向量计算指令中，mask用于控制向量寄存器的哪些通道参与计算。本样例通过`asc_update_mask_b32`函数设置32位的向量掩码，控制256位向量寄存器中每个通道的有效性。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

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
  # 在build目录执行以下内容
  ./c_api_add_example           # 执行样例
  ```

  执行结果如下，说明精度对比成功。

  ```bash
  [Success] Case accuracy is verification passed.
  ```
