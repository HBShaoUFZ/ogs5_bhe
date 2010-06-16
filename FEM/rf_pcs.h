/**************************************************************************
ROCKFLOW - Object: Process PCS
Task:
Programing:
02/2003 OK Implementation
11/2004 OK PCS2
**************************************************************************/
#ifndef rf_pcs_INC
#define rf_pcs_INC

#include "Configure.h"

#include "makros.h"

// MSHLib
#include "msh_lib.h"
// PCSLib
#include "rf_num_new.h"
#include "rf_bc_new.h"
//WW #include "rf_pcs.h" //Why this WW.
#include "rf_tim_new.h"
#include "rf_st_new.h"//CMCD 02_06
// C++ STL
#include <fstream>
//
// The follows are implicit declaration. WW
//---------------------------------------------------------------------------
namespace FiniteElement {class CFiniteElementStd; class CFiniteElementVec;
                         class ElementMatrix; class ElementValue;}
namespace Mesh_Group {class CFEMesh;}

#ifdef NEW_EQS    //WW
namespace Math_Group {class Linear_EQS;}
using Math_Group::Linear_EQS;
#endif

//
class CSourceTermGroup;
class CSourceTerm;
class CNodeValue;
class Problem; //WW
using FiniteElement::CFiniteElementStd;
using FiniteElement::CFiniteElementVec;
using FiniteElement::ElementMatrix;
using FiniteElement::ElementValue;
using Mesh_Group::CFEMesh;
//---------------------------------------------------------------------------

#define PCS_FILE_EXTENSION ".pcs"

typedef struct { /* Knotenwert-Informationen */
	char name[80]; /* Name der Knotengroesse */
	char einheit[10]; /* Einheit der Knotengroesse */
	int speichern; /* s.u., wird Wert gespeichert ? */
	int laden; /* s.u., wird Wert zu Beginn geladen ? */
	int restart; /* s.u., wird Wert bei Restart geladen ? */
	int adapt_interpol; /* Soll Wert bei Adaption auf Kinder interpoliert werden? */
	double vorgabe; /* Default-Wert fuer Initialisierung */
	int nval_index;
	int pcs_this;
	int timelevel;
} PCS_NVAL_DATA;

typedef struct { /* element data info */
	char name[80]; /* Name der Elementgroesse */
	char einheit[10]; /* Einheit der Elementgroesse */
	int speichern; /* s.u., wird Wert gespeichert ? */
	int laden; /* s.u., wird Wert zu Beginn geladen ? */
	int restart; /* s.u., wird Wert bei Restart geladen ? */
	int adapt_interpol; /* Soll Wert bei Adaption auf Kinder vererbt werden? */
	double vorgabe; /* Default-Wert fuer Initialisierung */
	int eval_index;
	int index;
} PCS_EVAL_DATA;

typedef struct {
	long index_node;
	double water_st_value;
} Water_ST_GEMS;  // HS 11.2008

//MB moved inside the Process object
//extern vector<double*>nod_val_vector; //OK
//extern vector<string>nod_val_name_vector; //OK
//extern void pcs->SetNodeValue(long,int,double); //OK
//extern double pcs->GetNodeValue(long,int); //OK
//extern int pcs->GetNodeValueIndex(string); //OK

// Element values for all process
//Moved inside Process object
//extern vector<double*>ele_val_vector; //PCH
//extern vector<string>eld_val_name_vector; //PCH
//extern void SetElementValue(long,int,double); //PCH
//extern double GetElementValue(long,int); //PCH
//extern int GetElementValueIndex(string); //PCH
/**************************************************************************
FEMLib-Class:
Task:
Programing:
03/2003 OK Implementation
02/2005 WW Local element assembly (all protected members)
12/2005 OK MSH_TYPE
last modification:
05/2010 TF inserted pointer to instance of class Problem inclusive access functions
**************************************************************************/

/**
 * class manages the physical processes
 */
class CRFProcess {
  //----------------------------------------------------------------------
  // Properties
  private:
	  /**
	   * _problem is a pointer to an instance of class Problem.
	   *  The pointer is used to get the geometric entities.
	   */
	  Problem* _problem;

