# fma接口样例

## 概述

本样例介绍如何使用实验性Tensor API `asc::te::experimental::fma` 对三组`float` Tensor执行逐元素融合乘加计算。

```text
dst[i] = src0[i] * src1[i] + src2[i]
```

## 支持的产品

- Ascend 950PR&950DT系列产品

## 目录结构介绍

```text
├── CMakeLists.txt                 // 编译工程
├── README.md                      // 样例说明
├── README_en.md                   // 英文样例说明
└── fma.asc                        // fma接口样例实现
```

## 样例描述

- 样例功能：

  样例将三组输入Tensor从GM搬运到UB，通过`load`加载到寄存器，调用`fma`完成逐元素融合乘加，再通过`store`将结果写回UB和GM。Host侧生成输入并校验输出。

- 样例场景：

  | SCENARIO_NUM | Tensor Layout | 说明 |
  | --- | --- | --- |
  | 1 | 一维，shape为`[128]` | 按一维偏移处理两个寄存器块。 |
  | 2 | 二维，shape为`[2, 64]` | 按二维坐标处理每一行。 |

- 样例实现：

  - Kernel关键步骤

    1. 根据`SCENARIO_NUM`构造一维或二维GM、UB Tensor。
    2. 使用`copy(dst, src)`完成GM与UB之间的数据搬运。
    3. 使用`update_mask`、`load`、`fma`和`store`完成逐元素融合乘加。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  配置CANN软件包环境变量，详细操作请参考[环境变量配置](../../../../../../docs/zh/quick_start.md#prepare&install)。

  ```bash
  source ${install_path}/set_env.sh
  ```

  `${install_path}`为CANN软件包安装目录，请根据实际安装路径替换。

- 样例执行

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  ./demo
  ```

  使用NPU仿真模式时，在CMake命令中增加`-DCMAKE_ASC_RUN_MODE=sim`。切换运行模式、芯片型号或场景时，建议清除`build`目录中的CMake缓存后重新配置。

- 执行结果

  验证成功时输出：

  ```text
  test pass!
  ```
