const {defineConfig} = require('@vue/cli-service')
module.exports = defineConfig({
    chainWebpack: (config) => {
        config.plugin('define').tap((definitions) => {
            Object.assign(definitions[0], {
                __VUE_OPTIONS_API__: 'true',
                __VUE_PROD_DEVTOOLS__: 'false',
                __VUE_PROD_HYDRATION_MISMATCH_DETAILS__: 'false'
            })
            return definitions
        })
    },
    transpileDependencies: true,
    css: {
        loaderOptions: {
            scss: {
                additionalData: `@import "@/styles/_variables.scss";`
            }
        }
    },
    configureWebpack: {
        module: {
            rules: [
                {
                    resourceQuery: /raw/,
                    type: 'asset/source'
                }
            ]
        },
        resolve: {
            fallback: {
                'url': require.resolve('url/')
            }
        }
    }
})
