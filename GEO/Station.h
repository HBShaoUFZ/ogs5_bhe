/**
 * \file Station.h
 * KR Initial implementation
 */

#ifndef GEO_STATION_H
#define GEO_STATION_H

#include <string>
#include <list>
#include <vector>
#include <map>

#include "Polyline.h"
#include "Point.h"
#include "Color.h"
#include "PropertyBounds.h"


namespace GEOLIB {

/**
 * \brief An observation station as a geometric object (i.e. basically a Point with some additional information.
 *
 * An observation station as a geometric object. Such a station is basically a point object
 * with some additional information such as colour, a name, etc.
 *
 * Notes concerning the property-system used in this class:
 * Variables of Station and derived classes can be defined to be "properties" of this class.
 * Certain functions in the GUI allow you to modify aspects of the visualisation based on these
 * properties (e.g. filtering operations such as "display only boreholes drilled after 1990 with a
 * depth between 400-800m").
 * To make use of this functionality you need to define properties using the "Station::addProperty()"-method.
 * Parameters for this function include the name of the property as well as a method to read and write the
 * value of the associated variable (see documentation for "addProperty" for details). Furthermore, these read
 * and write-functions need to be actually implemented as static functions to avoid casting problems with the
 * function pointers used to dynamically connect the GUI functionality to the variables defined within the
 * station-classes. Please refer to the documentation of the properties defined below for details.
 */
class Station : public Point
{

protected:

	//typedef double (Station::*getFct)();
	//typedef void (Station::*setFct)(double);

	/// Container for station-properties. Each property consists of a name, a get- and a set-function.
	/// Please refer to Station::addProperty for details.
	struct STNProperty
	{
		std::string name;
		double (*get)(void*);
		void (*set)(void*, double);
	};

public:
	/// Signals if the object is a "simple" Station or a Borehole (i.e. containing borehole-specific information).
	enum StationType
	{
		STATION  = 1,
		BOREHOLE = 2,
	};

	/// Constructor using a default colour
	/** constructor initialises the Station with the given coordinates */
	Station(double x = 0.0, double y = 0.0, double z = 0.0);

	/// Constructor using a specific colour
	Station(Color* const color);

	virtual ~Station();

	/**
	 * \brief Defines a property for this class.
	 *
	 * Variables in Station and its derived classes can be defined to be properties of this station.
	 * This definition consists of a name for the property as well as a function pointer to a method to
	 * read and to write this variable.
	 * Due to inheritance these function pointers only work correctly if the read and write functions are
	 * implemented as static functions, i.e. both the read and the write functin get a void-pointer to the
	 * actual station-object. This pointer is then casted to the correct class and the respective value can
	 * then be read or written dynamically from that object. It is highly recommended to define both the
	 * read and write function as protected because it does not actually make sense for these functions to be
	 * static except in the context of function pointers. Please refer to the examples below, i.e. the getX
	 * and setX methods.
	 * \param pname The name of the property.
	 * \param get A function pointer to a static read function for the variable referred to by pname
	 * \param set A function pointer to a static write function for the variable referred to by pname
	 * \return
	 */
	void addProperty(std::string pname, double (*get)(void*), void (*set)(void*, double));

	/// Sets colour for this station
	void setColor(unsigned char r, unsigned char g, unsigned char b);

	/// Sets colour for this station
	void setColor(const Color* color);

	/// returns the colour for this station
	Color* getColor () { return _color; }

	/// Returns a map containing all the properties of that station type.
	const std::map<std::string, double> getProperties();
	bool inSelection(const std::vector<PropertyBounds> &bounds);

	/// Returns true if all properties of this stations are within the boundaries given by \param bounds and false otherwise
	bool inSelection(std::map<std::string, double> properties) const;

	/// Returns the name of the station.
	std::string getName() const { return _name; }

	/// Returns the GeoSys-station-type for the station.
	int type() const { return _type; }

	/// Creates a Station-object from information contained in a string (assuming the string has the right format)
	static Station* createStation(const std::string &line);