    void VariableStaticProblem();
    void VariableDynamics();
    bool compute_domain_face_normal; //WW
    int continuum;
    bool continuum_ic;
  protected: //WW
    friend class FiniteElement::CFiniteElementStd;
    friend class FiniteElement::CFiniteElementVec;
    friend class FiniteElement::ElementValue;
    friend class ::CSourceTermGroup;
    friend class ::Problem;
    // Assembler
    CFiniteElementStd *fem;
    // Time step control
    bool accepted; //25.08.1008. WW
    int accept_steps;  //27.08.1008. WW
    int reject_steps;  //27.08.1008. WW
    //
    int dof;   //WW
    long orig_size; // Size of source term nodes
    // ELE
    std::vector<FiniteElement::ElementMatrix*> Ele_Matrices;
    // Storage type for all element matrices and vectors
    // Case:
    // 0. Do not keep them in the memory
    // 1. Keep them to vector Ele_Matrices
    int Memory_Type;
    //....................................................................
    int additioanl2ndvar_print; //WW
    // TIM
    friend class CTimeDiscretization;
  public: //OK
    CTimeDiscretization *Tim;    //time
  protected: //WW
    void CopyU_n(double *temp_v); //29.08.2008. WW
    // Time unit factor
    double time_unit_factor;
    int NumDeactivated_SubDomains;
    int *Deactivated_SubDomain;
    //New equation and solver objects WW
#ifdef NEW_EQS
#ifdef LIS
public:
	Linear_EQS *eqs_new;
#else
    Linear_EQS *eqs_new;
#endif // LIS endif for Fluid Momentum	// PCH
    bool configured_in_nonlinearloop;
#endif
    //
#ifdef USE_MPI //WW
    clock_t cpu_time_assembly;
#endif
    // Position of unkowns from different DOFs in the system equation
    //....................................................................
    // OUT
    // Write indices of the nodes with boundary conditons
    bool write_boundary_condition; //15.01.2008. WW
    // Element matrices output
  public: //OK
    bool Write_Matrix;
  protected: //WW
    fstream *matrix_file;
    // Write RHS from source or Neumann BC terms to file
    // 0: Do nothing
    // 1: Write
    // 2: Read
    int WriteSourceNBC_RHS;
    // Write the current solutions/Read the previous solutions WW
    // -1, default. Do nothing
    // 1. Write
    // 2. Read
    // 3 read and write
    int reload;
    long nwrite_restart;
    inline void  WriteRHS_of_ST_NeumannBC();
    inline void  ReadRHS_of_ST_NeumannBC();
    friend bool PCSRead(string);
    //....................................................................
    // 1-GEO
  public:
	  /**
	   * Sets the value for pointer _problem.
	   * @param problem the value for _problem
	   */
	  void setProblemObjectPointer (Problem* problem);

	  /**
	   * get access to the instance of class Problem
	   * @return
	   */
	  const Problem* getProblemObjectPointer () const;

	  string geo_type; //OK
	  string geo_type_name; //OK
	//....................................................................
    // 2-MSH
	//....................................................................
    // 3-TIM
    //....................................................................
    // 4-IC
    //....................................................................
    // 5-BC
    vector<CBoundaryConditionNode*> bc_node_value; //WW
    vector<CBoundaryCondition*> bc_node; //WW
    vector<long> bc_node_value_in_dom; //WW for domain decomposition
    vector<long> bc_local_index_in_dom; //WW for domain decomposition
    vector<long> rank_bc_node_value_in_dom; //WW
    vector<long> bc_transient_index; //WW/CB
    void UpdateTransientBC(); //WW/CB
    //....................................................................
    // 6-ST
    // Node values from sourse/sink or Neumann BC. WW
    vector<CNodeValue*> st_node_value; //WW
    vector<CSourceTerm*> st_node; //WW
    vector<long> st_node_value_in_dom; //WW for domain decomposition
    vector<long> st_local_index_in_dom; //WW for domain decomposition
    vector<long> rank_st_node_value_in_dom; //WW
    void RecordNodeVSize(const int Size) {orig_size = Size;} //WW
    int GetOrigNodeVSize () const {return orig_size;} //WW

