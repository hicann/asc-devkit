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

import {
  BAIDU_ANALYTICS_SITE_ID,
  baiduAnalyticsHead,
  createBaiduAnalyticsHead,
  DEFAULT_BAIDU_ANALYTICS_SITE_ID,
  getBaiduPageviewPath,
  installBaiduAnalytics,
  resolveBaiduAnalyticsSiteId,
} from '../../docs/.vitepress/baidu-analytics.mjs'

test('adds the Baidu analytics loader to every generated page', () => {
  const script = baiduAnalyticsHead[0][2]

  assert.match(script, /https:\/\/hm\.baidu\.com\/hm\.js\?/)
  assert.match(script, new RegExp(BAIDU_ANALYTICS_SITE_ID))
})

test('uses the production site ID by default', () => {
  assert.equal(BAIDU_ANALYTICS_SITE_ID, DEFAULT_BAIDU_ANALYTICS_SITE_ID)
  assert.equal(resolveBaiduAnalyticsSiteId(), DEFAULT_BAIDU_ANALYTICS_SITE_ID)
})

test('allows the site ID to be overridden during a test build', () => {
  const testSiteId = 'a'.repeat(32)
  const resolvedSiteId = resolveBaiduAnalyticsSiteId({
    BAIDU_ANALYTICS_SITE_ID: `  ${testSiteId}  `,
  })
  const script = createBaiduAnalyticsHead(resolvedSiteId)[0][2]

  assert.equal(resolvedSiteId, testSiteId)
  assert.match(script, new RegExp(`hm\\.js\\?${testSiteId}`))
  assert.doesNotMatch(script, new RegExp(DEFAULT_BAIDU_ANALYTICS_SITE_ID))
})

test('rejects an invalid site ID override', () => {
  assert.throws(
    () => resolveBaiduAnalyticsSiteId({ BAIDU_ANALYTICS_SITE_ID: 'invalid' }),
    /32-character hexadecimal value/
  )
})

test('does not install browser tracking during server-side rendering', () => {
  assert.equal(installBaiduAnalytics({}, undefined), false)
})

test('keeps the project and PR prefixes in virtual pageviews', async () => {
  const browser = {
    location: {
      pathname: '/ascendc.github.io/pr/123/',
      search: '',
    },
  }
  const router = {}

  assert.equal(installBaiduAnalytics(router, browser), true)
  await router.onAfterRouteChange('/ascendc.github.io/pr/123/')
  assert.equal(browser._hmt, undefined)

  browser.location.pathname = '/ascendc.github.io/pr/123/guide/getting-started.html'
  browser.location.search = '?from=preview'
  await router.onAfterRouteChange('/ascendc.github.io/pr/123/guide/getting-started.html')

  assert.deepEqual(browser._hmt, [[
    '_trackPageview',
    '/ascendc.github.io/pr/123/guide/getting-started.html?from=preview',
  ]])
})

test('preserves an existing VitePress route callback', async () => {
  let previousCalls = 0
  const browser = {
    location: {
      pathname: '/ascendc.github.io/',
      search: '',
    },
    _hmt: [],
  }
  const router = {
    onAfterRouteChange() {
      previousCalls += 1
    },
  }

  installBaiduAnalytics(router, browser)
  browser.location.pathname = '/ascendc.github.io/api/README.html'
  await router.onAfterRouteChange('/ascendc.github.io/api/README.html')

  assert.equal(previousCalls, 1)
  assert.deepEqual(browser._hmt, [[
    '_trackPageview',
    '/ascendc.github.io/api/README.html',
  ]])
})

test('normalizes an empty browser location to a root pageview', () => {
  assert.equal(getBaiduPageviewPath({}), '/')
})
