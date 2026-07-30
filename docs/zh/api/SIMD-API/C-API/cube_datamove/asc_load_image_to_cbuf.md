# asc_load_image_to_cbuf

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

将图像数据从Global Memory搬运到L1 Buffer。

## 函数原型

- 常规搬运

    ```cpp
    __aicore__ inline void asc_load_image_to_cbuf(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    __aicore__ inline void asc_load_image_to_cbuf(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    __aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    ```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| hor_size | 输入 | 从源图中加载图片的水平宽度，单位为像素。取值范围：[1, 4094]。 |
| ver_size | 输入 | 从源图中加载图片的垂直高度，单位为像素。取值范围：[1, 4094]。 |
| hor_start_pos | 输入 | 加载图片在源图片上的水平起始地址，单位为像素。取值范围：[0, 4095]。 |
| ver_start_pos | 输入 | 加载图片在源图片上的垂直起始地址，单位为像素。取值范围：[0, 4095]。 |
| src_hor_size | 输入 | 源图片水平宽度，单位为像素。取值范围：[1, 4094]。 |
| top_pad_size | 输入 | 目的图片顶部填充的像素数。取值范围：[0, 32]。 |
| bot_pad_size | 输入 | 目的图片底部填充的像素数。取值范围：[0, 32]。 |
| left_pad_size | 输入 | 目的图片左边填充的像素数。取值范围：[0, 32]。 |
| right_pad_size | 输入 | 目的图片右边填充的像素数。取值范围：[0, 32]。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- dst的起始地址需要32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../通用说明和约束.md#通用地址重叠约束)。

## 调用示例

```cpp
constexpr uint64_t total_length = 128;    // total_length指参与搬运的数据总长度
__cbuf__ half dst[total_length];
// 加载图片时的高度与宽度均为1，起始地址为(0,0)，源图片水平宽度为1，不做padding
asc_load_image_to_cbuf(dst, 1, 1, 0, 0, 1, 0, 0, 0, 0);
```
