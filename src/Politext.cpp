#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"

#include <math.h>

#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include <vtkStructuredPoints.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

int main( int argc, char *argv[] )
{

  static float puntos[5][3]={{0,0,0.5}, {1,0,0.5}, {1,1,0.5}, {0,1,0.5}, {0.5,0.5,1}};

  static int celdas[4][3]={{0,1,4},{1,2,4},{2,3,4},{3,0,4} };
    

  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);
    
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  vtkPolyData *piramide = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *polys = vtkCellArray::New();
  vtkFloatArray *scalars = vtkFloatArray::New();

  /* inserto puntos y conectividad en la piramide */
  for (int i=0; i<5; i++) points->InsertPoint(i,puntos[i]);
  for (int i=0; i<4; i++) polys->InsertNextCell(3,celdas[i]);
  for (int i=0; i<5; i++) scalars->InsertValue(i,0.0); 
  piramide->SetPoints(points);
  points->Delete();
  piramide->SetPolys(polys);
  polys->Delete();
  piramide->GetPointData()->SetScalars(scalars);
  scalars->Delete();


  /* estas operaciones son imprescindibles si queremos recorrer la sfcie:
     saber si dos puntos son vecinos, saber a que triangulos pertenece 
     un punto, etc. */
  piramide->BuildCells();
  piramide->BuildLinks();



  /* mapper */  
  vtkPolyDataMapper *piramideMapper = vtkPolyDataMapper::New();
  piramideMapper->SetInput(piramide);
  piramideMapper->ScalarVisibilityOff();
  vtkActor *piramideActor = vtkActor::New();
  piramideActor->SetMapper(piramideMapper);
  

  /* camara */  
  vtkCamera *camera = vtkCamera::New();
  camera->SetPosition(0,0,0);
  camera->SetFocalPoint(1,1,1);
  renderer->AddActor(piramideActor);
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->SetBackground(0.0,0.25,0.25);
  
  renWin->SetSize(300,300);

  // interact with data
  renWin->Render();

  iren->Start();

  // Clean up
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  piramide->Delete();
  points->Delete();
  polys->Delete();
  scalars->Delete();
  piramideMapper->Delete();
  camera->Delete();
}
