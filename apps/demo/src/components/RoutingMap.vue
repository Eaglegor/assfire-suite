<template>
  <div class="routing-map">
    <l-map class="rendered-map" :zoom="zoom" :center="center">
      <l-tile-layer
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
          layer-type="base"
          name="OpenStreetMap"></l-tile-layer>
      <l-marker v-for="marker in markers" :key="marker.key" :lat-lng="marker.value"/>
      <l-polyline v-for="polyline in polylines" :key="polyline.key" :lat-lngs="polyline.values"/>
    </l-map>
  </div>
</template>

<script>

import {LMap, LTileLayer, LMarker, LPolyline} from "@vue-leaflet/vue-leaflet";
import "leaflet/dist/leaflet.css";

class Route {
  constructor(pts) {
    this.pts = pts
  }

  asKey() {
    return this.pts.map(p=>p.asKey()).reduce((a,b)=>a+b)
  }

  toLatLngs() {
    return this.pts.map(l=>l.toLatLng())
  }
}

export default {
  name: 'RoutingMap',
  components: {
    LMap,
    LTileLayer,
    LMarker,
    LPolyline
  },
  props: {
    locations: Array,
    routes: Array
  },
  data() {
    return {
      center: [55.75414325118748, 37.62138538509248],
      zoom: 5
    }
  },
  computed: {
    markers: function () {
      return this.locations.map(function (l) {
        return {
          key: l.asKey(),
          value: l.toLatLng()
        }
      })
    },
    polylines: function () {
      return this.routes.map(function (r) {
        return {
          key: r.asKey(),
          values: r.toLatLngs()
        }
      })
    }
  },
  methods: {},

  Route
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.routing-map {
  height: 100%;
}

.rendered-map {

}
</style>
