# 关键特性说明

## 硬件循环（HardwareLoop）

Reg矢量计算架构中，Vector Function（VF）是实现高性能向量计算的核心载体。VF函数中可以包含最多四层嵌套循环，每层循环中还可以包含多个串行循环。VF循环对控制结构的支持有限，仅支持for循环和条件判断，不支持switch、do-while和while-do等其他控制结构。当VF函数中的循环满足Hardware Loop编码规范会被编译器优化为Hardware Loop，提升整体的编码性能，否则它的循环逻辑会由迭代变量和条件判断语句构成Software Loop，无法开启VF循环优化。

### Hardware Loop编码规范
- Hardware Loop最多能支持的嵌套层数为4层；支持串行的Hardware Loop循环。
- 迭代变量类型<br>VF内所有Loop的迭代变量必须是uint16_t类型。
- 起始值与步长<br>循环起始值从0开始。<br>每次迭代的步长必须是递增1。
- 循环内不允许跳转指令，比如条件判断跳转，如if/else、三元运算符?:。<br>VF内if/else在Loop内会阻碍Hardware Loop的生成，编译器虽然会尽可能的做if/else消除优化，但是不做完全性保证。
- 一旦执行，循环计数/边界不允许被更改。
- 若要利用外层循环的计数作为循环边界，将外层循环计数器赋值给标量后作为内层循环边界。

## 指令双发优化
Reg矢量计算接口指令单双发的情况请参考[Reg矢量计算接口指令单双发汇总](../../../appendix/reg_vector_compute_interface_instruction_single_dual_issue_summary.md)。

指令双发优化的性能优化详细内容请参考[算子实践参考-指令双发优化](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_issue_optimization.md)。

### 指令双发

指令双发指的是处理器在同一个时钟周期内，能够同时发射两条指令到执行单元进行处理。

这种机制可以在不改变程序逻辑的前提下，提升处理器在单位时间内的指令处理效率，是实现指令级并行的重要基础之一。

### 合理拆分VF循环

VF并不是写的越长，把所有运算都放在一个for循环内就好，需要适当的搬出中间结果到Unified Buffer（UB），减少数据依赖。当指令数量过多，可能触发Icache Miss，即使循环间不存在依赖，也无法进行双发。当循环内包含同步，可以通过切分循环将同步外提减少同步次数。

### 手动控制循环拆分

如果循环内存在依赖关系过多的指令，指令无法并发执行，即无法启用双发特性。可以通过展开循环，提升指令双发能力，贴近硬件乱序执行的特性；减少指令因为寄存器资源未到位而产生的等待。

## VF融合
VF融合是将代码中多个VF函数融合成一个VF函数，有效提升性能。VF融合特性是编译器优化特性，通过编译选项--cce-simd-vf-fusion=true开启，VF自动融合会借助Loop Fuse算法，将VF转换成Loop形态，然后将控制流等价（Control-Flow-Equivalent）的VF进行融合，最后将VF进行还原。编译器首先会做融合前的合法性检查，判断两个VF是否等价，Main侧中间代码是否能在VF内执行以及融合后是否可产生正收益。如果满足VF融合条件，编译器会自动执行VF融合优化，为保证融合后的VF执行逻辑与语义与融合前一致，会在原来两个VF之间保守地插入同步指令，编译器还会尝试外提、合并融合后的VF中的指令，对VF代码进行优化。融合策略是能融尽融，用户按照符合融合的合法性检查的模式进行编码，可以增加VF融合的机会。用户也可以参考融合原理手动进行融合优化。

VF融合的性能优化详细内容请参考[算子实践参考-VF融合优化](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md)。

## 数据搬运优化

Reg向量计算C API提供了下表所示多种搬运指令，合理选择搬运接口并利用接口能力进行优化。连续非对齐场景优化的性能优化详细内容请参考[算子实践参考-连续非对齐场景优化](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/continuous_unaligned_optimization.md)。