	/// Creates a new station object based on the given parameters.
	static Station* createStation(const std::string &name, double x, double y, double z);


protected:
	/**
	 * \brief Returns the x-coordinate of this station. See the detailed documentation for getX() concerning the syntax.
	 *
	 * Returns the x-coordinate of this station.
	 * This function belongs to the property system of Station and return the value for property "x"
	 * (i.e. the x-coordinate of the station). It is implemented as a static method to avoid casting issues
	 * related to the function pointer associated with this function. Therefore, this function needs to be
	 * called "getX((void*)this);". It is highly recommended to define this function as protected because it
	 * does not actually make sense for these functions to be static except in the context of function pointers.
	 * \param stnObject A pointer to the station object for which the x-coordinate should be returned, usually (void*)this will work fine.
	 * \return The x-coordinate for this station.
	 */
	static double getX(void* stnObject) { Station* stn = (Station*)stnObject; return (*stn)[0]; }
	/// Returns the y-coordinate of this station. See the detailed documentation for getX concerning the syntax.
	static double getY(void* stnObject) { Station* stn = (Station*)stnObject; return (*stn)[1]; }
	/// Returns the z-coordinate of this station. See the detailed documentation for getX concerning the syntax.
	static double getZ(void* stnObject) { Station* stn = (Station*)stnObject; return (*stn)[2]; }
	/// Sets the x-coordinate for this station. See the detailed documentation for getX concerning the syntax.
	static void setX(void* stnObject, double val) { Station* stn = (Station*)stnObject; (*stn)[0]=val; }
	/// Sets the y-coordinate for this station. See the detailed documentation for getX concerning the syntax.
	static void setY(void* stnObject, double val) { Station* stn = (Station*)stnObject; (*stn)[1]=val; }
	/// Sets the z-coordinate for this station. See the detailed documentation for getX concerning the syntax.
	static void setZ(void* stnObject, double val) { Station* stn = (Station*)stnObject; (*stn)[2]=val; }


	std::string _name;
	StationType _type;	// GeoSys Station Type
	std::vector<STNProperty> _properties;


private:
	Color* _color;
};


/********* Boreholes *********/


/**
 * \brief A borehole as a geometric object. 
 *
 * A borehole inherits Station but has some additional information such as a date, a borehole profile, etc.
 */
class StationBorehole : public Station
{

public:
	/** constructor initialises the borehole with the given coordinates */
	StationBorehole(double x = 0.0, double y = 0.0, double z = 0.0);
	~StationBorehole(void);

	/// Creates a StationBorehole-object from a string (assuming the string has the right format)
	static StationBorehole* createStation(const std::string &line);

	/// Creates a new borehole object based on the given parameters.
	static StationBorehole* createStation(const std::string &name, double x, double y, double z, double depth, std::string date);

	/// Reads the stratigraphy for a specified station from a file
	static int addStratigraphy(const std::string &path, StationBorehole* borehole);

	/**
	 * \brief Reads all stratigraphy information from a file in one go.
	 *
	 * Reads all stratigraphy information from a file in one go.
	 * Be very careful when using this method -- it is pretty fast but it checks nothing and just
	 * assumes that everything is in right order and will work out fine!
	 */
	static int addStratigraphies(const std::string &path, std::vector<Point*> *boreholes);

	/// Finds the given string in the vector of soil-names
	int find(const std::string &str);

	// Returns the depth of the borehole
	double getDepth() const { return _depth; }

	/// Returns the date entry for the borehole
	double getDate() const { return _date; }

	///Returns a reference to a vector of Points representing the stratigraphy of the borehole (incl. the station-point itself)
	const std::vector<Point*> &getProfile() { return _profilePntVec; }

	/// Exports the borehole data to a file in GMS-format.
	int writeAsGMS(const std::string &filename);

	/// Add a soil layer to the boreholes stratigraphy
	void addSoilLayer ( double thickness, const std::string &soil_name);

	/// Add a soil layer to the boreholes stratigraphy 
	void addSoilLayer ( double x, double y, double z, const std::string &soil_name);

protected:
	/// Returns the depth of this borehole. Please see the documentation for Station::getX for details concerning the syntax.
	static double getDepth(void* stnObject)  { StationBorehole* stn = (StationBorehole*)stnObject; return stn->_depth; }
	/// Returns the date this borehole has been drilled. Please see the documentation for Station::getX for details concerning the syntax.
	static double getDate(void* stnObject)  { StationBorehole* stn = (StationBorehole*)stnObject; return stn->_date; }
	/// Sets the depth of this borehole. Please see the documentation for Station::getX for details concerning the syntax.
	static void setDepth(void* stnObject, double val) { StationBorehole* stn = (StationBorehole*)stnObject; stn->_depth = val; }
	/// Sets the date when this borehole has been drilled. Please see the documentation for Station::getX for details concerning the syntax.
	static void setDate(void* stnObject, double val) { StationBorehole* stn = (StationBorehole*)stnObject; stn->_date = val; }

private:
	/// Adds a layer for the specified borehole profile based on the information given in the stringlist
	static int addLayer(std::list<std::string> fields, StationBorehole* borehole);

	/// Creates fake stratigraphies of only one layer with a thickness equal to the borehole depth
	static void createSurrogateStratigraphies(std::vector<Point*> *boreholes);

	/// Reads the specified file containing borehole stratigraphies into an vector of stringlists
	static int readStratigraphyFile(const std::string &path, std::vector<std::list<std::string> > &data);

	//long profile_type;
	std::vector<long> _soilType;
	double _zCoord; // height at which the borehole officially begins (this might _not_ be the actual elevation)
	double _depth;	// depth of the borehole
	double _date;	// date when the borehole has been drilled

	/// Contains the names for all the soil layers
	std::vector<std::string> _soilName;

	/// Contains the points for the lower boundaries of all layers
	std::vector<Point*> _profilePntVec;


};

} // namespace

#endif // GEO_STATION_H
