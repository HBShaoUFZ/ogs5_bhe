/**
 * \file GridAdapter.cpp
 * 24/03/2010 KR Initial implementation
 *
 */


#include "GridAdapter.h"

#include <iostream>
#include <list>
#include <cstdlib>
#include "StringTools.h"


GridAdapter::GridAdapter(const Mesh_Group::CFEMesh* mesh) :
	_nodes(new std::vector<GEOLIB::Point*>), _elems(new std::vector<Element*>)
{
	if (mesh) convertCFEMesh(mesh);
}


GridAdapter::GridAdapter(const std::string &filename) :
	_nodes(new std::vector<GEOLIB::Point*>), _elems(new std::vector<Element*>)
{
	readMeshFromFile(filename);
}


int GridAdapter::convertCFEMesh(const Mesh_Group::CFEMesh* mesh)
{
	Element* newElem;
	size_t nElemNodes = 0;

	size_t nElems = mesh->ele_vector.size();
	size_t nNodes = mesh->nod_vector.size();

	for (size_t i=0; i<nNodes; i++)
	{
		GEOLIB::Point* pnt = new GEOLIB::Point(mesh->nod_vector[i]->X(), mesh->nod_vector[i]->Y(), mesh->nod_vector[i]->Z());
		_nodes->push_back(pnt);
	}

	for (size_t i=0; i<nElems; i++)
	{
		newElem = new Element();

		int type = static_cast<int>(mesh->ele_vector[i]->GetElementType());
		newElem->type = getElementType(type);

		newElem->material = mesh->ele_vector[i]->GetPatchIndex();

		if (newElem->type != ERROR)
		{
			std::vector<long> elemNodes;
			nElemNodes = mesh->ele_vector[i]->nodes_index.Size();
			for (size_t j=0; j<nElemNodes; j++)
				newElem->nodes.push_back(mesh->ele_vector[i]->GetNodeIndex(j));

			_elems->push_back(newElem);
		}
		else
			std::cout << "GridAdapter::convertCFEMesh() - Error recognising element type..." << std::endl;
	}
	
	return 1;
}

int GridAdapter::readMeshFromFile(const std::string &filename)
{
	size_t cid = 0;
	std::string line;
	std::list<std::string>::const_iterator it;

	std::ifstream in( filename.c_str() );

	if (!in.is_open())
	{
		std::cout << "GridAdapter::readMeshFromFile() - Could not open file..."  << std::endl;
		return 0;
	}

	// try to find the start of the nodes list
	while ( getline(in, line) )
	{
		trim(line);
		if (line.compare("$NODES") == 0) break;
	}

	// read number of nodes
	getline(in, line);
	trim(line);
	size_t nNodes = atoi(line.c_str());

	// read all nodes
	while ( getline(in, line) )
	{
		trim(line);
		if (line.compare("$ELEMENTS") == 0) break;

		list<std::string> fields = splitString(line, ' ');

		if (fields.size() >= 4) 
		{
			it = fields.begin();

			if (atoi(it->c_str()) == _nodes->size())
			{
				GEOLIB::Point* pnt = new GEOLIB::Point();

				(*pnt)[0] = strtod((++it)->c_str(), 0);
				(*pnt)[1] = strtod((++it)->c_str(), 0);
				(*pnt)[2] = strtod((++it)->c_str(), 0);

				_nodes->push_back(pnt);
			}
			else
				std::cout << "GridAdapter::readMeshFromFile() - Index error while reading nodes..." << std::endl;
		}
		else
			std::cout << "GridAdapter::readMeshFromFile() - Error reading node format..." << std::endl;
	}

	if (line.compare("$ELEMENTS") == 0)
	{
		Element* newElem;

		// read number of elements
		getline(in, line);
		trim(line);
		int nElems = atoi(line.c_str());
		cid = 0;

		// read all elements
		while ( getline(in, line) )
		{
			trim(line);
			if (line.compare("$LAYER") == 0) break;

			list<std::string> fields = splitString(line, ' ');

			if (fields.size() >= 6) {

				it = fields.begin();
				if (atoi(it->c_str()) == _elems->size())
				{
					newElem = new Element();

					if ((++it)->empty()) it++;
					newElem->material = atoi(it->c_str());	// material group
					if ((++it)->empty()) it++;
					newElem->type = getElementType(*it);	// element type

					if (newElem->type != ERROR)
					{
						while ((++it) != fields.end())
						{
							if (it->empty()) continue;
							newElem->nodes.push_back(atoi(it->c_str()));	// next node id
						}

						_elems->push_back(newElem);
					}
					else
						std::cout << "GridAdapter::readMeshFromFile() - Error recognising element type..." << std::endl;
				}
				else
					std::cout << "GridAdapter::readMeshFromFile() - Index error while reading elements..." << std::endl;
			}
			else
				std::cout << "GridAdapter::readMeshFromFile() - Error reading element format..." << std::endl;
		}
	}
	else
		std::cout << "GridAdapter::readMeshFromFile() - Index error after reading nodes..." << std::endl;

	in.close();


	return 1;
}

GridAdapter::MeshType GridAdapter::getElementType(const std::string &t)
{
	if (t.compare("tri") == 0)  return TRIANGLE;
	if (t.compare("line") == 0)  return LINE;
	if (t.compare("quad") == 0) return QUAD;
	if (t.compare("tet") == 0)  return TETRAEDER;
	if (t.compare("hex") == 0)  return HEXAHEDRON;
	if (t.compare("pri") == 0)  return PRISM;
	else return ERROR;
}

GridAdapter::MeshType GridAdapter::getElementType(int type)
{
	if (type == TRIANGLE)  return TRIANGLE;
	if (type == LINE)  return LINE;
	if (type == QUAD) return QUAD;
	if (type == TETRAEDER)  return TETRAEDER;
	if (type == HEXAHEDRON)  return HEXAHEDRON;
	if (type == PRISM)  return PRISM;
	else return ERROR;
}

const size_t GridAdapter::getNumberOfMaterials() const
{
	size_t nElems = _elems->size();
	size_t maxMaterialID = 0;

	for (size_t i=0; i<nElems; i++)
	{
		if ((*_elems)[i]->material > maxMaterialID) maxMaterialID = (*_elems)[i]->material;
	}
	return maxMaterialID;
}

const std::vector<GridAdapter::Element*> *GridAdapter::getElements(size_t matID) const
{
	std::vector<GridAdapter::Element*> *matElems = new std::vector<GridAdapter::Element*>;
	size_t nElements = _elems->size();
	for (size_t i=0; i<nElements; i++)
	{
		if ((*_elems)[i]->material == matID)
			matElems->push_back((*_elems)[i]);
	}

	return matElems;
}