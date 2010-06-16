/**
 * \file VtkSurfacesSource.h
 * 3/2/2010 LB Initial implementation
 * 23/04/2010 KR Surface visualisation
 *
 */


#ifndef VTKSURFACESSOURCE_H
#define VTKSURFACESSOURCE_H

// ** INCLUDES **
#include <vtkPolyDataAlgorithm.h>
#include "VtkAlgorithmProperties.h"

#include "Surface.h"

/**
 * \brief VTK source object for the visualisation of surfaces.
 * Technically, surfaces are displayed as triangulated polydata objects.
 */
class VtkSurfacesSource : public vtkPolyDataAlgorithm, public VtkAlgorithmProperties
{

public:
	/// Create new objects with New() because of VTKs object reference counting.
	static VtkSurfacesSource* New();

	vtkTypeRevisionMacro(VtkSurfacesSource,vtkPolyDataAlgorithm);

	/// Sets the surfaces vector
	void setSurfaces(std::vector<GEOLIB::Surface*> *surfaces) { _surfaces = surfaces; };

	/// Prints its data on a stream.
	void PrintSelf(ostream& os, vtkIndent indent);

protected:
	VtkSurfacesSource();
	~VtkSurfacesSource() {};

	/// Computes the polygonal data object.
	int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

	int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

	/// The surfaces to visualize
	std::vector<GEOLIB::Surface*> *_surfaces;

private:

};

#endif // VTKSURFACESSOURCE_H
