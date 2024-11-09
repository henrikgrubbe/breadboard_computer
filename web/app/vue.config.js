const {defineConfig} = require('@vue/cli-service')
module.exports = defineConfig({
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
