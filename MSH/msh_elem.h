 /**************************************************************************
MSHLib - Object: 
Task: 
Programing:
08/2005 WW/OK Encapsulation from rf_ele_msh
last modified
**************************************************************************/
#ifndef msh_elem_INC
#define msh_elem_INC
// C++
#include <string>
#include<iostream>
using namespace std;
// MSHLib
#include "msh_edge.h"
#ifdef USE_TOKENBUF
#include "tokenbuf.h"
#endif
// PCSLib
namespace process{class CRFProcessDeformation;}
namespace Math_Group{class Matrix;}

class CRFProcess;
//------------------------------------------------------------------------
namespace Mesh_Group
{
 // Process
 using process::CRFProcessDeformation;
 using ::CRFProcess;
 using Math_Group::Matrix;
//------------------------------------------------------------------------
// Class definition
class CElem:public CCore
{
   private: // Members
      // ID
      int geo_type; // 1 Line, 2 Quad, 3 Hex, 4 Tri, 5 Tet, 6 Pris 
      CElem* owner;
      // Geometrical properties
      int ele_dim; // Dimension of element
      int nnodes;
      int nnodesHQ;
      //
      vec<CNode*> nodes;
      int nedges;
      vec<CEdge*> edges;
      vec<int> edges_orientation;
      int nfaces;
      int no_faces_on_surface;
      int face_index; // Local face index for the instance for face
      double volume;
      double gravity_center[3];
      int grid_adaptation;  // Flag for grid adapting.
      int patch_index;
      /*
      // Since m_tim->CheckCourant() is deactivated, the following member are 
      // put in comment.
      double representative_length;//For stability calculations
      double courant;
      double neumann;	  // MSH topology
      */
      double area;//Flux area
      //
	  // MSH topology
      Matrix* tranform_tensor;
      vec<CElem*> neighbors;
      //vec<CElem*> sons;
	  // double angle[3];	// PCH, angle[0] rotation along y axis
						//	    angle[1] rotation along x' axis
						//		angle[2] translation along z'' axis.		
      double *angle; // Dymanic allocate memory.  WW	
	 //WW double MatT[9];

      #ifdef RFW_FRACTURE
     //public:
     //for now I will store element specific variables here, until I find something better to do with it
     //If the aperture of the element is not stored, the CalculateFracAperture and PermeabilityFracAperture
     //will have to be called many more times during each timestep.
     bool Aperture_is_set;
     bool Permeability_is_set;
     double Aperture;
     double Permeability;
     bool in_frac;  //shows whether an element is in a fracture, this is slightly redundant given the next variable
     long frac_number;   //shows which fracture a frac element belongs to
     double f_dx;
     double f_dy; //related to fracture segment orientation
     double f_weight; 
     #endif
      // 
	  // -- Methods
      int GetElementFaces1D(int *FaceNode);
      int GetElementFacesTri(const int Face, int *FaceNode);
      int GetElementFacesQuad(const int Face, int *FaceNode);
      int GetElementFacesHex(const int Face, int *FaceNode);
      int GetElementFacesTet(const int Face, int *FaceNode);
      int GetElementFacesPri(const int Face, int *FaceNode);
      //-- Friends
      friend class CFEMesh;
      // FEM
      friend class FiniteElement::CElement;  
      friend class FiniteElement::CFiniteElementStd; 
      friend class FiniteElement::CFiniteElementVec; 
      friend class FiniteElement::ElementMatrix; 
      friend class FiniteElement::ElementMatrix_DM;
      // PCS
      friend class process::CRFProcessDeformation;
      friend class ::CRFProcess;
  public: // Methods
      CElem();
      CElem(const long Index);
      CElem( const long Index, CElem* onwer, const int Face); // For Faces: Face, local face index
      CElem(const long Index, CElem* m_ele_parent); //WWOK
      ~CElem();
      //------------------------------------------------------------------
      // Geometry
      int GetDimension() const {return ele_dim;}
      double* GetGravityCenter() {return gravity_center;}
      int GetPatchIndex() const {return patch_index;} //MatGroup
      void SetPatchIndex(const int value) {patch_index = value;}
      void ComputeVolume();
      void SetFluxArea(double fluxarea) {area = fluxarea;}//CMCD for <3D elements with varying area
      double GetFluxArea() {return area;}//CMCD for <3D elements with varying area
	  double GetVolume() const {return volume;}
      void SetVolume(const double Vol) {volume = Vol;}
// This will be activated after m_tim->CheckCourant() is ready to work
//      void SetCourant(double Cour) {courant = Cour;}//CMCD
//      double GetCourant() {return courant;}//CMCD
//      void SetNeumann(double Neum) {neumann = Neum;}//CMCD
//      double GetNeumann() {return neumann;}//CMCD
//      double GetRepLength() {return representative_length;}//CMCD
      //------------------------------------------------------------------
      // ID
      int GetElementType() const {return geo_type;}
      void SetElementType(const int Type) {geo_type=Type;}
      void MarkingAll(bool makop); 
      string GetName() const;
      //------------------------------------------------------------------
      // Nodes
      vec<long> nodes_index;      
      void GetNodeIndeces(vec<long>&  node_index) const 
	    {for (int i=0; i< (int) nodes_index.Size();i++)
           node_index[i]= nodes_index[i];} 
      long GetNodeIndex(const int index) const  {return  nodes_index[index];} 
      void SetNodeIndex(const int index, const long g_index) {nodes_index[index]= g_index;} 
      void GetNodes(vec<CNode*>&ele_nodes)
        {
#ifdef RFW_FRACTURE
        ele_nodes.resize((int)nodes.Size());
#endif
        for (int i=0; i< (int) nodes.Size();i++) ele_nodes[i]= nodes[i];}
        
