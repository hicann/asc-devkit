# RegTrait样例

## 概述

本样例演示了在Ascend C Reg矢量计算中，寄存器数据类型RegTensor、MaskReg模板参数regTrait的使用方法，通过两种不同的regTrait配置（RegTraitNumOne和RegTraitNumTwo），在寄存器层级执行除法运算。通过比较两种配置下的实现，展示对计算性能和资源使用的影响。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── trait
│   ├── scripts/             // 测试脚本目录
│   │   └── gen_data.py      // 生成测试输入和golden数据
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── data_utils.h         // 数据读写工具函数
│   ├── trait.asc            // Ascend C算子实现 & 调用样例
│   └── README.md            // 样例说明文档
```

## 样例描述

- 样例功能：  
  样例计算int64_t类型数据，分别使用RegTraitNumOne和RegTraitNumTwo配置regTrait作为RegTensor、MaskReg模板参数实现向量除法运算。计算公式如下：
  $$z_i = \frac{x_i}{y_i}$$

- 样例规格
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center" colspan="2">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[10240]</td><td align="center" colspan="2">int64_t</td></tr>
  <tr><td align="center">y</td><td align="center">[10240]</td><td align="center" colspan="2">int64_t</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[10240]</td><td align="center" colspan="2">int64_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">trait</td></tr>
  </table>

- 样例实现
    - 实现流程：
        1. 在Host侧分配内存并初始化输入数据
        2. 将数据从Host内存搬运至Device全局内存
        3. 调用核函数，在Vector核上执行计算
        4. 核函数内部：通过DataCopy将输入数据从Global Memory搬入Unified Buffer
        5. 在Unified Buffer上通过asc_vf_call调用VF（Vector Function）函数
        6. VF函数内部使用regTrait模板参数创建RegTensor，执行LoadAlign、Div、StoreAlign操作
        7. 将计算结果从Unified Buffer搬回Global Memory
        8. 在Host端进行精度验证和golden data对比

    - 多场景说明：  
        - 场景1（SCENARIO_NUM=1）：使用RegTraitNumOne作为RegTensor和MaskReg的regTrait模板参数
            - 实现函数：TraitNumOneVF<T>
            - 实现：  
              `AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> xReg, yReg, zReg;`
              `AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL, AscendC::Reg::RegTraitNumOne>();`
            - 可支持b8/b16/b32/b64类型数据，对b64类型数据单次指令发射收益变低。

        - 场景2（SCENARIO_NUM=2）：使用RegTraitNumTwo作为RegTensor和MaskReg的regTrait模板参数
            - 实现函数：TraitNumTwoVF<T>
            - 实现：  
              `AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumTwo> xReg, yReg, zReg;`  
              `AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL, AscendC::Reg::RegTraitNumTwo>();`
            - 仅支持b64/complex32类型数据，提高单次指令发射中处理的元素个数，提高并行度。

    - 性能对比：  
      下表为样例在Ascend 950系列产品上运行的性能数据：
       
       | 场景 | 核数 | Task Duration(μs) | aiv_total_cycles | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
       |:---:|:---:|:-----------------:|:----------------:|:------------:|:----------------:|:-------------:|:-------------------:|:----------------:|:-----------------:|:--------------:|:-----------------:|:--------------:|
       |  1  |  1  |      58.168       |      95271       |    57.92     |      56.09       |     0.969     |        0.268        |      0.004       |       1.082       |     0.019      |       0.463       |     0.008      |
       |  2  |  1  |      39.563       |      64543       |    39.318    |      37.471      |     0.953     |        0.273        |      0.007       |       1.082       |     0.038      |       0.471       |     0.012      |
        
       场景2相较场景1配置使用2倍的RegTensor位宽，同样数据规模整体耗时从 58.168 μs 减少到 39.563 μs，总耗时减少18.605 μs，性能提升了约 32%，指令执行周期从 95,271 锐减至 64,543。表明更大位宽的寄存器配置有效消除了由于位宽限制导致的指令拆分与流水线等待周期，释放了 Vector 单元的算力。  
      <br>
      **性能指标说明：**

        | 字段名 | 字段含义 |
        |:---:|:---|
        | Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
        | aiv_total_cycles|Task在 Vector Core上执行所消耗的CPU周期（Cycle）总数。|
        | aiv_time|Task在AI Vector Core上的理论执行时间，单位为μs。|
        | aiv_vec_time(μs) | vec类型指令（向量类运算指令）耗时，单位μs。 |
        | aiv_vec_ratio | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。 |
        | aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位μs。 |
        | aiv_scalar_ratio | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。 |
        | aiv_mte2_time(μs) | mte2类型指令（GM->UB搬运类指令）耗时，单位μs。 |
        | aiv_mte2_ratio | mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。 |
        | aiv_mte3_time(μs) | mte3类型指令（UB->GM搬运类指令）耗时，单位μs。 |
        | aiv_mte3_ratio | mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。 |

    - 约束条件：
        - 输入数据长度必须为GetVecLen()的整数倍
        - 除数不能为0，实际应用中需要增加除数为0的检查
        - 样例目前仅支持int64_t数据类型

## 编译运行

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  # 场景2：使用RegTraitNumTwo
  SCENARIO_NUM=2
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM; make -j;                           # 编译工程
  python3 ../scripts/gen_data.py;                                           # 生成测试输入数据
  ./demo                                                                    # 执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。
- 编译选项说明

  | 选项 | 可选值                   | 说明                                           |
  |:--------------------------|:----------------------|:---------------------------------------------|
  | `SCENARIO_NUM`            | `1`（默认）、`2`        | 1：支持RegTraitNumOne；<br/>2：支持RegTraitNumTwo；       |
  | `CMAKE_ASC_RUN_MODE`      | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真                      |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`            | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```