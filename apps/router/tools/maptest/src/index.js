import React, {Component} from 'react'
import ReactDOM from 'react-dom'
import {Map, TileLayer, Polyline, FeatureGroup} from 'react-leaflet'
import ReactJsonSyntaxHighlighter from "react-json-syntax-highlighter"
import './index.css'

class MapComponent extends Component {
    state = {
        center: [55.740090, 37.624716],
        zoom: 13
    };

    calculateBounds() {
        if (this.props.waypoints != null && this.props.waypoints.length >= 2) {
            return [this.props.waypoints[0], this.props.waypoints[this.props.waypoints.length - 1]];
        }
        return null;
    }

    render() {
        return (
            <Map bounds={this.calculateBounds()} center={this.state.center} zoom={this.state.zoom}>
                <TileLayer
                    attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
                    url='http://{s}.tile.osm.org/{z}/{x}/{y}.png'
                />
                <FeatureGroup>
                    <Polyline positions={this.props.waypoints}/>
                </FeatureGroup>
            </Map>
        );
    }


}

class ResponseComponent extends Component {
    render() {
        return (
            <div className="assfire-response"><ReactJsonSyntaxHighlighter obj={this.props.response}/></div>
        )
    }
}

class RequestControls extends Component {

    state = {
        endpoint: "https://assfire.org/router/api/v1/route",
        // endpoint: "http://localhost:8082/v1/route",
        from_latitude: 55740090,
        from_longitude: 37624716,
        to_latitude: 53205533,
        to_longitude: 50178594,
        velocity: 16.6,
        coordinates_format_precision: 6,
        routing_type: "ROUTER_ENGINE_TYPE_CROWFLIGHT",
        coordinates_format: "FIXED_POINT_INT",
        retrieve_waypoints: false,
        force_update: false,
        osrm_geometry: "OSRM_GEOMETRY_STRAIGHT_LINE"
    }

    buildRequest(state) {
        return state.endpoint + "?" + new URLSearchParams({
            "origin.encoded_latitude": state.from_latitude,
            "origin.encoded_longitude": state.from_longitude,
            "destination.encoded_latitude": state.to_latitude,
            "destination.encoded_longitude": state.to_longitude,
            "routing_profile.speed.meters_per_second": state.velocity,
            "settings.router_engine_type": state.routing_type,
            "settings.retrieve_waypoints": state.retrieve_waypoints,
            "settings.force_update": state.force_update,
            "settings.osrm_settings.geometry": state.osrm_geometry
        }).toString();
    }

    handleUpdate(updatedState) {
        let newState = Object.assign({}, this.state, updatedState);
        this.buildRequest(newState)
        this.setState(Object.assign({}, newState));
    }

    render() {
        return (
            <div>
                <form className="assfire-controls">
                    <label>endpoint: </label><input name="from_latitude" value={this.state.endpoint}
                                                    onChange={(event) => this.handleUpdate({endpoint: event.target.value})}/>
                    <label>from.latitude: </label><input name="from_latitude" value={this.state.from_latitude}
                                                         onChange={(event) => this.handleUpdate({from_latitude: event.target.value})}/>
                    <label>from.longitude: </label><input name="from_longitude" value={this.state.from_longitude}
                                                          onChange={(event) => this.handleUpdate({from_longitude: event.target.value})}/>
                    <label>to.latitude: </label><input name="to_latitude" value={this.state.to_latitude}
                                                       onChange={(event) => this.handleUpdate({to_latitude: event.target.value})}/>
                    <label>to.longitude: </label><input name="to_longitude" value={this.state.to_longitude}
                                                        onChange={(event) => this.handleUpdate({to_longitude: event.target.value})}/>
                    <label>velocity (m/s): </label><input name="velocity" value={this.state.velocity}
                                                          onChange={(event) => this.handleUpdate({velocity: event.target.value})}/>
                    <label>routing_type: </label>
                    <select name="routing_type" value={this.state.routing_type}
                            onChange={(event) => this.handleUpdate({routing_type: event.target.value})}>
                        <option>ROUTER_ENGINE_TYPE_CROWFLIGHT</option>
                        <option>ROUTER_ENGINE_TYPE_EUCLIDEAN</option>
                        <option>ROUTER_ENGINE_TYPE_RANDOM</option>
                        <option>ROUTER_ENGINE_TYPE_OSRM</option>
                    </select>
                    <label>retrieve_waypoints: </label><input type="checkbox" name="retrieve_waypoints"
                                                              checked={this.state.retrieve_waypoints}
                                                              onChange={(event) => this.handleUpdate({retrieve_waypoints: !this.state.retrieve_waypoints})}/>
                    <label>osrm_geometry: </label>
                    <select name="osrm_geometry" value={this.state.osrm_geometry}
                            onChange={(event) => this.handleUpdate({osrm_geometry: event.target.value})}>
                        <option>OSRM_GEOMETRY_STRAIGHT_LINE</option>
                        <option>OSRM_GEOMETRY_SIMPLIFIED</option>
                        <option>OSRM_GEOMETRY_FULL</option>
                    </select>
                    <label>force_update: </label><input type="checkbox" name="force_update"
                                                        checked={this.state.force_update}
                                                        onChange={(event) => this.handleUpdate({force_update: !this.state.force_update})}/>
                    <label>request:</label><input readOnly type="text" name="request"
                                                  value={this.buildRequest(this.state)}/>
                    <input type="button" value="Calculate"
                           onClick={() => this.props.onSubmit(this.buildRequest(this.state), this.state)}/>
                </form>
            </div>
        )
    }
}

class AssfireRouterTestUI extends Component {
    state = {
        response: {},
        waypoints: []
    };

    convertCoordinate(coord, request) {
        return coord / 1e6;
    }

    parseWaypoints(waypoints, request) {
        if (waypoints == null) return [];
        return waypoints.map((wp) => [this.convertCoordinate(wp.encodedLatitude, request), this.convertCoordinate(wp.encodedLongitude, request)])
    }

    sendRequest(requestString, request) {
        let httpRequest = new XMLHttpRequest()
        httpRequest.addEventListener('load', () => {
            const response = JSON.parse(httpRequest.responseText);
            this.setState(Object.assign({}, this.state,
                {
                    response: response
                },
                {
                    waypoints: this.parseWaypoints(response.routeInfo.waypoints, request)
                }))
        })
        httpRequest.open('GET', requestString)
        httpRequest.send()
    }

    render() {
        return (
            <div className="assfire-ui">
                <div className="assfire-map"><MapComponent waypoints={this.state.waypoints}/></div>
                <div className="assfire-request-controls"><RequestControls
                    onSubmit={(request) => this.sendRequest(request)}/></div>
                <div className="assfire-stats"><ResponseComponent response={this.state.response}/></div>
            </div>
        );
    }
}

ReactDOM.render(<AssfireRouterTestUI/>, document.getElementById('root'));