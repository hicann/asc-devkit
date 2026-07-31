# batch搬运

batch搬运是L0C Buffer到Global Memory或Unified Buffer搬运在矩阵最外层增加Batch维度的能力，用于一次完成多块矩阵计算结果的搬运。

## 功能说明

源张量为`NZ`格式，目的张量支持`ND`、`DN`或`NZ`格式。用户只需要在构造Layout时传入Batch参数，`Copy`调用形式与非batch模式保持一致。

源张量可通过`MakeFrameLayout<NZLayoutPtn>(batch, m, n)`构造。目的张量可通过`MakeFrameLayout<NDLayoutPtn>(batch, m, n)`、`MakeFrameLayout<DNLayoutPtn>(batch, m, n)`、`MakeFrameLayout<NDExtLayoutPtn>(batch, m, n)`、`MakeFrameLayout<DNExtLayoutPtn>(batch, m, n)`或`MakeFrameLayout<NZLayoutPtn, DstType>(batch, m, n)`构造。

## 搬运行为

不传入`quant`或传入`uint64_t`类型的scalar量化参数时，不涉及量化张量的Batch维度匹配。接口按源张量的Batch数量完成搬运；scalar量化场景下所有Batch共用同一个量化参数：


```cpp
auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn>(batch, m, n));
auto dst = MakeTensor(MakeMemPtr(dstAddr), MakeFrameLayout<NDExtLayoutPtn>(batch, m, n));

auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
Copy(atom, dst, l0c);

uint64_t quant = 1;
Copy(atom, dst, l0c, quant);
```

传入Tensor API张量作为tensor量化参数时，量化张量可以不带Batch维度，也可以带Batch维度。不带Batch维度表示所有Batch共用同一份量化参数；带Batch维度表示每个Batch使用各自的量化参数，此时源张量、目的张量和量化张量都需要带Batch维度。

量化张量不带Batch维度时，接口复用同一份量化参数完成batch搬运：

```cpp
auto quant = MakeTensor(MakeMemPtr(quantAddr), MakeFrameLayout<NDExtLayoutPtn>(1, n));

auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
Copy(atom, dst, l0c, quant);
```

量化张量带Batch维度时，不能通过一条Fixpipe指令完成全部Batch搬运。接口会按Batch执行`for`循环，逐Batch使用对应的量化参数完成搬运：

```cpp
auto quant = MakeTensor(MakeMemPtr(quantAddr), MakeFrameLayout<NDExtLayoutPtn>(batch, 1, n));

auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
Copy(atom, dst, l0c, quant);
```
