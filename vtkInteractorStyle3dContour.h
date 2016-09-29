#pragma once


#include <list>

#include <vtkPoints.h>

#include "AbstractInteractorStyle3D.h"


class vtkInteractorStyle3dContour :
	public AbstractInteractorStyle3D
{
public:
	vtkTypeMacro(vtkInteractorStyle3dContour, AbstractInteractorStyle3D);
	static vtkInteractorStyle3dContour* New();

	virtual void OnLeftButtonDown();
	virtual void OnKeyPress();


protected:
	vtkInteractorStyle3dContour();
	~vtkInteractorStyle3dContour();

	virtual void DropSpheres();
	virtual void ClearSpheres();
	virtual void LinkSpheres();
	virtual void ClearLinks();
	virtual void CutAlongLinks();


	std::list<vtkSmartPointer<vtkActor>> m_sphereActors;
	std::list<vtkSmartPointer<vtkActor>> m_linkActors;
	vtkSmartPointer<vtkActor> m_linkActorAppended;
	std::list<vtkIdType> m_sphereCenterPointIds;
	vtkActor* m_mainActor = nullptr;

};

