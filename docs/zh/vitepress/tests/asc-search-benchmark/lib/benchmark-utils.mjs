/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { realpathSync, statSync } from 'node:fs'
import { isAbsolute, relative, sep } from 'node:path'

const HTTP_PROTOCOLS = new Set(['http:', 'https:'])
const REDIRECT_STATUSES = new Set([301, 302, 303, 307, 308])
export const RESULT_HIGHLIGHT_SELECTOR = '.des mark, .headings mark'
export const SECTION_HIGHLIGHT_SELECTOR = '.section-title mark'

export function resolveSameOriginHttpUrl(value, targetUrl) {
  const target = targetUrl instanceof URL ? targetUrl : new URL(targetUrl)
  const candidate = value instanceof URL ? new URL(value.href) : new URL(value, target)
  if (!HTTP_PROTOCOLS.has(target.protocol) || !HTTP_PROTOCOLS.has(candidate.protocol)) {
    throw new Error('Benchmark link checks only support HTTP(S) URLs')
  }
  if (candidate.origin !== target.origin) {
    throw new Error(`Benchmark result URL must remain on ${target.origin}`)
  }
  return candidate
}

export async function fetchSameOrigin(url, targetUrl, init = {}, fetchImpl = fetch, maxRedirects = 5) {
  const target = resolveSameOriginHttpUrl(targetUrl, targetUrl)
  let current = resolveSameOriginHttpUrl(url, target)
  for (let redirectCount = 0; ; redirectCount++) {
    const response = await fetchImpl(current, { ...init, redirect: 'manual' })
    const location = response.headers.get('location')
    if (!REDIRECT_STATUSES.has(response.status) || !location) return response
    await response.body?.cancel()
    if (redirectCount >= maxRedirects) throw new Error(`Too many redirects while checking ${url}`)
    current = resolveSameOriginHttpUrl(new URL(location, current), target)
  }
}

export function markdownReportPath(jsonReportPath) {
  return /\.json$/i.test(jsonReportPath)
    ? jsonReportPath.replace(/\.json$/i, '.md')
    : `${jsonReportPath}.md`
}

export function pathMatchesPrefix(path, prefix) {
  return path === prefix || (prefix === '/' ? path.startsWith('/') : path.startsWith(`${prefix}/`))
}

export function assignUniqueTargetGrades(results, targets, topK, matches) {
  const unmatched = new Set(targets.map((_, index) => index))
  return results.slice(0, topK).map((result) => {
    let selected = -1
    let grade = 0
    for (const index of unmatched) {
      if (targets[index].grade > grade && matches(result, targets[index])) {
        selected = index
        grade = targets[index].grade
      }
    }
    if (selected >= 0) unmatched.delete(selected)
    return grade
  })
}

export function resolveFileWithinRoot(root, candidates) {
  const realRoot = realpathSync(root)
  for (const candidate of candidates) {
    try {
      const realCandidate = realpathSync(candidate)
      const relativePath = relative(realRoot, realCandidate)
      if (relativePath === '..' || relativePath.startsWith(`..${sep}`) || isAbsolute(relativePath)) continue
      if (statSync(realCandidate).isFile()) return realCandidate
    }
    catch {}
  }
  return null
}

export function escapeMarkdownCell(value) {
  return String(value ?? '-')
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('\\', '\\\\')
    .replaceAll('|', '\\|')
    .replaceAll('\r', ' ')
    .replaceAll('\n', ' ')
}
