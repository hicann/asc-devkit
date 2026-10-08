# Power样例

## 概述

本样例基于Power高阶API实现按元素做幂运算功能，支持三种功能：指数和底数分别为张量对张量、张量对标量、标量对张量的幂运算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── power
│   ├── scripts
│   │   └── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── power.asc               // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  实现按元素做幂运算功能，支持三种功能：指数和底数分别为张量对张量、张量对标量、标量对张量的幂运算。

  计算公式如下：
  $$Power(x, y) = x^y$$

  张量对张量，mode = 0：两个长度相同的张量，逐元素做幂运算
  $$dstTensor_i = Power(srcbaseTensor_i, srcexpTensor_i)$$

  张量对标量， mode = 1：以标量作为指数，张量都用同一个指数进行幂运算
  $$dstTensor_i = Power(srcbaseTensor_i, srcexpScalar)$$

  标量对张量， mode = 2：以标量作为固定的底数，张量都用同一个底数进行幂运算
  $$dstTensor_i = Power(srcbaseScalar, srcexpTensor_i)$$

- 样例规格：  
  <table>
  <caption>表1：样例输入输出规格</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> power </td></tr>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">srcbase</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">srcexp</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">power_custom</td></tr>
  </table>

- 场景说明：
  <table>
  <caption>表2：SCENARIO参数说明</caption>
  <tr><td align="center">SCENARIO</td><td align="center">底数</td><td align="center">指数</td><td align="center">说明</td></tr>
  <tr><td align="center">0</td><td align="center">张量</td><td align="center">张量</td><td align="center">底数和指数都是张量</td></tr>
  <tr><td align="center">1</td><td align="center">张量</td><td align="center">标量</td><td align="center">底数是张量，指数是标量</td></tr>
  <tr><td align="center">2</td><td align="center">标量</td><td align="center">张量</td><td align="center">底数是标量，指数是张量</td></tr>
  </table>

- 样例实现：  
  本样例中实现的是固定shape为输入srcbase[1, 16]、srcexp[1, 16]，输出dst[1, 16]的power_custom样例。样例功能mode参数默认为0，即指数和底数都为张量。

  - Kernel实现

    使用Power高阶API进行幂运算，支持张量对张量、张量对标量、标量对张量三种模式

  - Tiling实现

    Host侧通过GetPowerMaxMinTmpSize获取Power接口计算所需的最大和最小临时空间。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 编译运行  

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO=0
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO=$SCENARIO ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py --scenario $SCENARIO  # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO` | `0`（默认）、`1`、`2` | 场景：0-张量对张量，1-张量对标量，2-标量对张量 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
