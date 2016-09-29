#include "vtkInteractorStyle3dContour.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPropPicker.h>
#include <vtkSmartPointer.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkProperty.h>

#include <vtkAppendPolyData.h>

#include "CutAlongPolyLineFilter.h"

vtkStandardNewMacro(vtkInteractorStyle3dContour);

using namespace std;

void vtkInteractorStyle3dContour::OnLeftButtonDown()
{
	
	AbstractInteractorStyle3D::OnLeftButtonDown();
}

void vtkInteractorStyle3dContour::OnKeyPress()
{
	std::string key = this->Interactor->GetKeySym();
	cout << key << endl;
	if (key == "Return") {
		DropSpheres();
	}
	else if (key == "Escape") {
		ClearSpheres();
		ClearLinks();
		cout << "clear" << endl;
	}
	else if (key == "space") {
		LinkSpheres();
		cout << "LinkSpheres" << endl;
	}
	else if (key == "L") {
		CutAlongLinks();
		cout << "L" << endl;
	}

	AbstractInteractorStyle3D::OnKeyPress();
}

vtkInteractorStyle3dContour::vtkInteractorStyle3dContour()
{
}


vtkInteractorStyle3dContour::~vtkInteractorStyle3dContour()
{
}

void vtkInteractorStyle3dContour::DropSpheres()
{

	std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;

	vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
	propPicker->Pick(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1],
		0,  // always zero.
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	double picked[3];
	propPicker->GetPickPosition(picked);
	std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;

	if (propPicker->GetActor() == NULL) {
		return;
	}
	m_mainActor = propPicker->GetActor();

	vtkSmartPointer<vtkKdTreePointLocator> kDTree =
		vtkSmartPointer<vtkKdTreePointLocator>::New();
	kDTree->SetDataSet(m_mainActor->GetMapper()->GetInput());
	kDTree->BuildLocator();
	vtkIdType id = kDTree->FindClosestPoint(picked);


	cout << "sphere center id: " << id << endl;
	// add a point to the list
	m_sphereCenterPointIds.push_back(id);

	m_mainActor->GetMapper()->GetInput()->GetPoint(id, picked);

	//Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(picked);
	sphereSource->SetRadius(1);


	//Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// useless
	actor->PickableOff();

	this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	m_sphereActors.push_back(actor);


	this->Interactor->Render();

}

void vtkInteractorStyle3dContour::ClearSpheres()
{
	m_sphereCenterPointIds.clear();

	for (list<vtkSmartPointer<vtkActor>>::const_iterator cit = m_sphereActors.cbegin();
		cit != m_sphereActors.cend(); ++cit) {
		if ((*cit) != nullptr) {
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(*cit);
		}
	}
	m_sphereActors.clear();
	this->Interactor->Render();
}
#include <vtkXMLPolyDataWriter.h>
void vtkInteractorStyle3dContour::LinkSpheres()
{
	ClearLinks();

	if (m_mainActor == nullptr) {
		return;
	}
	//Append the multi links
	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();

	for (list<vtkIdType>::const_iterator cit1 = m_sphereCenterPointIds.cbegin(), cit2 = ++m_sphereCenterPointIds.cbegin();
		cit1 != (m_sphereCenterPointIds.cend()); ++cit1, ++cit2) {
		cout << "sphere center id: " << *cit1 << endl;

		vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra =
			vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
		dijkstra->SetInputData(m_mainActor->GetMapper()->GetInput());

		if (cit1 == --m_sphereCenterPointIds.cend()) {
			dijkstra->SetStartVertex(*cit1);
			dijkstra->SetEndVertex(*m_sphereCenterPointIds.cbegin());
			dijkstra->Update();

			// for not out of index 
			--cit2;
		}
		else {
			dijkstra->SetStartVertex(*cit1);
			dijkstra->SetEndVertex(*cit2);
			dijkstra->Update();
		}

		// multiple actor method
		vtkSmartPointer<vtkPolyDataMapper> pathMapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		pathMapper->SetInputConnection(dijkstra->GetOutputPort());
		pathMapper->Update();

		vtkSmartPointer<vtkActor> linkActor =
			vtkSmartPointer<vtkActor>::New();
		linkActor->SetMapper(pathMapper);
		linkActor->GetProperty()->SetColor(1, 0, 0); // Red
		linkActor->GetProperty()->SetLineWidth(4);

		m_linkActors.push_back(linkActor);

		// single actor method
		//appendFilter->SetInputData(dijkstra->GetOutput());

		appendFilter->AddInputConnection(dijkstra->GetOutputPort());
	}
	appendFilter->Update();
	vtkSmartPointer<vtkPolyDataMapper> linkMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	linkMapper->SetInputConnection(appendFilter->GetOutputPort());
	linkMapper->Update();

	m_linkActorAppended =
		vtkSmartPointer<vtkActor>::New();
	m_linkActorAppended->SetMapper(linkMapper);
	m_linkActorAppended->GetProperty()->SetColor(1, 0, 0); // Red
	m_linkActorAppended->GetProperty()->SetLineWidth(4);
	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(m_linkActorAppended);

	this->Interactor->Render();

}

void vtkInteractorStyle3dContour::ClearLinks()
{
	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(m_linkActorAppended);

	for (list<vtkSmartPointer<vtkActor>>::const_iterator cit = m_linkActors.cbegin();
		cit != m_linkActors.cend(); ++cit) {
		if ((*cit) != nullptr) {
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(*cit);
		}
	}
	m_linkActors.clear();
	this->Interactor->Render();

}

void vtkInteractorStyle3dContour::CutAlongLinks()
{
	if (m_mainActor == nullptr) {
		return;
	}
	vtkSmartPointer<CutAlongPolyLineFilter> cutter = 
		vtkSmartPointer<CutAlongPolyLineFilter>::New();
}
