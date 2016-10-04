#include "vtkInteractorStyle3dContour2.h"

#include <vtkProperty.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkMapper.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataMapper.h>

#include "CutAlongPolyLineFilter.h"

#include <string>

vtkStandardNewMacro(vtkInteractorStyle3dContour2);

using namespace std;

void vtkInteractorStyle3dContour2::EnableContourWidget(bool flag)
{
	if (m_enableFlag == flag || m_mainActor == nullptr)
		return;
	m_enableFlag = flag;
	if (m_enableFlag == false) {
		if (m_contourWidget != nullptr) {
			m_contourWidget->EnabledOff();
			m_contourWidget->Off();
		}
	}
	else {
		if (m_contourWidget == nullptr) {

			vtkSmartPointer<vtkPolygonalSurfacePointPlacer> pointPlacer =
				vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
			pointPlacer->AddProp(m_mainActor);



			m_contourWidgetRep = vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
			m_contourWidgetRep->SetLineInterpolator(vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator>::New());
			m_contourWidgetRep->SetHandleSize(10);
			m_contourWidgetRep->GetLinesProperty()->SetColor(1, 0, 0);
			m_contourWidgetRep->GetLinesProperty()->SetLineWidth(3.0);
			m_contourWidgetRep->SetPointPlacer(pointPlacer);



			m_contourWidget = vtkSmartPointer<vtkContourWidget>::New();
			m_contourWidget->SetInteractor(this->Interactor);
			m_contourWidget->SetRepresentation(m_contourWidgetRep);

		}
		m_contourWidget->EnabledOn();
		m_contourWidget->On();
		m_contourWidget->Initialize();
	}
	this->Interactor->Render();
}
#include <vtkXMLPolydataWriter.h>
void vtkInteractorStyle3dContour2::CutAlongLinks()
{
	if (m_mainActor == nullptr || m_contourWidgetRep == nullptr) {
		return;
	}
	vtkSmartPointer<CutAlongPolyLineFilter> cutter =
		vtkSmartPointer<CutAlongPolyLineFilter>::New();

	cutter->SetInputData(0, m_mainActor->GetMapper()->GetInput());
	cutter->SetInputData(1, m_contourWidgetRep->GetContourRepresentationAsPolyData());
	cutter->SetClipTolerance(0.1);
	cutter->Update();

	vtkSmartPointer<vtkPolyDataConnectivityFilter > conn =
		vtkSmartPointer<vtkPolyDataConnectivityFilter >::New();
	conn->SetInputConnection(cutter->GetOutputPort());
	conn->ColorRegionsOn();
	//conn->SetExtractionModeToAllRegions();
	conn->SetExtractionModeToLargestRegion();
	conn->DeleteSpecifiedRegion(1);
	conn->Update();

	// Write the file
	vtkSmartPointer<vtkXMLPolyDataWriter> writer =
		vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetFileName("test.vtp");
#if VTK_MAJOR_VERSION <= 5
	writer->SetInput(polydata);
#else
	writer->SetInputData(conn->GetOutput());
#endif

	// Optional - set the mode. The default is binary.
	//writer->SetDataModeToBinary();
	//writer->SetDataModeToAscii();

	writer->Write();

	//vtkSmartPointer<vtkGeometryFilter> geom = vtkSmartPointer<vtkGeometryFilter>::New();
	//geom->SetInputConnection(conn->GetOutputPort());
	//geom->Update();

	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(m_mainActor);

	m_mainActor = vtkSmartPointer<vtkActor>::New();
	m_mainActor->SetMapper(vtkSmartPointer<vtkPolyDataMapper>::New());
	m_mainActor->GetMapper()->SetInputConnection(conn->GetOutputPort());
	m_mainActor->GetMapper()->Update();
	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(m_mainActor);


	this->Interactor->Render();
}
void vtkInteractorStyle3dContour2::OnKeyPress()
{
	std::string key = this->Interactor->GetKeySym();
	cout << key << endl;
	if (key == "D") {
		EnableContourWidget(false);
		//ClearLinks();
		cout << "EnableContourWidget" << endl;
	}
	else if (key == "F") {
		CutAlongLinks();
		cout << "CutAlongLinks" << endl;
	}
}

void vtkInteractorStyle3dContour2::OnLeftButtonDown()
{
	// set the m_mainActor by the first click
	if (m_mainActor == nullptr) {
		vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
		propPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1],
			0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		m_mainActor = propPicker->GetActor();
	}

	AbstractInteractorStyle3D::OnLeftButtonDown();
	EnableContourWidget(true);
}

vtkInteractorStyle3dContour2::vtkInteractorStyle3dContour2()
{
}


vtkInteractorStyle3dContour2::~vtkInteractorStyle3dContour2()
{
}