      CNode* GetNode(const int index) { return nodes[index]; }
      void SetNodes(vec<CNode*>&  ele_nodes, const bool ReSize=false);
	  int GetNodesNumber_H() const  {return nnodesHQ;}
	  int GetNodesNumber(bool quad) const  {if(quad) return nnodesHQ; else return nnodes;}
	  int GetVertexNumber() const  {return nnodes;}
      void SetNodesNumber(int ivalue) {nnodes = ivalue;} //OK
      CElem* GetOwner() { return owner; }  //YD
      //------------------------------------------------------------------
      // Edges
      void GetEdges(vec<CEdge*>&  ele_edges) 
        {for (int i=0; i<nedges; i++) ele_edges[i]= edges[i];} 
      CEdge* GetEdge(const int index) 
        {return edges[index];} 
      void SetEdges(vec<CEdge*>&  ele_edges) 
        {for (int i=0; i<nedges; i++) edges[i]= ele_edges[i];} 
      int FindFaceEdges(const int LocalFaceIndex, vec<CEdge*>&  face_edges);
      void SetEdgesOrientation(vec<int>&  ori_edg) 
         {for (int i=0; i<nedges; i++) edges_orientation[i]= ori_edg[i];} 
      void GetLocalIndicesOfEdgeNodes(const int Edge, int *EdgeNodes);
      int GetEdgesNumber() const{return nedges;}
      //------------------------------------------------------------------
      // Faces
      int GetFacesNumber() const {return nfaces;}
      void SetFace();
      void SetFace(CElem* onwer, const int Face);
	  int GetSurfaceFacesNumber() const {return no_faces_on_surface;} 
	  int GetLocalFaceIndex() const {return face_index;} 
      int GetFaceType();
      int GetElementFaceNodes(const int Face, int *FacesNode);
      //------------------------------------------------------------------
      // Neighbors
      void SetNeighbors(vec<CElem*>&  ele_neighbors) 
         { for (int i=0; i< nfaces;i++) neighbors[i] = ele_neighbors[i];}
      void SetNeighbor(const int LocalIndex, CElem* ele_neighbor) 
         { neighbors[LocalIndex] = ele_neighbor;}
      void GetNeighbors(vec<CElem*>&  ele_neighbors)  
         {for (int i=0; i< nfaces;i++) ele_neighbors[i]= neighbors[i];} 
      CElem* GetNeighbor(const int index) { return neighbors[index];} 

      //------------------------------------------------------------------
      // Coordinates transform
      void FillTransformMatrix();  
	  void FillTransformMatrix(int noneed);
	  double getTransformTensor(const int idx);
	  void AllocateMeomoryforAngle() {if (!angle) angle = new double [3];}	// WW
	  double GetAngle(const int i) const { return angle[i]; }	// PCH
	  void SetAngle(const int i, const double value) 
	  { 
		angle[i] = value; 
	  }	// PCH
      //------------------------------------------------------------------
      // I/O
      void Read(istream& is=cin, int fileType=0);
#ifdef USE_TOKENBUF
      void Read(TokenBuf* tokenbuf, int fileType=0);
#endif
      void WriteIndex(ostream& os=cout) const;
      void WriteIndex_TEC(ostream& os=cout) const;
      void WriteAll(ostream& os=cout) const;
      void WriteNeighbors(ostream& os=cout) const;
      void Config(); //OK
      //------------------------------------------------------------------
      // MAT
      Vec mat_vector;  //OKWW
      int matgroup_view;//TK
      //------------------------------------------------------------------
      // Operator
      // virtual void operator = (const CElem& elem);
      //-------------------------------------------------------------------
      //GUI control variables 
       int selected;
       void FaceNormal(const int index0, const int index1, double*);   //YD
       double *normal_vector; //WW normal_vector[3]; //OK
       void SetNormalVector(); //OK
      //
      #ifdef RFW_FRACTURE
      bool ApertureIsSet() {return Aperture_is_set;}
      void ApertureIsNotSet() {Aperture_is_set = false; } 
      double GetAperture() {return Aperture;}
      void SetAperture(double value) {Aperture = value; Aperture_is_set=true;} 
      bool PermeabilityIsSet() {return Permeability_is_set;}
      void PermeabilityIsNotSet() {Permeability_is_set = false; } 
      double GetPermeability() {return Permeability;}
      void SetPermeability(double value) {Permeability = value; Permeability_is_set=true;} 
      bool InFrac() {return in_frac;}
      double GetWeight() {return f_weight;}
      void SetFrac(double value) {f_weight = value; in_frac=true;}
      long GetFracNum() {return frac_number;}
      void SetFracNum(long value) {frac_number = value; } 
      double GetFracDx() {return f_dx;}
      void SetFracDx(double value) {f_dx = value;}
      double GetFracDy() {return f_dy;}
      void SetFracDy(double value) {f_dy = value;} 
      void CalcDispGravityCenter(vector<double>&);
      #endif

};

} // namespace Mesh_Group
#endif
