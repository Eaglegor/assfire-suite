<template>
  <div class="routing-settings">
    <div class="block-title-container">
      <span class="block-title">Routing settings</span>
    </div>

    <div class="form-contents">
      <div class="labeled-input">
        <label for="speed">Speed (m/s):</label>
        <input id="speed" v-model.lazy="routingSettings.speed"/>
      </div>

      <div class="labeled-input">
        <label for="engine">Engine:</label>
        <select id="engine" v-model="routingSettings.engine">
          <option v-for="engine in engines" :key="engine.value" :value="engine.value">
            {{ engine.name }}
          </option>
        </select>
      </div>

      <div class="labeled-input" v-if="isOsrm()">
        <label for="geometry">Geometry:</label>
        <select id="geometry" v-model="routingSettings.geometry">
          <option v-for="geometry in geometries" :key="geometry.value" :value="geometry.value">
            {{ geometry.name }}
          </option>
        </select>
      </div>

      <div class="labeled-checkbox">
        <label for="waypoints">Retrieve waypoints:</label>
        <input id="waypoints" class="checkbox-input" type="checkbox"
               v-model="routingSettings.retriveWaypoints"/>
        <div class="label-circle"/>
      </div>

      <div class="labeled-checkbox">
        <label for="forceUpdate">Force update:</label>
        <input id="forceUpdate" class="checkbox-input" type="checkbox" v-model="routingSettings.forceUpdate"/>
      </div>
    </div>
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

class RoutingSettingsModel {
  constructor(speed, engine, geometry, retrieveWaypoints, forceUpdate) {
    this.speed = speed
    this.engine = engine
    this.geometry = geometry
    this.retriveWaypoints = retrieveWaypoints
    this.forceUpdate = forceUpdate
  }
}

export default {
  name: 'RoutingSettings',
  props: {
    modelValue: RoutingSettingsModel
  },
  data() {
    return {
      routingSettings: this.modelValue,
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
  watch: {
    routingSettings: {
      handler(val) {
        this.$emit('update:modelValue', val)
      },
      deep: true
    }
  },
  methods: {
    isOsrm: function () {
      return this.routingSettings.engine === ROUTER_ENGINE_TYPE_OSRM;
    }
  },
  createDefaultSettings: function () {
    return new RoutingSettingsModel(16.6, ROUTER_ENGINE_TYPE_CROWFLIGHT, OSRM_GEOMETRY_STRAIGHT_LINE, false, false)
  },
  createSettings: function (speed, engine, geometry, retrieveWaypoints, forceUpdate) {
    return new RoutingSettingsModel(speed, engine, geometry, retrieveWaypoints, forceUpdate)
  },
  settingsToRequest(settings) {
    return {
      routingSettings: {
        routing_profile: {
          speed: {
            meters_per_second: settings.speed
          }
        },
        settings: {
          router_engine_type: settings.engine,
          osrm_settings: {
            geometry: settings.geometry
          },
          retrieve_waypoints: settings.retriveWaypoints,
          force_update: settings.forceUpdate
        }
      }
    }
  },

  ROUTER_ENGINE_TYPE_CROWFLIGHT: ROUTER_ENGINE_TYPE_CROWFLIGHT,
  ROUTER_ENGINE_TYPE_EUCLIDEAN: ROUTER_ENGINE_TYPE_EUCLIDEAN,
  ROUTER_ENGINE_TYPE_RANDOM: ROUTER_ENGINE_TYPE_RANDOM,
  ROUTER_ENGINE_TYPE_OSRM: ROUTER_ENGINE_TYPE_OSRM,

  OSRM_GEOMETRY_STRAIGHT_LINE: OSRM_GEOMETRY_STRAIGHT_LINE,
  OSRM_GEOMETRY_SIMPLIFIED: OSRM_GEOMETRY_SIMPLIFIED,
  OSRM_GEOMETRY_FULL: OSRM_GEOMETRY_FULL
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.routing-settings {
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

.form-contents input, select {
  width: 100%;
  padding: 10px 20px;
  margin: 8px 0;
  display: inline-block;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
}

.form-contents {
  text-align: left;
}

.labeled-input {
  margin-bottom: 10px;
}

.labeled-checkbox {
  height: 40px;
  line-height: 40px;
  display: flex;
  align-items: center;
}

.labeled-checkbox input[type=checkbox] {
  display: flex;
  width: 25px;
  height: 25px;
  margin-left: 10px;
}
</style>
