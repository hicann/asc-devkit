# API列表

## 同步与内存栅栏

**表1**  同步接口

| 接口名 | 功能描述 |
| --- | --- |
| [asc_syncthreads](../sync_and_memory_fence/sync_interface/asc_syncthreads.md) | 等待当前thread block内所有thread代码都执行到该函数位置。 |

**表2**  内存栅栏接口

| 接口名 | 功能描述 |
| --- | --- |
| [asc_threadfence](../sync_and_memory_fence/memory_fence/asc_threadfence.md) | 用于保证不同核对同一份全局、共享内存的访问过程中，写入操作的时序性。 |
| [asc_threadfence_block](../sync_and_memory_fence/memory_fence/asc_threadfence_block.md) | 用于协调同一线程块（Thread Block）内线程之间的内存操作顺序，确保某一线程在调用asc_threadfence_block()之前的所有内存读写操作对同一线程块内的其他线程可见。 |

## 原子操作

**表3**  原子操作

| 接口名 | 功能描述 |
| --- | --- |
| [asc_atomic_add](../atomic_operations/asc_atomic_add.md) | 对UB或Global Memory上的数据与指定数据执行原子加操作，即将指定数据累加到UB或Global Memory的数据中。 |
| [asc_atomic_sub](../atomic_operations/asc_atomic_sub.md) | 对UB或Global Memory上的数据与指定数据执行原子减操作，即在UB或Global Memory的数据上减去指定数据。 |
| [asc_atomic_exch](../atomic_operations/asc_atomic_exch.md) | 对UB或Global Memory地址做原子赋值操作，即将指定数据赋值到UB或Global Memory地址中。 |
| [asc_atomic_max](../atomic_operations/asc_atomic_max.md) | 对UB或Global Memory数据做原子求最大值操作，即将UB或Global Memory的数据与指定数据中的最大值赋值到UB或Global Memory地址中。 |
| [asc_atomic_min](../atomic_operations/asc_atomic_min.md) | 对UB或Global Memory数据做原子求最小值操作，即将UB或Global Memory的数据与指定数据中的最小值赋值到UB或Global Memory地址中。 |
| [asc_atomic_inc](../atomic_operations/asc_atomic_inc.md) | 对UB或Global Memory上address的数值进行原子加1操作，如果address上的数值大于等于指定数值val，则对address赋值为0，否则将address上数值加1。 |
| [asc_atomic_dec](../atomic_operations/asc_atomic_dec.md) | 对UB或Global Memory上address的数值进行原子减1操作，如果address上的数值等于0或大于指定数值val，则对address赋值为val，否则将address上数值减1。 |
| [asc_atomic_cas](../atomic_operations/asc_atomic_cas.md) | 对UB或Global Memory上address的数值进行原子比较赋值操作，如果address上的数值等于指定数值compare，则对address赋值为指定数值val，否则address的数值不变。 |
| [asc_atomic_and](../atomic_operations/asc_atomic_and.md) | 对UB或Global Memory上address的数值与指定数值val进行原子与（&）操作，即将address数值与（&）val的结果赋值到UB或Global Memory上。 |
| [asc_atomic_or](../atomic_operations/asc_atomic_or.md) | 对UB或Global Memory上address的数值与指定数值val进行原子或（\|）操作，即将address数值或（\|）val的结果赋值到UB或Global Memory上。 |
| [asc_atomic_xor](../atomic_operations/asc_atomic_xor.md) | 对UB或Global Memory上address的数值与指定数值val进行原子异或（^）操作，即将address数值异或（^）val的结果赋值到UB或Global Memory上。 |

## Warp函数

**表4**  Warp Vote类函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_all](../Warp_functions/Warp_vote_functions/asc_all.md) | 判断是否所有活跃线程的输入均不为0。 |
| [asc_any](../Warp_functions/Warp_vote_functions/asc_any.md) | 判断是否有活跃线程的输入不为0。 |
| [asc_ballot](../Warp_functions/Warp_vote_functions/asc_ballot.md) | 判断Warp内每个活跃线程的输入是否不为0。 |
| [asc_activemask](../Warp_functions/Warp_vote_functions/asc_activemask.md) | 查看Warp内所有线程是否为活跃状态。 |

**表5**  Warp Shfl类函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_shfl](../Warp_functions/Warp_shfl_functions/asc_shfl.md) | 获取Warp内指定线程srcLane输入的用于交换的var值。 |
| [asc_shfl_up](../Warp_functions/Warp_shfl_functions/asc_shfl_up.md) | 获取Warp内当前线程向前偏移delta（当前线程Lane ID - delta）的线程输入的用于交换的var值。 |
| [asc_shfl_down](../Warp_functions/Warp_shfl_functions/asc_shfl_down.md) | 获取Warp内当前线程向后偏移delta（当前线程Lane ID + delta）的线程输入的用于交换的var值。 |
| [asc_shfl_xor](../Warp_functions/Warp_shfl_functions/asc_shfl_xor.md) | 获取Warp内当前线程Lane ID与输入laneMask做异或操作（Lane ID ^ laneMask）得到的dstLaneId对应线程输入的用于交换的var值。 |

**表6**  Warp Reduce类函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_reduce_add](../Warp_functions/Warp_reduce_functions/asc_reduce_add.md) | 对Warp内所有活跃线程输入的val求和。 |
| [asc_reduce_max](../Warp_functions/Warp_reduce_functions/asc_reduce_max.md) | 对Warp内所有活跃线程输入的val求最大值。 |
| [asc_reduce_min](../Warp_functions/Warp_reduce_functions/asc_reduce_min.md) | 对Warp内所有活跃线程输入val求最小值。 |

**表7** Lane ID类函数

| 接口名 | 功能描述 |
| --- | --- |
| [laneid](../Warp_functions/lane_id_functions/laneid.md) | 获取线程在其线程束内的索引。 |
| [lanemask_eq](../Warp_functions/lane_id_functions/lanemask_eq.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，只有当前线程所在的lane位被置为1，其余位为0。 |
| [lanemask_le](../Warp_functions/lane_id_functions/lanemask_le.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID小于等于当前线程”的线程的对应位设为1，其余位为0。 |
| [lanemask_lt](../Warp_functions/lane_id_functions/lanemask_lt.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID严格小于当前线程”的线程的对应位设为1，其余位为0。 |
| [lanemask_ge](../Warp_functions/lane_id_functions/lanemask_ge.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID大于等于当前线程”的线程的对应位设为1，其余位为0。 |
| [lanemask_gt](../Warp_functions/lane_id_functions/lanemask_gt.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID严格大于当前线程”的线程的对应位设为1，其余位为0。 |

## 数学函数

**表8**  half类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__habs](../math_functions/half_type/half_arithmetic_functions/__habs.md) | 获取输入数据的绝对值。 |
| [__hfma](../math_functions/half_type/half_arithmetic_functions/__hfma.md) | 对输入数据x、y、z，计算x与y相乘加上z的结果。 |
| [__hadd](../math_functions/half_type/half_arithmetic_functions/__hadd.md) | 计算两个half类型数据的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsub](../math_functions/half_type/half_arithmetic_functions/__hsub.md) | 计算两个half类型数据的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmul](../math_functions/half_type/half_arithmetic_functions/__hmul.md) | 计算两个half类型数据的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdiv](../math_functions/half_type/half_arithmetic_functions/__hdiv.md) | 计算两个half类型数据的相除结果，并遵循CAST_RINT模式舍入。 |
| [__hneg](../math_functions/half_type/half_arithmetic_functions/__hneg.md) | 获取输入half类型数据的负值。 |
| [__hfma_relu](../math_functions/half_type/half_arithmetic_functions/__hfma_relu.md) | 对输入half类型数据x、y、z，计算x与y相乘加上z的结果，并遵循CAST_RINT模式舍入。负数结果置为0。 |

**表9**  half类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hmax](../math_functions/half_type/half_compare_functions/__hmax.md) | 获取两个输入数据中的最大值。 |
| [__hmin](../math_functions/half_type/half_compare_functions/__hmin.md) | 获取两个输入数据中的最小值。 |
| [__hisnan](../math_functions/half_type/half_compare_functions/__hisnan.md) | 判断浮点数是否为nan。 |
| [__hisinf](../math_functions/half_type/half_compare_functions/__hisinf.md) | 判断浮点数是否为无穷。 |
| [__heq](../math_functions/half_type/half_compare_functions/__heq.md) | 比较两个half类型数据是否相等，相等时返回true。 |
| [__hne](../math_functions/half_type/half_compare_functions/__hne.md) | 比较两个half类型数据是否不相等，不相等时返回true。 |
| [__hle](../math_functions/half_type/half_compare_functions/__hle.md) | 比较两个half类型数据，仅当第一个数小于或等于第二个数时返回true。 |
| [__hge](../math_functions/half_type/half_compare_functions/__hge.md) | 比较两个half类型数据，仅当第一个数大于或等于第二个数时返回true。 |
| [__hlt](../math_functions/half_type/half_compare_functions/__hlt.md) | 比较两个half类型数据，仅当第一个数小于第二个数时返回true。 |
| [__hgt](../math_functions/half_type/half_compare_functions/__hgt.md) | 比较两个half类型数据，仅当第一个数大于第二个数时返回true。 |
| [__hequ](../math_functions/half_type/half_compare_functions/__hequ.md) | 比较两个half类型数据是否相等，相等时返回true。若任一输入为nan，返回true。 |
| [__hneu](../math_functions/half_type/half_compare_functions/__hneu.md) | 比较两个half类型数据是否不相等，不相等时返回true。若任一输入为nan，返回true。 |
| [__hleu](../math_functions/half_type/half_compare_functions/__hleu.md) | 比较两个half类型数据，当第一个数小于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgeu](../math_functions/half_type/half_compare_functions/__hgeu.md) | 比较两个half类型数据，当第一个数大于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hltu](../math_functions/half_type/half_compare_functions/__hltu.md) | 比较两个half类型数据，当第一个数小于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgtu](../math_functions/half_type/half_compare_functions/__hgtu.md) | 比较两个half类型数据，当第一个数大于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hmax_nan](../math_functions/half_type/half_compare_functions/__hmax_nan.md) | 获取两个输入数据中的最大值。任一输入为nan时返回nan。 |
| [__hmin_nan](../math_functions/half_type/half_compare_functions/__hmin_nan.md) | 获取两个输入数据中的最小值。任一输入为nan时返回nan。 |

