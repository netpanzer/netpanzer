import { defineConfig } from '@playwright/test';
export default defineConfig({
  testDir: '.',
  testMatch: 'browser.spec.mjs',
  timeout: 120000,
  workers: 1,
  use: { baseURL: 'http://127.0.0.1:8080', viewport: { width: 1120, height: 1000 }, screenshot: 'only-on-failure' },
  projects: [
    { name: 'chrome', use: { browserName: 'chromium', channel: 'chrome' } },
    { name: 'firefox', use: { browserName: 'firefox' } },
  ],
});
