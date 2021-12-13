<template>
  <div class="app-content">
    <header class="header"><label>Selected scenario:</label>
      <select v-model="selectedScenario">
        <option v-for="scenario in scenarios" :key="scenario.value" :value="scenario.value">{{ scenario.name }}</option>
      </select>
    </header>
    <main class="scenario-content">
      <component :is="selectedScenario"/>
    </main>
    <footer class="footer"><label>(c) Assfire.org</label>
    </footer>
  </div>
</template>

<script>
import RouterScenario from "@/components/scenarios/RouterScenario";
import TspScenario from "@/components/scenarios/TspScenario";

export default {
  name: 'AssfireDemo',
  components: {
    RouterScenario,
    TspScenario
  },
  data() {
    return {
      selectedScenario: "RouterScenario",
      scenarios: [
        {
          name: "Assfire Router",
          value: "RouterScenario"
        },
        {
          name: "Assfire TSP",
          value: "TspScenario"
        }
      ]
    }
  },
}
</script>

<style>
htlm, body {
  margin: 0;
  font-family: Avenir, Helvetica, Arial, sans-serif;
}
.app-content {
  display: flex;
  flex-direction: column;
  height: 100vh;
}
.header {
  padding: 10px;
  flex-shrink: 0;
}
.footer {
  padding: 5px 10px 5px;
}
.header select {
  margin-left: 10px;
}
.scenario-content {
  flex-grow: 1;
}
</style>
