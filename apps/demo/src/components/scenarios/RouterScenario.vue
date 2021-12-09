<template>
  <div class="router-scenario">
    <main class="map-container">
      <l-map class="rendered-map" :zoom="5" :center="[55.75414325118748, 37.62138538509248]">
        <l-tile-layer
            url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            layer-type="base"
            name="OpenStreetMap"></l-tile-layer>
      </l-map>
    </main>
    <aside class="sidebar-controls">
      <routing-settings v-model="routingSettings"/>
      <locations-list v-model="locations"/>
      {{ filteredLocations }}
    </aside>
  </div>
</template>

<script>
import RoutingSettings from '@/components/RoutingSettings.vue'
import LocationsList from "@/components/LocationsList";
import RoutingConstants from "@/components/RoutingSettingsModel";
import {LMap, LTileLayer} from "@vue-leaflet/vue-leaflet";
import "leaflet/dist/leaflet.css";

export default {
  name: 'RouterScenario',
  components: {
    LMap,
    LTileLayer,
    RoutingSettings,
    LocationsList
  },
  data() {
    return {
      routingSettings: {
        routing_profile: {
          speed: {
            meters_per_second: 16.6
          }
        },
        settings: {
          router_engine_type: RoutingConstants.ROUTER_ENGINE_TYPE_CROWFLIGHT,
          osrm_settings: {
            geometry: RoutingConstants.OSRM_GEOMETRY_STRAIGHT_LINE
          },
          retrieve_waypoints: false,
          force_update: false
        },
      },
      locations: [
        {lat: 55.234, lon: 53.234},
        {lat: 57.234, lon: 52.234}
      ]
    }
  },
  methods: {
    routingSettingsHandler(evt) {
      this.routingSettings = evt
    }
  },
  computed: {
    filteredLocations: function () {
      return this.locations.filter(l => l != null && l.lat != null && l.lon != null);
    }
  },
  watch: {
    routingSettings: {
      handler(val) {
        console.log(val)
      },
      deep: true
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

.rendered-map {

}
</style>
