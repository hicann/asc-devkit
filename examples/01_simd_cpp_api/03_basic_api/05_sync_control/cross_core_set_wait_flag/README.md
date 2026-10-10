# CrossCoreSetFlag和CrossCoreWaitFlag核间同步样例

## 概述
本样例介绍核间同步接口[CrossCoreSetFlag](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md)的模式0、模式1、模式2和模式4，并通过纯Vector计算以及Cube与Vector融合计算场景说明各模式的使用方法。其中模式4仅支持Ascend 950PR/Ascend 950DT。

<p align="center">
  <img src="figures/sync_control_mode_diagram.png" width="100%">
</p>
<p align="center">
图1：同步控制模式示意图
</p>

<table border="1" align="center">
  <tr bgcolor="lightgray">
    <td>同步控制模式</td>
    <td align="center">说明</td>
  </tr>
  <tr>
    <td rowspan="2">mode 0</td>
    <td>对于AIC场景，同步所有的AIC核，直到所有的AIC核都执行到<code>CrossCoreSetFlag</code>时，<code>CrossCoreWaitFlag</code>后续的指令才会执行。</td>
  </tr>
  <tr>
    <td>对于AIV场景，同步所有的AIV核，直到所有的AIV核都执行到<code>CrossCoreSetFlag</code>时，<code>CrossCoreWaitFlag</code>后续的指令才会执行。</td>
  </tr>
  <tr>
    <td>mode 1</td>
    <td>单个AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了<code>CrossCoreSetFlag</code>，<code>CrossCoreWaitFlag</code>后续的指令才会执行。</td>
  </tr>
  <tr>
    <td rowspan="2">mode 2</td>
    <td>在AIC核执行<code>CrossCoreSetFlag</code>之后，两个AIV上<code>CrossCoreWaitFlag</code>后续的指令才会继续执行。</td>
  </tr>
  <tr>
    <td>两个AIV都执行<code>CrossCoreSetFlag</code>后，AIC上<code>CrossCoreWaitFlag</code>后续的指令才能执行。</td>
  </tr>
  <tr>
    <td>mode 4</td>
    <td>单个AI Core内部，AIC与指定的单个AIV之间进行同步控制。AIV0和AIV1通过不同的<code>flagId</code>映射分别与AIC同步。</td>
  </tr>
