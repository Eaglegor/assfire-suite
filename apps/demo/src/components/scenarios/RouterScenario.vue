<template>
  <div class="router-scenario">
    <main class="map-container">
      <routing-map :locations="validLocations" :routes="routes" @update-location="updateLocationByCoords"
                   @new-location="addLocation"/>
    </main>
    <aside class="sidebar-controls">
      <routing-settings class="control-block" v-model="routingSettings"/>
      <locations-list class="control-block" v-model="locations"/>
      <route-summary class="control-block" v-if="routeSummary != null" :summary="routeSummary"/>
    </aside>
  </div>
</template>

<script>
import RoutingSettings from '@/components/RoutingSettings.vue'
import LocationsList from "@/components/LocationsList";
import RoutingMap from "@/components/RoutingMap";
import RouteSummary from "@/components/RouteSummary";
import axios from "axios";

let lastRequestController = null

export default {
  name: 'RouterScenario',
  components: {
    RoutingSettings,
    LocationsList,
    RoutingMap,
    RouteSummary
  },
  data() {
    return {
      routingSettings: RoutingSettings.RoutingSettingsModel.createDefault(),
      locations: [
        new LocationsList.Location(55.75718, 37.62355),
        new LocationsList.Location(53.20780, 50.19780)
      ],
      routes: [],
      routeSummaries: []
    }
  },
  methods: {
    routingSettingsHandler(evt) {
      this.routingSettings = evt
    },
    makeRequest(origin, destination) {
      return {
        ...this.routingSettings.toRequest(),
        ...origin.toRequest('origin'),
        ...destination.toRequest('destination')
      }
    },
    makeVectorRequest(locations) {
      return {
        ...this.routingSettings.toRequestRaw(),
        locations: locations.map(l => l.toRequestRaw())
      }
    },
    updateRoutes(locations) {
      if (lastRequestController != null) {
        lastRequestController.abort()
      }
      lastRequestController = new AbortController();

      let ghostRoutes = []
      for (let i = 0; i < locations.length - 1; ++i) {
        ghostRoutes.push(new RoutingMap.Route([locations[i], locations[i + 1]], '#999999'))
      }
      this.routes = ghostRoutes
      this.routeSummaries = []

      let promises = []
      for (let i = 0; i < locations.length - 1; ++i) {
        let request = this.makeRequest(locations[i], locations[i + 1])
        promises.push(axios
            .get('http://localhost:8082/v1/route',
                {
                  params: request,
                  signal: lastRequestController.signal
                }))
      }

      Promise.all(promises)
          .then(responses => {
            let newRoutes = []
            let newRouteSummaries = []

            for (let response of responses) {
              let waypoints = response.data.routeInfo.waypoints;
              let resultingPoints = waypoints.map(wp => LocationsList.Location.fromResponse(wp));
              newRoutes.push(new RoutingMap.Route(resultingPoints, '#0e0ede'))
              newRouteSummaries.push({
                seconds: response.data.routeInfo.duration.seconds,
                meters: response.data.routeInfo.distance.meters
              })
            }

            this.routes = newRoutes;
            this.routeSummaries = newRouteSummaries;
          })
          .catch(error => {
            if (!(error instanceof axios.Cancel)) {
              let failedRoutes = []
              for (let i = 0; i < locations.length - 1; ++i) {
                failedRoutes.push(new RoutingMap.Route([locations[i], locations[i + 1]], '#de0e0e'))
              }
              console.log(error)
              this.routes = failedRoutes
              this.routeSummaries = []
            } else {
              console.log("Cancelled");
            }
          });
    },
    updateRoutesVector(locations) {
      if (lastRequestController != null) {
        lastRequestController.abort()
      }
      lastRequestController = new AbortController();

      let ghostRoutes = []
      for (let i = 0; i < locations.length - 1; ++i) {
        ghostRoutes.push(new RoutingMap.Route([locations[i], locations[i + 1]], '#999999'))
      }
      this.routes = ghostRoutes
      this.routeSummaries = []

      axios
          .post('http://localhost:8083/v1/route/vector',
              {
                ...this.makeVectorRequest(locations),
                signal: lastRequestController.signal
              })
          .then(response => {
            let newRoutes = []
            let newRouteSummaries = []

            for (let route of response.data.routeInfos) {
              let waypoints = route.waypoints;
              let resultingPoints = waypoints.map(wp => LocationsList.Location.fromResponse(wp));
              newRoutes.push(new RoutingMap.Route(resultingPoints, '#0e0ede'))
              newRouteSummaries.push({
                seconds: route.duration.seconds,
                meters: route.distance.meters
              })
            }

            this.routes = newRoutes;
            this.routeSummaries = newRouteSummaries;
          })
          .catch(error => {
            if (!(error instanceof axios.Cancel)) {
              let failedRoutes = []
              for (let i = 0; i < locations.length - 1; ++i) {
                failedRoutes.push(new RoutingMap.Route([locations[i], locations[i + 1]], '#de0e0e'))
              }
              console.log(error)
              this.routes = failedRoutes
              this.routeSummaries = []
            } else {
              console.log("Cancelled");
            }
          });
    },
    updateLocationByCoords(event) {
      let updateIndex = this.locations.findIndex(l => l.asKey() === event.location.asKey())
      if (updateIndex >= 0) {
        this.locations[updateIndex] = new LocationsList.Location(event.newPosition.lat, event.newPosition.lng)
      } else {
        console.log("couldn't find location for key: " + event.location.asKey())
      }
    },
    addLocation(event) {
      this.locations.push(new LocationsList.Location(event.lat, event.lng))
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
    routeSummary: function () {
      if (this.routes.length === 0) return null
      if (this.routes.length !== this.routeSummaries.length) return null
      let total_meters = 0;
      let total_seconds = 0;
      this.routeSummaries.forEach(function (s) {
        total_meters += s.meters;
        total_seconds += s.seconds;
      });
      return new RouteSummary.RouteSummary(total_seconds, total_meters)
    }
  },
  watch: {
    validLocations: {
      handler: function (val) {
        if (this.routingSettings.vectorMode) {
          this.updateRoutesVector(val)
        } else {
          this.updateRoutes(val)
        }
      },
      immediate: true
    },
    routingSettings: {
      handler: function () {
        if (this.routingSettings.vectorMode) {
          this.updateRoutesVector(this.locations)
        } else {
          this.updateRoutes(this.validLocations)
        }
      },
      deep: true
    }
  }
}
</script>

<style scoped>
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
}

.control-block {
  background-color: #eeeeee;
  margin: 10px;
  border-radius: 10px;
}
</style>
