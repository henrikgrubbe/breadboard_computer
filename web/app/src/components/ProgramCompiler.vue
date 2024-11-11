<template>
  <div class="row mb-5">
    <div class="col-12 col-lg-6 mb-3 overflow-hidden">
      <label class="form-label">Write your program</label>
      <prism-editor class="editor source"
                    v-model="source"
                    :highlight="sourceHighlighter"
                    :line-numbers="true">
      </prism-editor>
    </div>

    <div class="col-12 col-lg-4 mb-3 overflow-hidden">
      <div class="mb-3">
        <label for="program-txt" class="form-label">Compiled program</label>
        <div v-if="error">
          <prism-editor class="editor error"
                        v-model="error"
                        :highlight="(err) => err"
                        :readonly="true">
          </prism-editor>
        </div>
        <div v-else>
          <prism-editor class="editor compiled"
                        v-model="compiled"
                        :highlight="compiledHighlighter"
                        :readonly="true">
          </prism-editor>
        </div>
      </div>

      <div class="btn-group d-flex justify-content-end" role="group" aria-label="button group">
        <button @click="uploadProgram" :disabled="error !== ''" type="button" class="btn btn-lg btn-primary">
          Upload
        </button>
        <button @click="writeProgram" type="button" class="btn btn-lg btn-success">
          Write to RAM
        </button>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import {defineComponent} from 'vue';
import {PrismEditor} from 'vue-prism-editor';
import * as peggy from 'peggy';
import {debounceFunction} from '@/utils/debounceFunction';
import {publishToTopic} from '@/utils/mqtt-upload';
import 'vue-prism-editor/dist/prismeditor.min.css';
// eslint-disable-next-line @typescript-eslint/no-require-imports
const grammar = require('@/assets/grammar.peggy?raw');

export default defineComponent({
  name: 'ProgramCompiler',
  components: {
    PrismEditor
  },
  data() {
    return {
      parser: peggy.generate(grammar),
      source: '' as string,
      compiled: '' as string,
      error: ' ' as string,
      debouncedCompiler: debounceFunction((code: string) => this.compile(code), 250),
    };
  },
  mounted() {
    this.source = '      ldi 4\nx:j:  add data\n      out\n      jmp j\ndata: 0x02\n'
  },
  watch: {
    source: function (code: string) {
      this.debouncedCompiler.call(null, code);
    },
  },
  methods: {
    compile(code: string): void {
      let parsed: string;
      try {
        parsed = this.parser.parse(code, {compile: true});
      } catch (e: unknown) {
        this.error = (e as { message: string }).message as string;
        return;
      }
      this.error = '';
      this.compiled = parsed;
    },
    sourceHighlighter(code: string) {
      return this.parser.parse(code, {highlight: true});
    },
    compiledHighlighter(code: string) {
      return code
          .split('\n')
          .map((line: string) => line.replace(/(\d{4}):(\d{4})(\d{4})/, '<addr>$1</addr>:<instr>$2</instr><opcode>$3</opcode>'))
          .join('\n');
    },
    uploadProgram(): void {
      if (this.error) {
        return;
      }

      const body: Record<string, number> = {};
      this.compiled.split('\n')
          .forEach((line: string) => {
            const [address, instruction] = line.split(':');
            body[parseInt(address, 2).toString()] = parseInt(instruction, 2);
          });

      publishToTopic('program', body);
    },
    writeProgram(): void {
      publishToTopic('command', {
        command: 'PROGRAM',
      });
    },
  }
});
</script>

<style lang="scss">
.editor {
  background: #2B2B2B;
  color: #A9B7C6;
  padding: 5px;
}

.editor,
.prism-editor__line-number {
  font-family: Fira code, Fira Mono, Consolas, Menlo, Courier, monospace;
  font-size: 1rem;
  line-height: 1.5;
}

.prism-editor__textarea:focus {
  outline: none;
}


.prism-editor-wrapper.source {
  token-mnemonic {
    color: #CC7832;
  }

  token-immediate {
    color: #6897BB;
  }

  token-instr token-label {
    color: #9876AA;

    &[defined='false'] {
      color: #BC3F3C;

      display: inline-block;
      position: relative;

      &:after {
        content: '';
        width: 100%;
        border-bottom: 2px dotted red;
        position: absolute;
        font-size: 16px;
        top: 16px;
        left: 1px;
        display: block;
        height: 4px;
      }
    }
  }

  token-label-definition {
    color: #808080;
  }

  token-label-definition token-label {
    color: #FFC66D;
  }
}

.prism-editor-wrapper.error {
  color: #BC3F3C;
}

.prism-editor-wrapper.compiled {
  addr {
    color: #A5C261;
  }

  instr {
    color: #6D9CBE;
  }

  opcode {
    color: #BC3F3C;
  }
}
</style>
