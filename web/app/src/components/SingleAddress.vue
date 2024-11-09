<template>
  <div class="row">
    <div class="col-lg-10 col-xl-8 mb-3">
      <label for="program-txt" class="form-label">Write a single RAM address</label>
      <div class="input-group mb-3">
        <input v-model="data" type="number" min="0" max="255" class="form-control" placeholder="Value"
               aria-label="Value">
        <span class="input-group-text">@</span>
        <input v-model="address" type="number" min="0" max="15" class="form-control" placeholder="Address"
               aria-label="Address">
      </div>
    </div>

    <div class="col-12">
      <button @click="writeImmediate" :disabled="data === '' || address === ''" type="button"
              class="btn btn-success btn-lg me-1">Write
      </button>
    </div>
  </div>
</template>

<script lang="ts">
import {publishToTopic} from '@/utils/mqtt-upload';
import {defineComponent} from 'vue';

export default defineComponent({
  name: 'SingleAddress',
  data() {
    return {
      data: '' as string,
      address: '' as string
    }
  },
  methods: {
    writeImmediate(): void {
      if (this.address === '' || this.data === '') {
        return;
      }

      publishToTopic('command', {
        command: 'WRITE',
        address: this.address,
        data: this.data
      });
    }
  }
});
</script>

<style scoped>

</style>