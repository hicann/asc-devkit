# 矩阵乘法中LoadDataWithStride数据搬运示例


## 概述

本样例介绍LoadDataWithStride指令在矩阵乘法中的使用场景和方法。配合SetLoadDataRepeatWithStride可以将二维的A、B矩阵从L1搬运到L0A/L0B，其中 A 和 B 分别表示矩阵乘法的左右输入矩阵。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── load_data_with_stride
│   ├── figures                     // 图示
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── load_data_with_stride.asc   // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

LoadDataWithStride指令对于二维矩阵的转置能力和支持的数据类型，具体来说：

- 支持数据类型为：int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/half/bfloat16_t/int32_t/uint32_t/float。
- 仅在目的地址位于L0A上，并且类型为b8/b16/b32时，enTranspose参数有效，能够决定是否启用转置功能。
  目的地址位于L0B上时，会自动进行转置，enTranspose参数无效。

由于本样例暂不支持输入数据类型为int4b_t，因此本样例展示了以下六种LoadDataWithStride在矩阵乘法的使用：

### LoadDataWithStride接口场景对照表

| scenarioNum | 输入数据类型 | A矩阵转置 | B矩阵转置 |
| --- | --- | --- | --- |
| 1 | int8_t | 不转置 | 不转置 |
| 2 | int8_t | 转置 | 不转置 |
| 3 | half | 不转置 | 不转置 |
| 4 | half | 转置 | 不转置 |
| 5 | float | 不转置 | 不转置 |
| 6 | float | 转置 | 不转置 |

### 使用LoadDataWithStride完成矩阵计算所需2D格式数据的搬运

LoadDataWithStride本质上是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置。
根据`LoadDataWithStride`指令完成img2col的过程，可知当N=1, filterW/H=1, padding=0, strideW/H=1, dilation=1时，可用于2D矩阵搬运，示意图如下所示：

<div align="center">
  <img src="figures/load_data_with_stride_2d.png" width="800"><br>
  图1：half数据类型下，L1 -> L0A，不转置，调用LoadDataWithStride进行2D矩阵搬运示意图
</div>

以左矩阵为例，如下代码所示：此时l1W = CeilAlign(m, BLOCK_CUBE)，对应对应L1上左矩阵的M，channelSize = CeilAlign(k, c0Size)对应对应L1上左矩阵的K。

```cpp
// LoadData3DParamsV2: 描述L1到L0的数据搬运参数（2D数据搬运）
AscendC::LoadData3DParamsV2<T> loadDataParams;
loadDataParams.l1H = 1;                               // 源操作数height，2D数据搬运固定为1
loadDataParams.l1W = CeilAlign(m, BLOCK_CUBE);        // 源操作数width，对应矩阵M
loadDataParams.channelSize = CeilAlign(k, c0Size);    // 通道数，对应矩阵K
loadDataParams.kExtension = CeilAlign(k, c0Size);     // 目的操作数width维度传输长度（K方向）
loadDataParams.mExtension = CeilAlign(m, BLOCK_CUBE); // 目的操作数height维度传输长度（M方向）
loadDataParams.strideW = 1;                           // 卷积核W方向滑动步长，2D数据搬运固定为1
loadDataParams.strideH = 1;                           // 卷积核H方向滑动步长，2D数据搬运固定为1
loadDataParams.filterW = 1;                           // 卷积核width，2D数据搬运固定为1
loadDataParams.filterH = 1;                           // 卷积核height，2D数据搬运固定为1
loadDataParams.dilationFilterW = 1;                   // 卷积核W方向膨胀系数
loadDataParams.dilationFilterH = 1;                   // 卷积核H方向膨胀系数
loadDataParams.filterSizeW = false;                   // 是否扩展filterW（true时+256）
loadDataParams.filterSizeH = false;                   // 是否扩展filterH（true时+256）
loadDataParams.enTranspose = false;                   // 是否转置，仅L0A通路且b8/b16/b32时有效
loadDataParams.fMatrixCtrl = false;                   // 从左矩阵获取FeatureMap属性（当前仅支持false）

// 调用LoadDataWithStride时，须调用此接口配置dstStride。
AscendC::LoadDataRepeatParamWithStride repeatParams;
repeatParams.dstStride = CeilDiv(CeilAlign(m, BLOCK_CUBE), BLOCK_CUBE); // 输出矩阵K轴方向的偏移
AscendC::SetLoadDataRepeatWithStride(repeatParams);
// 使用LoadDataWithStride接口搬运数据从L1到L0A
AscendC::LoadDataWithStride(a2Local, a1Local, loadDataParams);
```

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim`、`cpu` | 运行模式：NPU运行、NPU仿真、CPU调试 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（必须） | NPU架构：dav-3510对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4`、`5`、`6` | 场景编号 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
