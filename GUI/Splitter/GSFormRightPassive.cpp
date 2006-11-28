// GSFormRightPassive.cpp : implementation file
//
#include "stdafx.h"
#include "afxpriv.h"
#include "Windowsx.h"
#include "GeoSys.h"
#include "GeoSysDoc.h"
#include "MainFrm.h"
#include "GSFormRightPassive.h"
#include "GSForm3DLeft.h"
#include "GeoSysTreeView.h"
#include "COGLPickingView.h"
//GEOLIB
#include "geo_lib.h"
#include "geo_pnt.h"
#include "geo_lin.h"
#include "geo_ply.h"
#include "geo_sfc.h"
#include "geo_vol.h"
//MSHLIB
#include "msh_elements_rfi.h"
#include "msh_quality.h"
#include "dtmesh.h"
//FEM
#include "rf_pcs.h"
#include "rf_tim_new.h"
#include "rf_bc_new.h"
#include "rf_mmp_new.h"
#include "rf_st_new.h"
#include "gs_pcs_oglcontrol.h"
#include "gs_meshtypechange.h"
#include "gs_project.h"
#include ".\gsformrightpassive.h"

//Iso function
#include "IsoControl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CGSFormRightPassive

IMPLEMENT_DYNCREATE(CGSFormRightPassive, CFormView)

CGSFormRightPassive::CGSFormRightPassive()
	: CFormView(CGSFormRightPassive::IDD)
{
    CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();    
    mainframe->m_image_distort_factor_x = m_image_distort_factor_x = 1.0;
    mainframe->m_image_distort_factor_y = m_image_distort_factor_y = 1.0;
    mainframe->m_image_distort_factor_z = m_image_distort_factor_z = 1.0;

    m_tolerancefactor=0.000;
    m_polyline_min_seg_length = 0.000;
    m_polyline_max_seg_length = 1.000;
    m_polyline_smaller_seg_length_def = 0.000;
    m_pcs_min= 0.0;
    m_pcs_max= 0.0;
    GetPcsMinmax();
	//{{AFX_DATA_INIT(CGSFormRightPassive)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    //HS, Iso function
    m_pmodeless = NULL;
}

CGSFormRightPassive::~CGSFormRightPassive()
{
}

void CGSFormRightPassive::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CGSFormRightPassive)
    DDX_Control(pDX, IDC_LIST, m_List);
    DDX_Text(pDX,IDC_X_IMAGEDISTORT, m_image_distort_factor_x);
    DDX_Text(pDX,IDC_Y_IMAGEDISTORT, m_image_distort_factor_y);
    DDX_Text(pDX,IDC_Z_IMAGEDISTORT, m_image_distort_factor_z);
    DDX_Text(pDX,IDC_EDIT_TOLERANCE, m_tolerancefactor);
    DDX_Text(pDX, IDC_EDIT_MIN_POLYLENGTH, m_polyline_min_seg_length);
    DDX_Text(pDX, IDC_EDIT_MAX_POLYLENGTH, m_polyline_max_seg_length);
    DDX_Text(pDX, IDC_EDIT_SMALER_POLYLENGTH_DEF, m_polyline_smaller_seg_length_def);
    DDX_Text(pDX,IDC_PCS_MIN_EDIT, m_pcs_min);
    DDX_Text(pDX,IDC_PCS_MAX_EDIT, m_pcs_max);
    DDX_Control(pDX, IDC_SLIDER1, m_slider1);
    DDX_Control(pDX, IDC_SLIDER2, m_slider2);


    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGSFormRightPassive, CFormView)
    ON_BN_CLICKED(IDC_BOUNDINGBOX_ON_OFF, OnBnClickedBoundingboxOnOff)
    ON_BN_CLICKED(IDC_BUTTON_NOT_DISTORT, OnNotDistortImage)
    ON_BN_CLICKED(IDC_BUTTON_DISTORT, OnDistortImage)
    ON_BN_CLICKED(IDC_CHECK_DOUBLE_POINTS, OnBnClickedCheckDoublePoints)
    ON_BN_CLICKED(IDC_DELETE_DOUBLE_POINTS_BUTTON, OnBnClickedDeleteDoublePointsButton)
    ON_BN_CLICKED(IDC_ADD_POLYGON_BUTTON, OnBnClickedAddPolygonButton)
    ON_BN_CLICKED(IDC_ADD_NEXT_POLYGON_BUTTON, OnBnClickedAddNextPolygonButton)
    ON_BN_CLICKED(IDC_GET_MINMAXSEGLENGTH_BUTTON, OnBnClickedGetMinmaxseglengthButton)
    ON_BN_CLICKED(IDC_SET_MINMAXSEGLENGTH_BUTTON, OnBnClickedSetMinmaxseglengthButton)
    ON_BN_CLICKED(IDC_RELOAD_GEO_BUTTON, OnBnClickedReloadGeoButton)
    ON_BN_CLICKED(IDC_EDITOR_GEO_BUTTON, OnBnClickedEditorGeoButton)
    ON_BN_CLICKED(IDC_POINT_NUMBERS_CHECK, OnBnClickedPointNumbersCheck)
    ON_BN_CLICKED(IDC_GET_SEGLENGTH_SMALLER_DEF, OnBnClickedGetSeglengthSmallerDef)
    ON_BN_CLICKED(IDC_GET_PCS_MINMAX_BUTTON3, OnBnClickedGetPcsMinmaxButton3)
    ON_BN_CLICKED(IDC_SET_PCS_MINMAX_BUTTON2, OnBnClickedSetPcsMinmaxButton2)
    ON_BN_CLICKED(IDC_VALUE_POINTS_BUTTON, OnBnClickedValuePointsButton)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CGSFormRightPassive diagnostics

