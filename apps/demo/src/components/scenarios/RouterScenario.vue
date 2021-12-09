<template>
  <div class="router-scenario">
    <main class="map-container">
      <l-map class="rendered-map">
        <l-tile-layer
            url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            layer-type="base"
            name="OpenStreetMap"></l-tile-layer>
      </l-map>
    </main>
    <aside class="sidebar-controls">
      <routing-settings v-model="routingSettings"/>
    </aside>
  </div>
</template>

<script>
import RoutingSettings from '@/components/RoutingSettings.vue'
import RoutingConstants from "@/components/RoutingSettingsModel";
import {LMap, LTileLayer} from "@vue-leaflet/vue-leaflet";
import "leaflet/dist/leaflet.css";

export default {
  name: 'RouterScenario',
  components: {
    LMap,
    LTileLayer,
    RoutingSettings
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
      }
    }
  },
  methods: {
    routingSettingsHandler(evt) {
      this.routingSettings = evt
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
  width: 300px;
  overflow: auto;
}

.rendered-map {

}
</style>
