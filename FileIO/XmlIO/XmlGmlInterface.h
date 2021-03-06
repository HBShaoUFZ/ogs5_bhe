/**
 * \file XmlGmlInterface.h
 * 2011/11/23 KR as derived class from XMLInterface
 */

#ifndef XMLGMLINTERFACE_H
#define XMLGMLINTERFACE_H

#include "XMLInterface.h"

namespace FileIO
{

/**
 * \brief Reads and writes GeoObjects to and from XML files.
 */
class XmlGmlInterface : public XMLInterface
{
public:
	/**
	 * Constructor
	 * \param project Project data.
	 * \param schemaFile An XML schema file (*.xsd) that defines the structure of a valid data file.
	 */
	XmlGmlInterface(ProjectData* project, const std::string &schemaFile);

	virtual ~XmlGmlInterface() {};

	/// Reads an xml-file containing geometric object definitions into the GEOObjects used in the contructor
	int readFile(const QString &fileName);

protected:
	int write(std::ostream& stream);

private:
	/// Reads GEOLIB::Point-objects from an xml-file
	void readPoints    ( const QDomNode &pointsRoot,
	                     std::vector<GEOLIB::Point*>* points,
	                     std::map<std::string, size_t>* pnt_names );

	/// Reads GEOLIB::Polyline-objects from an xml-file
	void readPolylines ( const QDomNode &polylinesRoot,
	                     std::vector<GEOLIB::Polyline*>* polylines,
	                     std::vector<GEOLIB::Point*>* points,
	                     const std::vector<size_t> &pnt_id_map,
	                     std::map<std::string, size_t>* ply_names );

	/// Reads GEOLIB::Surface-objects from an xml-file
	void readSurfaces  ( const QDomNode &surfacesRoot,
	                     std::vector<GEOLIB::Surface*>* surfaces,
	                     std::vector<GEOLIB::Point*>* points,
	                     const std::vector<size_t> &pnt_id_map,
	                     std::map<std::string, size_t>* sfc_names );

};

}

#endif // XMLGMLINTERFACE_H
