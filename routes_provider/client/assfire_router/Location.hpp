#pragma once

namespace assfire
{
	class Location
	{
	public:
		Location(int latitude, int longitude):
		latitude(latitude), longitude(longitude)
		{}

		int getLatitude() const {
			return latitude;
		}

		int getLongitude() const {
			return longitude;
		}

		bool operator==(const Location& rhs) const {
			return latitude == rhs.latitude && longitude == rhs.longitude;
		}

	private:
		int latitude;
		int longitude;
	};
}