    //....................................................................
    // 7-MFP
    //....................................................................
    // 8-MSP
    //....................................................................
    // 9-MMP
    int GetContinnumType() {return continuum;}
    // const int number_continuum=1;
    vector<double> continuum_vector;
    //....................................................................
    // 10-MCP
    //....................................................................
    // 11-OUT
    void  WriteSolution();  //WW
    void  ReadSolution();   //WW
    //....................................................................
    // 12-NUM
    //....................................................................
    // 13-ELE
    //----------------------------------------------------------------------
    // Methods
    //....................................................................
    // Construction / destruction
    CRFProcess(void);
    void Create(void);
#ifndef NEW_EQS //WW. 07.11.2008
    void CreateNew(void);
    void CreateFDMProcess();
    void DestroyFDMProcess();
#endif
    virtual ~CRFProcess();
    //....................................................................
    // IO
    ios::pos_type Read(ifstream*);
    void Write(fstream*);
    //....................................................................
    // 1-GEO
    //....................................................................
    // 2-MSH
    //....................................................................
    // 3-TIM
    //....................................................................
    // 4-IC
    //....................................................................
    // 5-BC
    void CreateBCGroup();
    void SetBC(); //OK
    void WriteBC(); //15.01.2008. WW
    //....................................................................
    // 6-ST
    void CreateSTGroup();
    //....................................................................
    // 7-MFP
    //....................................................................
    // 8-MSP
    //....................................................................
    // 9-MMP
    //....................................................................
    // 10-MCP
    //....................................................................
    // 11-OUT
    void WriteAllVariables(); //OK
    //....................................................................
    // 12-NUM
    //....................................................................
    // 13-ELE
    //....................................................................
    // 14-CPL
    void SetCPL(); //OK8 OK4310
	//....................................................................
	// 15-EQS
    //WW void AssembleParabolicEquationRHSVector(CNode*); //(vector<long>&); //OK
	//....................................................................
    int Shift[10];
	// 16-GEM  // HS 11.2008
	int flag_couple_GEMS;     // 0-no couple; 1-coupled
	vector<Water_ST_GEMS> Water_ST_vec;
    vector<long> stgem_node_value_in_dom; //KG for domain decomposition
    vector<long> stgem_local_index_in_dom; //KG for domain decomposition
    vector<long> rank_stgem_node_value_in_dom; //KG

