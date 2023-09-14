import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
  plugins: [react()],
  server: {
    port: 3001,
    host: 'localhost',
  },
  build: {
    name: 'frontend',
    fileName: 'frontend',
    emptyOutDir: true,
    sourcemap: true,
    outDir: '../dist/frontend',
  },
});
