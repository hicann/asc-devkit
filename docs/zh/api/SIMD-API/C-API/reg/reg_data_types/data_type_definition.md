# reg数据类型定义

## 矢量数据寄存器

矢量数据寄存器用于存储矢量数据，其位宽为VL（Vector Length），可存储VL/sizeof(T)的数据（T表示数据类型）。在Ascend 950PR/Ascend 950DT版本中， VL = 256B。例如对于矢量数据类型vector_float，该寄存器可存储的元素数量为256B / sizeof(float) = 64个。

以下是以位宽为分类，列举的所有矢量数据寄存器的数据类型：

| 位宽      | 矢量数据类型|
| --------- | --------- |
| b8        | vector_int8_t/vector_uint8_t/vector_int4x2_t/vector_fp4x2_e2m1_t/vector_fp4x2_e1m2_t/vector_hifloat8_t/vector_fp8_e4m3fn_t/vector_fp8_e5m2_t/vector_fp8_e8m0_t |
| b16       | vector_int16_t/vector_uint16_t/vector_half/vector_bfloat16_t |
| b32       | vector_int32_t/vector_uint32_t/vector_float |
| b64       | vector_int64_t/vector_uint64_t |

**注意：**
vector_int4x2_t、vector_fp4x2_e2m1_t、vector_fp4x2_e1m2_t这三个矢量数据类型在内存中的排布需要将两个元素打包为一个字节的存储单元。

### 调用示例<a name="调用示例-1"></a>

```cpp
vector_half dst;
half index = 0;
asc_arange(dst, index);
```

## 掩码寄存器

掩码寄存器的数据类型为vector_bool，用于矢量计算中选择参与计算的元素，其位宽为VL/8。当搬运掩码寄存器时，ubuf中地址偏移单位为字节。

![vector_bool](../../figures/vector_bool.png)

### 调用示例<a name="调用示例-2"></a>

```cpp
uint32_t length = 255;
vector_bool mask = asc_create_mask_b16(PAT_ALL); // 创建一个所有元素为True的掩码寄存器
vector_bool mask = asc_update_mask_b16(length); // 根据矢量计算需要操作的元素的具体数量，生成对应的掩码寄存器
```

## 非对齐寄存器

非对齐寄存器包括vector_load_unalign和vector_store_unalign。这些寄存器作为缓冲区，用于在UB和矢量数据寄存器之间进行连续的非对齐数据搬运，其中非对齐特指数据起始地址未按32字节对齐。在搬运过程中，非对齐数据首先被加载到专用的非对齐寄存器，随后通过相应的搬运接口完成数据的分块读取或写入。

在读非对齐地址前，vector_load_unalign应该通过asc_loadunalign_pre初始化，然后再使用asc_loadunalign。在写非对齐地址时，应先使用asc_storeunalign，再使用asc_storeunalign_post进行处理。

### 调用示例<a name="调用示例-3"></a>

```cpp
constexpr uint32_t one_repeat_size = 256 / sizeof(int8_t); // VL / sizeof(T)
uint32_t total_length = 255;
uint32_t repeat_time = (total_length + one_repeat_size - 1) / one_repeat_size;
__simd_vf__ inline void neg_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr, uint32_t count,
    uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int8_t src;
    vector_int8_t dst;
    vector_load_unalign ureg0;
    vector_store_unalign ureg1;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b8(count);
        asc_loadunalign_pre(ureg0, src_addr + i * one_repeat_size); // 非对齐搬入前的初始化
        asc_loadunalign(src, ureg0, src_addr + i * one_repeat_size); // 配合vector_load_unalign的使用，非对齐搬入源数据
        asc_neg(dst, src, mask);
        asc_storeunalign(dst_addr + i * one_repeat_size, ureg1, dst, one_repeat_size); // 配合vector_store_unalign的使用，非对齐搬出目的数据
        asc_storeunalign_post(dst_addr + i * one_repeat_size, ureg1, 0); // 处理非对齐搬出的尾块
    }
}
```

## 地址寄存器

地址寄存器的数据类型为addr_reg，用于存储地址偏移量。addr_reg通过asc_update_addr_reg初始化，然后在循环之中使用addr_reg存储地址偏移量。addr_reg在每层循环中根据所设置的步长进行自增。

### 调用示例<a id="example4"></a>

#### 单参数版本
```cpp
constexpr uint32_t one_repeat_size = 256 / sizeof(int8_t); // VL / sizeof(T)
uint32_t total_length = 256;
uint32_t repeat_time = total_length / one_repeat_size;
__simd_vf__ inline void add_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src0_addr, __ubuf__ int8_t* src1_addr, uint32_t count,
    uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int8_t src0;
    vector_int8_t src1;
    vector_int8_t dst;
    vector_bool mask;
    addr_reg addr_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        addr_reg = asc_update_addr_reg_b8(one_repeat_size); // 通过初始化addr_reg，每一次循环，地址偏移one_repeat_size
        mask = asc_update_mask_b8(count);
        asc_loadalign(src0, src0_addr, addr_reg);
        asc_loadalign(src1, src1_addr, addr_reg);
        asc_add(dst, src0, src1, mask);
        asc_storealign(dst_addr, dst, addr_reg, mask);
    }
}
```

#### 多参数版本（以4参数为例）
```cpp
__simd_vf__ inline void add_4d_vf(
    __ubuf__ float* dst_addr,
    __ubuf__ float* src_addr,
    uint32_t n_stride,    // N间偏移 = C×H×W (float元素数)
    uint32_t c_stride,    // C间偏移 = H×W (float元素数)
    uint32_t h_stride,    // H间偏移 = W (float元素数)
    uint32_t w_stride,    // W间偏移 = 64 (一个VL的float数)
    uint16_t N, uint16_t C, uint16_t H, uint16_t W)
{
    vector_float src_reg, dst_reg;
    vector_bool mask = asc_create_mask_b32(PAT_ALL);
    addr_reg a_reg;
    for (uint16_t n = 0; n < N; n++) {          
        for (uint16_t c = 0; c < C; c++) {       
            for (uint16_t h = 0; h < H; h++) {    
                for (uint16_t w = 0; w < W; w++) {  
                    a_reg = asc_update_addr_reg_b32(n_stride, c_stride, h_stride, w_stride);
                    asc_loadalign(src_reg, src_addr, a_reg);
                    asc_add(dst_reg, src_reg, src_reg, mask);
                    asc_storealign(dst_addr, dst_reg, a_reg, mask);
                }
            }
        }
    }
}
```