	void Clean_Water_ST_vec(void);
	void Add_GEMS_Water_ST(long idx, double val);
	void SetSTWaterGemSubDomain(int myrank);
	//....................................................................
    // Construction / destruction
    char pcs_name[MAX_ZEILE]; //string pcs_name;
    int pcs_number;
    int mobile_nodes_flag;
    string pcs_type_name;
    int pcs_type_number;
    vector<string> pcs_type_name_vector;
    int type;
    int GetObjType() {return type;}
    int pcs_component_number; //SB: counter for transport components
		int ML_Cap;	// 23.01 2009 PCH
    int PartialPS; // 16.02 2009 PCH
    int GetProcessComponentNumber() const { return pcs_component_number;} //SB:namepatch
    string file_name_base; //OK
    // Access to PCS
    // Configuration 1 - NOD
    PCS_NVAL_DATA *pcs_nval_data; ///OK
    int number_of_nvals;
    int pcs_number_of_primary_nvals;
    int GetPrimaryVNumber() const {return pcs_number_of_primary_nvals;}
    char *pcs_primary_function_unit[4];
    char *pcs_primary_function_name[4];
    char* GetPrimaryVName(const int index) const {return pcs_primary_function_name[index];}
    string primary_variable_name; //OK
    int pcs_number_of_secondary_nvals;
    int GetSecondaryVNumber() const {return pcs_number_of_secondary_nvals;}
    char *pcs_secondary_function_name[PCS_NUMBER_MAX];
    char* GetSecondaryVName(const int index) const {return pcs_secondary_function_name[index];}
    char *pcs_secondary_function_unit[PCS_NUMBER_MAX];
    int pcs_secondary_function_timelevel[PCS_NUMBER_MAX];
    int pcs_number_of_history_values;
    /*double pcs_secondary_function_time_history[PCS_NUMBER_MAX];//CMCD for analytical solution
    double pcs_secondary_function_value_history[PCS_NUMBER_MAX];//CMCD for analytical solution
    void Set_secondary_function_time_history(const int index, double value) {pcs_secondary_function_time_history[index]=value;}//CMCD for analytical solution
    void Set_secondary_function_value_history(const int index, double value) {pcs_secondary_function_value_history[index]=value;}//CMCD for analytical solution
    double Get_secondary_function_time_history(const int index){return pcs_secondary_function_time_history[index];}//CMCD for analytical solution
    double Get_secondary_function_value_history(const int index){return pcs_secondary_function_value_history[index];}//CMCD for analytical solution*/
    // Configuration 2 - ELE
    PCS_EVAL_DATA* pcs_eval_data;
    int pcs_number_of_evals;
    char *pcs_eval_name[PCS_NUMBER_MAX];
    char *pcs_eval_unit[PCS_NUMBER_MAX];
    // Configuration 3 - ELE matrices
    // Execution
    // NUM
#ifndef NEW_EQS //WW 07.11.2008
    LINEAR_SOLVER *eqs;
#endif
    string num_type_name;
    int	rwpt_app;
    char *pcs_num_name[2];  //For monolithic scheme
    double pcs_nonlinear_iteration_tolerance;
    int pcs_nonlinear_iterations; //OK
    int pcs_coupling_iterations; //OK
    string tim_type_name; //OK
    char *pcs_sol_name;
    string cpl_type_name;
    CNumerics* m_num;
    //
    bool selected; //OK
	bool saturation_switch; // JOD
    // MSH
    CFEMesh* m_msh; //OK
    string msh_type_name; //OK
    //MB-------------
    vector<double*> nod_val_vector; //OK
    vector<string> nod_val_name_vector; //OK
    void SetNodeValue(long,int,double); //OK
    double GetNodeValue(long,int); //OK
    int GetNodeValueIndex(const string&); //OK
    //-----------------------------
    vector<double*> ele_val_vector; //PCH
    vector<string> ele_val_name_vector; //PCH
    void SetElementValue(long,int,double); //PCH
    double GetElementValue(long,int); //PCH
    int GetElementValueIndex(const string&); //PCH
    //CB-----------------------------
    int flow_pcs_type;
    //----------------------------------------------------------------------
    // Methods
    // Access to PCS
    CRFProcess *GetProcessByFunctionName(char* name);
    CRFProcess *GetProcessByNumber(int);
    CFiniteElementStd* GetAssembler() {return fem; }
    // CRFProcess *Get(string); // WW Removed
    // Configuration
    void Config();
    void ConfigGroundwaterFlow();
    void ConfigLiquidFlow();
    void ConfigNonIsothermalFlow();
    void ConfigNonIsothermalFlowRichards();
    void ConfigMassTransport();
    void ConfigHeatTransport();
    void ConfigDeformation();
    void ConfigMultiphaseFlow();
    void ConfigGasFlow();
    void ConfigUnsaturatedFlow(); //OK4104
	void ConfigFluidMomentum();
    void ConfigRandomWalk();
    void ConfigMultiPhaseFlow();
		void ConfigPS_Global();	// PCH
    // Configuration 1 - NOD
#ifndef NEW_EQS //WW. 07.11.2008
    void ConfigNODValues1(void);
    void ConfigNODValues2(void);
    void CreateNODValues(void);
    void SetNODValues(); //OK
    void CalcFluxesForCoupling(); //MB
    double CalcCouplingNODError(); //MB
    void SetNODFlux(); //OK
    //
    void AssembleParabolicEquationRHSVector(); //OK
	// 15-EQS
    void AssembleParabolicEquationRHSVector(CNode*); //(vector<long>&); //OK
#endif
    double CalcIterationNODError(int method); //OK
    void CopyTimestepNODValues(bool forward = true); // Add bool forward = true. WW
    //Coupling
    //WW double CalcCouplingNODError(); //MB
    void CopyCouplingNODValues();
    //WWvoid CalcFluxesForCoupling(); //MB
    // Configuration 2 - ELE
    void ConfigELEValues1(void);
    void ConfigELEValues2(void);
    void CreateELEValues(void);
    void CreateELEGPValues();
    void AllocateMemGPoint(); //NEW Gauss point values for CFEMSH WW
    void CalcELEVelocities(void);
    void CalcELEMassFluxes();
 //WW   double GetELEValue(long index,double*gp,double theta,string nod_fct_name);
    void CheckMarkedElement(); //WW
    // Configuration 3 - ELE matrices
    void CreateELEMatricesPointer(void);
    // Equation system
    //---WW
    CFiniteElementStd* GetAssember () { return fem; }
    void AllocateLocalMatrixMemory();
    void GlobalAssembly(); //NEW
    void ConfigureCouplingForLocalAssemblier();
    void CalIntegrationPointValue();
    bool cal_integration_point_value; //WW
	void CalGPVelocitiesfromFluidMomentum(); //SB 4900
	bool use_velocities_for_transport; //SB4900
    //---
    double Execute();
    double ExecuteNonLinear();
    virtual void CalculateElementMatrices(void) ;
    void DDCAssembleGlobalMatrix();
    virtual void AssembleSystemMatrixNew(void);
    // This function is a part of the monolithic scheme
    //  and it is related to ST, BC, IC, TIM and OUT. WW
    void SetOBJNames();
    // ST
    void IncorporateSourceTerms(const int rank=-1);
    //WW void CheckSTGroup(); //OK
#ifdef GEM_REACT
    void IncorporateSourceTerms_GEMS(void);//HS: dC/dt from GEMS chemical solver.
    int GetRestartFlag(){return reload;}
#endif
    // BC
    void IncorporateBoundaryConditions(const int rank=-1);
		void IncorporateBoundaryConditions(const int rank, const int axis);	// PCH for FLUID_MOMENTUM
    void SetBoundaryConditionSubDomain(); //WW
    //WW void CheckBCGroup(); //OK
#ifdef NEW_EQS //WW
    void EQSInitialize();
	void EQSSolver(double* x);	// PCH
#else
    void InitEQS();
    int ExecuteLinearSolver(void);
    int ExecuteLinearSolver(LINEAR_SOLVER *eqs);
#endif
    //Time Control
    double timebuffer; //YD
    int iter;  //YD
    // Specials
    void PCSMoveNOD();
    void PCSDumpModelNodeValues(void);
    int GetNODValueIndex(const string &name,int timelevel); //WW
    // BC for dynamic problems. WW
    inline void setBC_danymic_problems();
    inline void setST_danymic_problems();
    inline void setIC_danymic_problems();
    // Extropolate Gauss point values to node values. WW
    void Extropolation_GaussValue();
    void Extropolation_MatValue();  //WW
    // Auto time step size control. WW
    void PI_TimeStepSize(double *u_n); //WW
    bool TimeStepAccept() const { return accepted;}
    void SetDefaultTimeStepAccepted() { accepted = true;}
    // USER
    //ToDo
    double *TempArry; //MX
    void PCSOutputNODValues(void);
    void PCSSetTempArry(void);  //MX
    double GetTempArryVal(int index)  {return TempArry[index];} //MX
    void LOPCopySwellingStrain(CRFProcess *m_pcs);
    VoidFuncInt PCSSetIC_USER;
    void SetIC();
    void CalcSecondaryVariables(const bool initial = false); // Remove argument. WW
    void MMPCalcSecondaryVariablesRichards(int timelevel, bool update);
    //WW Reomve int timelevel, bool update
    void CalcSecondaryVariablesUnsaturatedFlow(bool initial = false); //WW
	void CalcSecondaryVariablesPSGLOBAL();	// PCH
	double GetCapillaryPressureOnNodeByNeighobringElementPatches(int nodeIdx, int meanOption, double Sw); // PCH
	void CalcSaturationRichards(int timelevel, bool update); // JOD
    bool non_linear; //OK/CMCD
    void InterpolateTempGP(CRFProcess *, string); //MX
    void ExtropolateTempGP(CRFProcess *, string); //MX
    //Repeat Calculation,    JOD removed // HS reactivated
    void PrimaryVariableReload();  //YD
    void PrimaryVariableReloadRichards(); //YD
    void PrimaryVariableStorageRichards(); //YD
    bool adaption;
    void PrimaryVariableReloadTransport(); //kg44
    void PrimaryVariableStorageTransport(); //kg44
    //double GetNewTimeStepSizeTransport(double mchange); //kg44
    // FLX
    void CalcELEFluxes(CGLPoint*); //OK
    double CalcELEFluxes(CGLPolyline*); //OK
    // NEW
    CRFProcess* CopyPCStoDM_PCS();
    bool OBJRelations(); //OK
    void OBJRelationsDelete(); //OK
    bool NODRelations(); //OK
    bool ELERelations(); //OK
#ifndef NEW_EQS //WW 07.11.2008
    bool CreateEQS(); //OK
    void EQSDelete(); //OK
    // Dumping matrix and RHS. WW
    void DumpEqs(string file_name);
#endif
    bool Check(); //OK
    void NODRelationsDelete(); //OK
    void ELERelationsDelete(); //OK
    bool m_bCheckOBJ; //OK
    bool m_bCheckNOD; //OK
    bool m_bCheckELE; //OK
    bool m_bCheckEQS; //OK
    void Delete(); //OK
    bool m_bCheck; //OK
#ifdef USE_MPI //WW
    void Print_CPU_time_byAssembly(ostream &os=cout)
      {   os<<"\n***\nCPU time elapsed in the linear equation of "<<pcs_type_name<<"\n";
          os<<"--Global assembly: "<<(double)cpu_time_assembly/CLOCKS_PER_SEC<<"\n";
      }
#endif

};

