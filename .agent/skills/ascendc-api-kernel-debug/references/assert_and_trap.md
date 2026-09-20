# assert、ascendc_assert 和 __trap

## 断言

```cpp
#include "utils/debug/asc_assert.h"

ascendc_assert(baseM % 16 == 0);
ascendc_assert(idx < limit, "idx=%u limit=%u\\n", idx, limit);
```

`assert(expr)` 和 `ascendc_assert(expr)` 提供相同的 AscendC 断言能力：条件为真时继续执行；条件为假时输出断言上下文并触发异常，算子执行失败。

### 重要限制

- Release 工程通常定义 `NDEBUG`，此时断言不生效。
- `ASCENDC_DUMP=0` 也会使该调试接口不生效。
- 自定义格式字符串和参数的能力依赖编程模式；CPU 调试、SIMT VF 和 SIMD VF 可能只输出标准断言上下文。
- 需要包含 `<cassert>` 时，先包含 `<cassert>`，再包含 `kernel_operator.h` 或 `utils/debug/asc_assert.h`，避免标准库覆盖 AscendC 的宏定义。
- 断言会影响性能，只在调试阶段或明确需要的防护中使用。

## `__trap`

```cpp
#include "utils/debug/asc_assert.h"

if (isnan(x[idx])) {
    __trap();
}
```

`__trap` 无参数、无返回值，在 SIMT 代码中用于立即中断 Kernel。它适合与 NaN、非法索引或不可恢复状态的条件判断配合；需要条件表达式、源码位置或自定义消息时使用断言更合适。

当前公开文档将 `__trap` 的支持限定在特定 SIMT/SIMT VF 场景，不能向 A2、A3 或通用 SIMD 无条件承诺可用。目标平台未明确时先查当前 API 文档。

## 判断“没有失败”的方法

断言未触发不一定表示条件正确，至少要区分：

1. 条件确实为真。
2. 当前代码路径没有执行。
3. `NDEBUG` 或 `ASCENDC_DUMP=0` 禁用了断言。
4. 目标编程模式不支持该断言形式。

使用一个确定失败的最小输入验证断言链路，再恢复真实条件。