**表10**  half类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [htanh](../math_functions/half_type/half_math_functions/htanh.md) | 获取输入数据的双曲正切值。 |
| [hexp](../math_functions/half_type/half_math_functions/hexp.md) | 指定输入x，获取e的x次方。 |
| [hexp2](../math_functions/half_type/half_math_functions/hexp2.md) | 指定输入x，获取2的x次方。 |
| [hexp10](../math_functions/half_type/half_math_functions/hexp10.md) | 指定输入x，获取10的x次方。 |
| [hlog](../math_functions/half_type/half_math_functions/hlog.md) | 获取以e为底，输入数据的对数。 |
| [hlog2](../math_functions/half_type/half_math_functions/hlog2.md) | 获取以2为底，输入数据的对数。 |
| [hlog10](../math_functions/half_type/half_math_functions/hlog10.md) | 获取以10为底，输入数据的对数。 |
| [hcos](../math_functions/half_type/half_math_functions/hcos.md) | 获取输入数据的三角函数余弦值。 |
| [hsin](../math_functions/half_type/half_math_functions/hsin.md) | 获取输入数据的三角函数正弦值。 |
| [hsqrt](../math_functions/half_type/half_math_functions/hsqrt.md) | 获取输入数据x的平方根。 |
| [hrsqrt](../math_functions/half_type/half_math_functions/hrsqrt.md) | 获取输入数据x的平方根的倒数。 |
| [hrcp](../math_functions/half_type/half_math_functions/hrcp.md) | 获取输入数据x的倒数。 |
| [hrint](../math_functions/half_type/half_math_functions/hrint.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [hfloor](../math_functions/half_type/half_math_functions/hfloor.md) | 获取小于或等于输入数据的最大整数值。 |
| [hceil](../math_functions/half_type/half_math_functions/hceil.md) | 获取大于或等于输入数据的最小整数值。 |
| [htrunc](../math_functions/half_type/half_math_functions/htrunc.md) | 获取对输入数据的浮点数截断后的整数。 |

**表11**  half类型精度转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__float2half](../math_functions/half_type/half_precision_conversion_functions/__float2half.md) | 获取输入遵循CAST_RINT模式转换成的半精度浮点数。 |
| [__float2half_rn](../math_functions/half_type/half_precision_conversion_functions/__float2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的半精度浮点数。 |
| [__float2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rn_sat.md) | 饱和模式下获取输入遵循CAST_RINT模式转换成的半精度浮点数。 |
| [__float22half2_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rn_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_RINT模式转换成的half2类型数据。 |
| [__float2half_rz](../math_functions/half_type/half_precision_conversion_functions/__float2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的半精度浮点数。 |
| [__float2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rz_sat.md) | 饱和模式下获取输入遵循CAST_TRUNC模式转换成的半精度浮点数。 |
| [__float22half2_rz](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rz.md) | 获取输入的两个分量遵循CAST_TRUNC模式转换成的half2类型数据。 |
| [__float22half2_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rz_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_TRUNC模式转换成的half2类型数据。 |
| [__float2half_rd](../math_functions/half_type/half_precision_conversion_functions/__float2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的半精度浮点数。 |
| [__float2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rd_sat.md) | 饱和模式下获取输入遵循CAST_FLOOR模式转换成的半精度浮点数。 |
| [__float22half2_rd](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rd.md) | 获取输入的两个分量遵循CAST_FLOOR模式转换成的half2类型数据。 |
| [__float22half2_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rd_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_FLOOR模式转换成的half2类型数据。 |
| [__float2half_ru](../math_functions/half_type/half_precision_conversion_functions/__float2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的半精度浮点数。 |
| [__float2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_ru_sat.md) | 饱和模式下获取输入遵循CAST_CEIL模式转换成的半精度浮点数。 |
| [__float22half2_ru](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ru.md) | 获取输入的两个分量遵循CAST_CEIL模式转换成的half2类型数据。 |
| [__float22half2_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ru_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_CEIL模式转换成的half2类型数据。 |
| [__float2half_rna](../math_functions/half_type/half_precision_conversion_functions/__float2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的半精度浮点数。 |
| [__float2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rna_sat.md) | 饱和模式下获取输入遵循CAST_ROUND模式转换成的半精度浮点数。 |
| [__float22half2_rna](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rna.md) | 获取输入的两个分量遵循CAST_ROUND模式转换成的half2类型数据。 |
| [__float22half2_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rna_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_ROUND模式转换成的half2类型数据。 |
| [__float2half_ro](../math_functions/half_type/half_precision_conversion_functions/__float2half_ro.md) | 获取输入遵循CAST_ODD模式转换成的半精度浮点数。 |
| [__float2half_ro_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_ro_sat.md) | 饱和模式下获取输入遵循CAST_ODD模式转换成的半精度浮点数。 |
| [__float22half2_ro](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ro.md) | 获取输入的两个分量遵循CAST_ODD模式转换成的half2类型数据。 |
| [__float22half2_ro_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ro_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_ODD模式转换成的half2类型数据。 |
| [__half2float](../math_functions/half_type/half_precision_conversion_functions/__half2float.md) | 获取输入转换成的浮点数。 |
| [__half2half_rn](../math_functions/half_type/half_precision_conversion_functions/__half2half_rn.md) | 获取输入遵循CAST_RINT模式取整后的half类型数据。 |
| [__half2half_rz](../math_functions/half_type/half_precision_conversion_functions/__half2half_rz.md) | 获取输入遵循CAST_TRUNC模式取整后的half类型数据。 |
| [__half2half_rd](../math_functions/half_type/half_precision_conversion_functions/__half2half_rd.md) | 获取输入遵循CAST_FLOOR模式取整后的half类型数据。 |
| [__half2half_ru](../math_functions/half_type/half_precision_conversion_functions/__half2half_ru.md) | 获取输入遵循CAST_CEIL模式取整后的half类型数据。 |
| [__half2half_rna](../math_functions/half_type/half_precision_conversion_functions/__half2half_rna.md) | 获取输入遵循CAST_ROUND模式取整后的half类型数据。 |
| [__half2uint_rn](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rn.md) | 获取输入遵循CAST_RINT模式转换成的无符号整数。 |
| [__half2uint_rz](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的无符号整数。 |
| [__half2uint_rd](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的无符号整数。 |
| [__half2uint_ru](../math_functions/half_type/half_precision_conversion_functions/__half2uint_ru.md) | 获取输入遵循CAST_CEIL模式转换成的无符号整数。 |
| [__half2uint_rna](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rna.md) | 获取输入遵循CAST_ROUND模式转换成的无符号整数。 |
| [__half2int_rn](../math_functions/half_type/half_precision_conversion_functions/__half2int_rn.md) | 获取输入遵循CAST_RINT模式转换成的有符号整数。 |
| [__half2int_rz](../math_functions/half_type/half_precision_conversion_functions/__half2int_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的有符号整数。 |
| [__half2int_rd](../math_functions/half_type/half_precision_conversion_functions/__half2int_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的有符号整数。 |
| [__half2int_ru](../math_functions/half_type/half_precision_conversion_functions/__half2int_ru.md) | 获取输入遵循CAST_CEIL模式转换成的有符号整数。 |
| [__half2int_rna](../math_functions/half_type/half_precision_conversion_functions/__half2int_rna.md) | 获取输入遵循CAST_ROUND模式转换成的有符号整数。 |
| [__half2char_rz](../math_functions/half_type/half_precision_conversion_functions/__half2char_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的signed char类型数据。 |
| [__half2uchar_rz](../math_functions/half_type/half_precision_conversion_functions/__half2uchar_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的unsigned char类型数据。 |
| [__half2short_rn](../math_functions/half_type/half_precision_conversion_functions/__half2short_rn.md) | 获取输入遵循CAST_RINT模式转换成的short int类型数据。 |
| [__half2short_rz](../math_functions/half_type/half_precision_conversion_functions/__half2short_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的short int类型数据。 |
| [__half2short_rd](../math_functions/half_type/half_precision_conversion_functions/__half2short_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的short int类型数据。 |
| [__half2short_ru](../math_functions/half_type/half_precision_conversion_functions/__half2short_ru.md) | 获取输入遵循CAST_CEIL模式转换成的short int类型数据。 |
| [__half2ushort_rn](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_rn.md) | 获取输入遵循CAST_RINT模式转换成的unsigned short int类型数据。 |
| [__half2ushort_rz](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的unsigned short int类型数据。 |
| [__half2ushort_rd](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的unsigned short int类型数据。 |
| [__half2ushort_ru](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_ru.md) | 获取输入遵循CAST_CEIL模式转换成的unsigned short int类型数据。 |
| [__half2ull_rn](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位无符号整数。 |
| [__half2ull_rz](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位无符号整数。 |
| [__half2ull_rd](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位无符号整数。 |
| [__half2ull_ru](../math_functions/half_type/half_precision_conversion_functions/__half2ull_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位无符号整数。 |
| [__half2ull_rna](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位无符号整数。 |
| [__half2ll_rn](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位有符号整数。 |
| [__half2ll_rz](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位有符号整数。 |
| [__half2ll_rd](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位有符号整数。 |
| [__half2ll_ru](../math_functions/half_type/half_precision_conversion_functions/__half2ll_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位有符号整数。 |
| [__half2ll_rna](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位有符号整数。 |
| [__bfloat162half_rn](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__bfloat162half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn_sat.md) | 饱和模式下获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__bfloat162half_rz](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__bfloat162half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz_sat.md) | 饱和模式下获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__bfloat162half_rd](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__bfloat162half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd_sat.md) | 饱和模式下获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__bfloat162half_ru](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__bfloat162half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru_sat.md) | 饱和模式下获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__bfloat162half_rna](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__bfloat162half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna_sat.md) | 饱和模式下获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__uint2half_rn](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__uint2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rn_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_RINT模式。 |
| [__uint2half_rz](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__uint2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rz_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_TRUNC模式。 |
| [__uint2half_rd](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__uint2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rd_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_FLOOR模式。 |
| [__uint2half_ru](../math_functions/half_type/half_precision_conversion_functions/__uint2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__uint2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_ru_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_CEIL模式。 |
| [__uint2half_rna](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__uint2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rna_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_ROUND模式。 |
| [__int2half_rn](../math_functions/half_type/half_precision_conversion_functions/__int2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__int2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rn_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_RINT模式。 |
| [__int2half_rz](../math_functions/half_type/half_precision_conversion_functions/__int2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__int2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rz_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_TRUNC模式。 |
| [__int2half_rd](../math_functions/half_type/half_precision_conversion_functions/__int2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__int2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rd_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_FLOOR模式。 |
| [__int2half_ru](../math_functions/half_type/half_precision_conversion_functions/__int2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__int2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_ru_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_CEIL模式。 |
| [__int2half_rna](../math_functions/half_type/half_precision_conversion_functions/__int2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__int2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rna_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_ROUND模式。 |
| [__short2half_rn](../math_functions/half_type/half_precision_conversion_functions/__short2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__short2half_rz](../math_functions/half_type/half_precision_conversion_functions/__short2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__short2half_rd](../math_functions/half_type/half_precision_conversion_functions/__short2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__short2half_ru](../math_functions/half_type/half_precision_conversion_functions/__short2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__ushort2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__ushort2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__ushort2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__ushort2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__ull2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__ull2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__ull2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__ull2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ull2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__ull2half_rna](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__ll2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__ll2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__ll2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__ll2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ll2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__ll2half_rna](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__floats2half2_rn](../math_functions/half_type/half_precision_conversion_functions/__floats2half2_rn.md) | 将输入的数据x，y遵循CAST_RINT模式分别转换为half类型并填充到half2的前后两部分，返回转换后的half2类型数据。 |
| [__float22half2_rn](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rn.md) | 将float2类型数据遵循CAST_RINT模式转换为half2类型，返回转换后的half2类型数据。 |
| [__float2half2_rn](../math_functions/half_type/half_precision_conversion_functions/__float2half2_rn.md) | 获取输入遵循CAST_RINT模式转换并复制到两个分量后的half2类型数据。 |
| [__low2half](../math_functions/half_type/half_precision_conversion_functions/__low2half.md) | 返回输入数据的低16位。 |
| [__low2half2](../math_functions/half_type/half_precision_conversion_functions/__low2half2.md) | 将输入数据的低16位填充到half2并返回。 |
| [__low2float](../math_functions/half_type/half_precision_conversion_functions/__low2float.md) | 将输入数据的低16位转换为浮点数并返回结果。 |
| [__lowhigh2highlow](../math_functions/half_type/half_precision_conversion_functions/__lowhigh2highlow.md) | 将输入数据的高低16位进行交换并返回。 |
| [__high2half](../math_functions/half_type/half_precision_conversion_functions/__high2half.md) | 提取输入half2的高16位，并返回 |
| [__high2half2](../math_functions/half_type/half_precision_conversion_functions/__high2half2.md) | 将输入数据的高16位填充到half2并返回结果。 |
| [__high2float](../math_functions/half_type/half_precision_conversion_functions/__high2float.md) | 将输入数据的高16位转换为float类型并返回结果。 |
| [__highs2half2](../math_functions/half_type/half_precision_conversion_functions/__highs2half2.md) | 分别提取两个half2输入的高16位，并填充到half2中。返回填充后的数据。 |
| [__lows2half2](../math_functions/half_type/half_precision_conversion_functions/__lows2half2.md) | 分别提取两个half2输入的低16位，并填充到half2中。返回填充后的数据。 |
| [__halves2half2](../math_functions/half_type/half_precision_conversion_functions/__halves2half2.md) | 将输入的数据分别填充为half2前后两个分量，返回填充后数据。 |
| [__half2half2](../math_functions/half_type/half_precision_conversion_functions/__half2half2.md) | 将half输入数据复制到half2的两个分量并返回。 |
| [__half22float2](../math_functions/half_type/half_precision_conversion_functions/__half22float2.md) | 将half2的两个分量分别转换为float，并填充到float2返回。 |
| [__half_as_short](../math_functions/half_type/half_precision_conversion_functions/__half_as_short.md) | 将half数据按位重新解释为short int类型数据。 |
| [__half_as_ushort](../math_functions/half_type/half_precision_conversion_functions/__half_as_ushort.md) | 将half数据按位重新解释为unsigned short int类型数据。 |
| [__short_as_half](../math_functions/half_type/half_precision_conversion_functions/__short_as_half.md) | 将short int的按位重新解释为half，即将short int的数据存储的位按照half的格式进行读取。 |
| [__ushort_as_half](../math_functions/half_type/half_precision_conversion_functions/__ushort_as_half.md) | 将unsigned short int的按位重新解释为half，即将unsigned short int的数据存储的位按照half的格式进行读取。 |

**表12**  half2类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__haddx2](../math_functions/half_type/half2_arithmetic_functions/__haddx2.md) | 计算两个half2类型数据各分量的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsubx2](../math_functions/half_type/half2_arithmetic_functions/__hsubx2.md) | 计算两个half2类型数据各分量的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmulx2](../math_functions/half_type/half2_arithmetic_functions/__hmulx2.md) | 计算两个half2类型数据各分量的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdivx2](../math_functions/half_type/half2_arithmetic_functions/__hdivx2.md) | 计算两个half2类型数据各分量的相除结果，并遵循CAST_RINT模式舍入。 |
| [__habsx2](../math_functions/half_type/half2_arithmetic_functions/__habsx2.md) | 计算输入half2类型数据各分量的绝对值。 |
| [__hfmax2](../math_functions/half_type/half2_arithmetic_functions/__hfmax2.md) | 计算两个half2类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。 |
| [__hnegx2](../math_functions/half_type/half2_arithmetic_functions/__hnegx2.md) | 获取输入half2类型数据各分量的负值。 |
| [__hfmax2_relu](../math_functions/half_type/half2_arithmetic_functions/__hfmax2_relu.md) | 计算两个half2类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。负数结果置为0。 |
| [__hcmadd](../math_functions/half_type/half2_arithmetic_functions/__hcmadd.md) | 将三个half2输入视为复数（第一个分量为实部，第二个分量为虚部），执行复数乘加运算x*y+z。 |

**表13**  half2类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hbeqx2](../math_functions/half_type/half2_compare_functions/__hbeqx2.md) | 比较两个half2类型数据的两个分量是否相等，仅当两个分量均相等时返回true。 |
| [__hbnex2](../math_functions/half_type/half2_compare_functions/__hbnex2.md) | 比较两个half2类型数据的两个分量是否不相等，仅当两个分量均不相等时返回true。 |
| [__hblex2](../math_functions/half_type/half2_compare_functions/__hblex2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数小于或等于第二个数时返回true。 |
| [__hbgex2](../math_functions/half_type/half2_compare_functions/__hbgex2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数大于或等于第二个数时返回true。 |
| [__hbltx2](../math_functions/half_type/half2_compare_functions/__hbltx2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数小于第二个数时返回true。 |
| [__hbgtx2](../math_functions/half_type/half2_compare_functions/__hbgtx2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数大于第二个数时返回true。 |
| [__hbequx2](../math_functions/half_type/half2_compare_functions/__hbequx2.md) | 比较两个half2类型数据的两个分量是否相等，当两个分量均相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbneux2](../math_functions/half_type/half2_compare_functions/__hbneux2.md) | 比较两个half2类型数据的两个分量是否不相等，当两个分量均不相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbleux2](../math_functions/half_type/half2_compare_functions/__hbleux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数小于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgeux2](../math_functions/half_type/half2_compare_functions/__hbgeux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数大于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbltux2](../math_functions/half_type/half2_compare_functions/__hbltux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数小于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgtux2](../math_functions/half_type/half2_compare_functions/__hbgtux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数大于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__heqx2](../math_functions/half_type/half2_compare_functions/__heqx2.md) | 比较两个half2类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。 |
| [__hnex2](../math_functions/half_type/half2_compare_functions/__hnex2.md) | 比较两个half2类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。 |
| [__hlex2](../math_functions/half_type/half2_compare_functions/__hlex2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgex2](../math_functions/half_type/half2_compare_functions/__hgex2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hltx2](../math_functions/half_type/half2_compare_functions/__hltx2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgtx2](../math_functions/half_type/half2_compare_functions/__hgtx2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hequx2](../math_functions/half_type/half2_compare_functions/__hequx2.md) | 比较两个half2类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hneux2](../math_functions/half_type/half2_compare_functions/__hneux2.md) | 比较两个half2类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hleux2](../math_functions/half_type/half2_compare_functions/__hleux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgeux2](../math_functions/half_type/half2_compare_functions/__hgeux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hltux2](../math_functions/half_type/half2_compare_functions/__hltux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgtux2](../math_functions/half_type/half2_compare_functions/__hgtux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__heqx2_mask](../math_functions/half_type/half2_compare_functions/__heqx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hnex2_mask](../math_functions/half_type/half2_compare_functions/__hnex2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hlex2_mask](../math_functions/half_type/half2_compare_functions/__hlex2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgex2_mask](../math_functions/half_type/half2_compare_functions/__hgex2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hltx2_mask](../math_functions/half_type/half2_compare_functions/__hltx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgtx2_mask](../math_functions/half_type/half2_compare_functions/__hgtx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hequx2_mask](../math_functions/half_type/half2_compare_functions/__hequx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hneux2_mask](../math_functions/half_type/half2_compare_functions/__hneux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hleux2_mask](../math_functions/half_type/half2_compare_functions/__hleux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgeux2_mask](../math_functions/half_type/half2_compare_functions/__hgeux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hltux2_mask](../math_functions/half_type/half2_compare_functions/__hltux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgtux2_mask](../math_functions/half_type/half2_compare_functions/__hgtux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__isnanx2](../math_functions/half_type/half2_compare_functions/__isnanx2.md) | 判断half2类型数据的两个分量是否为nan。 |
| [__hmaxx2](../math_functions/half_type/half2_compare_functions/__hmaxx2.md) | 获取两个half2类型数据各分量的最大值。 |
| [__hmaxx2_nan](../math_functions/half_type/half2_compare_functions/__hmaxx2_nan.md) | 获取两个half2类型数据各分量的最大值。任一分量为nan时对应结果为nan。 |
| [__hminx2](../math_functions/half_type/half2_compare_functions/__hminx2.md) | 获取两个half2类型数据各分量的最小值。 |
| [__hminx2_nan](../math_functions/half_type/half2_compare_functions/__hminx2_nan.md) | 获取两个half2类型数据各分量的最小值。任一分量为nan时对应结果为nan。 |

**表14**  half2类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [h2tanh](../math_functions/half_type/half2_math_functions/h2tanh.md) | 获取输入数据各元素的双曲正切值。 |
| [h2exp](../math_functions/half_type/half2_math_functions/h2exp.md) | 指定输入x，对x的各元素，获取e的该元素次方。 |
| [h2exp2](../math_functions/half_type/half2_math_functions/h2exp2.md) | 指定输入x，对x的各元素，获取2的该元素次方。 |
| [h2exp10](../math_functions/half_type/half2_math_functions/h2exp10.md) | 指定输入x，对x的各元素，获取10的该元素次方。 |
| [h2log](../math_functions/half_type/half2_math_functions/h2log.md) | 获取以e为底，输入数据各元素的对数。 |
| [h2log2](../math_functions/half_type/half2_math_functions/h2log2.md) | 获取以2为底，输入数据各元素的对数。 |
| [h2log10](../math_functions/half_type/half2_math_functions/h2log10.md) | 获取以10为底，输入数据各元素的对数。 |
| [h2cos](../math_functions/half_type/half2_math_functions/h2cos.md) | 获取输入数据各元素的三角函数余弦值。 |
| [h2sin](../math_functions/half_type/half2_math_functions/h2sin.md) | 获取输入数据各元素的三角函数正弦值。 |
| [h2sqrt](../math_functions/half_type/half2_math_functions/h2sqrt.md) | 获取输入数据x各元素的平方根。 |
| [h2rsqrt](../math_functions/half_type/half2_math_functions/h2rsqrt.md) | 获取输入数据x各元素的平方根的倒数。 |
| [h2rcp](../math_functions/half_type/half2_math_functions/h2rcp.md) | 获取输入数据x各元素的倒数。 |
| [h2rint](../math_functions/half_type/half2_math_functions/h2rint.md) | 获取与输入数据各元素最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [h2floor](../math_functions/half_type/half2_math_functions/h2floor.md) | 获取小于或等于输入数据各元素的最大整数值。 |
| [h2ceil](../math_functions/half_type/half2_math_functions/h2ceil.md) | 获取大于或等于输入数据各元素的最小整数值。 |
| [h2trunc](../math_functions/half_type/half2_math_functions/h2trunc.md) | 获取对输入数据各元素的浮点数截断后的整数。 |

**表15**  bfloat16类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__habs](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs.md) | 获取输入数据的绝对值。 |
| [__hfma](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma.md) | 对输入数据x、y、z，计算x与y相乘加上z的结果。 |
| [__hadd](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd.md) | 计算两个bfloat16类型数据的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsub](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hsub.md) | 计算两个bfloat16类型数据的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmul](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul.md) | 计算两个bfloat16类型数据的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdiv](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv.md) | 计算两个bfloat16类型数据的相除结果，并遵循CAST_RINT模式舍入。 |
| [__hneg](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hneg.md) | 获取输入bfloat16类型数据的负值。 |
| [__hfma_relu](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_relu.md) | 对输入bfloat16类型数据x、y、z，计算x与y相乘加上z的结果，并遵循CAST_RINT模式舍入。负数结果置为0。 |

**表16**  bfloat16类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hmax](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmax.md) | 获取两个输入数据中的最大值。 |
| [__hmin](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmin.md) | 获取两个输入数据中的最小值。 |
| [__hisnan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hisnan.md) | 判断浮点数是否为nan。 |
| [__hisinf](../math_functions/bfloat16_type/bfloat16_compare_functions/__hisinf.md) | 判断浮点数是否为无穷。 |
| [__heq](../math_functions/bfloat16_type/bfloat16_compare_functions/__heq.md) | 比较两个bfloat16类型数据是否相等，相等时返回true。 |
| [__hne](../math_functions/bfloat16_type/bfloat16_compare_functions/__hne.md) | 比较两个bfloat16类型数据是否不相等，不相等时返回true。 |
| [__hle](../math_functions/bfloat16_type/bfloat16_compare_functions/__hle.md) | 比较两个bfloat16类型数据，仅当第一个数小于或等于第二个数时返回true。 |
| [__hge](../math_functions/bfloat16_type/bfloat16_compare_functions/__hge.md) | 比较两个bfloat16类型数据，仅当第一个数大于或等于第二个数时返回true。 |
| [__hlt](../math_functions/bfloat16_type/bfloat16_compare_functions/__hlt.md) | 比较两个bfloat16类型数据，仅当第一个数小于第二个数时返回true。 |
| [__hgt](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgt.md) | 比较两个bfloat16类型数据，仅当第一个数大于第二个数时返回true。 |
| [__hequ](../math_functions/bfloat16_type/bfloat16_compare_functions/__hequ.md) | 比较两个bfloat16类型数据是否相等，相等时返回true。若任一输入为nan，返回true。 |
| [__hneu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hneu.md) | 比较两个bfloat16类型数据是否不相等，不相等时返回true。若任一输入为nan，返回true。 |
| [__hleu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hleu.md) | 比较两个bfloat16类型数据，当第一个数小于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgeu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgeu.md) | 比较两个bfloat16类型数据，当第一个数大于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hltu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hltu.md) | 比较两个bfloat16类型数据，当第一个数小于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgtu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgtu.md) | 比较两个bfloat16类型数据，当第一个数大于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hmax_nan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmax_nan.md) | 获取两个输入数据中的最大值。任一输入为nan时返回nan。 |
| [__hmin_nan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmin_nan.md) | 获取两个输入数据中的最小值。任一输入为nan时返回nan。 |

**表17**  bfloat16数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [htanh](../math_functions/bfloat16_type/bfloat16_math_functions/htanh.md) | 获取输入数据的双曲正切值。 |
| [hexp](../math_functions/bfloat16_type/bfloat16_math_functions/hexp.md) | 指定输入x，获取e的x次方。 |
| [hexp2](../math_functions/bfloat16_type/bfloat16_math_functions/hexp2.md) | 指定输入x，获取2的x次方。 |
| [hexp10](../math_functions/bfloat16_type/bfloat16_math_functions/hexp10.md) | 指定输入x，获取10的x次方。 |
| [hlog](../math_functions/bfloat16_type/bfloat16_math_functions/hlog.md) | 获取以e为底，输入数据的对数。 |
| [hlog2](../math_functions/bfloat16_type/bfloat16_math_functions/hlog2.md) | 获取以2为底，输入数据的对数。 |
| [hlog10](../math_functions/bfloat16_type/bfloat16_math_functions/hlog10.md) | 获取以10为底，输入数据的对数。 |
| [hcos](../math_functions/bfloat16_type/bfloat16_math_functions/hcos.md) | 获取输入数据的三角函数余弦值。 |
| [hsin](../math_functions/bfloat16_type/bfloat16_math_functions/hsin.md) | 获取输入数据的三角函数正弦值。 |
| [hsqrt](../math_functions/bfloat16_type/bfloat16_math_functions/hsqrt.md) | 获取输入数据x的平方根。 |
| [hrsqrt](../math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt.md) | 获取输入数据x的平方根的倒数。 |
| [hrcp](../math_functions/bfloat16_type/bfloat16_math_functions/hrcp.md) | 获取输入数据x的倒数。 |
| [hrint](../math_functions/bfloat16_type/bfloat16_math_functions/hrint.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [hfloor](../math_functions/bfloat16_type/bfloat16_math_functions/hfloor.md) | 获取小于或等于输入数据的最大整数值。 |
| [hceil](../math_functions/bfloat16_type/bfloat16_math_functions/hceil.md) | 获取大于或等于输入数据的最小整数值。 |
| [htrunc](../math_functions/bfloat16_type/bfloat16_math_functions/htrunc.md) | 获取对输入数据的浮点数截断后的整数。 |

**表18**  bfloat16类型精度转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__float2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rn_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn_sat.md) | 饱和模式下获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rn_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_RINT模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rz_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz_sat.md) | 饱和模式下获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz.md) | 获取输入的两个分量遵循CAST_TRUNC模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_rz_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_TRUNC模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rd_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd_sat.md) | 饱和模式下获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd.md) | 获取输入的两个分量遵循CAST_FLOOR模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_rd_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_FLOOR模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_ru_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru_sat.md) | 饱和模式下获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru.md) | 获取输入的两个分量遵循CAST_CEIL模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_ru_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_CEIL模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rna_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna_sat.md) | 饱和模式下获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna.md) | 获取输入的两个分量遵循CAST_ROUND模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_rna_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_ROUND模式转换成的bfloat16x2_t类型数据。 |
| [__half2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__bfloat162float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162float.md) | 获取输入转换为浮点数的结果。 |
| [__bfloat162bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rn.md) | 获取输入遵循CAST_RINT模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式取整后的bfloat16_t类型数据。 |
| [__bfloat162uint_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rn.md) | 获取输入遵循CAST_RINT模式转换成的无符号整数。 |
| [__bfloat162uint_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的无符号整数。 |
| [__bfloat162uint_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的无符号整数。 |
| [__bfloat162uint_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_ru.md) | 获取输入遵循CAST_CEIL模式转换成的无符号整数。 |
| [__bfloat162uint_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rna.md) | 获取输入遵循CAST_ROUND模式转换成的无符号整数。 |
| [__bfloat162int_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rn.md) | 获取输入遵循CAST_RINT模式转换成的有符号整数。 |
| [__bfloat162int_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的有符号整数。 |
| [__bfloat162int_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的有符号整数。 |
| [__bfloat162int_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_ru.md) | 获取输入遵循CAST_CEIL模式转换成的有符号整数。 |
| [__bfloat162int_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rna.md) | 获取输入遵循CAST_ROUND模式转换成的有符号整数。 |
| [__bfloat162char_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162char_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的signed char类型数据。 |
| [__bfloat162uchar_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uchar_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的unsigned char类型数据。 |
| [__bfloat162short_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rn.md) | 获取输入遵循CAST_RINT模式转换成的short int类型数据。 |
| [__bfloat162short_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的short int类型数据。 |
| [__bfloat162short_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的short int类型数据。 |
| [__bfloat162short_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_ru.md) | 获取输入遵循CAST_CEIL模式转换成的short int类型数据。 |
| [__bfloat162ushort_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_rn.md) | 获取输入遵循CAST_RINT模式转换成的unsigned short int类型数据。 |
| [__bfloat162ushort_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的unsigned short int类型数据。 |
| [__bfloat162ushort_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的unsigned short int类型数据。 |
| [__bfloat162ushort_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_ru.md) | 获取输入遵循CAST_CEIL模式转换成的unsigned short int类型数据。 |
| [__bfloat162ull_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位无符号整数。 |
| [__bfloat162ull_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位无符号整数。 |
| [__bfloat162ull_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位无符号整数。 |
| [__bfloat162ull_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位无符号整数。 |
| [__bfloat162ull_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位无符号整数。 |
| [__bfloat162ll_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位有符号整数。 |
| [__bfloat162ll_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位有符号整数。 |
| [__bfloat162ll_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位有符号整数。 |
| [__bfloat162ll_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位有符号整数。 |
| [__bfloat162ll_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位有符号整数。 |
| [__uint2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__short2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__short2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__short2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__short2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__ushort2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__ushort2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__ushort2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__ushort2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__float2bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat162_rn.md) | 将float类型数据遵循CAST_RINT模式转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回填充后的bfloat16x2类型数据。 |
| [__floats2bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__floats2bfloat162_rn.md) | 将输入的数据x，y遵循CAST_RINT模式分别转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回转换后的bfloat16x2类型数据。 |
| [__float22bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn.md) | 将float2类型数据遵循CAST_RINT模式转换为bfloat16x2类型，返回转换后的bfloat16x2类型数据。 |
| [__bfloat162bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat162.md) | 将输入的数据填充为bfloat16x2前后两个分量，返回转换后的bfloat16x2类型数据。 |
| [__halves2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__halves2bfloat162.md) | 将输入的数据分别填充为bfloat16x2前后两个分量，返回填充后数据。 |
| [__high2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat16.md) | 提取输入bfloat16x2的高16位，并返回。 |
| [__high2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat162.md) | 将输入数据的高16位填充到bfloat16x2并返回结果。 |
| [__high2float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2float.md) | 将输入数据的高16位转换为float类型并返回结果。 |
| [__highs2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__highs2bfloat162.md) | 分别提取两个bfloat162输入的高16位，并填充到bfloat162中。返回填充后的数据。 |
| [__low2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat16.md) | 返回输入数据的低16位。 |
| [__low2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat162.md) | 将输入数据的低16位填充到bfloat16x2并返回。 |
| [__low2float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2float.md) | 将输入数据的低16位转换为浮点数并返回结果。 |
| [__lowhigh2highlow](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lowhigh2highlow.md) | 将输入数据的高低16位进行交换并返回。 |
| [__lows2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lows2bfloat162.md) | 分别提取两个bfloat162输入的低16位，并填充到bfloat162中。返回填充后的数据。 |
| [__bfloat1622float2](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat1622float2.md) | 将bfloat16x2的两个分量分别转换为float，并填充到float2返回。 |
| [__bfloat16_as_short](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat16_as_short.md) | 将bfloat16数据按位重新解释为short int类型数据。 |
| [__bfloat16_as_ushort](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat16_as_ushort.md) | 将bfloat16数据按位重新解释为unsigned short int类型数据。 |
| [__short_as_bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short_as_bfloat16.md) | 将short int的按位重新解释为bfloat16，即将short int的数据存储的位按照bfloat16的格式进行读取。 |
| [__ushort_as_bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort_as_bfloat16.md) | 将unsigned short int的按位重新解释为bfloat16，即将unsigned short int的数据存储的位按照bfloat16的格式进行读取。 |

**表19**  bfloat16x2类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__haddx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2.md) | 计算两个bfloat16x2_t类型数据各分量的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsubx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hsubx2.md) | 计算两个bfloat16x2_t类型数据各分量的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmulx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hmulx2.md) | 计算两个bfloat16x2_t类型数据各分量的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdivx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2.md) | 计算两个bfloat16x2_t类型数据各分量的相除结果，并遵循CAST_RINT模式舍入。 |
| [__habsx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__habsx2.md) | 计算输入bfloat16x2_t类型数据各分量的绝对值。 |
| [__hfmax2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2.md) | 计算两个bfloat16x2_t类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。 |
| [__hnegx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hnegx2.md) | 获取输入bfloat16x2_t类型数据各分量的负值。 |
| [__hfmax2_relu](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_relu.md) | 计算两个bfloat16x2_t类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。负数结果置为0。 |
| [__hcmadd](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hcmadd.md) | 将三个bfloat16x2_t输入视为复数（第一个分量为实部，第二个分量为虚部），执行复数乘加运算x*y+z。 |

**表20**  bfloat16x2类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hbeqx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbeqx2.md) | 比较两个bfloat16x2_t类型数据的两个分量是否相等，仅当两个分量均相等时返回true。 |
| [__hbnex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbnex2.md) | 比较两个bfloat16x2_t类型数据的两个分量是否不相等，仅当两个分量均不相等时返回true。 |
| [__hblex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hblex2.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数小于或等于第二个数时返回true。 |
| [__hbgex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgex2.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数大于或等于第二个数时返回true。 |
| [__hbltx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltx2.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数小于第二个数时返回true。 |
| [__hbgtx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtx2.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数大于第二个数时返回true。 |
| [__hbequx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbequx2.md) | 比较两个bfloat16x2_t类型数据的两个分量是否相等，当两个分量均相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbneux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbneux2.md) | 比较两个bfloat16x2_t类型数据的两个分量是否不相等，当两个分量均不相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbleux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbleux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数小于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgeux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgeux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数大于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbltux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数小于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgtux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数大于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__heqx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。 |
| [__hnex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。 |
| [__hlex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hltx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgtx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hequx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hneux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hleux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgeux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hltux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgtux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__heqx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hnex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hlex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hltx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgtx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hequx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hneux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hleux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgeux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hltux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgtux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2_mask.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__isnanx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__isnanx2.md) | 判断bfloat16x2_t类型数据的两个分量是否为nan。 |
| [__hmaxx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2.md) | 获取两个bfloat16x2_t类型数据各分量的最大值。 |
| [__hmaxx2_nan](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2_nan.md) | 获取两个bfloat16x2_t类型数据各分量的最大值。任一分量为nan时对应结果为nan。 |
| [__hminx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2.md) | 获取两个bfloat16x2_t类型数据各分量的最小值。 |
| [__hminx2_nan](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2_nan.md) | 获取两个bfloat16x2_t类型数据各分量的最小值。任一分量为nan时对应结果为nan。 |

**表21**  bfloat16x2类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [h2tanh](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh.md) | 获取输入数据各元素的双曲正切值。 |
| [h2exp](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp.md) | 指定输入x，对x的各元素，获取e的该元素次方。 |
| [h2exp2](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2.md) | 指定输入x，对x的各元素，获取2的该元素次方。 |
| [h2exp10](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10.md) | 指定输入x，对x的各元素，获取10的该元素次方。 |
| [h2log](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log.md) | 获取以e为底，输入数据各元素的对数。 |
| [h2log2](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2.md) | 获取以2为底，输入数据各元素的对数。 |
| [h2log10](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10.md) | 获取以10为底，输入数据各元素的对数。 |
| [h2cos](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2cos.md) | 获取输入数据各元素的三角函数余弦值。 |
| [h2sin](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2sin.md) | 获取输入数据各元素的三角函数正弦值。 |
| [h2sqrt](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt.md) | 获取输入数据x各元素的平方根。 |
| [h2rsqrt](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt.md) | 获取输入数据x各元素的平方根的倒数。 |
| [h2rcp](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rcp.md) | 获取输入数据x各元素的倒数。 |
| [h2rint](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rint.md) | 获取与输入数据各元素最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [h2floor](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2floor.md) | 获取小于或等于输入数据各元素的最大整数值。 |
| [h2ceil](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2ceil.md) | 获取大于或等于输入数据各元素的最小整数值。 |
| [h2trunc](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2trunc.md) | 获取对输入数据各元素的浮点数截断后的整数。 |

**表22**  float类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [tanf](../math_functions/float_math_functions/tanf.md) | 获取输入数据的三角函数正切值。 |
| [tanhf](../math_functions/float_math_functions/tanhf.md) | 获取输入数据的双曲正切值。 |
| [tanpif](../math_functions/float_math_functions/tanpif.md) | 获取输入数据与π相乘的正切值。 |
| [atanf](../math_functions/float_math_functions/atanf.md) | 获取输入数据的反正切值。 |
| [atan2f](../math_functions/float_math_functions/atan2f.md) | 获取输入数据y/x的反正切值。 |
| [atanhf](../math_functions/float_math_functions/atanhf.md) | 获取输入数据的反双曲正切值。 |
| [expf](../math_functions/float_math_functions/expf.md) | 指定输入x，获取e的x次方。 |
| [exp2f](../math_functions/float_math_functions/exp2f.md) | 指定输入x，获取2的x次方。 |
| [exp10f](../math_functions/float_math_functions/exp10f.md) | 指定输入x，获取10的x次方。 |
| [expm1f](../math_functions/float_math_functions/expm1f.md) | 指定输入x，获取e的x次方减1。 |
| [logf](../math_functions/float_math_functions/logf.md) | 获取以e为底，输入数据的对数。 |
| [log2f](../math_functions/float_math_functions/log2f.md) | 获取以2为底，输入数据的对数。 |
| [log10f](../math_functions/float_math_functions/log10f.md) | 获取以10为底，输入数据的对数。 |
| [log1pf](../math_functions/float_math_functions/log1pf.md) | 获取以e为底，输入数据加1的对数。 |
| [logbf](../math_functions/float_math_functions/logbf.md) | 计算以2为底，输入数据的对数，并对结果向下取整，返回浮点数。 |
| [ilogbf](../math_functions/float_math_functions/ilogbf.md) | 计算以2为底，输入数据的对数，并对结果向下取整，返回整数。 |
| [cosf](../math_functions/float_math_functions/cosf.md) | 获取输入数据的三角函数余弦值。 |
| [coshf](../math_functions/float_math_functions/coshf.md) | 获取输入数据的双曲余弦值。 |
| [cospif](../math_functions/float_math_functions/cospif.md) | 获取输入数据与π相乘的余弦值。 |
| [acosf](../math_functions/float_math_functions/acosf.md) | 获取输入数据的反余弦值。 |
| [acoshf](../math_functions/float_math_functions/acoshf.md) | 获取输入数据的双曲反余弦值。 |
| [sinf](../math_functions/float_math_functions/sinf.md) | 获取输入数据的三角函数正弦值。 |
| [sinhf](../math_functions/float_math_functions/sinhf.md) | 获取输入数据的双曲正弦值。 |
| [sinpif](../math_functions/float_math_functions/sinpif.md) | 获取输入数据与π相乘的正弦值。 |
| [asinf](../math_functions/float_math_functions/asinf.md) | 获取输入数据的反正弦值。 |
| [asinhf](../math_functions/float_math_functions/asinhf.md) | 获取输入数据的双曲反正弦值。 |
| [sincosf](../math_functions/float_math_functions/sincosf.md) | 获取输入数据的三角函数正弦值和余弦值。 |
| [sincospif](../math_functions/float_math_functions/sincospif.md) | 获取输入数据与π相乘的三角函数正弦值和余弦值。 |
| [frexpf](../math_functions/float_math_functions/frexpf.md) | 将x转换为归一化[1/2, 1)的有符号数乘以2的整数次幂。 |
| [ldexpf](../math_functions/float_math_functions/ldexpf.md) | 获取输入x乘以2的exp次幂的结果。 |
| [sqrtf](../math_functions/float_math_functions/sqrtf.md) | 获取输入数据x的平方根。 |
| [rsqrtf](../math_functions/float_math_functions/rsqrtf.md) | 获取输入数据x的平方根的倒数。 |
| [hypotf](../math_functions/float_math_functions/hypotf.md) | 获取输入数据x、y的平方和x^2 + y^2的平方根。 |
| [rhypotf](../math_functions/float_math_functions/rhypotf.md) | 获取输入数据x、y的平方和x^2 + y^2的平方根的倒数。 |
| [powf](../math_functions/float_math_functions/powf.md) | 获取输入数据x的y次幂。 |
| [norm3df](../math_functions/float_math_functions/norm3df.md) | 获取输入数据a、b、c的平方和a^2 + b^2 + c^2的平方根。 |
| [rnorm3df](../math_functions/float_math_functions/rnorm3df.md) | 获取输入数据a、b、c的平方和a^2 + b^2 + c^2的平方根的倒数。 |
| [norm4df](../math_functions/float_math_functions/norm4df.md) | 获取输入数据a、b、c、d的平方和a^2 + b^2+ c^2+ d^2的平方根。 |
| [rnorm4df](../math_functions/float_math_functions/rnorm4df.md) | 获取输入数据a、b、c、d的平方和a^2 + b^2 + c^2 + d^2的平方根的倒数。 |
| [normf](../math_functions/float_math_functions/normf.md) | 获取输入数据a中前n个元素的平方和a[0]^2 + a[1]^2 +...+ a[n-1]^2的平方根。 |
| [rnormf](../math_functions/float_math_functions/rnormf.md) | 获取输入数据a中前n个元素的平方和a[0]^2 + a[1]^2 + ...+ a[n-1]^2的平方根的倒数。 |
| [cbrtf](../math_functions/float_math_functions/cbrtf.md) | 获取输入数据x的立方根。 |
| [rcbrtf](../math_functions/float_math_functions/rcbrtf.md) | 获取输入数据x的立方根的倒数。 |
| [erff](../math_functions/float_math_functions/erff.md) | 获取输入数据的误差函数值。 |
| [erfcf](../math_functions/float_math_functions/erfcf.md) | 获取输入数据的互补误差函数值。 |
| [erfinvf](../math_functions/float_math_functions/erfinvf.md) | 获取输入数据的逆误差函数值。 |
| [erfcinvf](../math_functions/float_math_functions/erfcinvf.md) | 获取输入数据的逆互补误差函数值。 |
| [erfcxf](../math_functions/float_math_functions/erfcxf.md) | 获取输入数据的缩放互补误差函数值。 |
| [tgammaf](../math_functions/float_math_functions/tgammaf.md) | 获取输入数据x的伽马函数值。 |
| [lgammaf](../math_functions/float_math_functions/lgammaf.md) | 获取输入数据x伽马值的绝对值并求自然对数。 |
| [cyl_bessel_i0f](../math_functions/float_math_functions/cyl_bessel_i0f.md) | 获取输入数据x的0阶常规修正圆柱贝塞尔函数的值。 |
| [cyl_bessel_i1f](../math_functions/float_math_functions/cyl_bessel_i1f.md) | 获取输入数据x的1阶常规修正圆柱贝塞尔函数的值。 |
| [normcdff](../math_functions/float_math_functions/normcdff.md) | 获取输入数据x的标准正态分布的累积分布函数值。 |
| [normcdfinvf](../math_functions/float_math_functions/normcdfinvf.md) | 获取输入数据x的标准正态累积分布的逆函数 |
| [j0f](../math_functions/float_math_functions/j0f.md) | 获取输入数据x的0阶第一类贝塞尔函数j0的值。 |
| [j1f](../math_functions/float_math_functions/j1f.md) | 获取输入数据x的1阶第一类贝塞尔函数j1的值。 |
| [jnf](../math_functions/float_math_functions/jnf.md) | 获取输入数据x的n阶第一类贝塞尔函数jn的值。 |
| [y0f](../math_functions/float_math_functions/y0f.md) | 获取输入数据x的0阶第二类贝塞尔函数y0的值。 |
| [y1f](../math_functions/float_math_functions/y1f.md) | 获取输入数据x的1阶第二类贝塞尔函数y1的值。 |
| [ynf](../math_functions/float_math_functions/ynf.md) | 获取输入数据x的n阶第二类贝塞尔函数yn的值。 |
| [fabsf](../math_functions/float_math_functions/fabsf.md) | 获取输入数据的绝对值。 |
| [abs](../math_functions/float_math_functions/abs.md) | 获取float类型输入数据的绝对值。 |
| [fmaf](../math_functions/float_math_functions/fmaf.md) | 对输入数据x、y、z，计算x与y相乘加上z的结果。 |
| [fmaxf](../math_functions/float_math_functions/fmaxf.md) | 获取两个输入数据中的最大值。 |
| [max](../math_functions/float_math_functions/max.md) | 获取两个输入数据中的最大值。 |
| [fminf](../math_functions/float_math_functions/fminf.md) | 获取两个输入数据中的最小值。 |
| [min](../math_functions/float_math_functions/min.md) | 获取两个输入数据中的最小值。 |
| [nanf](../math_functions/float_math_functions/nanf.md) | 根据字符串生成float类型的安静NaN值（浮点尾数最高位为1）。 |
| [fdimf](../math_functions/float_math_functions/fdimf.md) | 获取输入数据的差值，差值小于0时，返回0。 |
| [remquof](../math_functions/float_math_functions/remquof.md) | 获取输入数据x除以y的余数。求余数时，商取最接近x除以y浮点数结果的整数，当x除以y的浮点数结果与左右最接近的整数距离相等时，商取偶数，同时将商赋值给指针变量quo。 |
| [fmodf](../math_functions/float_math_functions/fmodf.md) | 获取输入数据x除以y的余数。求余数时，商取x除以y浮点数结果的整数部分。 |
| [remainderf](../math_functions/float_math_functions/remainderf.md) | 获取输入数据x除以y的余数。求余数时，商取最接近x除以y浮点数结果的整数，当x除以y的浮点数结果与左右最接近的整数距离相等时，商取偶数。 |
| [copysignf](../math_functions/float_math_functions/copysignf.md) | 获取由第一个输入x的数值部分和第二个输入y的符号部分拼接得到的浮点数。 |
| [nearbyintf](../math_functions/float_math_functions/nearbyintf.md) | 获取与输入浮点数最接近的整数，输入浮点数与左右整数的距离相等时，返回偶数。 |
| [nextafterf](../math_functions/float_math_functions/nextafterf.md) | 如果y大于x，返回比x大的下一个可表示的浮点值，即浮点数二进制最低位加1。<br><br>如果y小于x，返回比x小的下一个可表示的浮点值，即浮点数二进制最低位减1。<br><br>如果y等于x，返回x。 |
| [scalbnf](../math_functions/float_math_functions/scalbnf.md) | 获取输入数据x与2的n次方的乘积。 |
| [scalblnf](../math_functions/float_math_functions/scalblnf.md) | 获取输入数据x与2的n次方的乘积。 |
| [modff](../math_functions/float_math_functions/modff.md) | 将输入数据分解为小数部分和整数部分。 |
| [fdividef](../math_functions/float_math_functions/fdividef.md) | 获取两个输入数据相除的结果。 |
| [signbit](../math_functions/float_math_functions/signbit.md) | 获取输入数据的符号位。 |
| [__saturatef](../math_functions/float_math_functions/__saturatef.md) | 将输入数据钳位到[0.0, 1.0]区间。 |
| [__fdividef](../math_functions/float_math_functions/__fdividef.md) | 获取两个输入数据相除的结果。 |
| [rintf](../math_functions/float_math_functions/rintf.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [lrintf](../math_functions/float_math_functions/lrintf.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [llrintf](../math_functions/float_math_functions/llrintf.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [roundf](../math_functions/float_math_functions/roundf.md) | 获取对输入数据四舍五入后的整数。 |
| [lroundf](../math_functions/float_math_functions/lroundf.md) | 获取对输入数据四舍五入后的整数。 |
| [llroundf](../math_functions/float_math_functions/llroundf.md) | 获取对输入数据四舍五入后的整数。 |
| [floorf](../math_functions/float_math_functions/floorf.md) | 获取小于或等于输入数据的最大整数值。 |
| [ceilf](../math_functions/float_math_functions/ceilf.md) | 获取大于或等于输入数据的最小整数值。 |
| [truncf](../math_functions/float_math_functions/truncf.md) | 获取对输入数据的浮点数截断后的整数。 |
| [isfinite](../math_functions/float_math_functions/isfinite.md) | 判断浮点数是否为有限数（非inf、非nan）。 |
| [isnan](../math_functions/float_math_functions/isnan.md) | 判断浮点数是否为nan。 |
| [isinf](../math_functions/float_math_functions/isinf.md) | 判断浮点数是否为无穷。 |

**表23**  类型转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__float2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rn.md) | 获取输入遵循CAST_RINT模式取整后的浮点数。 |
| [__float2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rz.md) | 获取输入遵循CAST_TRUNC模式取整后的浮点数。 |
| [__float2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rd.md) | 获取输入遵循CAST_FLOOR模式取整后的浮点数。 |
| [__float2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2float_ru.md) | 获取输入遵循CAST_CEIL模式取整后的浮点数。 |
| [__float2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rna.md) | 获取输入遵循CAST_ROUND模式取整后的浮点数。 |
| [__float2uint_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rn.md) | 获取输入遵循CAST_RINT模式转换成的无符号整数。 |
| [__float2uint_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的无符号整数。 |
| [__float2uint_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的无符号整数。 |
| [__float2uint_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_ru.md) | 获取输入遵循CAST_CEIL模式转换成的无符号整数。 |
| [__float2uint_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rna.md) | 获取输入遵循CAST_ROUND模式转换成的无符号整数。 |
| [__float2int_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rn.md) | 获取输入遵循CAST_RINT模式转换成的有符号整数。 |
| [__float2int_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的有符号整数。 |
| [__float2int_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的有符号整数。 |
| [__float2int_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2int_ru.md) | 获取输入遵循CAST_CEIL模式转换成的有符号整数。 |
| [__float2int_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rna.md) | 获取输入遵循CAST_ROUND模式转换成的有符号整数。 |
| [__float2ull_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位无符号整数。 |
| [__float2ull_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位无符号整数。 |
| [__float2ull_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位无符号整数。 |
| [__float2ull_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位无符号整数。 |
| [__float2ull_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位无符号整数。 |
| [__float2ll_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位有符号整数。 |
| [__float2ll_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位有符号整数。 |
| [__float2ll_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位有符号整数。 |
| [__float2ll_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位有符号整数。 |
| [__float2ll_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位有符号整数。 |
| [__uint2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__uint2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__uint2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__uint2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__uint2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__int2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__int2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__int2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__int2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__int2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__int2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__ull2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__ull2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__ull2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__ull2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__ull2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__ll2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__ll2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__ll2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__ll2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__ll2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__int_as_float](../math_functions/data_type_conversion/type_conversion_functions/__int_as_float.md) | 将整数中的位重新解释为浮点数。 |
| [__uint_as_float](../math_functions/data_type_conversion/type_conversion_functions/__uint_as_float.md) | 将无符号整数中的位重新解释为浮点数。 |
| [__float_as_int](../math_functions/data_type_conversion/type_conversion_functions/__float_as_int.md) | 将浮点数中的位重新解释为有符号整数。 |
| [__float_as_uint](../math_functions/data_type_conversion/type_conversion_functions/__float_as_uint.md) | 将浮点数中的位重新解释为无符号整数。 |

**表24**  整型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [labs](../math_functions/integer_math_functions/labs.md) | 获取输入数据的绝对值。 |
| [llabs](../math_functions/integer_math_functions/llabs.md) | 获取输入数据的绝对值。 |
| [llmax](../math_functions/integer_math_functions/llmax.md) | 获取两个输入数据中的最大值。 |
| [ullmax](../math_functions/integer_math_functions/ullmax.md) | 获取两个输入数据中的最大值。 |
| [umax](../math_functions/integer_math_functions/umax.md) | 获取两个输入数据中的最大值。 |
| [llmin](../math_functions/integer_math_functions/llmin.md) | 获取两个输入数据中的最小值。 |
| [ullmin](../math_functions/integer_math_functions/ullmin.md) | 获取两个输入数据中的最小值。 |
| [umin](../math_functions/integer_math_functions/umin.md) | 获取两个输入数据中的最小值。 |
| [__mulhi](../math_functions/integer_math_functions/__mulhi.md) | 获取输入int32类型数据x和y乘积的高32位。 |
| [__umulhi](../math_functions/integer_math_functions/__umulhi.md) | 获取输入uint32类型数据x和y乘积的高32位。 |
| [__mul64hi](../math_functions/integer_math_functions/__mul64hi.md) | 获取输入int64类型数据x和y乘积的高64位。 |
| [__umul64hi](../math_functions/integer_math_functions/__umul64hi.md) | 获取输入uint64类型数据x和y乘积的高64位。 |
| [__mul_i32toi64](../math_functions/integer_math_functions/__mul_i32toi64.md) | 计算输入32位整数x和y的乘积，返回64位结果。 |
| [__brev](../math_functions/integer_math_functions/__brev.md) | 将输入数据的位序反转，返回反转后的值。 |
| [__clz](../math_functions/integer_math_functions/__clz.md) | 从输入数据的二进制最高有效位开始，返回连续的前导零的位数。 |
| [__ffs](../math_functions/integer_math_functions/__ffs.md) | 从二进制输入数据的最低位开始，查找第一个值为1的比特位的位置，并返回该位置的索引，索引从1开始计数；如果二进制数据中没有1，则返回0。 |
| [__popc](../math_functions/integer_math_functions/__popc.md) | 统计输入数据从二进制的高位到低位比特位为1的数量。 |
| [__byte_perm](../math_functions/integer_math_functions/__byte_perm.md) | 由输入的两个4字节的uint32_t类型数据组成一个8个字节的64比特位的整数，通过选择器s指定选取其中的4个字节，将这4个字节从低位到高位拼成一个uint32_t类型的整数。 |
| [__sad](../math_functions/integer_math_functions/__sad.md) | 对输入数据x、y、z，计算|x - y|+z的结果，即第一个入参和第二个入参之差的绝对值与第三个入参的和。 |
| [__usad](../math_functions/integer_math_functions/__usad.md) | 对输入数据x、y、z，计算|x - y|+z的结果，即第一个入参和第二个入参之差的绝对值与第三个入参的和。 |
| [__mul24](../math_functions/integer_math_functions/__mul24.md) | 获取输入int32类型数据x和y低24位乘积的低32位结果。x和y的高8位被忽略。 |
| [__umul24](../math_functions/integer_math_functions/__umul24.md) | 获取输入uint32类型数据x和y低24位乘积的低32位结果。x和y的高8位被忽略。 |
| [__hadd](../math_functions/integer_math_functions/__hadd.md) | 获取输入int32类型数据x和y的平均值，避免中间求和溢出。 |
| [__rhadd](../math_functions/integer_math_functions/__rhadd.md) | 获取输入int32类型数据x和y的向上取整平均值，避免中间求和溢出。 |
| [__uhadd](../math_functions/integer_math_functions/__uhadd.md) | 获取输入uint32类型数据x和y的平均值，避免中间求和溢出。 |
| [__urhadd](../math_functions/integer_math_functions/__urhadd.md) | 获取输入uint32类型数据x和y的向上取整平均值，避免中间求和溢出。 |
| [max](../math_functions/integer_math_functions/max.md) | 获取两个输入数据中的最大值。 |
| [min](../math_functions/integer_math_functions/min.md) | 获取两个输入数据中的最小值。 |

## 访存函数<a name="section1064618511375"></a>

**表25**  访存函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_ldcg](../memory_access_functions/asc_ldcg.md) | 从L2 Cache加载缓存的数据，如果缓存命中，则直接返回数据。若未命中，则从Global Memory地址预加载数据缓存至L2 Cache，并返回数据。 |
| [asc_ldca](../memory_access_functions/asc_ldca.md) | 首先从Data Cache加载缓存数据，若未命中，则尝试从L2 Cache加载。如果Data Cache和L2 Cache中均未找到所需数据，则从Global Memory中读取数据，然后将其缓存到L2 Cache和Data Cache中。 |
| [asc_stcg](../memory_access_functions/asc_stcg.md) | 将指定数据存储到Global Memory的地址address中，并缓存到L2 Cache，但不缓存至Data Cache。 |
| [asc_stwt](../memory_access_functions/asc_stwt.md) | 将指定数据存储到Global Memory的地址address中，并缓存至Data Cache和L2 Cache。 |
| [asc_dcci_single](../memory_access_functions/asc_dcci_single.md) | 刷新指定地址所在的Cache Line，保证数据读取时Cache的一致性。 |
| [asc_dcci_entire](../memory_access_functions/asc_dcci_entire.md) | 刷新核内的整个Data Cache，保证数据读取时Cache的一致性。 |
| [asc_nop](../memory_access_functions/asc_nop.md) | 生成一条空操作指令，占用当前线程15个时钟周期，不执行任何实际计算与访存操作。 |

## 地址空间谓词函数<a name="section97001946144014"></a>

**表26**  地址空间谓词函数

| 接口名 | 功能描述 |
| --- | --- |
| [__isGlobal](../address_space_predicate_functions/__isGlobal.md) | 判断输入的指针是否指向Global Memory内存空间的地址。 |
| [__isUbuf](../address_space_predicate_functions/__isUbuf.md) | 判断输入的指针是否指向UB内存空间的地址。 |
| [__isLocal](../address_space_predicate_functions/__isLocal.md) | 判断输入的指针是否指向栈空间的地址。 |

## 地址空间转换函数

**表27**  地址空间转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__cvta_generic_to_global](../address_space_conversion_functions/__cvta_generic_to_global.md) | 将输入的指针转换为其指向的Global Memory内存空间的地址值并返回。 |
| [__cvta_generic_to_ubuf](../address_space_conversion_functions/__cvta_generic_to_ubuf.md) | 将输入的指针转换为其指向的UB内存空间的地址值并返回。 |
| [__cvta_generic_to_local](../address_space_conversion_functions/__cvta_generic_to_local.md) | 将输入的指针转换为其指向的栈空间地址的值并返回。 |
| [__cvta_global_to_generic](../address_space_conversion_functions/__cvta_global_to_generic.md) | 将Global Memory内存空间的地址值转换为对应的指针。 |
| [__cvta_ubuf_to_generic](../address_space_conversion_functions/__cvta_ubuf_to_generic.md) | 将UB内存空间的地址值转换为对应的指针。 |
| [__cvta_local_to_generic](../address_space_conversion_functions/__cvta_local_to_generic.md) | 将栈空间的地址值转换为对应的指针。 |

## 协作组

**表28**  thread_block接口

| 接口名 | 功能描述 |
| --- | --- |
| [this_thread_block](../cooperative_groups/thread_block/thread_block_constructor.md) | 获取当前线程块协作组对象。 |
| [sync](../cooperative_groups/thread_block/sync.md) | 同步线程块内所有线程。 |
| [size](../cooperative_groups/thread_block/size.md) | 获取线程块内线程总数。 |
| [num_threads](../cooperative_groups/thread_block/num_threads.md) | 获取线程块内线程总数。 |
| [thread_rank](../cooperative_groups/thread_block/thread_rank.md) | 获取当前线程在线程块内的排名。 |
| [thread_index](../cooperative_groups/thread_block/thread_index.md) | 获取当前线程在线程块内的三维索引。 |
| [group_index](../cooperative_groups/thread_block/group_index.md) | 获取当前线程块在网格中的三维索引。 |
| [group_dim](../cooperative_groups/thread_block/group_dim.md) | 获取线程块的维度。 |
| [dim_threads](../cooperative_groups/thread_block/dim_threads.md) | 获取线程块内线程的三维维度。 |

**表29**  coalesced_group接口

| 接口名 | 功能描述 |
| --- | --- |
| [coalesced_threads](../cooperative_groups/coalesced_group/coalesced_group_constructor.md) | 获取当前活跃线程组成的协作组对象。 |
| [sync](../cooperative_groups/coalesced_group/sync.md) | 同步组内所有线程。 |
| [size](../cooperative_groups/coalesced_group/size.md) | 获取组内线程总数。 |
| [num_threads](../cooperative_groups/coalesced_group/num_threads.md) | 获取组内线程总数。 |
| [thread_rank](../cooperative_groups/coalesced_group/thread_rank.md) | 获取当前线程在组内的排名。 |
| [meta_group_rank](../cooperative_groups/coalesced_group/meta_group_rank.md) | 获取当前协作组在父组的排名。 |
| [meta_group_size](../cooperative_groups/coalesced_group/meta_group_size.md) | 获取父组被划分时创建的子组数量。 |
| [shfl](../cooperative_groups/coalesced_group/shfl.md) | 组内线程的数据交换，直接读取组内指定线程的数据。 |
| [shfl_up](../cooperative_groups/coalesced_group/shfl_up.md) | 获取组内当前线程向前偏移delta的线程的数据。 |
| [shfl_down](../cooperative_groups/coalesced_group/shfl_down.md) | 获取组内当前线程向后偏移delta的线程的数据。 |
| [ballot](../cooperative_groups/coalesced_group/ballot.md) | 判断组内每个活跃线程的输入是否非零。 |
| [any](../cooperative_groups/coalesced_group/any.md) | 判断是否有组内线程的输入不为0。 |
| [all](../cooperative_groups/coalesced_group/all.md) | 判断是否所有组内线程的输入均不为0。 |

**表30**  thread_block_tile接口

| 接口名 | 功能描述 |
| --- | --- |
| [tiled_partition](../cooperative_groups/thread_block_tile/thread_block_tile_constructor.md) | 创建指定大小的thread_block_tile协作组。 |
| [sync](../cooperative_groups/thread_block_tile/sync.md) | 同步组内所有线程。 |
| [size](../cooperative_groups/thread_block_tile/size.md) | 获取组内线程总数。 |
| [num_threads](../cooperative_groups/thread_block_tile/num_threads.md) | 获取组内线程总数。 |
| [thread_rank](../cooperative_groups/thread_block_tile/thread_rank.md) | 获取当前线程在组内的排名。 |
| [meta_group_rank](../cooperative_groups/thread_block_tile/meta_group_rank.md) | 获取当前协作组在父组的排名。 |
| [meta_group_size](../cooperative_groups/thread_block_tile/meta_group_size.md) | 获取父组被划分时创建的子组数量。 |
| [shfl](../cooperative_groups/thread_block_tile/shfl.md) | 组内线程的数据交换，直接读取组内指定线程的数据。 |
| [shfl_up](../cooperative_groups/thread_block_tile/shfl_up.md) | 获取组内当前线程向前偏移delta的线程的数据。 |
| [shfl_down](../cooperative_groups/thread_block_tile/shfl_down.md) | 获取组内当前线程向后偏移delta的线程的数据。 |
| [shfl_xor](../cooperative_groups/thread_block_tile/shfl_xor.md) | 获取组内与当前线程rank做异或运算后的线程的数据。 |
| [ballot](../cooperative_groups/thread_block_tile/ballot.md) | 判断组内每个活跃线程的输入是否非零。 |
| [any](../cooperative_groups/thread_block_tile/any.md) | 判断是否有组内线程的输入不为0。 |
| [all](../cooperative_groups/thread_block_tile/all.md) | 判断是否所有组内线程的输入均不为0。 |

**表31**  grid_group接口

| 接口名 | 功能描述 |
| --- | --- |
| [this_grid](../cooperative_groups/grid_group/grid_group_constructor.md) | 获取当前Grid协作组对象。 |
| [is_valid](../cooperative_groups/grid_group/is_valid.md) | 判断当前Grid协作组是否可使用sync()接口进行跨线程块同步。 |
| [sync](../cooperative_groups/grid_group/sync.md) | 同步Grid内所有线程。 |
| [thread_rank](../cooperative_groups/grid_group/thread_rank.md) | 获取当前线程在Grid内的排名。 |
| [block_rank](../cooperative_groups/grid_group/block_rank.md) | 获取当前线程所属线程块在Grid内的排名。 |
| [num_threads](../cooperative_groups/grid_group/num_threads.md) | 获取Grid内线程总数。 |
| [num_blocks](../cooperative_groups/grid_group/num_blocks.md) | 获取Grid内线程块总数。 |
| [dim_blocks](../cooperative_groups/grid_group/dim_blocks.md) | 获取Grid的线程块维度配置。 |
| [block_index](../cooperative_groups/grid_group/block_index.md) | 获取当前线程所属线程块在Grid中的三维索引。 |
| [size](../cooperative_groups/grid_group/size.md) | 获取Grid内线程总数。 |
| [group_dim](../cooperative_groups/grid_group/group_dim.md) | 获取Grid的线程块维度配置。 |

**表32**  协作组通用与划分接口

| 接口名 | 功能描述 |
| --- | --- |
| [sync](../cooperative_groups/sync.md) | 同步指定协作组内的所有线程。 |
| [tiled_partition](../cooperative_groups/tiled_partition.md) | 将一个线程组按指定大小划分为多个子组。 |
| [binary_partition](../cooperative_groups/binary_partition.md) | 根据一个标签（0或1）将父组划分为两个子组。 |
