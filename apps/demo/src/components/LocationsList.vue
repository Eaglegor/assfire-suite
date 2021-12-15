<template>
  <div class="locations-list">
    <div class="block-title-container">
      <span class="block-title">LocationsList</span>
    </div>
    <div class="locations-inputs">
      <div class="location-block" v-for="(location, index) in locations" :key="stringLocation(location)">
        <input @change="updateLocation(index, $event.target.value)" :value="stringLocation(location)">
<!--        <button type="button">M</button>-->
        <button type="button" @click.prevent="removeLocation(index)">X</button>
      </div>
      <button type="button" @click.prevent="addLocation()">Add</button>
    </div>
  </div>
</template>

<script>

class Location {
  constructor(lat, lon) {
    this.lat = lat;
    this.lon = lon;
  }

  asKey() {
    return this.lat + ' ' + this.lon
  }

  toRequest(name) {
    let result = {}
    result[name+'.encoded_latitude'] = Math.trunc(this.lat * 1000000);
    result[name+'.encoded_longitude'] = Math.trunc(this.lon * 1000000);
    return result
  }

  toRequestRaw() {
    return {
      encoded_latitude: Math.trunc(this.lat * 1000000),
      encoded_longitude: Math.trunc(this.lon * 1000000),
    }
  }

  isValid() {
    return this.lat != null && this.lon != null && !isNaN(this.lat) && !isNaN(this.lon)
  }

  static fromEncoded(lat, lon) {
    return new Location(lat / 1000000.0, lon / 1000000.0)
  }

  static fromResponse(loc) {
    return this.fromEncoded(loc.encodedLatitude, loc.encodedLongitude)
  }

  toLatLng() {
    return [this.lat, this.lon]
  }
}

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
    stringLocation: function (location) {
      if (location == null) return '';
      return (location.lat == null ? '' : location.lat.toPrecision(9)) + ' ' + (location.lon == null ? '' : location.lon.toPrecision(9));
    },
    removeLocation(index) {
      this.locations.splice(index, 1)
    },
    updateLocation(index, value) {
      let [lat, lon] = value.split(' ').map(v => parseFloat(v));
      this.locations[index] = new Location(isNaN(lat) ? null : lat, isNaN(lon) ? null : lon);
    },
    addLocation() {
      this.locations.push(null)
    }
  },

  Location
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
