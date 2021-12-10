<template>
  <div class="route-summary">
    <div class="block-title-container">
      <span class="block-title">Route summary</span>
    </div>
    <div class="metric-line">
      <div class="metric-name">Total distance (m):</div>
      <div class="metric-value">{{ summary.meters }}</div>
    </div>
    <div class="metric-line">
      <div class="metric-name">Total time (s):</div>
      <div class="metric-value">{{ summary.seconds }}</div>
    </div>
  </div>
</template>

<script>

class RouteSummary {
  constructor(seconds, meters) {
    this.seconds = seconds;
    this.meters = meters;
  }
}

export default {
  name: 'RouteSummary',
  props: {
    summary: RouteSummary
  },
  data() {
    return {}
  },
  watch: {},
  methods: {},

  RouteSummary
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.route-summary {
  padding: 10px 20px 20px;
}

.block-title-container {
  display: flex;
  width: 100%;
}

.block-title {
  text-align: center;
  font-weight: bold;
  padding: 5px 10px 5px;
  width: 100%;
}

.metric-line {
  padding: 5px;
}

.metric-name {
  padding: 5px 0;
}

.metric-value {
  font-size: large;
}

</style>