//========================================================================
// PCS
extern vector<CRFProcess*>pcs_vector;
extern vector<ElementValue*> ele_gp_value; // Gauss point value for velocity. WW
extern bool PCSRead(string);
extern void PCSWrite(string);
extern void RelocateDeformationProcess(CRFProcess *m_pcs);
extern void PCSDestroyAllProcesses(void);
extern CRFProcess* PCSGet(const string&);
extern CRFProcess* PCSGetNew(const string&,const string&);
extern void PCSDelete();
extern void PCSDelete(string);
extern void PCSCreate();
extern int PCSGetPCSIndex(const string&,const string&); //SB
extern CRFProcess *PCSGet(const string&,const string&); //SB
extern CRFProcess *PCSGet(const string&,bool); //OK
extern CRFProcess *PCSGetFluxProcess();//CMCD
extern CRFProcess *PCSGetFlow(); //OK
extern bool PCSConfig(); //OK
// NOD
extern int PCSGetNODValueIndex(const string&,int);
extern double PCSGetNODValue(long,char*,int);
extern void PCSSetNODValue(long,const string&,double,int);
// ELE
extern int PCSGetELEValueIndex(char*);
extern double PCSGetELEValue(long index,double*gp,double theta,const string &nod_fct_name);
// Specials
extern void PCSRestart();
extern string PCSProblemType();
// PCS global variables
extern int pcs_no_fluid_phases;
extern int pcs_no_components;
extern bool pcs_monolithic_flow;
extern int pcs_deformation;
extern int dm_pcs_number;