#ifdef _DEBUG
void CGSFormRightPassive::AssertValid() const
{
	CFormView::AssertValid();
}

void CGSFormRightPassive::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CGSFormRightPassive message handlers

void CGSFormRightPassive::OnDataChange()
{
    	if (!UpdateData())
		return;

    CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();    

    mainframe->m_bounding_box = m_bounding_box;
    mainframe->m_image_distort_factor_x = m_image_distort_factor_x;
    mainframe->m_image_distort_factor_y = m_image_distort_factor_y;
    mainframe->m_image_distort_factor_z = m_image_distort_factor_z;
    mainframe->m_3dcontrol_double_points = m_3dcontrol_double_points;
    mainframe->m_tolerancefactor = m_tolerancefactor;

    mainframe->m_something_changed = 1;
    CGeoSysDoc* pdoc = GetDocument();
    pdoc->UpdateAllViews(NULL);

}


void CGSFormRightPassive::OnInitialUpdate() 
{
 CFormView::OnInitialUpdate();

 //OUTPUT LIST
  m_List.ResetContent();
  m_List.AddString(_T("INFO:"));
  m_List.AddString(_T(" "));
  m_List.AddString(_T(" "));
  m_List.AddString(_T(" "));
  m_List.AddString(_T("UNDER CONSTRUCTION!"));

  //HS
  m_pmodeless = NULL;
}


void CGSFormRightPassive::OnDraw(CDC* pDC)
{
    pDC=pDC;//TK
   
//OUTPUT LIST
  string info_string;
  //char string2add [56];
  m_List.ResetContent();
  m_List.AddString(_T(">INFO:"));
  m_List.AddString(_T("***********************************"));
  if ((int)gli_points_vector.size() > 0) m_List.AddString(_T(">GEO data loaded"));
  if ((int)gli_points_vector.size() == 0) m_List.AddString(_T(">GEO data not loaded"));
  if ((int)fem_msh_vector.size() > 0) {
      m_List.AddString(_T(">MSH data loaded"));

      //sprintf(string2add, "%ld",NodeListSize());
      //info_string = "> Nodes:" ;
      //info_string.append(string2add);
      //m_List.AddString(_T(info_string.data()));
  
  }
  if ((int)fem_msh_vector.size() == 0) m_List.AddString(_T(">MSH data not loaded"));
  if ((int)pcs_vector.size() > 0) m_List.AddString(_T(">PCS data loaded"));
  if ((int)pcs_vector.size() == 0) m_List.AddString(_T(">PCS data not loaded"));
  m_List.AddString(_T("***********************************"));
//

    
}

