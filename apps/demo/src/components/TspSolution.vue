<template>
  <div class="tsp-solution">
    <div class="block-title-container">
      <span class="block-title">Solution</span>
    </div>
    <div class="solution-controls">
      <div class="metric-line-status">
        <div class="metric-name">Status:</div>
        <div class="metric-value">{{ this.status }}</div>
      </div>
      <div v-if="currentTaskId" class="metric-line">
        <div class="metric-name">Task id:</div>
        <div class="metric-value">{{ this.currentTaskId }}</div>
      </div>
      <button type="button" @click.prevent="this.startTsp()">SolveTsp</button>
    </div>
  </div>
</template>

<script>
import axios from "axios";

export default {
  name: "TspSolution",
  props: {
    task: Object
  },
  emits: ['solutionFound'],
  data() {
    return {
      status: "IDLE",
      currentTaskId: null
    }
  },
  methods: {
    toJson: function (binArray) {
      var str = "";
      for (var i = 0; i < binArray.length; i++) {
        str += String.fromCharCode(parseInt(binArray[i]));
      }
      return str.split("\n").map(c => c.trim()).filter(c => c !== "").map(c => JSON.parse(c))
    },
    startTsp() {
      console.log(this.status)
      if(this.status === "IN PROGRESS" || this.status === "PENDING") {
        axios.get('http://localhost:8082/v1/optimize/tsp/stop/' + this.currentTaskId)
        .then(r=>console.log("stopped" + this.currentTaskId + ": " + JSON.stringify(r)))
        .catch(e=>console.log(e))
      }

      axios.post("http://localhost:8082/v1/optimize/tsp/start",
          this.task
      )
          .then(r => {
            console.log(r)
            this.currentTaskId = r.data.taskId
          })
          .catch(e => {
            console.log(e)
          })
    },
    parseStatusUpdate(upd) {
      if(upd === "TSP_STATUS_UPDATE_TYPE_PENDING") {
        return "PENDING"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_STARTED") {
        return "IN PROGRESS"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION") {
        return "IN PROGRESS"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_ERROR") {
        return "ERROR"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_PAUSED") {
        return "PAUSED"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_FINISHED") {
        return "FINISHED"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_INTERRUPTED") {
        return "INTERRUPTED"
      }
      if(upd === "TSP_STATUS_UPDATE_TYPE_IN_PROGRESS") {
        return "IN PROGRESS"
      }
      return upd
    }
  },
  watch: {
    currentTaskId(newVal) {
      console.log(newVal)
      if (this.abortController != null) {
        this.abortController.abort()
      }
      this.abortController = new AbortController()

      fetch('http://localhost:8082/v1/optimize/tsp/status?' + new URLSearchParams({task_selector: this.currentTaskId,}),
          {
            signal: this.abortController.signal
          })
          .then(response => {
            const reader = response.body.getReader()
            let parse = this.toJson
            let thisptr = this
            return processor()

            function processor() {
              reader.read().then(({done, value}) => {
                if (done) {
                  return
                }
                let updates = parse(value)
                for (let chunk of updates) {
                  let update = chunk.result.statusUpdate
                  thisptr.status = thisptr.parseStatusUpdate(update.type)
                  if (update.taskId === newVal && update.type === "TSP_STATUS_UPDATE_TYPE_FINISHED") {
                    axios.get('http://localhost:8082/v1/optimize/tsp/solution/' + newVal)
                        .then(r => {
                          thisptr.$emit('solutionFound', r.data.solution)
                        })
                        .catch(e => {
                          console.log(e)
                        })
                    return
                  }
                }
                return processor()
              })
            }
          })
          .catch(err => {
            console.log(err)
          })
    }
  }
}
</script>

<style scoped>
.tsp-solution {
  padding: 10px 20px 20px;
}

.block-title-container {
  display: flex;
  width: 100%;
}

.block-title {
  text-align: center;
  font-weight: bold;
  padding: 5px 10px 15px;
  width: 100%;
}

.solution-controls {
  display: flex;
  flex-direction: column;
  width: 100%;
}

.solution-status {
  padding: 0 10px 10px;
}

.solution-controls button {
  padding: 5px 10px;
  margin: 2px;
  display: inline-block;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
  background-color: #f5f5f5;
}

.solution-controls button:hover {
  background-color: #fefefe;
}

.solution-controls button:active {
  background-color: #dddddd;
}

.metric-line {
  padding: 5px;
}

.metric-line-status {
  display: flex;
  flex-direction: row;
  padding: 5px;
}


.metric-name {
  padding: 5px 0;
}

.metric-value {
  padding: 5px;
  align-self: center;
}
</style>