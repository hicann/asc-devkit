# 随路量化

随路量化是矩阵计算结果从L0C Buffer搬出到Global Memory或Unified Buffer过程中的数据转换能力。L0C Buffer到Global Memory/Unified Buffer搬运根据是否传入`quant`参数自动选择不量化、直接cast、scalar量化或tensor量化模式。

## 调用形式

不传入`quant`参数时，接口根据源类型和目的类型选择不量化输出或直接cast输出：

```cpp
auto atom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(atom, dst, src);
```

传入`uint64_t`类型的`quant`参数时，表示scalar量化。整个C矩阵共用同一个量化参数：

```cpp
uint64_t quant = 1;
auto atom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(atom, dst, src, quant);
```

传入Tensor API张量作为`quant`参数时，表示tensor量化。量化参数通常位于L1 Buffer，元素类型为`uint64_t`，每个channel维度对应一个量化参数：

```cpp
auto quant = MakeTensor(MakeMemPtr(quantAddr),
    MakeFrameLayout<NDExtLayoutPtn>(1, n));
auto atom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(atom, dst, src, quant);
```

## 量化参数

scalar量化参数和tensor量化参数的元素类型均为`uint64_t`。

## 约束说明

- tensor量化参数张量应位于L1 Buffer，元素类型为`uint64_t`，地址要求32字节对齐。
- 量化参数不能为`inf`、`nan`或非规格化数。该约束属于硬件约束，接口不单独检查参数值。
- `RoundMode::HYBRID`仅在源类型为`float`、目的类型为`hifloat8_t`的量化输出场景支持。
- 不传入`quant`时，仅支持不量化输出或`float`到`half`/`bfloat16_t`的直接cast输出。