void CGSFormRightPassive::OnBnClickedBoundingboxOnOff()
{
  if(m_bounding_box==1)m_bounding_box=0;
  else m_bounding_box=1;

  OnDataChange();   

}

void CGSFormRightPassive::OnDistortImage()
{
    CGeoSysDoc* pdoc = GetDocument();
    pdoc->UpdateAllViews(this);
    OnDataChange();
}

void CGSFormRightPassive::OnNotDistortImage()
{

    m_image_distort_factor_x = 1.0;
    m_image_distort_factor_y = 1.0;
    m_image_distort_factor_z = 1.0;
    UpdateData(FALSE);
    CGeoSysDoc* pdoc = GetDocument();
    pdoc->UpdateAllViews(this);
    OnDataChange();
 
    // TODO: Add your control notification handler code here
}

void CGSFormRightPassive::OnBnClickedCheckDoublePoints()
{
  CWnd *pWin = ((CWinApp *) AfxGetApp())->m_pMainWnd;
  //CGeoSysDoc *m_pDoc = GetDocument();
  //CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  

  if(double_points_button_check_on==1)double_points_button_check_on=0;
  else double_points_button_check_on=1;

  if(double_points_button_check_on==1)
  {
    UpdateData(TRUE);
    pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Serializing point numbers");
    // TODO: GEO_Serialize_Point_Numbers();
    pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Searching double points");
    GEO_Search_DoublePoints(m_tolerancefactor);
    pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)" ");
    m_3dcontrol_double_points = 1;
  }
  else
  {
    m_3dcontrol_double_points = 0;
  }

  GEOSurfaceTopology();
  GEOCreateSurfacePointVector(); //OK
  OnDataChange();
  //m_frame->m_rebuild_formtree = 1;
  //m_pDoc->UpdateAllViews(NULL);
  //Invalidate(TRUE);
}


void CGSFormRightPassive::OnBnClickedDeleteDoublePointsButton()
{
  CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  CWnd *pWin = ((CWinApp *) AfxGetApp())->m_pMainWnd;
  CGeoSysDoc *m_pDoc = GetDocument();
  UpdateData(TRUE);

  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Serializing point numbers");
  GEO_Serialize_Point_Numbers();
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Searching double points");
  GEO_Search_DoublePoints(m_tolerancefactor);
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Deleting double points");
  GEO_Delete_DoublePoints();
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)" ");

  //writing and reading of a temp file to ensure gloabal loading and updating
  CString m_strFileNameBase = m_pDoc->m_strGSPFilePathBase;
  string m_strFileNameGEO_temp = m_strFileNameBase + "_temp";
  GEOWrite(m_strFileNameGEO_temp);
  GEOLIB_Read_GeoLib(m_strFileNameGEO_temp);
  const char *file_name_const_char = 0;
  file_name_const_char = m_strFileNameGEO_temp.data();
  remove(file_name_const_char);
 
  OnDataChange();
  m_frame->m_rebuild_formtree = 1;
  m_pDoc->UpdateAllViews(NULL);
  Invalidate(TRUE);

}

