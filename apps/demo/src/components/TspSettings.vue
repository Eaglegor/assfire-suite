<template>
  <div class="tsp-settings">
    <div class="block-title-container">
      <span class="block-title">TSP settings</span>
    </div>

    <div class="form-contents">
      <div class="labeled-input">
        <label for="algorithm">Algorithm:</label>
        <select id="algorithm" v-model="tspSettings.algorithm_type">
          <option v-for="algorithm in algorithms" :key="algorithm.value" :value="algorithm.value">
            {{ algorithm.name }}
          </option>
        </select>
      </div>

      <div class="labeled-input">
        <label for="costFunction">Cost function:</label>
        <select id="costFunction" v-model="tspSettings.cost_function_type">
          <option v-for="costFunction in costFunctions" :key="costFunction.value" :value="costFunction.value">
            {{ costFunction.name }}
          </option>
        </select>
      </div>

      <div class="labeled-input" v-if="isTotalDistanceCostFunction()">
        <label for="kmCost">Cost per km:</label>
        <input id="kmCost" v-model="tspSettings.cost_per_km"/>
      </div>

      <div class="labeled-input">
        <label for="validator">Validation model:</label>
        <select id="validator" v-model="tspSettings.validator_type">
          <option v-for="validator in validators" :key="validator.value" :value="validator.value">
            {{ validator.name }}
          </option>
        </select>
      </div>
    </div>
  </div>
</template>

<script>
import RoutingSettings from "@/components/RoutingSettings";

const TSP_ALGORITHM_TYPE_AUTO = "TSP_ALGORITHM_TYPE_AUTO";
const TSP_ALGORITHM_TYPE_TWO_OPT = "TSP_ALGORITHM_TYPE_TWO_OPT";
const TSP_ALGORITHM_TYPE_TIME_WASTING = "TSP_ALGORITHM_TYPE_TIME_WASTING";

const TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE = "TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE";

const TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION = "TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION";


class TspSettingsModel {
  constructor(routing_settings, algorithm_type, cost_function_type, cost_per_km, validator_type) {
    this.routing_settings = routing_settings;
    this.algorithm_type = algorithm_type
    this.cost_function_type = cost_function_type
    this.validator_type = validator_type
    this.cost_per_km = cost_per_km
  }

  static createDefault() {
    return new TspSettingsModel(RoutingSettings.RoutingSettingsModel.createDefault(), TSP_ALGORITHM_TYPE_AUTO, TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE, 1, TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION)
  }

  withRoutingSettings(settings) {
    return new TspSettingsModel(settings, this.algorithm_type, this.cost_function_type, this.cost_per_km, this.validator_type)
  }

  toRequestRaw() {
    return {
      estimator_settings: {
        cost_function_settings: {
          type: this.cost_function_type,
          total_distance_cost_function_settings: {
            cost_per_km: this.cost_per_km
          }
        },
        validator_settings: {
          type: this.validator_type
        },
        routing_settings: {
          route_provider_settings: this.routing_settings.toRequestRaw().settings,
          routing_profile: this.routing_settings.toRequestRaw().routing_profile
        }
      }
    }
  }
}

export default {
  name: 'RoutingSettings',
  props: {
    modelValue: TspSettingsModel,
    routingSettings: RoutingSettings.RoutingSettingsModel
  },
  data() {
    return {
      tspSettings: this.modelValue,
      algorithms: [
        {name: "Auto", value: TSP_ALGORITHM_TYPE_AUTO},
        {name: "2-opt", value: TSP_ALGORITHM_TYPE_TWO_OPT},
        {name: "Waste time", value: TSP_ALGORITHM_TYPE_TIME_WASTING},
      ],
      costFunctions: [
        {name: "Total distance", value: TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE},
      ],
      validators: [
        {name: "Reject on any violation", value: TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION},
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
    isTotalDistanceCostFunction: function () {
      return this.tspSettings.cost_function_type === TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE;
    }
  },

  TSP_ALGORITHM_TYPE_AUTO: TSP_ALGORITHM_TYPE_AUTO,
  TSP_ALGORITHM_TYPE_TWO_OPT: TSP_ALGORITHM_TYPE_TWO_OPT,
  TSP_ALGORITHM_TYPE_TIME_WASTING: TSP_ALGORITHM_TYPE_TIME_WASTING,
  TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE: TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE,
  TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION: TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION,

  TspSettingsModel
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.tsp-settings {
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
