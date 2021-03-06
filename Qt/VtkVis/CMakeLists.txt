# Source files
SET( SOURCES
	MeshFromRasterDialog.cpp
	QVtkDataSetMapper.cpp
	VisPrefsDialog.cpp
	VisualizationWidget.cpp
	VtkAddFilterDialog.cpp
	VtkAlgorithmPropertyCheckbox.cpp
	VtkAlgorithmPropertyLineEdit.cpp
	VtkAlgorithmPropertyVectorEdit.cpp
	VtkBGImageSource.cpp
	VtkColorByHeightFilter.cpp
	VtkColorLookupTable.cpp
	VtkCompositeFilter.cpp
	VtkCompositeColorByHeightFilter.cpp
	VtkCompositeColormapToImageFilter.cpp
	VtkCompositeContourFilter.cpp
	VtkCompositeGeoObjectFilter.cpp
	VtkCompositeImageToCylindersFilter.cpp
	VtkCompositeLineToTubeFilter.cpp
	VtkCompositePointToGlyphFilter.cpp
	VtkCompositeSelectionFilter.cpp	
	VtkCompositeTextureOnSurfaceFilter.cpp
	VtkCompositeThresholdFilter.cpp
	VtkConditionSource.cpp
	VtkFilterFactory.cpp
	VtkGeoImageSource.cpp
	VtkImageDataToLinePolyDataFilter.cpp
	VtkMeshConverter.cpp
	VtkMeshSource.cpp
	VtkPolylinesSource.cpp
	VtkPointsSource.cpp
	VtkRaster.cpp
	VtkSelectionFilter.cpp
	VtkStationSource.cpp
	VtkSurfacesSource.cpp
	VtkTextureOnSurfaceFilter.cpp
	VtkTrackedCamera.cpp
	VtkVisHelper.cpp
	VtkVisImageItem.cpp
	VtkVisPipeline.cpp
	VtkVisPipelineItem.cpp
	VtkVisPipelineView.cpp
	VtkVisPointSetItem.cpp
	VtkVisTabWidget.cpp
	VtkAlgorithmProperties.cpp
)

# Moc headers
SET( MOC_HEADERS
	MeshFromRasterDialog.h
	QVtkDataSetMapper.h
	VisPrefsDialog.h
	VisualizationWidget.h
	VtkAddFilterDialog.h
	VtkAlgorithmProperties.h
	VtkAlgorithmPropertyLineEdit.h
	VtkAlgorithmPropertyCheckbox.h
	VtkAlgorithmPropertyVectorEdit.h
	VtkTrackedCamera.h
	VtkVisPipeline.h
	VtkVisPipelineView.h
	VtkVisTabWidget.h
)

# Header files
SET( HEADERS
	VtkBGImageSource.h
	VtkColorByHeightFilter.h
	VtkColorLookupTable.h
	VtkCompositeFilter.h
	VtkCompositeColorByHeightFilter.h
	VtkCompositeColormapToImageFilter.h
	VtkCompositeContourFilter.h
	VtkCompositeGeoObjectFilter.h
	VtkCompositeImageToCylindersFilter.h
	VtkCompositeLineToTubeFilter.h
	VtkCompositePointToGlyphFilter.h
	VtkCompositeSelectionFilter.h
	VtkCompositeTextureOnSurfaceFilter.h
	VtkCompositeThresholdFilter.h
	VtkConditionSource.h
	VtkFilterFactory.h
	VtkGeoImageSource.h
	VtkImageDataToLinePolyDataFilter.h
	VtkMeshConverter.h
	VtkMeshSource.h
	VtkPolylinesSource.h
	VtkPointsSource.h
	VtkRaster.h
	VtkSelectionFilter.h
	VtkStationSource.h
	VtkSurfacesSource.h
	VtkTextureOnSurfaceFilter.h
	VtkVisHelper.h
	VtkVisImageItem.h
	VtkVisPipelineItem.h
	VtkVisPointSetItem.h
)

# UI files
SET ( UIS
	MeshFromRaster.ui
	VtkAddFilterDialogBase.ui
	VtkVisTabWidgetBase.ui
	VisualizationWidgetBase.ui
	VisPrefs.ui
)

# Put moc files in a project folder
SOURCE_GROUP("UI Files" REGULAR_EXPRESSION "\\w*\\.ui")
SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION "moc_.*")

# Put filter in a folder
SOURCE_GROUP("Filter Header Files" REGULAR_EXPRESSION "[.]*Filter.h")
SOURCE_GROUP("Filter Source Files" REGULAR_EXPRESSION "[.]*Filter.cpp")

# Put vtk source files in a folder
SOURCE_GROUP("Source Header Files" REGULAR_EXPRESSION "[.]*Source.h")
SOURCE_GROUP("Source Source Files" REGULAR_EXPRESSION "[.]*Source.cpp")

# Run Qts user interface compiler uic on .ui files
QT4_WRAP_UI( UI_HEADERS ${UIS} )

# Run Qts meta object compiler moc on header files
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )

INCLUDE( ${VTK_USE_FILE} )

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/Qt/VtkVis
	${CMAKE_SOURCE_DIR}/Qt/VtkAct
	${CMAKE_BINARY_DIR}/Qt/VtkVis
	${CMAKE_SOURCE_DIR}/Base
	${CMAKE_SOURCE_DIR}/FEM
	${CMAKE_SOURCE_DIR}/GEO
	${CMAKE_SOURCE_DIR}/MathLib
	${CMAKE_SOURCE_DIR}/FileIO
	${CMAKE_SOURCE_DIR}/MSH
	${CMAKE_SOURCE_DIR}/Qt/Base
	${CMAKE_SOURCE_DIR}/Qt/DataView
	${CMAKE_BINARY_DIR}/Qt/DataView
)

# Create the library
ADD_LIBRARY( VtkVis STATIC
	${SOURCES}
	${HEADERS}
	${MOC_HEADERS}
	${MOC_SOURCES}
	${UIS}
)

IF (libgeotiff_FOUND)
	INCLUDE_DIRECTORIES( ${libgeotiff_INCLUDE_DIR} ${libgeotiff_INCLUDE_DIR}/libxtiff ${VTK_DIR}/../Utilities/vtktiff )
	TARGET_LINK_LIBRARIES( VtkVis ${libgeotiff_LIBRARIES} )
ENDIF () # libgeotiff_FOUND

TARGET_LINK_LIBRARIES( VtkVis ${QT_LIBRARIES} )

ADD_DEPENDENCIES(VtkVis QtDataView)

IF(OGS_USE_VRPN)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/Qt/Vrpn)
ENDIF() # OGS_USE_VRPN

IF (OGS_USE_OPENSG)
	USE_OPENSG(VtkVis)
	INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Qt/OpenSG )
ENDIF (OGS_USE_OPENSG)
