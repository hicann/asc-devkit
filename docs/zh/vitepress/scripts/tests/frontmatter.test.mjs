/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import assert from 'node:assert/strict'
import test from 'node:test'

import { stripFrontMatter } from '../../docs/.vitepress/theme/frontmatter.mjs'

const OKF_HEADER = `---
okftype: reference.api
description: "更新地址寄存器addr_reg以支持1至4维迭代寻址。"
tag:
  - roles: [data_movement_configuration]
version: ">=9.2.0"
---
`

test('strips a leading OKF front matter block', () => {
  assert.equal(stripFrontMatter(OKF_HEADER + '# 基础定义\n'), '# 基础定义\n')
})

test('strips CRLF front matter', () => {
  const source = OKF_HEADER.replace(/\n/g, '\r\n') + '# 标题\r\n'
  assert.equal(stripFrontMatter(source), '# 标题\r\n')
})

test('supports BOM and dots terminator', () => {
  assert.equal(stripFrontMatter('\uFEFF---\nokftype: reference.api\n...\n# 标题\n'), '# 标题\n')
})

test('keeps unclosed front matter', () => {
  const source = '---\nokftype: reference.api\n# 标题\n'
  assert.equal(stripFrontMatter(source), source)
})

test('does not treat a leading horizontal rule as front matter', () => {
  const source = '---\n\n正文内容\n\n---\n'
  assert.equal(stripFrontMatter(source), source)
})

test('keeps documents without front matter unchanged', () => {
  const source = '# 标题\n\n正文内容\n'
  assert.equal(stripFrontMatter(source), source)
})
