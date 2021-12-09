<template>
  <div class="routing-settings">
    <div class="block-title-container">
      <span class="block-title">Routing settings</span>
    </div>

    <div class="form-contents">
      <div class="labeled-input">
        <label for="speed">Speed (m/s):</label>
        <input id="speed" v-model.lazy="routingSettings.routing_profile.speed.meters_per_second"/>
      </div>

      <div class="labeled-input">
        <label for="engine">Engine:</label>
        <select id="engine" v-model="routingSettings.settings.router_engine_type">
          <option v-for="engine in engines" :key="engine.value" :value="engine.value">
            {{ engine.name }}
          </option>
        </select>
      </div>

      <div class="labeled-input" v-if="isOsrm()">
        <label for="geometry">Geometry:</label>
        <select id="geometry" v-model="routingSettings.settings.osrm_settings.geometry">
          <option v-for="geometry in geometries" :key="geometry.value" :value="geometry.value">
            {{ geometry.name }}
          </option>
        </select>
      </div>

      <div class="labeled-checkbox">
        <label for="waypoints">Retrieve waypoints:</label>
        <input id="waypoints" class="checkbox-input" type="checkbox"
               v-model="routingSettings.settings.retrieve_waypoints"/>
        <div class="label-circle"/>
      </div>

      <div class="labeled-checkbox">
        <label for="forceUpdate">Force update:</label>
        <input id="forceUpdate" class="checkbox-input" type="checkbox" v-model="routingSettings.settings.force_update"/>
      </div>
    </div>
  </div>
</template>

<script>
import RoutingConstants from "@/components/RoutingSettingsModel";

export default {
  name: 'RoutingSettings',
  props: {
    modelValue: Object
  },
  data() {
    return {
      routingSettings: this.modelValue,
      engines: [
        {name: "Crowflight", value: RoutingConstants.ROUTER_ENGINE_TYPE_CROWFLIGHT},
        {name: "Euclidean", value: RoutingConstants.ROUTER_ENGINE_TYPE_EUCLIDEAN},
        {name: "Random", value: RoutingConstants.ROUTER_ENGINE_TYPE_RANDOM},
        {name: "OSRM", value: RoutingConstants.ROUTER_ENGINE_TYPE_OSRM}
      ],
      geometries: [
        {name: "Straight line", value: RoutingConstants.OSRM_GEOMETRY_STRAIGHT_LINE},
        {name: "Simplified", value: RoutingConstants.OSRM_GEOMETRY_SIMPLIFIED},
        {name: "Full", value: RoutingConstants.OSRM_GEOMETRY_FULL},
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
      return this.routingSettings.settings.router_engine_type === RoutingConstants.ROUTER_ENGINE_TYPE_OSRM;
    }
  }
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
