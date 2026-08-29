# batch搬运

batch搬运是L0C Buffer到Global Memory或Unified Buffer（UB）搬运在矩阵最外层增加Batch维度的能力，用于一次完成多块矩阵计算结果的搬运。

## 功能说明

源张量为`NZ`格式，目的张量支持`ND`、`DN`或`NZ`格式。用户只需要在构造layout时传入Batch参数，`copy`调用形式与非batch模式保持一致。

源张量可通过`make_frame_layout<nz_layout_ptn>(batch, m, n)`构造。目的张量可通过`make_frame_layout<nd_layout_ptn>(batch, m, n)`、`make_frame_layout<dn_layout_ptn>(batch, m, n)`、`make_frame_layout<nd_ext_layout_ptn>(batch, m, n)`、`make_frame_layout<dn_ext_layout_ptn>(batch, m, n)`或`make_frame_layout<nz_layout_ptn, DstType>(batch, m, n)`构造。

## 搬运行为

不传入`quant`或传入`uint64_t`类型的scalar量化参数时，不涉及量化张量的Batch维度匹配。接口按源张量的Batch数量完成搬运；scalar量化场景下所有Batch共用同一个量化参数：


```cpp
auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn>(batch, m, n));
auto dst = make_tensor(make_mem_ptr(dst_addr), make_frame_layout<nd_ext_layout_ptn>(batch, m, n));

auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
copy(atom, dst, l0c);

uint64_t quant = 1;
copy(atom, dst, l0c, quant);
```

传入Tensor API张量作为tensor量化参数时，量化张量可以不带Batch维度，也可以带Batch维度。不带Batch维度表示所有Batch共用同一份量化参数；带Batch维度表示每个Batch使用各自的量化参数，此时源张量、目的张量和量化张量都需要带Batch维度。

量化张量不带Batch维度时，接口复用同一份量化参数完成batch搬运：

```cpp
auto quant = make_tensor(make_mem_ptr(quant_addr), make_frame_layout<nd_ext_layout_ptn>(1, n));

auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
copy(atom, dst, l0c, quant);
```

量化张量带Batch维度时，不能通过一条Fixpipe指令完成全部Batch搬运。接口会按Batch执行`for`循环，逐Batch使用对应的量化参数完成搬运：

```cpp
auto quant = make_tensor(make_mem_ptr(quant_addr), make_frame_layout<nd_ext_layout_ptn>(batch, 1, n));

auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
copy(atom, dst, l0c, quant);
```