void CGSFormRightPassive::OnBnClickedAddPolygonButton()
{
  //========================================================================
  //File handling
  //========================================================================
  CGeoSysDoc *m_pDoc = GetDocument();
  m_pDoc->OnRemoveGEO();
  m_pDoc->OnRemoveMSH();
  CString m_strFileNameBase = m_pDoc->m_strGSPFilePathBase;
  CString m_strFileNameRFI = m_strFileNameBase + ".rfi";
  CString m_strFileNameGEO = m_strFileNameBase + ".gli";
  CString m_strFileNameGMSH = m_strFileNameBase + ".msh";
  CString m_strFileNameGSP = m_strFileNameBase + ".gsp";
  const char *file_name_const_char = 0;
  file_name_const_char = m_strFileNameGEO;
  //========================================================================

  const char *gli_file_name_const_char = 0;
  const char *rfi_file_name_const_char = 0;
  gli_file_name_const_char = m_strFileNameGEO;
  rfi_file_name_const_char = m_strFileNameRFI;

  FILE *plg_file_orig=NULL;
  FILE *geo_file=NULL;
  geo_file = fopen(gli_file_name_const_char, "w+t");
  char input_text[1024];
  int ok=1;
  int i=0;
  long id=0;
  double x=0.0,y=0.0,z=0.0;

  CFileDialog fileDlg(TRUE, "plg", NULL, OFN_ENABLESIZING," POLYGON File (*.plg)|*.plg|| ");
  if(fileDlg.DoModal()==IDOK){

    plg_file_orig = fopen(fileDlg.GetPathName(),"rt"); 

    if (fileDlg.GetFileExt() == "plg")
    {
      fgets(input_text,1024,plg_file_orig);

      //FORMAT: 
      //POINTS 
      //ID X Y Z
      //POLYGONS
      //ID point_numbers

      if (!strncmp(input_text, "POINTS",6))
      {
       fprintf(geo_file,"%s\n","#POINTS");
       while (!feof(plg_file_orig)){
           fgets(input_text,1024,plg_file_orig);
           if (!strncmp(input_text, "PLG",3))break;
           else
           {
                sscanf(input_text,"%i %lg %lg %lg", &id, &x, &y, &z);
                fprintf(geo_file,"%i %lg %lg %lg\n",id, x, y, z);
           }
           //ok = sscanf(input_text,"%i %lg %lg %lg", &id, &x, &y, &z);
           //fprintf(geo_file,"%s", input_text);
           
       }
       

        rewind (plg_file_orig);
        int plg_start=0;
        int first_hit=0;
        int first_point=0;
        char prename[1024];

        while (!feof(plg_file_orig)){
            fgets(input_text,1024,plg_file_orig);
        if (!strncmp(input_text, "PLG",3))
        {
            if (first_hit > 0)
            {
             fprintf(geo_file," %i\n",first_point);

                /*SFC*/ 
                fprintf(geo_file,"%s\n","#SURFACE");
                fprintf(geo_file,"%s\n"," $NAME");
                fprintf(geo_file,"   %s\n",prename);
                fprintf(geo_file,"%s\n"," $TYPE 0");
                fprintf(geo_file,"%s\n","  0");
                fprintf(geo_file,"%s\n"," $POLYLINES");
                fprintf(geo_file,"   %s\n",prename);

            }

            plg_start++;
            first_hit=-1;
            first_point=-1;
            strcpy(prename,input_text);
            fprintf(geo_file,"%s\n","#POLYLINE");
            fprintf(geo_file,"%s\n"," $NAME");
            fprintf(geo_file,"%s\n", input_text);
            fprintf(geo_file,"%s\n"," $TYPE 0");
            fprintf(geo_file,"%s\n","  0");
            fprintf(geo_file,"%s\n"," $EPSILON");
            fprintf(geo_file,"%s\n","  0.000000e+000");
            fprintf(geo_file,"%s\n"," $MAT_GROUP");
            fprintf(geo_file,"%s\n","  -1");
            fprintf(geo_file,"%s\n"," $POINTS");
        }
        if (plg_start>0 && strncmp(input_text, "PLG",3))
        {
             ok = sscanf(input_text,"%i", &id);
             fprintf(geo_file," %i\n",id);
             if (first_hit == -1) first_point = id;
             first_hit++;
        }
       }   

      
        fprintf(geo_file,"%s\n","#STOP");
      }


      //FORMAT: x,y,z in order
      else
      {
      rewind (plg_file_orig);
      
      if(plg_file_orig)
      {

       fprintf(geo_file,"%s\n","#POINTS");

       while (!feof(plg_file_orig)&& ok>0){

           id++;
           fgets(input_text,1024,plg_file_orig);
           ok = sscanf(input_text,"%lg %lg %lg", &x, &y, &z);
           fprintf(geo_file,"%i %lg %lg %lg\n",id, x, y, z);
       }
       
       /*fprintf(geo_file,"%s\n","#LINES");
       for (i=0;i<id-1;i++)
       {
          fprintf(geo_file,"%i %i %i\n",i+1, i+1, i+2);
       }
          fprintf(geo_file,"%i %i %i\n",i+1, i+1, i-i+1);
       */
   
       fprintf(geo_file,"%s\n","#POLYLINE");
       fprintf(geo_file,"%s\n"," $NAME");
       fprintf(geo_file,"%s\n","  Polygon_01");
       fprintf(geo_file,"%s\n"," $TYPE 0");
       fprintf(geo_file,"%s\n","  0");
       fprintf(geo_file,"%s\n"," $EPSILON");
       fprintf(geo_file,"%s\n","  0.000000e+000");
       fprintf(geo_file,"%s\n"," $MAT_GROUP");
       fprintf(geo_file,"%s\n","  -1");
       fprintf(geo_file,"%s\n"," $POINTS");

       for (i=0;i<id;i++)
       {
          fprintf(geo_file," %i\n",i+1);
       }
          fprintf(geo_file," %i\n",i-i+1);

        fprintf(geo_file,"%s\n","#SURFACE");
        fprintf(geo_file,"%s\n"," $NAME");
        fprintf(geo_file,"%s\n","  Polygon_01");
        fprintf(geo_file,"%s\n"," $TYPE 0");
        fprintf(geo_file,"%s\n","  0");
        fprintf(geo_file,"%s\n"," $POLYLINES");
        fprintf(geo_file,"%s\n","  Polygon_01");
        fprintf(geo_file,"%s\n","#STOP");
        
      }
    }
    }
  fclose(geo_file);
  GEOLIB_Read_GeoLib((string)m_strFileNameBase);

  OnDataChange();
  CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  m_frame->m_rebuild_formtree = 1;//TK - left tree in form view
  m_pDoc->UpdateAllViews(NULL);
  Invalidate(TRUE);

  // Add MSH member to GSP vector
  GSPAddMember((string)m_pDoc->m_strGSPFileBase + ".gli");
  

  }
}
void CGSFormRightPassive::OnBnClickedAddNextPolygonButton()
{
  /*FILE *plg_file_orig=NULL;
  char input_text[1024];
  long id=0;
  double x=0.0,y=0.0,z=0.0;
  long pointsvectorsize,polylinesvectorsize,surfacesvectorsize;

  CGLPoint *m_point = NULL;
  CGLPoint *m_polyline_start_point = NULL;
  vector<CGLPoint*> gli_points_vector;
  gli_points_vector = GEOLIB_GetGLIPoints_Vector();
  pointsvectorsize =(long)gli_points_vector.size();
 
  CGLPolyline *gl_polyline = NULL;
  list<CGLPolyline*> polyline_list;
  polyline_list = gl_polyline->GEOGetPolylines();
  polylinesvectorsize =(long)polyline_list.size();
  //list<CGLPolyline*>::const_iterator p = polyline_list.begin();
  sprintf(input_text,"%s%d","Polygon_",polylinesvectorsize+1);
  //gl_polyline = gl_polyline->GEOCreatePolyline(input_text);
  //gl_polyline->number_this = polylinesvectorsize+1;
  //gs_polyline = gs_polyline->Create(c_string_ply);//CC
  gs_polyline = new CGLPolyline;
  gs_polyline->name = c_string_ply;
  polyline_vector.push_back(gs_polyline);
  gs_polyline->id = size;

  Surface *gl_surface = NULL;
  list<Surface*> surface_list;
  surface_list = gl_surface->GEOGetSurfaces();
  surfacesvectorsize = (long)surface_list.size();
  //list<Surface*>::const_iterator ps = surface_list.begin();
  sprintf(input_text,"%s%d","Polygon_",surfacesvectorsize+1);
  gl_surface = gl_surface->GEOCreateSurface(input_text);
  gl_surface->number_this = surfacesvectorsize+1;

  CFileDialog fileDlg(TRUE, "plg", NULL, OFN_ENABLESIZING," POLYGON File (*.plg)|*.plg|| ");
  if(fileDlg.DoModal()==IDOK){
    if (fileDlg.GetFileExt() == "plg")
    {
      plg_file_orig = fopen(fileDlg.GetPathName(),"rt"); 
      if(plg_file_orig)
      {
       while (!feof(plg_file_orig)){

           id++;
           fgets(input_text,1024,plg_file_orig);
           sscanf(input_text,"%lg %lg %lg", &x, &y, &z);

           m_point = new CGLPoint;
           m_point->id = pointsvectorsize+id;
           m_point->x = x;
           m_point->y = y;
           m_point->z = z;           
           gli_points_vector.push_back(m_point);

           gl_polyline->point_list.push_back(m_point);
           gl_polyline->point_vector.push_back(m_point);
          
           if (id==1)m_polyline_start_point = m_point;
       } 
           gl_polyline->point_list.push_back(m_polyline_start_point);
           gl_polyline->point_vector.push_back(m_polyline_start_point);
           gl_surface->polyline_of_surface_list.push_back(gl_polyline);
      }
    }

  GEOLIB_SetGLIPoints_Vector(gli_points_vector);

  OnDataChange();
  CGeoSysDoc *m_pDoc = GetDocument();
  CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  m_frame->m_rebuild_formtree = 1;//TK - left tree in form view
  m_pDoc->UpdateAllViews(NULL);
  Invalidate(TRUE);

  }*/
}

