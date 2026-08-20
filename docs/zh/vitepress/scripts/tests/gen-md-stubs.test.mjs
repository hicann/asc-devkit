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
import { createRequire } from 'node:module'
import { test } from 'node:test'

const require = createRequire(import.meta.url)
const { extractHeaders } = require('../gen-md-stubs.js')

test('decodes HTML entities in cached outline headings and links', () => {
  assert.deepEqual(extractHeaders(`
    <h2>核函数配置<a name="legacy"></a></h2>
    <h3>&lt;&lt;&lt;&gt;&gt;&gt;调用</h3>
    <h3>A &amp; B</h3>
  `), [
    { level: 2, title: '核函数配置', slug: '核函数配置', link: '#核函数配置' },
    { level: 3, title: '<<<>>>调用', slug: '<<<>>>调用', link: '#<<<>>>调用' },
    { level: 3, title: 'A & B', slug: 'A-&-B', link: '#A-&-B' },
  ])
})
