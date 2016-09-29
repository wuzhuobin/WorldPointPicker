#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkConnectivityFilter.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include "CutAlongPolyLineFilter.h"

vtkActor* GetActor(vtkPolyData*);

int main() {
	// Load data
	vtkSmartPointer<vtkXMLPolyDataReader> xmlReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
	vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();

	xmlReader->SetFileName("C:/Users/lwong/PycharmProjects/MyUtilites/data/cutLineForZhouguixian.vtp");
	stlReader->SetFileName("C:/Users/lwong/PycharmProjects/MyUtilites/data/Zhouguixian.stl");
	
	xmlReader->Update();
	stlReader->Update();

	vtkSmartPointer<vtkPolyData> cutline = vtkSmartPointer<vtkPolyData>::New();
	cutline->DeepCopy(xmlReader->GetOutput());
	cutline->GetPointData()->RemoveArray(0);
	cutline->GetCellData()->RemoveArray(0);

	vtkSmartPointer<CutAlongPolyLineFilter> cutter = vtkSmartPointer<CutAlongPolyLineFilter>::New();
	cutter->SetInputData(0, stlReader->GetOutput());
	cutter->SetInputData(1, xmlReader->GetOutput());
	cutter->SetClipTolerance(0.5);
	cutter->Update(); 

	vtkPolyData* output = cutter->GetOutput();
	output->Print(cout);

	vtkSmartPointer<vtkConnectivityFilter> conn = vtkSmartPointer<vtkConnectivityFilter>::New();
	conn->SetInputConnection(cutter->GetOutputPort());
	conn->ColorRegionsOn();
	conn->SetExtractionModeToAllRegions();
	conn->Update();

	vtkSmartPointer<vtkGeometryFilter> geom = vtkSmartPointer<vtkGeometryFilter>::New();
	geom->SetInputConnection(conn->GetOutputPort());
	geom->Update();

	vtkActor* lineActor;
	lineActor = GetActor(cutline);
	lineActor->GetProperty()->SetColor(1, 1, 0);

	vtkActor* handActor;
	handActor = GetActor(geom->GetOutput());
	handActor->GetProperty()->SetRepresentationToWireframe();

	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renwin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	ren->AddActor(lineActor);
	ren->AddActor(handActor);

	renwin->AddRenderer(ren);
	iren->SetRenderWindow(renwin);
	iren->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
	iren->Initialize();
	renwin->Render();
	iren->Start();
	return 0;	
}

vtkActor* GetActor(vtkPolyData *pd)
{
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(pd);
	mapper->Update();

	vtkActor* actor = vtkActor::New();
	actor->SetMapper(mapper);
	return actor;
}