</table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.2.0 |
| Atlas A3系列产品 | >= CANN 9.2.0 |
| Atlas A2系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── cross_core_set_wait_flag
│   ├── scripts
│   │   ├── gen_data.py              // 输入数据和真值数据生成脚本
│   │   └── verify_result.py         // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt               // 编译工程文件
│   ├── data_utils.h                 // 数据读入写出函数
│   ├── figures                      // 图示
│   ├── cross_core_set_wait_flag.h   // Ascend C样例实现
│   ├── cross_core_set_wait_flag.asc // 调用样例以及结果校验
│   └── README.md                    // 样例说明文档
```

## 样例描述
<table border="1" style="text-align: center;">
  <tr>
    <td>SCENARIO_NUM取值</td>
    <td>业务场景</td>
    <td>使用的同步模式</td>
  </tr>
  <tr>
    <td>0</td>
    <td>纯Vector计算场景（16个AIV）</td>
    <td>mode 0（AIV全核同步）</td>
  </tr>
  <tr>
    <td>1</td>
    <td>纯Vector计算场景（2个AIV）</td>
    <td>mode 1</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Cube与Vector融合计算场景</td>
    <td>mode 2（AIC等AIV）、mode 2（AIV等AIC）、mode 0（AIC全核同步）</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Cube与Vector融合计算场景（仅Ascend 950PR/Ascend 950DT）</td>
    <td>mode 4（AIC分别与AIV0、AIV1同步）</td>
  </tr>
</table>
本样例通过`SCENARIO_NUM`控制执行分支：

- 场景0~2可在NPU架构`dav-2201`和NPU架构`dav-3510`上编译。
- 场景3使用模式4以及L0C Buffer到UB的数据通路，因此必须配置NPU架构`dav-3510`。

### 计算公式与样例规格

#### `SCENARIO_NUM=0`（纯Vector计算场景，模式0）
- 计算公式：  
  $$
  z = \sum_{i=0}^{15} (x \times i)
  $$
  - `x`为输入向量，全为1。
  - `i`为每个AIV的`BlockIdx`（取值范围0-15）。
  - `z`为所有AIV计算结果的累加值。

- 样例规格：
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型（OpType）</td><td colspan="4" align="center"><code>CrossCoreSetFlagMode0</code></td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### `SCENARIO_NUM=1`（纯Vector计算场景，模式1）
- 计算公式：  
  $$
  z = (x \times 2) + (x \times 3)
  $$
  - `x`为输入向量，全为1。
  - 仅`BlockIdx=2`和`BlockIdx=3`的AIV参与计算。
  - `z`为这两个AIV计算结果的累加值。

- 样例规格：
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型（OpType）</td><td colspan="4" align="center"><code>CrossCoreSetFlagMode1</code></td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### `SCENARIO_NUM=2`（Cube与Vector融合计算场景）
- 计算公式：  
  $$
  C = \text{LeakyRelu}(Cast(A) \times Cast(B))
  $$
  - `A`为左矩阵，形状为`[M, K]`，数据类型为`uint8`。
  - `B`为右矩阵，形状为`[K, N]`，数据类型为`uint8`。
  - 首先将`A`和`B`数据类型从`uint8`转换为`half`。
  - 然后执行矩阵乘：`A × B`。
  - 最后对结果执行`LeakyRelu`运算。
  - `C`为最终结果，形状为`[M, N]`，数据类型为`float32`。

- 样例规格：
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型（OpType）</td><td colspan="5" align="center"><code>CrossCoreSetFlagMode2</code></td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[32, 32]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[32, 64]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[32, 64]</td><td align="center">float32</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">mmad_custom</td></tr>
  </table>

#### `SCENARIO_NUM=3`（Cube与Vector融合计算场景，模式4）

场景3的数学计算公式、输入输出规格与场景2相同，但计算组织方式不同：

- 场景2将K维切分给多个AIC计算，各AIC的部分矩阵乘结果需要在GM上原子累加；
- 场景3仅启动1个AI Core，由单个AIC处理完整K维，因此该AIC已经得到完整矩阵乘结果，不使用模式0的AIC全核同步，也不进行多AIC分块结果的原子累加。AIV0和AIV1分别完成A、B矩阵的精度转换，并通过模式4独立通知AIC；矩阵乘结果的前16行通过Fixpipe从L0C Buffer直达AIV0的UB，后16行先从L0C Buffer搬至GM，再由AIV1搬入UB。两个AIV分别执行`LeakyRelu`并写回各自对应的GM区域。

模式4下AIV1与AIC的`flagId`存在偏移映射：

- AIV1使用0~15时，AIC侧对应16~31。
- AIC使用16~31时，AIV1侧对应0~15。

具体规则请参见[CrossCoreSetFlag的flagId取值范围说明](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md#flagId取值范围说明)。

## 样例实现
### 1. Cube与Vector融合计算场景
#### 1.1 整体逻辑
<p align="center">
  <img src="figures/融合场景_示意图.png" width="100%">
   </p>
<p align="center">
图2：Cube与Vector场景融合计算场景，整体计算逻辑示意图
</p>

本样例针对融合算子（配置`__mix__(1, 2)`）场景展开，每个AI Core内部有1个AIC和2个AIV。配置逻辑核数`numBlocks = 8`，对应8个AIC与16个AIV。

如图2所示，整体计算逻辑分为三个核心阶段：精度转换阶段、分块矩阵乘与原子累加阶段、`LeakyRelu`运算与结果回写阶段。
#### 1.2精度转换阶段（模式2，单个AI Core内部AIC等AIV）
由于GM上左右矩阵的数据类型为`uint8`，并不满足`mmad`指令对输入数据类型的要求，需要先将GM上的数据搬运到AIV中进行精度转换后，才能在AIC中进行分块矩阵乘计算。因此，
每个AI Core内部，一个AIC需要等待该AI Core内部其它2个AIV完成数据精度转换后，才能开始分块矩阵乘计算。

具体来说：将GM中左矩阵（A矩阵）数据沿K轴切分8份，分配至`BlockIdx`为偶数的AIV中完成`uint8`到`half`的精度转换；将GM中右矩阵（B矩阵）数据沿K轴切分8份，分配至`BlockIdx`为奇数的AIV中完成`uint8`到`half`的精度转换。如图3所示，根据以上描述，需要使用核间同步模式2。
上述描述对应的代码段如下：

```cpp
if (blockIdx % 2 == 0) {
    AscendC::DataCopy(aLocal, aGM, A_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::Cast(castALocal, aLocal, AscendC::RoundMode::CAST_NONE, A_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::DataCopy(AVectorGM, castALocal, A_BLOCKS_LENGTH);
} else {
    AscendC::DataCopy(bLocal, bGM, B_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::Cast(castBLocal, bLocal, AscendC::RoundMode::CAST_NONE, B_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::DataCopy(BVectorGM, castBLocal, B_BLOCKS_LENGTH);
}
// 模式2，每一个AI Core内部，一个AIC等2个AIV
AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(SYNC_AIV_AIC_FLAG);
```

<p align="center">
  <img src="figures/融合场景_精度转换阶段.png" width="100%">
   </p>
<p align="center">
图3：精度转换阶段，模式2示意图
</p>

#### 1.3分块矩阵乘与原子累加阶段（模式0，AIC全核同步）
每个AIC执行分块矩阵乘计算后，开启原子累加求和机制，将计算结果搬运至同一块GM区域，将8个AIC各自分块矩阵乘的结果在GM上进行累加，最终得到完整的C矩阵。为了获取正确的C矩阵，需要等待8个AIC都完成分块矩阵乘计算并且将结果通过`FixPipe`搬运到GM上。如图4所示，根据以上描述，需要使用核间同步模式0（AIC全核同步）。上述描述对应的代码段如下：
$$
C = \sum_{i=1}^{8} A_i \cdot B_i
$$

```cpp
// 模式2，每一个AI Core内部，AIC等2个AIV
CrossCoreWaitFlagForArch<2, PIPE_MTE2>(SYNC_AIV_AIC_FLAG);

CopyIn(a1Local, b1Local);
SplitA(a1Local, a2Local);
SplitBTranspose(b1Local, b2Local);
Compute(a2Local, b2Local, c1Local);
CopyOut(c1Local);
// 模式0：8个AIC全核同步，确保原子累加结果正确。
AscendC::CrossCoreSetFlag<0, PIPE_FIX>(SYNC_AIC_FLAG);
CrossCoreWaitFlagForArch<0, PIPE_FIX>(SYNC_AIC_FLAG);

// 模式2：AIC通知本AI Core内2个AIV可以执行LeakyRelu。
AscendC::CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_AIV_FLAG);
```

<p align="center">
  <img src="figures/融合场景_分块矩阵乘与原子累加阶段.png" width="100%">
   </p>
<p align="center">
图4：分块矩阵乘与原子累加阶段，模式0示意图
</p>

#### 1.4 LeakyRelu与结果回写阶段（模式2，2个AIV等AIC）
每个AI Core内部，2个AIV需等待AIC完成分块矩阵乘及原子累加操作后，才能对C矩阵分块执行`LeakyRelu`运算。
具体来说是对累加得到的C矩阵，沿M轴切分16份，分配至16个AIV中分别执行`LeakyRelu`运算。如图5所示，根据以上描述，需要使用核间同步模式2（单个AI Core内部2个AIV等一个AIC）。上述描述对应的代码段如下：

```cpp
// 模式2，每一个AI Core内部，2个AIV等AIC
CrossCoreWaitFlagForArch<2, PIPE_MTE2>(SYNC_AIC_AIV_FLAG);
AscendC::DataCopy(cLocal, CVectorGM, C_AIV_BLOCKS_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

// 进行LeakyRelu运算
float alpha = 0.001;
AscendC::LeakyRelu(reluCLocal, cLocal, alpha, C_AIV_BLOCKS_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::DataCopy(CVectorGM, reluCLocal, C_AIV_BLOCKS_LENGTH);
```

<p align="center">
  <img src="figures/融合场景_LeakyRelu运算与结果回写阶段.png" width="100%">
   </p>
<p align="center">
图5：LeakyRelu运算与结果回写阶段，模式2示意图
</p>

#### 1.5 L0C到UB/GM分流阶段（模式4）

场景3将`FUSED_NUM_BLOCKS`设置为1，AIC侧的`AIC_K`等于完整K维度，因此单个AIC完成的矩阵乘结果与场景2经过多AIC分块累加后的完整矩阵乘结果一致。AIC完成矩阵乘后，使用两个模式4通知分别唤醒AIV0和AIV1：AIV0直接消费L0C Buffer到UB的前半矩阵，AIV1从GM搬入后半矩阵，两个AIV获得的`LeakyRelu`输入共同覆盖完整C矩阵。这样既展示了模式4能够选择单个AIV同步，也体现了Ascend 950PR/Ascend 950DT支持的L0C Buffer到UB数据通路。

```cpp
// AIV0/AIV1分别通知AIC输入已准备完成。
AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(SYNC_MODE4_INPUT_FLAG);

// AIC分别等待AIV0和AIV1完成精度转换，等待AIV1时使用flagId + 16。
AscendC::CrossCoreWaitFlag<4, PIPE_MTE2>(SYNC_MODE4_INPUT_FLAG);
AscendC::CrossCoreWaitFlag<4, PIPE_MTE2>(SYNC_MODE4_INPUT_FLAG + SYNC_MODE4_AIV1_OFFSET);

// AIC分别通知AIV0和AIV1输出已准备完成。
AscendC::CrossCoreSetFlag<4, PIPE_FIX>(SYNC_MODE4_OUTPUT_FLAG);
AscendC::CrossCoreSetFlag<4, PIPE_FIX>(SYNC_MODE4_OUTPUT_FLAG + SYNC_MODE4_AIV1_OFFSET);
```

### 2. 纯Vector计算场景
#### 2.1模式0和模式1的对比
本样例设置`NUM_BLOCKS`为8（8个AI Core），每一个AI Core内AIC与AIV配比为1:2，即本样例总共起了8个AIC和16个AIV，AIV的`BlockIdx`范围为0~15。
如下图6所示，本样例中模式0和模式1计算逻辑几乎相同，区别仅在于参与同步的AIV数目：模式0时，全部的16个AIV参与同步；模式1时，仅有第二个AI Core中的2个AIV（`BlockIdx=2`和`BlockIdx=3`）参与同步。因此，下一节将详细介绍模式0的整体逻辑，模式1就不再详细介绍。
<p align="center">
  <img src="figures/纯aiv_模式1和模式0的区别.png" width="100%">
</p>
<p align="center">
图6：纯AIV场景，模式0与模式1的区别示意图
</p>

#### 2.2模式0的整体逻辑
本样例用到的GM分为2块，一块用于存储输入数据（`initialDataGm`），一块用于存储所有AIV的累加结果（`atomicResultGm`）。
如下图7所示，模式0的整体逻辑分为以下步骤：
(1)每个AIV均从`initialDataGm`上搬运全为1的数据到UB上，即为`PIPE_MTE2`的流水操作。

(2)每个AIV对UB上的数据进行矢量计算：通过`Muls`指令乘以每个核对应的`BlockIdx`，即为`PIPE_V`的流水操作。

(3)步骤3对应代码片段如下，`atomicResultGm`用于存储16个AIV全部搬运完成后的累加结果。通过调用`CrossCoreSetFlag`与`CrossCoreWaitFlag`接口，实现同步控制：确保16个AIV均完成`PIPE_MTE3`搬运指令后，方可执行`CrossCoreWaitFlag`后续指令。

```cpp
// UB 到 GM 搬运启用原子累加：搬运至 atomicResult 的数据与原值累加后覆盖原值
AscendC::SetAtomicAdd<float>();
// DataCopy属于PIPE_MTE3流水操作
AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
// 当本AIV完成前置PIPE_MTE3(DataCopy)流水操作后，通知其他AIV核，本AIV已经完成
AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
// 阻塞本AIV继续往下执行指令，直到其他AIV全部都完成PIPE_MTE3流水操作，才解除阻塞往下执行。
CrossCoreWaitFlagForArch<0, PIPE_MTE2>(0);
// 关闭原子累加
AscendC::DisableDmaAtomic();
```

上述同步完成之后，此时`atomicResultGm`已经是16个AIV中矢量计算结果的累加值。
如果上一步骤的同步插入不正确，那么从`atomicResultGm`往AIV搬运的数据可能是部分AIV中矢量计算结果的累加值，导致结果不准确。

```cpp
if (AscendC::GetBlockIdx() == 0) {
    AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);   // PIPE_MTE2
    AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
    AscendC::DataCopy(atomicResultGm, yLocal, this->blockLength);
    return;
}
```
<p align="center">
  <img src="figures/纯aiv_模式0示意图.png" width="100%">
</p>
<p align="center">
图7：纯AIV场景，模式0计算逻辑示意图
</p>

### 3. 注意事项
#### 3.1 Cube与Vector融合计算场景
(1) Cube与Vector融合计算场景中，融合算子（配置`__mix__(1, 2)`）中需要通过`ASCEND_IS_AIV`/`ASCEND_IS_AIC`进行AIV和AIC核代码的隔离。

```cpp
AscendC::InitSocState();
KernelMmad op;
if ASCEND_IS_AIC {
    op.InitAIC(A, B, c);
    op.ProcessAIC();
} 
if ASCEND_IS_AIV {
    op.InitAIV(a, b, A, B, c);
    op.ProcessAIV();
}
AscendC::PipeBarrier<PIPE_ALL>();
```
(2) `GetBlockIdx`的返回值（获取当前核的index）在AIC和AIV的取值范围不同，其取值与算子设置的逻辑核数和一个AI Core中的AIC与AIV的比例有关。本样例中设置`NUM_BLOCKS=8`、AIC与AIV的比例为1:2，因此`GetBlockIdx`的返回值在AIC和AIV的取值范围分别为0-7和0-15。

(3)样例采用静态Tensor编程范式需要手动插入核内同步。另外，静态Tensor编程方式中需要开发者手动调用`InitSocState()`接口初始化全局状态寄存器。

#### 3.2核间同步接口约束

(1) `CrossCoreSetFlag`与`CrossCoreWaitFlag`必须配对使用，模式支持范围、Kernel类型、pipe类型以及`flagId`范围请参见[CrossCoreSetFlag接口约束](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md#section633mcpsimp)和[CrossCoreWaitFlag接口约束](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md#section633mcpsimp)。

(2)模式4仅支持Ascend 950PR/Ascend 950DT（对应NPU架构`dav-3510`），且Kernel必须配置为`__mix__(1, 2)`。

(3) `CrossCoreWaitFlag`的`pipe`模板参数在NPU架构`dav-2201`上不影响硬件指令，但在NPU架构`dav-3510`上会生效。由于NPU架构`dav-3510`的模式0、模式1和模式2不支持默认的`PIPE_S`，样例代码通过架构条件分别处理：NPU架构`dav-3510`显式传入`PIPE_MTE2`或`PIPE_FIX`，NPU架构`dav-2201`保留默认参数行为。具体约束请参见[CrossCoreWaitFlag模板参数默认值及生效情况](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md#template-parameter-defaults)。

为兼容上述两种NPU架构，样例封装了`CrossCoreWaitFlagForArch`函数。该函数在NPU架构`dav-3510`上显式传入`pipe`模板参数，在其他NPU架构上省略该参数并使用默认值。具体实现如下：

```cpp
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void CrossCoreWaitFlagForArch(uint16_t flagId)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    AscendC::CrossCoreWaitFlag<modeId, pipe>(flagId);
#else
    AscendC::CrossCoreWaitFlag<modeId>(flagId);
#endif
}
```

#### 3.3纯Vector计算场景
(1)在使用`CrossCoreSetFlag`与`CrossCoreWaitFlag`核间同步接口时，即使是纯Vector计算场景，核函数也不能使用`__vector__`修饰符。
本样例核函数采用`__mix__(1, 2)`修饰，但在纯Vector场景下，由于仅执行矢量计算，必须通过`ASCEND_IS_AIV`宏确保程序仅在AIV核上运行，否则会导致程序卡死。

```cpp
AscendC::InitSocState();
KernelCrossCoreSetFlag op;
if ASCEND_IS_AIV {
    op.Init(x, z, dataLength);
    op.Process();
}
AscendC::PipeBarrier<PIPE_ALL>();
```
 (2)模式1要求参与同步的2个AIV必须属于同一个AI Core，否则程序会卡死。本样例中，参与同步的两个AIV的`GetBlockIdx`的返回值为2、3，同属第2个AI Core（下标从1开始）；若将`GetBlockIdx`的返回值改为3和4（分别属于两个不同的AI Core），程序将卡死。

 (3)样例采用静态Tensor编程范式需要手动插入核内同步。另外，静态Tensor编程方式中需要开发者手动调用`InitSocState()`接口初始化全局状态寄存器。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。
    
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=0  # 设置场景编号（取值为0、1、2、3）
  ASC_ARCH=dav-2201  # 设置NPU架构，场景3必须设置为NPU架构dav-3510
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。
  
  示例如下：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：NPU架构`dav-2201`对应Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品，NPU架构`dav-3510`对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`（默认）、`1`、`2`、`3` | 场景编号：0（纯Vector模式0）、1（纯Vector模式1）、2（Cube+Vector模式0/2）、3（Cube+Vector模式4，仅NPU架构`dav-3510`） |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
