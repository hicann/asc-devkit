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
import { test } from 'node:test'

import { loadGitTimestamps } from '../git-timestamps.mjs'

function fakeGit(outputs) {
  const calls = []
  return {
    calls,
    run(args) {
      calls.push(args)
      return outputs.get(args.join('\0')) || ''
    },
  }
}

test('loads unescaped Unicode paths from Git history', () => {
  const outputs = new Map([
    [['rev-parse', '--is-shallow-repository'].join('\0'), 'false'],
    [[
      '-c', 'core.quotePath=false', 'log', '--format=@@%H:%ct', '--name-only', '--', 'docs',
    ].join('\0'), '@@abc:10\n\ndocs/zh/guide/编程指南/概述.md'],
  ])
  const git = fakeGit(outputs)
  const timestamps = loadGitTimestamps(git.run.bind(git))

  assert.equal(timestamps.get('docs/zh/guide/编程指南/概述.md'), 10000)
  assert.ok(git.calls.some(args => args.includes('core.quotePath=false')))
})

test('does not report a shallow boundary as a document update', () => {
  const boundary = '1111111111111111111111111111111111111111'
  const latest = '2222222222222222222222222222222222222222'
  const outputs = new Map([
    [['rev-parse', '--is-shallow-repository'].join('\0'), 'true'],
    [['rev-list', '--max-parents=0', 'HEAD'].join('\0'), boundary],
    [[
      '-c', 'core.quotePath=false', 'log', '--format=@@%H:%ct', '--name-only', '--', 'docs',
    ].join('\0'), [
      `@@${latest}:20`,
      '',
      'docs/zh/guide/new.md',
      `@@${boundary}:10`,
      '',
      'docs/zh/guide/new.md',
      'docs/zh/guide/unchanged.md',
    ].join('\n')],
  ])
  const git = fakeGit(outputs)
  const timestamps = loadGitTimestamps(git.run.bind(git))

  assert.equal(timestamps.get('docs/zh/guide/new.md'), 20000)
  assert.equal(timestamps.has('docs/zh/guide/unchanged.md'), false)
})
