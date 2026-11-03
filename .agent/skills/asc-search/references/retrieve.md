# Agent 查询组合

自然语言提取、澄清和人类答案由调用方处理。asc-search 只消费已整理条件。

## 准备与只读查询

由前置准备流程或维护者构建对应的数据 root 和独立 index-dir。每个实例固定来源提交、版本和架构。
`search.py` 检查资源指纹；缺失、stale 或错误时返回结构化错误，不能隐式联网或重建。

```bash
python3 <skill>/scripts/search.py --root <data> --index-dir <instance> \
  --platform 950 --version 9.2.0 --operation copy --keywords 非对齐
```

目标产品和版本优先使用调用方明确条件；缺失时懒调用 detect_context.py。不能将本机覆盖远端目标，
不能以共享架构推断产品，也不能从 SDK 路径名或资源快照默认目标版本。

## 维度和输出

`--keywords` 的各词 AND，与结构化维度求交；维度间 AND，逗号内值 OR。已知 API 用 `--api`，
配套关系用 `--deps-of`，参数与枚举用 `--param/--enum`。`--version` 查目标版本可用性，
`--changed-in` 查变更事实。`--gap` 是独立的范围外登记查询，不与关键词或版本/操作过滤组合。

默认最多返回 20 条，可指定 `--limit`；`matched_count/truncated` 明确是否截断。
候选保留来源提交、数据根、产品和架构、文档路径、声明及样例定位。命中不是语义正确性或全量覆盖证明，
消费 Agent 按需核对所需章节、具体重载和条件，不全量精读或用名称相似替代声明判断。

空结果时只放松有依据的可选关键词；不擅自去掉已明确的产品、版本或 dtype 条件。
解析错误与零命中分开处理，错误交回调用方处理回退。
