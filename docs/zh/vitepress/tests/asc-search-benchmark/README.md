# Ascend C 搜索质量测试

这是一个独立于文档代码仓的黑盒测试工具。它使用真实 Chromium 操作文档站搜索框，因此既能验证本地静态产物和指定 Git commit，也能直接评测已经部署的站点。

测试仓不会向被测文档仓注入脚本，不依赖文档仓内的测试路由，也不会修改被测仓工作树。Node.js 22 或更高版本和 Chromium/Chrome 是仅有的运行依赖。

工具默认启用 Chromium 沙箱。仅当基准已经运行在可信容器或其他外层隔离环境中、且 Chromium 因 root 等环境限制无法启动时，才可显式传入 `--no-sandbox`；该选项会输出安全警告。不要在缺少外层隔离的主机上使用 `--no-sandbox` 测试远程站点。

## 目标模式

直接测试线上站点：

```bash
node bin/asc-search-benchmark.mjs \
  --url https://asc.gitcode.com/ \
  --output results/asc-gitcode.json \
  --enforce
```

测试已经构建好的静态目录：

```bash
node bin/asc-search-benchmark.mjs \
  --dist /path/to/asc-devkit/docs/zh/vitepress/docs/.vitepress/dist \
  --output results/local-dist.json
```

测试文档仓的指定 commit：

```bash
node bin/asc-search-benchmark.mjs \
  --repo /path/to/asc-devkit \
  --commit a8343c58dc1b9c445a7edc4aba1c622c30d1f546 \
  --output results/a8343c58d.json \
  --enforce
```

`--repo` 也接受 Git URL。工具在临时目录中 clone 并 detached checkout 指定 commit，默认从仓根执行 `bash docs/zh/vitepress/deploy.sh`，然后测试 `docs/zh/vitepress/docs/.vitepress/dist`。结束后临时 checkout 自动删除；排查构建问题时可加 `--keep-checkout`。

对于目录布局或构建方式不同的版本，可覆盖构建参数：

```bash
node bin/asc-search-benchmark.mjs \
  --repo https://gitcode.com/cann/asc-devkit.git \
  --commit <commit> \
  --build-command 'npm ci && npm run docs:build' \
  --dist-relative path/to/dist
```

只运行部分用例适合快速诊断：

```bash
node bin/asc-search-benchmark.mjs \
  --url https://asc.gitcode.com/ \
  --case api-case-sensitive-isnan-lower \
  --case technical-combination-ub-bank-conflict
```

## 查询集与指标

[`cases/ascendc.json`](cases/ascendc.json) 包含 170 个版本化用例，覆盖 API 精确名称、NPU 产品筛选、目录与具体定义页排序、标题和摘要高亮、大小写同名 API、下划线标识符的空格/连字符写法、不完整标识符前缀、数据类型后缀、中英文组合、中文标题和正文、Unicode 归一化、拼写错误、特殊符号及无结果查询。每个期望 URL 按 3、2、1 三级相关性标注。

用例可通过 `expect.descriptionIncludes` 声明相关结果摘要必须包含的文本，用于看护标识符定义等不能只靠页面排名验证的搜索质量。
`expect.highlightIncludes` 用于声明相关结果必须以可见的非透明背景高亮指定文本；高亮节点至少 95% 的面积必须位于摘要或标题的可视区域内，避免 `<mark>` 被单行裁剪。用例可通过 `viewportWidth` 指定 320～3840 像素的视口宽度，以验证响应式布局。
`expect.sectionTitleIncludes` 用于声明相关结果展示的小节标题，并验证它紧跟大标题或换行后左对齐；`expect.sectionHighlightIncludes` 进一步验证查询词在小标题中具有可见高亮。小标题与结果 URL 的锚点由同一个搜索结果提供，防止展示标题和实际跳转位置错配。
`expect.sectionHash` 用于声明结果应定位到的既有标题锚点；基准会真实点击相关结果，并验证目标标题存在且已经滚动进入可视区域。
使用 `--enforce` 时，只有聚合指标达到阈值且所有用例的结果数量、目标页面、摘要和链接约束全部通过，命令才会成功。

报告同时输出 JSON 和 Markdown，包含：

- 查询无异常完成率；
- Success@1、Success@3、Success@10；
- MRR@10、nDCG@10、目标召回率@10；
- 无结果准确率和 Top 10 无效链接率；
- 包含页面防抖、WASM、索引加载和渲染的 P50/P95 延迟；
- 浏览器、Pagefind 版本、被测 URL 和 commit 等可复现元数据。

线上模式通过 HTTP `HEAD` 检查结果链接，本地模式直接检查静态文件。网络策略不允许 `HEAD` 时可用 `--no-link-check` 跳过该指标；此时无效链接阈值不会参与 `--enforce`。

## 数据维护

查询集应独立于某一种搜索引擎。调整搜索实现时只生成新报告，不修改目标规则；只有文档 URL 或用户意图发生变化时才修改用例，并在 code review 中人工确认相关性等级。

常规自检：

```bash
npm test
node bin/asc-search-benchmark.mjs --help
```
