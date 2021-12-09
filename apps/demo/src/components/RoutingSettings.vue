<template>
  <div class="routing-settings">
    <input id="origin" v-model="stringOrigin"/><label for="origin">Origin</label>
    <input id="destination" v-model="stringDestination"/><label for="destination">Destination</label>
    <input id="speed" v-model="routing_profile.speed.meters_per_second"/><label for="speed">Speed (m/s)</label>
    <select id="engine" v-model="settings.router_engine_type">
      <option v-for="engine in engines" :key="engine.value" :value="engine.value">
        {{ engine.name }}
      </option>
    </select>
    <label for="engine">Engine</label>
    <select id="geometry" v-model="settings.osrm_settings.geometry" v-if="isOsrm()">
      <option v-for="geometry in geometries" :key="geometry.value" :value="geometry.value">
        {{ geometry.name }}
      </option>
    </select>
    <label for="geometry" v-if="isOsrm()">Geometry</label>
    <input id="waypoints" type="checkbox"  v-model="settings.retrieve_waypoints"/><label for="waypoints">Retrieve waypoints</label>
    <input id="forceUpdate" type="checkbox" v-model="settings.force_update"/><label for="forceUpdate">Force update</label>
  </div>
</template>

<script>
const ROUTER_ENGINE_TYPE_CROWFLIGHT = "ROUTER_ENGINE_TYPE_CROWFLIGHT"
const ROUTER_ENGINE_TYPE_EUCLIDEAN = "ROUTER_ENGINE_TYPE_EUCLIDEAN"
const ROUTER_ENGINE_TYPE_RANDOM = "ROUTER_ENGINE_TYPE_RANDOM"
const ROUTER_ENGINE_TYPE_OSRM = "ROUTER_ENGINE_TYPE_OSRM"

const OSRM_GEOMETRY_STRAIGHT_LINE = "OSRM_GEOMETRY_STRAIGHT_LINE"
const OSRM_GEOMETRY_SIMPLIFIED = "OSRM_GEOMETRY_SIMPLIFIED"
const OSRM_GEOMETRY_FULL = "OSRM_GEOMETRY_FULL"

const COORDINATE_ENCODING_MULTIPLIER = 1000000.0;

function encodeCoordinate(c) {
  return Math.trunc(c * COORDINATE_ENCODING_MULTIPLIER)
}

export default {
  name: 'RoutingSettings',
  data() {
    return {
      origin: {
        latitude: 55.740090,
        longitude: 37.624716
      },
      destination: {
        latitude: 53.205533,
        longitude: 50.178594
      },
      routing_profile: {
        speed: {
          meters_per_second: 16.6
        }
      },
      settings: {
        router_engine_type: ROUTER_ENGINE_TYPE_CROWFLIGHT,
        osrm_settings: {
          geometry: OSRM_GEOMETRY_STRAIGHT_LINE
        },
        retrieve_waypoints: false,
        force_update: false
      },
      engines: [
        {name: "Crowflight", value: ROUTER_ENGINE_TYPE_CROWFLIGHT},
        {name: "Euclidean", value: ROUTER_ENGINE_TYPE_EUCLIDEAN},
        {name: "Random", value: ROUTER_ENGINE_TYPE_RANDOM},
        {name: "OSRM", value: ROUTER_ENGINE_TYPE_OSRM}
      ],
      geometries: [
        {name: "Straight line", value: OSRM_GEOMETRY_STRAIGHT_LINE},
        {name: "Simplified", value: OSRM_GEOMETRY_SIMPLIFIED},
        {name: "Full", value: OSRM_GEOMETRY_FULL},
      ]
    }
  },
  computed: {
    stringOrigin: {
      get: function() {
        return this.origin.latitude + ' ' + this.origin.longitude
      },
      set: function(val) {
        var names = val.split(' ')
        this.origin.latitude = parseFloat(names[0])
        this.origin.longitude = parseFloat(names[1])
      }
    },
    stringDestination: {
      get: function() {
        return this.destination.latitude + ' ' + this.destination.longitude
      },
      set: function(val) {
        var names = val.split(' ')
        this.destination.latitude = parseFloat(names[0])
        this.destination.longitude = parseFloat(names[1])
      }
    },
    request: function() {
      return {
        routing_profile: this.routing_profile,
        settings: this.settings,
        origin: {
          encoded_latitude: encodeCoordinate(this.origin.latitude),
          encoded_longitude: encodeCoordinate(this.origin.longitude)
        },
        destination: {
          encoded_latitude: encodeCoordinate(this.destination.latitude),
          encoded_longitude: encodeCoordinate(this.destination.longitude)
        }
      }
    }
  },
  methods: {
    isOsrm: function() {
      return this.settings.router_engine_type === ROUTER_ENGINE_TYPE_OSRM;
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.routing-settings {
  grid-column: 2;
  grid-row: 1;
  overflow: auto;
}
</style>
