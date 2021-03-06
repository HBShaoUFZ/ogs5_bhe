/**
 * \file GEOModels.cpp
 * 9/2/2010 LB Initial implementation
 *
 * Implementation of GEOModels
 */

// ** INCLUDES **
#include "GEOModels.h"

#include "GeoTreeModel.h"
#include "StationTreeModel.h"

#include "StringTools.h"

#include "OGSError.h"

GEOModels::GEOModels( QObject* parent /*= 0*/ ) :
	QObject (parent)
{
	_geoModel = new GeoTreeModel();
	_stationModel = new StationTreeModel();
}

GEOModels::~GEOModels()
{
	delete _stationModel;
	delete _geoModel;
}

void GEOModels::removeGeometry(std::string geo_name, GEOLIB::GEOTYPE type)
{
	if (type == GEOLIB::INVALID || type == GEOLIB::SURFACE)
		this->removeSurfaceVec(geo_name);
	if (type == GEOLIB::INVALID || type == GEOLIB::POLYLINE)
		this->removePolylineVec(geo_name);
	if (type == GEOLIB::INVALID || type == GEOLIB::POINT)
		this->removePointVec(geo_name);
}

void GEOModels::addPointVec( std::vector<GEOLIB::Point*>* points,
                             std::string &name,
                             std::map<std::string, size_t>* name_pnt_id_map,
                             double eps)
{
	GEOObjects::addPointVec(points, name, name_pnt_id_map, eps);
	_geoModel->addPointList(QString::fromStdString(name), this->getPointVecObj(name));
	emit geoDataAdded(_geoModel, name, GEOLIB::POINT);
}

bool GEOModels::appendPointVec(const std::vector<GEOLIB::Point*> &points,
                               const std::string &name, std::vector<size_t>* ids)
{
	bool ret (GEOLIB::GEOObjects::appendPointVec (points, name, ids));
	// TODO import new points into geo-treeview
	return ret;
}

bool GEOModels::removePointVec( const std::string &name )
{
	if (!isPntVecUsed(name))
	{
		emit geoDataRemoved(_geoModel, name, GEOLIB::POINT);
		this->_geoModel->removeGeoList(name, GEOLIB::POINT);
		return GEOObjects::removePointVec(name);
	}
	std::cout <<
	"GEOModels::removePointVec() - There are still Polylines or Surfaces depending on these points."
	          << std::endl;
	return false;
}

void GEOModels::addStationVec( std::vector<GEOLIB::Point*>* stations,
                               std::string &name)
{
	GEOObjects::addStationVec(stations, name);

	_stationModel->addStationList(QString::fromStdString(name), stations);
	emit stationVectorAdded(_stationModel, name);
}

void GEOModels::filterStationVec(const std::string &name, const std::vector<PropertyBounds> &bounds)
{
	emit stationVectorRemoved(_stationModel, name);
	const std::vector<GEOLIB::Point*>* stations (GEOObjects::getStationVec(name));
	_stationModel->filterStations(name, stations, bounds);
	emit stationVectorAdded(_stationModel, name);
}

bool GEOModels::removeStationVec( const std::string &name )
{
	emit stationVectorRemoved(_stationModel, name);
	_stationModel->removeStationList(name);
	return GEOObjects::removeStationVec(name);
}

void GEOModels::addPolylineVec( std::vector<GEOLIB::Polyline*>* lines,
                                const std::string &name,
                                std::map<std::string,size_t>* ply_names )
{
	GEOObjects::addPolylineVec(lines, name, ply_names);
	if (lines->empty())
		return;

	_geoModel->addPolylineList(QString::fromStdString(name), this->getPolylineVecObj(name));
	emit geoDataAdded(_geoModel, name, GEOLIB::POLYLINE);
}

bool GEOModels::appendPolylineVec(const std::vector<GEOLIB::Polyline*> &polylines,
                                  const std::string &name)
{
	bool ret (GEOLIB::GEOObjects::appendPolylineVec (polylines, name));

	this->_geoModel->appendPolylines(name, this->getPolylineVecObj(name));
	return ret;
}

bool GEOModels::removePolylineVec( const std::string &name )
{
	emit geoDataRemoved(_geoModel, name, GEOLIB::POLYLINE);
	this->_geoModel->removeGeoList(name, GEOLIB::POLYLINE);
	return GEOObjects::removePolylineVec (name);
}

void GEOModels::addSurfaceVec( std::vector<GEOLIB::Surface*>* surfaces,
                               const std::string &name,
                               std::map<std::string,size_t>* sfc_names )
{
	GEOObjects::addSurfaceVec(surfaces, name, sfc_names);
	if (surfaces->empty())
		return;

	_geoModel->addSurfaceList(QString::fromStdString(name), this->getSurfaceVecObj(name));
	emit geoDataAdded(_geoModel, name, GEOLIB::SURFACE);
}

