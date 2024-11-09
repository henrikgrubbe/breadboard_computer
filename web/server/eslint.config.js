// @ts-check
const eslint = require('@eslint/js');
const tseslint= require('typescript-eslint');
const eslintPluginPrettierRecommended = require('eslint-plugin-prettier/recommended');

module.exports = tseslint.config(
  {
    files: ['**/*.ts'],
    extends: [
      eslint.configs.recommended,
      ...tseslint.configs.recommended,
      ...tseslint.configs.stylistic,
      eslintPluginPrettierRecommended
    ],
    rules: {
      '@typescript-eslint/no-explicit-any': ['warn'],
      '@typescript-eslint/no-inferrable-types': ['off'],
      '@typescript-eslint/no-unused-vars': ['error', {
        'argsIgnorePattern': '^_',
        'varsIgnorePattern': '^_',
        'caughtErrorsIgnorePattern': '^_'
      }],
      'prettier/prettier': ['error', {'endOfLine': 'auto'}],
      'sort-imports': ['error', {'ignoreDeclarationSort': true}]
    }
  },
);
