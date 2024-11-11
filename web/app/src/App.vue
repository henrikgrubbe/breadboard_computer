<script lang="ts">
import "@/assets/style.scss";
import { defineComponent } from "vue";
import SingleAddress from "@/components/SingleAddress.vue";
import ProgramCompiler from "@/components/ProgramCompiler.vue";

export default defineComponent({
  name: "Breadboard PC programmer",
  components: {
    ProgramCompiler,
    SingleAddress,
  },
});
</script>

<template>
  <div class="container">
    <div class="row my-5">
      <h1 class="display-4">Breadboard PC</h1>
    </div>

    <ProgramCompiler />

    <SingleAddress />
  </div>
</template>

<style lang="scss"></style>
