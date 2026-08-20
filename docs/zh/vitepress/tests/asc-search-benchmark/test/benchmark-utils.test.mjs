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
import { mkdir, mkdtemp, rm, symlink, writeFile } from 'node:fs/promises'
import { tmpdir } from 'node:os'
import { join } from 'node:path'
import { test } from 'node:test'

import {
  assignUniqueTargetGrades,
  escapeMarkdownCell,
  fetchSameOrigin,
  markdownReportPath,
  pathMatchesPrefix,
  RESULT_HIGHLIGHT_SELECTOR,
  resolveFileWithinRoot,
  resolveSameOriginHttpUrl,
  SECTION_HIGHLIGHT_SELECTOR,
} from '../lib/benchmark-utils.mjs'

test('accepts only same-origin HTTP(S) result URLs', () => {
  assert.equal(
    resolveSameOriginHttpUrl('/guide/page', 'https://docs.example/').href,
    'https://docs.example/guide/page',
  )
  assert.throws(
    () => resolveSameOriginHttpUrl('http://127.0.0.1/private', 'https://docs.example/'),
    /must remain/,
  )
  assert.throws(
    () => resolveSameOriginHttpUrl('file:///etc/passwd', 'https://docs.example/'),
    /HTTP\(S\)/,
  )
})

test('validates every redirect before following it', async () => {
  const visited = []
  const sameOriginFetch = async (url, init) => {
    visited.push([url.href, init.redirect])
    if (url.pathname === '/start') {
      return new Response(null, { status: 302, headers: { location: '/final' } })
    }
    return new Response(null, { status: 200 })
  }
  const response = await fetchSameOrigin(
    'https://docs.example/start',
    'https://docs.example/',
    { method: 'HEAD' },
    sameOriginFetch,
  )
  assert.equal(response.status, 200)
  assert.deepEqual(visited, [
    ['https://docs.example/start', 'manual'],
    ['https://docs.example/final', 'manual'],
  ])

  let requestCount = 0
  const crossOriginFetch = async () => {
    requestCount++
    return new Response(null, {
      status: 302,
      headers: { location: 'http://169.254.169.254/latest/meta-data/' },
    })
  }
  await assert.rejects(
    fetchSameOrigin('https://docs.example/start', 'https://docs.example/', {}, crossOriginFetch),
    /must remain/,
  )
  assert.equal(requestCount, 1)
})

test('derives a distinct Markdown report path', () => {
  assert.equal(markdownReportPath('/tmp/report.json'), '/tmp/report.md')
  assert.equal(markdownReportPath('/tmp/report.JSON'), '/tmp/report.md')
  assert.equal(markdownReportPath('/tmp/report'), '/tmp/report.md')
  assert.equal(markdownReportPath('/tmp/report.data'), '/tmp/report.data.md')
})

test('matches route prefixes only at path segment boundaries', () => {
  assert.equal(pathMatchesPrefix('/api/asc_copy_gm2ub', '/api/asc_copy_gm2ub'), true)
  assert.equal(pathMatchesPrefix('/api/asc_copy_gm2ub/page', '/api/asc_copy_gm2ub'), true)
  assert.equal(pathMatchesPrefix('/api/asc_copy_gm2ub_align/page', '/api/asc_copy_gm2ub'), false)
  assert.equal(pathMatchesPrefix('/api/page', '/'), true)
})

test('keeps result and section highlight selectors independent', () => {
  assert.equal(RESULT_HIGHLIGHT_SELECTOR, '.des mark, .headings mark')
  assert.equal(SECTION_HIGHLIGHT_SELECTOR, '.section-title mark')
  assert.equal(RESULT_HIGHLIGHT_SELECTOR.includes('.section-title'), false)
})

test('assigns each result to at most one overlapping recall target', () => {
  const exact = { grade: 3, routes: ['/guide/page'] }
  const prefix = { grade: 2, routes: ['/guide/page', '/guide/sibling'] }
  const matches = (result, target) => target.routes.includes(result.route)

  assert.deepEqual(assignUniqueTargetGrades(
    [{ route: '/guide/page' }],
    [exact, prefix],
    10,
    matches,
  ), [3])
  assert.deepEqual(assignUniqueTargetGrades(
    [{ route: '/guide/page' }, { route: '/guide/sibling' }],
    [exact, prefix],
    10,
    matches,
  ), [3, 2])
})

test('does not resolve files through symlinks outside the static root', async () => {
  const testDirectory = await mkdtemp(join(tmpdir(), 'asc-search-static-root-'))
  const root = join(testDirectory, 'dist')
  const outside = join(testDirectory, 'outside.txt')
  const inside = join(root, 'inside.txt')
  const escaped = join(root, 'escaped.txt')
  try {
    await mkdir(root)
    await writeFile(outside, 'outside', 'utf8')
    await writeFile(inside, 'inside', 'utf8')
    await symlink(outside, escaped)

    assert.equal(resolveFileWithinRoot(root, [inside]), inside)
    assert.equal(resolveFileWithinRoot(root, [escaped]), null)
  }
  finally {
    await rm(testDirectory, { recursive: true, force: true })
  }
})

test('escapes HTML and table delimiters in Markdown cells', () => {
  assert.equal(
    escapeMarkdownCell('<img src=x onerror=alert(1)> & value|next\r\nrow'),
    '&lt;img src=x onerror=alert(1)&gt; &amp; value\\|next  row',
  )
  assert.deepEqual([...escapeMarkdownCell('\\|')], ['\\', '\\', '\\', '|'])
})
