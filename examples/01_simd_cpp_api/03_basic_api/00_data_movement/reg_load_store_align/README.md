# ld_st_reg_align样例

## 概述
本样例基于Reg编程接口实现UB(Unified Buffer)对RegTensor(Reg矢量计算基本单元)的连续和非连续的对齐数据搬运操作，该样例使用LoadAlign，StoreAlign接口，以及POST_MODE_UPDATE、DATA_BLOCK_COPY等模式的使能。本样例支持6种搬运场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    <table>
      <tr>
        <td>SCENARIO_NUM</td>
        <td>搬运场景</td>
      </tr>
      <tr>
        <td>1</td>
        <td>使用开发者自定义的迭代间偏移</td>
      </tr>
      <tr>
        <td>2</td>
        <td>使用PostUpdate模式表示迭代间偏移</td>
      </tr>
      <tr>
        <td>3</td>
        <td>使用地址寄存器（AddrReg）表示迭代间偏移</td>
      </tr>
      <tr>
        <td>4</td>
        <td>以DataBlock为单位非连续搬运</td>
      </tr>
      <tr>
        <td>5</td>
        <td>广播（broadcast）模式搬入</td>
      </tr>
      <tr>
        <td>6</td>
        <td>上采样（upsample）模式搬入</td>
      </tr>
    </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── reg_load_store_align
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── ld_st_reg_align.asc            // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
**场景1：使用开发者自定义的迭代间偏移**
- 样例功能：  
  每次迭代重新配置搬入搬出的地址，实现数据的连续搬入搬出。同时介绍通过MaskReg处理最后一次迭代时，搬出数据不足VL的数据量非VL对齐场景。
- 样例实现：  
  - 基础场景，LoadAlign和StoreAlign采用默认参数配置。
  - 输入1024个元素，输出1021个元素，需要8次迭代。其中最后一次迭代仅计算并搬出125个元素，不足VL。
  - VF函数for循环内每次迭代用UpdateMask接口更新mask。对于half类型：
    - 当outputLength>=128时，mask处理128个元素
    - 当outputLength<128时，mask处理outputLength个元素
    - UpdateMask执行完后，outputLength自减mask处理的元素个数。  
    ```cpp
    mask = AscendC::Reg::UpdateMask<T>(outputLength);
    ```
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1021]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">VF函数名</td><td colspan="4" align="center">CopyWithOffsetVF</td></tr>
  </table>

**场景2：使用PostUpdate模式表示迭代间偏移**
- 样例功能：  
  搬入搬出使用POST_MODE_UPDATE模式，实现数据的连续搬入搬出。
- 样例实现：  
  - LoadAlign/StoreAlign模板参数 postMode = PostLiteral::POST_MODE_UPDATE
  - LoadAlign/StoreAlign入参 postUpdateStride = 128，即VL/sizeof(half)。以LoadAlign搬入为例：
    - 每次迭代的UB起始地址为srcAddr
    - LoadAlign执行后，srcAddr会自动更新为srcAddr+postUpdateStride。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">VF函数名</td><td colspan="4" align="center">CopyWithPostUpdateVF</td></tr>
  </table>

**场景3：使用地址寄存器（AddrReg）表示迭代间偏移**
- 样例功能：  
  搬入搬出使用AddrReg（地址寄存器），实现数据的连续搬入搬出。
- 样例实现：  
  - 初始化地址寄存器，表示i轴每次循环完成时aReg自增stride0  
    ```cpp
    uint32_t stride0 = AscendC::GetVecLen() / sizeof(T);
    AddrReg aReg = AscendC::Reg::CreateAddrReg<T>(i, stride0);
    ```
  - LoadAlign/StoreAlign入参 offset = aReg。以LoadAlign搬入为例：  
    - 每次迭代的UB起始地址为srcAddr + aReg
    - 迭代后aReg会自动更新为aReg + stride0。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">VF函数名</td><td colspan="4" align="center">CopyWithAddrRegVF</td></tr>
  </table>

**场景4：以DataBlock为单位非连续搬运**
- 样例功能：  
  搬入搬出使用DataBlock（32字节）搬运模式。搬入时单次搬运相邻DataBlock间间隔2个DataBlock，即搬运32B，间隔32B；搬出时单次搬运相邻DataBlock间隔1个DataBlock，相当于连续搬运。
- 样例实现：  
  - LoadAlign/StoreAlign模板参数 dataMode = DataCopyMode::DATA_BLOCK_COPY
  - LoadAlign入参dataBlockStride配置为2，StoreAlign入参dataBlockStride配置为1
  - 每次repeat处理256B，即8个DataBlock，所以LoadAlign入参repeatStride配置为16，StoreAlign入参repeatStride配置为8
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">VF函数名</td><td colspan="4" align="center">CopyDataBlock</td></tr>
  </table>

**场景5：广播（broadcast）模式搬入**
- 样例功能：  
  搬入使用广播（broadcast）模式。每次迭代搬运UB起始地址的第一个元素，并Broadcast到regTensor所有元素的位置。
- 样例实现：  
  - LoadAlign模板参数 dist = LoadDist::DIST_BRC_B16
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">VF函数名</td><td colspan="4" align="center">CopyInBroadcastVF</td></tr>
  </table>

**场景6：上采样（upsample）模式搬入**
- 样例功能：  
  搬入使用上采样（upsample）模式。每次迭代加载VL/2数据，每个输入元素重复两次。
- 样例实现：  
  - LoadAlign模板参数 dist = LoadDist::DIST_US_B16
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 2048]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">VF函数名</td><td colspan="4" align="center">CopyInUpsampleVF</td></tr>
  </table>

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
  SCENARIO_NUM=1                                                                # 执行场景1
  mkdir -p build && cd build;                                                   # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM                     # 生成测试输入数据
  ./demo                                                                        # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或场景前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2`、`3`、`4`、`5`、`6` | 场景编号，具体含义见概述 |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