void CGSFormRightPassive::OnBnClickedGetMinmaxseglengthButton()
{
  m_polyline_min_seg_length = GEO_Get_Min_PolySeg_length();
  m_polyline_max_seg_length = GEO_Get_Max_PolySeg_length();
  UpdateData(FALSE);
}

void CGSFormRightPassive::OnBnClickedSetMinmaxseglengthButton()
{
  //CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  CWnd *pWin = ((CWinApp *) AfxGetApp())->m_pMainWnd;
  //CGeoSysDoc *m_pDoc = GetDocument();
  

  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Serializing point numbers");
  GEO_Serialize_Point_Numbers();
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)" ");
 
  UpdateData(TRUE);

  if (m_polyline_max_seg_length-m_polyline_min_seg_length > 0.0)
  {
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Setting segment length");
  GEO_Set_Poly_Seg_Length(m_polyline_min_seg_length,m_polyline_max_seg_length);
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Serializing");
  GEO_Serialize_Point_Numbers();
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Searching double points");
  GEO_Search_DoublePoints(0);
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)"Merging");
  GEO_Delete_DoublePoints();
  pWin->SendMessage(WM_SETMESSAGESTRING,0,(LPARAM)(LPCSTR)" ");
  }
  else AfxMessageBox("Segment length < 0.000 !!! ");

  m_polyline_min_seg_length = GEO_Get_Min_PolySeg_length();
  m_polyline_max_seg_length = GEO_Get_Max_PolySeg_length();
  UpdateData(FALSE);

  CGeoSysDoc *m_pDoc = GetDocument();
  CString m_strFileNameBase = m_pDoc->m_strGSPFilePathBase;
  CString m_strFileNameRFI = m_strFileNameBase + ".rfi";
  CString m_strFileNameGEO = m_strFileNameBase + ".gli";
  GSPWriteData();
  GEOLIB_Read_GeoLib((string)m_strFileNameBase);

  OnDataChange();
  CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  m_frame->m_rebuild_formtree = 1;//TK - left tree in form view
  m_pDoc->UpdateAllViews(NULL);
  Invalidate(TRUE);

  
  
}

