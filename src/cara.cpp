#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPlane.h"//
#include "vtkMath.h"//
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"

#include <math.h>

#include "vtkPolyDataReader.h"
#include "vtkPolyDataNormals.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include <vtkStructuredPoints.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

double distanceToAveragePlain(double* A,double* Pn,double* Po){
	return vtkPlane::DistanceToPlane(A,Pn,Po);
}

void normalFrom3Points(double* A,double* B,double* C,double* Pn){

	double v1[3],v2[3];
	for(int i=0;i<3;i++){v1[i]=B[i]-A[i];v2[i]=C[i]-A[i];}
	vtkMath::Cross(v1,v2,Pn);

	//normalitza
	vtkMath::Normalize(Pn);
}

void averageNormal(){
	/*Per implementar*/
}

int main( int argc, char *argv[] )
{
	/* inicializacion objetos graficos */
	vtkRenderer *renderer = vtkRenderer::New();
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(renderer);
	
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	/* leemos de disco la sfcie. poligonal, la guardamos en cara_sfcie */
	vtkPolyDataReader * cara=vtkPolyDataReader::New();
	cara->SetFileName("../vtkdata/fran_cut2.vtk");
	cara->Update();
	vtkPolyData * cara_sfcie=cara->GetOutput();
	cara_sfcie->Update();
	cara_sfcie->BuildCells();
	cara_sfcie->BuildLinks();


	/* ejemplo: son vecinos los nodos con Id=1 y Id=5 ? */
	if(( cara_sfcie->IsEdge(1,5))!=0)
		fprintf(stderr,"\n Los nodos 1 y 5 son vecinos \n");
	else
		fprintf(stderr,"\n Los nodos 1 y 5 no estan unidos \n");
	
	/* Total de puntos de la sfcie: */
	vtkPoints * verts = cara_sfcie->GetPoints();
	long N=verts->GetNumberOfPoints();
	fprintf(stderr,"\n El total de puntos de la sfcie es: %d \n",N);


	/* creo una lista de Id's, guardo en ella los Id's de las celdas
		 (triangulos) a los que pertenece el punto con Id=13 */
	vtkIdList * cellIds=vtkIdList::New();
	cara_sfcie->GetPointCells(13,cellIds);
	int n_cids=cellIds->GetNumberOfIds(); //la lista tendra n_cids elementos


	/* creo una lista de Id's, guardo en ella los Id's de los nodos
		 que hay en la celda (triangulo) que esta al final de la lista
		 anterior */
	vtkIdList * pointIds=vtkIdList::New();
	cara_sfcie->GetCellPoints(cellIds->GetId(n_cids-1),pointIds);
	int n_pids=pointIds->GetNumberOfIds(); //la lista tendra n_pids elementos

	/* es el ultimo punto de la lista pointIds usado por el primer triangulo
		 de la lista cell_Ids ? */
	if(cara_sfcie->IsPointUsedByCell(pointIds->GetId(n_pids),cellIds->GetId(0))!=0)
		fprintf(stderr,"\n El punto %d esta en el triangulo %d \n",pointIds->GetId(n_pids),cellIds->GetId(0));
	else
		fprintf(stderr,"\n El punto %d no esta en el triangulo %d \n \n",pointIds->GetId(n_pids),cellIds->GetId(0));

	/* en una lista, pongo el valor Id	en la posicion pos */
	int pos=n_pids-1;
	int Id=58;
	pointIds->InsertId(pos,Id);


	/* calculo de normales para shading */
	vtkPolyDataNormals *normals = vtkPolyDataNormals::New();
	normals->SetInput(cara_sfcie);
	normals->SetFeatureAngle(45);
	normals->Update();
	

	/*mapper */
	vtkPolyDataMapper *cara_sfcieMapper = vtkPolyDataMapper::New();
	//comentamos la siguiente linea para que no haya shading
	//	cara_sfcieMapper->SetInput(normals->GetOutput());
	cara_sfcieMapper->SetInput(cara_sfcie);
	vtkActor *cara_sfcieActor = vtkActor::New();
	cara_sfcieActor->SetMapper(cara_sfcieMapper);


	renderer->AddActor(cara_sfcieActor);
	renderer->SetBackground(0.0,0.25,0.25);
	renWin->SetSize(300,300);

	// interact with data
	renWin->Render();

	iren->Start();

	// Clean up
	renderer->Delete();
	renWin->Delete();
	iren->Delete();
	cara_sfcieMapper->Delete();
	cara_sfcieActor->Delete();

/*edu*/
	static double A[]={1,1,1};
	static double x1[]={1,2,0};
	static double x2[]={7,1,0};
	static double x3[]={4,5,0};
	double Pn[3];
	static double* Po=x1;

	normalFrom3Points(x1,x2,x3,Pn);
	averageNormal();
	double d=distanceToAveragePlain(A,Pn,Po);

	fprintf(stderr,"Normal: %f %f %f\n\n",Pn[0],Pn[1],Pn[2]);
	fprintf(stderr,"Distancia: %f\n\n",d);
}

