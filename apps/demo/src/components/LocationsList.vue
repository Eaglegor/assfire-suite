<template>
  <div class="locations-list">
    <div class="block-title-container">
      <span class="block-title">Locations</span>
    </div>
    <div class="locations-inputs">
      <div class="location-block" v-for="(location, index) in locations" :key="stringLocation(location)">
        <input @change="updateLocation(index, $event.target.value)" :value="stringLocation(location)">
        <button type="button">M</button>
        <button type="button" @click.prevent="removeLocation(index)">X</button>
      </div>
      <button type="button" @click.prevent="addLocation()">Add</button>
    </div>
  </div>
</template>

<script>

export default {
  name: 'LocationsList',
  props: {
    modelValue: Array
  },
  data() {
    return {
      locations: this.modelValue,
    }
  },
  watch: {},
  methods: {
    stringLocation: function(location) {
      if(location == null) return '';
      return location.lat + ' ' + location.lon;
    },
    removeLocation(index) {
      this.locations.splice(index, 1)
    },
    updateLocation(index, value) {
      var lanlon = value.split(' ');
      this.locations[index] = {lat: parseFloat(lanlon[0]), lon: parseFloat(lanlon[1])};
    },
    addLocation() {
      this.locations.push(null)
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.locations-list {
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

.locations-inputs {
  display: flex;
  flex-direction: column;
  width: 100%;
}

.location-block {
  display: flex;
  margin: 5px 0 5px;
  width: 100%;
}

.location-block input {
  width: 100%;
  padding: 5px 10px;
  margin: 2px;
  display: inline-block;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
}

.locations-inputs button {
  padding: 5px 10px;
  margin: 2px;
  display: inline-block;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
  background-color: #f5f5f5;
}

.locations-inputs button:hover {
  background-color: #fefefe;
}

.locations-inputs button:active {
  background-color: #dddddd;
}

</style>