| 场景 | C API接口 | 描述 |
| :-- | :-- | :-- |
| 连续对齐搬入 | [asc_loadalign](./load/asc_loadalign.md) | 从32字节对齐的UB起始地址连续搬入到向量数据寄存器或掩码寄存器。|
| 非连续对齐搬入 | [asc_loadalign](./load/asc_loadalign.md) | 从32字节对齐的UB起始地址非连续搬入多个DataBlock。|
| 连续非对齐搬入 | [asc_loadunalign](./load/asc_loadunalign.md) | 从非32字节对齐的UB起始地址连续搬入到向量数据寄存器。|
| 掩码寄存器搬入 | [asc_loadalign](./load/asc_loadalign.md)、[asc_loadalign_mask](./load/asc_loadalign_mask.md) | 从UB连续搬入掩码寄存器。|
| 离散搬入 | [asc_gather](./ub_gather/asc_gather.md) | 根据索引值将UB中的元素收集到向量数据寄存器。|
| DataBlock离散搬入 | [asc_gather_datablock](./ub_gather/asc_gather_datablock.md) | 根据索引值将UB中的元素按DataBlock（32B）收集到向量数据寄存器。|
| 连续对齐搬出 | [asc_storealign](./store/asc_storealign.md) | 将向量数据寄存器或掩码寄存器连续搬出到32字节对齐的UB地址。|
| 连续非对齐搬出 | [asc_storeunalign](./store/asc_storeunalign.md) | 将向量数据寄存器连续搬出到非32字节对齐的UB地址。|
| 离散搬出 | [asc_scatter](./scatter/asc_scatter.md) | 根据索引值将向量数据寄存器中的元素离散搬出到UB。|
| 寄存器间搬运 | [asc_copy](./reg_copy/asc_copy.md) | 支持向量数据寄存器复制到向量数据寄存器，掩码寄存器复制到掩码寄存器，向量数据寄存器复制到掩码寄存器三种模式。|

### 数据搬运启用分布模式

- 连续对齐搬入

  如图[half转int32_t类型转换过程](#fig1)，将UB地址`x_addr`上数据量为VL/2的`half`类型元素通过[asc_loadalign_unpack](./load/asc_loadalign_unpack.md)搬入`x_reg`，调用[asc_half2int32_rd](./reg_convert/asc_half2int32.md)将`half`转换为`int32_t`并写入`y_reg`，最后通过[asc_storealign](./store/asc_storealign.md)搬出至UB地址`y_addr`。

  **图1** half转int32_t类型转换过程<a id="fig1"></a>

  ![](../figures/capi_half_to_int32_conversion.png "half转int32_t类型转换过程")

  ```cpp
  __simd_vf__ inline void half_to_int32_vf(__ubuf__ int32_t* y_addr, __ubuf__ half* x_addr)
  {
      vector_half x_reg;
      vector_int32_t y_reg;
      uint32_t count = 64;
      vector_bool mask = asc_update_mask_b32(count);

      asc_loadalign_unpack(x_reg, x_addr);
      asc_half2int32_rd(y_reg, x_reg, mask, ASC_POSITION_EVEN);
      asc_storealign(y_addr, y_reg, mask);
  }
  ```

- 连续对齐搬出

  如图[float转int16_t类型转换过程](#fig2)，将UB地址`x_addr`上数据量为VL的`float`类型元素通过[asc_loadalign](./load/asc_loadalign.md)搬入`x_reg`，调用[asc_float2int16_rn](./reg_convert/asc_float2int16.md)将`float`转换为`int16_t`并写入`y_reg`，最后通过[asc_storealign_pack](./store/asc_storealign_pack.md)搬出至UB地址`y_addr`。

  **图2** float转int16_t类型转换过程<a id="fig2"></a>

  ![](../figures/capi_float_to_int16_conversion.png "float转int16_t类型转换过程")

  ```cpp
  __simd_vf__ inline void float_to_int16_vf(__ubuf__ int16_t* y_addr, __ubuf__ float* x_addr)
  {
      vector_float x_reg;
      vector_int16_t y_reg;
      uint32_t count = 64;
      vector_bool mask = asc_update_mask_b32(count);

      asc_loadalign(x_reg, x_addr);
      asc_float2int16_rn(y_reg, x_reg, mask, ASC_POSITION_EVEN);
      asc_storealign_pack(y_addr, y_reg, mask);
  }
  ```
