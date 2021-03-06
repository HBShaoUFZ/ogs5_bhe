/**
 * \file VtkSurfacesSource.cpp
 * 3/2/2010 LB Initial implementation
 * 23/04/2010 KR Surface visualisation
 *
 * Implementation of VtkSurfacesSource
 */

// ** INCLUDES **
#include "Color.h"
#include "VtkSurfacesSource.h"
#include <limits>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkProperty.h>

vtkStandardNewMacro(VtkSurfacesSource);
vtkCxxRevisionMacro(VtkSurfacesSource, "$Revision$");

VtkSurfacesSource::VtkSurfacesSource()
	: _surfaces(NULL)
{
	_removable = false; // From VtkAlgorithmProperties
	this->SetNumberOfInputPorts(0);
	//this->SetColorBySurface(true);

	const GEOLIB::Color* c = GEOLIB::getRandomColor();
	vtkProperty* vtkProps = GetProperties();
	vtkProps->SetColor((*c)[0] / 255.0,(*c)[1] / 255.0,(*c)[2] / 255.0);
	vtkProps->SetEdgeVisibility(0);
}

void VtkSurfacesSource::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf(os,indent);

	if (_surfaces->size() == 0)
		return;

	os << indent << "== VtkSurfacesSource ==" << "\n";
}

int VtkSurfacesSource::RequestData( vtkInformation* request,
                                    vtkInformationVector** inputVector,
                                    vtkInformationVector* outputVector )
{
	(void)request;
	(void)inputVector;

	const int nSurfaces = _surfaces->size();
	if (nSurfaces == 0)
		return 0;

	const std::vector<GEOLIB::Point*>* surfacePoints = (*_surfaces)[0]->getPointVec();
	size_t nPoints = surfacePoints->size();

	vtkSmartPointer<vtkInformation> outInfo = outputVector->GetInformationObject(0);
	vtkSmartPointer<vtkPolyData> output =
	        vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	if (outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) > 0)
		return 1;

	vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
	newPoints->Allocate(nPoints);

	vtkSmartPointer<vtkCellArray> newPolygons = vtkSmartPointer<vtkCellArray>::New();
	//newPolygons->Allocate(nSurfaces);

	vtkSmartPointer<vtkIntArray> sfcIDs = vtkSmartPointer<vtkIntArray>::New();
	sfcIDs->SetNumberOfComponents(1);
	sfcIDs->SetName("SurfaceIDs");

	for (size_t i = 0; i < nPoints; i++)
	{
		double* coords = const_cast<double*>((*surfacePoints)[i]->getData());
		newPoints->InsertNextPoint(coords);
	}

	vtkIdType count(0);
	for (std::vector<GEOLIB::Surface*>::const_iterator it = _surfaces->begin();
	     it != _surfaces->end(); ++it)
	{
		const size_t nTriangles = (*it)->getNTriangles();

		for (size_t i = 0; i < nTriangles; i++)
		{
			vtkPolygon* aPolygon = vtkPolygon::New();
			aPolygon->GetPointIds()->SetNumberOfIds(3);

			const GEOLIB::Triangle* triangle = (**it)[i];
			for (size_t j = 0; j < 3; j++)
				aPolygon->GetPointIds()->SetId(j, ((*triangle)[2 - j]));
			newPolygons->InsertNextCell(aPolygon);
			sfcIDs->InsertNextValue(count);
			aPolygon->Delete();
		}
		count++;
	}

	output->SetPoints(newPoints);
	output->SetPolys(newPolygons);
	output->GetCellData()->AddArray(sfcIDs);
	output->GetCellData()->SetActiveAttribute("SurfaceIDs", vtkDataSetAttributes::SCALARS);

	return 1;
}

int VtkSurfacesSource::RequestInformation( vtkInformation* request,
                                           vtkInformationVector** inputVector,
                                           vtkInformationVector* outputVector )
{
	(void)request;
	(void)inputVector;

	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(), -1);

	return 1;
}

void VtkSurfacesSource::SetUserProperty( QString name, QVariant value )
{
	VtkAlgorithmProperties::SetUserProperty(name, value);
	(*_algorithmUserProperties)[name] = value;
}