void CGSFormRightPassive::OnBnClickedReloadGeoButton()
{
  CGeoSysDoc *m_pDoc = GetDocument();
  CString m_strFileNameBase = m_pDoc->m_strGSPFilePathBase;
  CString m_strFileNameRFI = m_strFileNameBase + ".rfi";
  CString m_strFileNameGEO = m_strFileNameBase + ".gli";
  //GSPWriteData();
  GEOLIB_Read_GeoLib((string)m_strFileNameBase);
  
  m_polyline_min_seg_length = GEO_Get_Min_PolySeg_length();
  m_polyline_max_seg_length = GEO_Get_Max_PolySeg_length();
  UpdateData(FALSE);


  OnDataChange();
  CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  m_frame->m_rebuild_formtree = 1;//TK - left tree in form view
  m_pDoc->UpdateAllViews(NULL);
  Invalidate(TRUE);



}

void CGSFormRightPassive::OnBnClickedEditorGeoButton()
{
  CGeoSysDoc *m_pDoc = GetDocument();
  CString m_strFileNameBase = m_pDoc->m_strGSPFilePathBase;
  CString m_strFileNameRFI = m_strFileNameBase + ".rfi";
  CString m_strFileNameGEO = m_strFileNameBase + ".gli";
  CString exe_call = "notepad.exe " + m_strFileNameGEO;
  WinExec(exe_call, SW_SHOW);
}

