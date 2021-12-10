<template>
  <div class="router-scenario">
    <main class="map-container">
      <routing-map :locations="validLocations" :routes="routes"/>
    </main>
    <aside class="sidebar-controls">
      <routing-settings v-model="routingSettings"/>
      <locations-list v-model="locations"/>
    </aside>
  </div>
</template>

<script>
import RoutingSettings from '@/components/RoutingSettings.vue'
import LocationsList from "@/components/LocationsList";
import RoutingMap from "@/components/RoutingMap";

export default {
  name: 'RouterScenario',
  components: {
    RoutingSettings,
    LocationsList,
    RoutingMap
  },
  data() {
    return {
      routingSettings: RoutingSettings.RoutingSettingsModel.createDefault(),
      locations: [
        new LocationsList.Location(55.75718, 37.62355),
        new LocationsList.Location(53.20780, 50.19780)
      ]
    }
  },
  methods: {
    routingSettingsHandler(evt) {
      this.routingSettings = evt
    }
  },
  computed: {
    validLocations: function () {
      return this.locations.filter(function (l) {
        if (l == null) return false;
        if (l.lat == null) return false;
        return l.lon != null;
      });
    },
    routingSettingsRequest: function () {
      return this.routingSettings.toRequest()
    },
    routes: function () {
      let result = []
      for (let i = 0; i < this.validLocations.length - 1; ++i) {
        result.push(new RoutingMap.Route([this.validLocations[i], this.validLocations[i + 1]]))
      }
      return result;
    }
  }
}
</script>

<style>
.router-scenario {
  display: flex;
  flex-direction: row;
  height: 100%;
  width: 100%;
}

.map-container {
  flex: 1;
  align-self: stretch;
}

.sidebar-controls {
  display: flex;
  flex-direction: column;
  width: 300px;
  overflow: auto;
}
</style>