bool GEOModels::appendSurfaceVec(const std::vector<GEOLIB::Surface*> &surfaces,
                                 const std::string &name)
{
	bool ret (GEOLIB::GEOObjects::appendSurfaceVec (surfaces, name));

	if (ret)
		this->_geoModel->appendSurfaces(name, this->getSurfaceVecObj(name));
	else
	{
		std::vector<GEOLIB::Surface*>* sfc = new std::vector<GEOLIB::Surface*>;
		for (size_t i = 0; i < surfaces.size(); i++)
			sfc->push_back(surfaces[i]);
		this->addSurfaceVec(sfc, name);
	}

	return ret;
}

bool GEOModels::removeSurfaceVec( const std::string &name )
{
	emit geoDataRemoved(_geoModel, name, GEOLIB::SURFACE);
	this->_geoModel->removeGeoList(name, GEOLIB::SURFACE);
	return GEOObjects::removeSurfaceVec (name);
}

void GEOModels::connectPolylineSegments(const std::string &geoName,
                                        std::vector<size_t> indexlist,
                                        double proximity,
                                        std::string ply_name,
                                        bool closePly,
                                        bool triangulatePly)
{
	GEOLIB::PolylineVec* plyVec = this->getPolylineVecObj(geoName);

	if (plyVec)
	{
		const std::vector<GEOLIB::Polyline*>* polylines = plyVec->getVector();
		std::vector<GEOLIB::Polyline*> ply_list;
		for (size_t i = 0; i < indexlist.size(); i++)
			ply_list.push_back( (*polylines)[indexlist[i]] );

		// connect polylines
		GEOLIB::Polyline* new_line = GEOLIB::Polyline::constructPolylineFromSegments(
		        ply_list,
		        proximity);

		if (new_line)
		{
			// insert result in a new vector of polylines (because the GEOObjects::appendPolylines()-method requires a vector)
			std::vector<GEOLIB::Polyline*> connected_ply;

			if (closePly)
			{
				new_line = GEOLIB::Polyline::closePolyline(*new_line);

				if (triangulatePly)
				{
					std::vector<GEOLIB::Surface*> new_sfc;
					new_sfc.push_back(GEOLIB::Surface::createSurface(*new_line));
					this->appendSurfaceVec(new_sfc, geoName);
				}
			}

			connected_ply.push_back(new_line);
			if (!ply_name.empty())
				plyVec->setNameOfElementByID(polylines->size(), ply_name);
			this->appendPolylineVec(connected_ply, geoName);
		}
		else
			OGSError::box("Error connecting polyines.");
	}
	else
		OGSError::box("Corresponding geometry not found.");
}


void GEOModels::addNameForElement(const std::string &geometry_name, const GEOLIB::GEOTYPE object_type, size_t id, std::string new_name)
{
	if (object_type == GEOLIB::POINT)
		this->getPointVecObj(geometry_name)->setNameForElement(id, new_name);
	else if (object_type == GEOLIB::POLYLINE)
		this->getPolylineVecObj(geometry_name)->setNameForElement(id, new_name);
	else if (object_type == GEOLIB::SURFACE)
		this->getSurfaceVecObj(geometry_name)->setNameForElement(id, new_name);
	else
		std::cout << "Error in GEOModels::addNameForElement() - Unknown GEOTYPE..." << std::endl;
}

void GEOModels::addNameForObjectPoints(const std::string &geometry_name, const GEOLIB::GEOTYPE object_type, const std::string &geo_object_name, const std::string &new_name)
{
	const GEOLIB::GeoObject* obj = this->getGEOObject(geometry_name, object_type, geo_object_name);
	GEOLIB::PointVec* pnt_vec = this->getPointVecObj(geometry_name);
	if (object_type == GEOLIB::POLYLINE)
	{
		const GEOLIB::Polyline* ply = dynamic_cast<const GEOLIB::Polyline*>(obj);
		size_t nPoints = ply->getNumberOfPoints();
		for (size_t i=0; i<nPoints; i++)
			pnt_vec->setNameForElement(ply->getPointID(i), new_name + "_Point" + number2str(ply->getPointID(i)));
	}
	else if (object_type == GEOLIB::SURFACE)
	{
		const GEOLIB::Surface* sfc = dynamic_cast<const GEOLIB::Surface*>(obj);
		size_t nTriangles = sfc->getNTriangles();
		for (size_t i=0; i<nTriangles; i++)
		{
			const GEOLIB::Triangle* tri = (*sfc)[i];
			pnt_vec->setNameForElement((*tri)[0], new_name + "_Point" + number2str((*tri)[0]));
			pnt_vec->setNameForElement((*tri)[1], new_name + "_Point" + number2str((*tri)[1]));
			pnt_vec->setNameForElement((*tri)[2], new_name + "_Point" + number2str((*tri)[2]));
		}
	}
	else
		std::cout << "Error in GEOModels::addNameForElement() - Unknown GEOTYPE..." << std::endl;
}