void CGSFormRightPassive::OnBnClickedPointNumbersCheck()
{
  CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
  CGeoSysDoc *m_pDoc = GetDocument();

  if(point_numbers_button_on==1)point_numbers_button_on=0;
  else point_numbers_button_on=1;

  if(point_numbers_button_on==1)
  {
    m_frame->m_3dcontrol_points = 1;
    m_frame->m_3dcontrol_point_numbers = 1; 
  }
  else
  {
    m_frame->m_3dcontrol_point_numbers = 0; 

  }

  OnDataChange();
  m_pDoc->UpdateAllViews(NULL);
  Invalidate(TRUE);


}


void CGSFormRightPassive::OnBnClickedGetSeglengthSmallerDef()
{
     UpdateData(TRUE);

	int j=0, l=0, hit=0;
	long pointsvectorsize, polylinesvectorsize;
	long number_of_polylinepoints;
	long check_point;
    double x1=0.0,y1=0.0,z1=0.0,seg_length=0.0;
    double x2=0.0,y2=0.0,z2=0.0,seg_length_saved=0.0;
	vector<CGLPoint*> gli_points_vector;
	gli_points_vector = GetPointsVector();
    pointsvectorsize =(long)gli_points_vector.size();
	CGLPolyline *gl_polyline = NULL;
    vector<CGLPolyline*> polyline_vector;
	polyline_vector = GetPolylineVector();
	polylinesvectorsize =(long)polyline_vector.size();
	vector<CGLPolyline*>::const_iterator p = polyline_vector.begin();

	string Name;
    
   	for (l=0;l<polylinesvectorsize;l++)
    { 	
		gl_polyline = *p;
		Name = gl_polyline->name;
        gl_polyline->min_plg_Dis = m_polyline_smaller_seg_length_def;
		number_of_polylinepoints = (long)gl_polyline->point_vector.size();
		for (j=0;j<number_of_polylinepoints;j++)
		{ 
          gl_polyline->point_vector[j]->plg_hightlight_seg = 0;
        }           
        for (j=0;j<number_of_polylinepoints-1;j++)
		{ 
            check_point = gl_polyline->point_vector[j]->id;
            x1 = gl_polyline->point_vector[j]->x;
            y1 = gl_polyline->point_vector[j]->y;
            z1 = gl_polyline->point_vector[j]->z;

            check_point = gl_polyline->point_vector[j+1]->id;
            x2 = gl_polyline->point_vector[j+1]->x;
            y2 = gl_polyline->point_vector[j+1]->y;
            z2 = gl_polyline->point_vector[j+1]->z;

            seg_length = EuklVek3dDistCoor ( x1, y1, z1, x2, y2, z2 ); 

            if (seg_length <  m_polyline_smaller_seg_length_def)
            {
                gl_polyline->point_vector[j]->plg_hightlight_seg = 1;    
                gl_polyline->point_vector[j+1]->plg_hightlight_seg = 1;
            }
		}
		++p;
	}

 
 CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
 CGeoSysDoc *m_pDoc = GetDocument();
 m_frame->m_something_changed = 1;
 //m_frame->m_rebuild_formtree = 1;//TK - left tree in form view
 m_pDoc->UpdateAllViews(NULL);
 Invalidate(TRUE);



}