// ToDo
extern double PCSGetNODConcentration(long index, long component, long timelevel); //SB
extern void PCSSetNODConcentration(long index, long component, long timelevel, double value); //SB
extern char *GetCompNamehelp(char *name); //SB:namepatch - superseded by GetPFNamebyCPName
extern double PCSGetEleMeanNodeSecondary(long index, const string &pcs_name, const string &var_name, int timelevel); //SB4218
extern double PCSGetEleMeanNodeSecondary_2(long index, int pcsT, const string &var_name, int timelevel); //CB
extern string GetPFNamebyCPName(string line_string);

extern int memory_opt;

typedef struct {  /* Knotenwert-Informationen */
    char *name;  /* Name der Knotengroesse */
    char *einheit;  /* Einheit der Knotengroesse */
    int speichern;  /* s.u., wird Wert gespeichert ? */
    int laden;  /* s.u., wird Wert zu Beginn geladen ? */
    int restart;  /* s.u., wird Wert bei Restart geladen ? */
    int adapt_interpol;  /* Soll Wert bei Adaption auf Kinder interpoliert werden? */
    double vorgabe;  /* Default-Wert fuer Initialisierung */
} NvalInfo;
extern int anz_nval;  /* Anzahl der Knotenwerte */
extern int anz_nval0;  /* Anzahl der Knotenwerte */
extern NvalInfo *nval_data;
extern int ModelsAddNodeValInfoStructure(char*,char*,int,int,int,int,double);

