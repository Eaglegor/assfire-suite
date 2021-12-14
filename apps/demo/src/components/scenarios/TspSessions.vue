<template>
  <div class="tsp-sessions-table">
    <div class="tsp-session-row">
      <div class="tsp-caption-row">
        <div class="tsp-task-id">Task id</div>
        <div class="tsp-task-status">Latest status</div>
        <div class="tsp-latest-cost">Latest cost</div>
        <div class="tsp-latest-validation-result">Latest validation result</div>
      </div>
    </div>
    <div class="tsp-session-row" v-for="(upd, _, index) in taskUpdates" :key="upd.taskId">
      <div :class="{'tsp-session-row-even': index % 2 === 0, 'tsp-session-row-odd': index % 2 !== 0 }">
        <div class="tsp-task-id">{{ upd.taskId }}</div>
        <div class="tsp-task-status">{{ upd.type }}</div>
        <div class="tsp-latest-cost">{{ upd.newSolutionCost == null ? '-' : upd.newSolutionCost.value }}</div>
        <div class="tsp-latest-validation-result">{{ upd.newSolutionValidationResult == null ? ' - ' : upd.newSolutionValidationResult.isValid ? "VALID" : "INVALID" }}</div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "TspSessions",
  data() {
    return {
      taskUpdates: {},
      abortController: null
    }
  },
  methods: {
    toJson: function (binArray) {
      var str = "";
      for (var i = 0; i < binArray.length; i++) {
        str += String.fromCharCode(parseInt(binArray[i]));
      }
      return str.split("\n").map(c => c.trim()).filter(c => c !== "").map(c => JSON.parse(c))
    }
  },
  mounted() {
    this.abortController = new AbortController()
    fetch('http://localhost:8082/v1/optimize/tsp/status',
        {
          signal: this.abortController.signal
        })
        .then(response => {
          const reader = response.body.getReader()
          let parse = this.toJson
          let result = this.taskUpdates
          return processor()

          function processor() {
            reader.read().then(({done, value}) => {
              if (done) {
                return
              }
              let updates = parse(value)
              for (let chunk of updates) {
                let update = chunk.result.statusUpdate
                result[update.taskId] = update
              }
              return processor()
            })
          }
        })
        .catch(err => {
          console.log(err)
        })
  },
  unmounted() {
    this.abortController.abort()
  }
}
</script>

<style scoped>
.tsp-sessions-table {
  padding: 20px;
  background-color: #dedede;
}

.tsp-caption-row {
  padding: 10px;
  width: 100%;
  background-color: #bbbbbb;
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  font-weight: bold;
}

.tsp-session-row {
  display: flex;
  width: 100%;
  flex-direction: row;
}

.tsp-session-row-even {
  padding: 10px;
  background-color: #eeeeee;
  flex: 1;
  display: grid;
  grid-template-columns: repeat(4, 1fr);
}

.tsp-session-row-odd {
  padding: 10px;
  background-color: #dddddd;
  flex: 1;
  display: grid;
  grid-template-columns: repeat(4, 1fr);
}

.tsp-task-id {
  padding: 10px;
  grid-row: 1;
  grid-column: 1;
}

.tsp-task-status {
  padding: 10px;
  grid-row: 1;
  grid-column: 2;
}

.tsp-latest-cost {
  padding: 10px;
  grid-row: 1;
  grid-column: 3;
}

.tsp-latest-validation-result {
  padding: 10px;
  grid-row: 1;
  grid-column: 4;
}
</style>