void CGSFormRightPassive::OnBnClickedGetPcsMinmaxButton3()
{
 CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
 CGeoSysDoc *m_pDoc = GetDocument();
 m_pcs_name =  m_frame->m_pcs_name;
 GetPcsMinmax();
 UpdateData(FALSE);
 m_frame->m_something_changed = 1;
 //m_frame->m_rebuild_formtree = 1;//TK - left tree in form view
 m_pDoc->UpdateAllViews(NULL);
 Invalidate(TRUE);
}

void CGSFormRightPassive::GetPcsMinmax()
{
  long i=0, j=0;
  double value;
  int nb_processes = 0;
  CString pcs_name;
  CString pcs_value_name;
  m_pcs_min = 1.e+19;
  m_pcs_max = -1.e+19;
 
  CRFProcess* m_process = NULL;
  nb_processes = (int)pcs_vector.size();
  for(i=0;i<nb_processes;i++)
  {
      m_process = pcs_vector[i];
      pcs_name = m_process->pcs_type_name.data();
      if (m_process->pcs_primary_function_name[0]== m_pcs_name)
      {
        int nidx = m_process->GetNodeValueIndex((string)m_pcs_name);
        for(j=0;j<(long)m_process->nod_val_vector.size();j++){
        value = m_process->GetNodeValue(j,nidx);
        if(value<m_pcs_min) m_pcs_min = value;
        if(value>m_pcs_max) m_pcs_max = value;
        }


      }  
  }
    CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
    mainframe->m_pcs_min = m_pcs_min;
    mainframe->m_pcs_max = m_pcs_max;
    mainframe->m_something_changed = 1;
}

void CGSFormRightPassive::OnBnClickedSetPcsMinmaxButton2()
{
    UpdateData(TRUE);
    CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
    CGeoSysDoc *m_pDoc = GetDocument();
    m_frame->m_pcs_min = m_pcs_min;
    m_frame->m_pcs_max = m_pcs_max;
    m_frame->m_something_changed = 1;
    m_pDoc->UpdateAllViews(NULL);
    Invalidate(TRUE);
}

void CGSFormRightPassive::OnBnClickedValuePointsButton()
{
    CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
    CGeoSysDoc *m_pDoc = GetDocument();
    m_frame->move_distance = 0;
    m_frame->pcs_value_distort_factor = 0;
    if (m_frame->m_pcs_values_mesh != 1)
    m_frame->m_pcs_values_mesh = 1;
    else m_frame->m_pcs_values_mesh = 0;
    m_frame->m_something_changed = 1;
    m_pDoc->UpdateAllViews(NULL);
    Invalidate(TRUE);


}

void CGSFormRightPassive::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
 
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

    CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
    CGeoSysDoc *m_pDoc = GetDocument();
 
    m_slider1.SetRangeMin(-100);
    m_slider1.SetRangeMax(100);
    int max = m_slider1.GetRangeMax();
    int min = m_slider1.GetRangeMin(); 
    int pos = m_slider1.GetPos();

    if (pos != m_frame->move_distance)
    {
    m_frame->move_distance = pos;
    
    //START UpdateSpecificViews   
    m_frame-> UpdateSpecificView("COGLView", m_pDoc);

    

    }
    *pResult = 0;


}



void CGSFormRightPassive::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

    CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
    CGeoSysDoc *m_pDoc = GetDocument();
 
    m_slider2.SetRangeMin(-100);
    m_slider2.SetRangeMax(100);
    int max = m_slider2.GetRangeMax();
    int min = m_slider2.GetRangeMin();
    int pos = m_slider2.GetPos();

    if (pos != m_frame->pcs_value_distort_factor)
    {
        m_frame->pcs_value_distort_factor = pos;
    
    //START UpdateSpecificViews   
    m_frame-> UpdateSpecificView("COGLView", m_pDoc);

    

    }
    *pResult = 0;


}

void CGSFormRightPassive::OnBnClickedButton1()
{
	if(m_pmodeless)
		m_pmodeless->SetForegroundWindow();
	else
	{
		m_pmodeless = new CIsoControl(this);
		m_pmodeless->Create(CIsoControl::IDD,GetDesktopWindow());
		m_pmodeless->ShowWindow(SW_SHOW);
	}
}