typedef struct {  /* Elementwert-Informationen */
    char *name;  /* Name der Elementgroesse */
    char *einheit;  /* Einheit der Elementgroesse */
    int speichern;  /* s.u., wird Wert gespeichert ? */
    int laden;  /* s.u., wird Wert zu Beginn geladen ? */
    int restart;  /* s.u., wird Wert bei Restart geladen ? */
    int adapt_interpol;  /* Soll Wert bei Adaption auf Kinder vererbt werden? */
    double vorgabe;  /* Default-Wert fuer Initialisierung */
} EvalInfo;
extern int anz_eval;  /* Anzahl der Elementwerte */
extern EvalInfo *eval_data;
extern int ModelsAddElementValInfoStructure(char*,char*,int,int,int,int,double);

extern int GetRFControlGridAdapt(void);
extern int GetRFControlModel(void);
extern int GetRFProcessChemicalModel(void);
extern int GetRFProcessFlowModel(void);
extern int GetRFProcessHeatReactModel(void);
extern int GetRFProcessNumPhases(void);
extern int GetRFProcessProcessing(char*);
extern int GetRFProcessProcessingAndActivation(char*);
extern long GetRFProcessNumComponents(void);
extern int GetRFControlModex(void);
extern int GetRFProcessDensityFlow(void);
extern int GetRFProcessNumContinua(void);
extern int GetRFProcessNumElectricFields(void);
extern int GetRFProcessNumTemperatures(void);
extern int GetRFProcessSimulation(void);

// Coupling Flag. WW
extern bool T_Process;
extern bool H_Process;
extern bool M_Process;
extern bool RD_Process;
extern bool MH_Process; // MH monolithic scheme
extern bool MASS_TRANSPORT_Process;
extern bool FLUID_MOMENTUM_Process;
extern bool RANDOM_WALK_Process;
extern bool PS_Global; //NB
extern string project_title; //OK41
extern bool pcs_created;
extern vector<LINEAR_SOLVER *> PCS_Solver; //WW
extern void MMPCalcSecondaryVariablesNew(CRFProcess*m_pcs, bool NAPLdiss); //OK
extern void CalcNewNAPLSat(CRFProcess*m_pcs); //CB 01/08
extern double CalcNAPLDens(CRFProcess*m_pcs, int node); //CB 01/08
extern void SetFlowProcessType();//CB 01/08
extern void CopyTimestepNODValuesSVTPhF(); //CB 13/08
#ifndef NEW_EQS //WW. 07.11.2008
extern void PCSCreateNew(); //OK
#endif
extern bool PCSCheck(); //OK
// New solvers WW
// Create sparse graph for each mesh    //1.11.2007 WW
#ifdef NEW_EQS    //1.11.2007 WW
extern void CreateEQS_LinearSolver();
#endif

#ifdef GEM_REACT
class REACT_GEM;
extern REACT_GEM *m_vec_GEM;
#endif

#ifdef BRNS
class REACT_BRNS;
extern REACT_BRNS *m_vec_BRNS;
#endif

extern bool hasAnyProcessDeactivatedSubdomains; //NW

#endif
