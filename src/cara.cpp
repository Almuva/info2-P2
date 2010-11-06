#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPlane.h"//
#include "vtkMath.h"//
#include "vtkLine.h"//
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

double distanceToAveragePlain(double A[3],double Pn[3],double Po[3]){
	return vtkPlane::DistanceToPlane(A,Pn,Po);
}

void normalFrom3Points(const double A[3],const double B[3],const double C[3],double Pn[3]){
	//Normal = (p2-p1)x(p3-p1) <- multiplicacio en creu

	double v1[3],v2[3];
	for(int i=0;i<3;i++){v1[i]=B[i]-A[i];v2[i]=C[i]-A[i];}
	vtkMath::Cross(v1,v2,Pn);// <- multiplicacio en creu
	
	//normalitza
	vtkMath::Normalize(Pn);
}

void averageNormal(const double Normals[][3],const int N,double AN[3]){
	AN[0]=AN[1]=AN[2]=0;
	for(int i=0;i<N;i++){AN[0]+=Normals[i][0];AN[1]+=Normals[i][1];AN[2]+=Normals[i][2];}
	vtkMath::Normalize(AN);
}

double distanceToLine(double A[3],double P1[3],double P2[3]){
	return sqrt(vtkLine::DistanceToLine(A,P1,P2));//<-Per comparar no caldria fer sqrt.
}

double distanceToEdge(){//double A[3],double Pts[][3],int N){
	double d=vtkMath::Inf();
	return d;
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
	fprintf(stderr,"\n El total de puntos de la sfcie es: %ld \n",N);


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
		fprintf(stderr,"\n El punto %d esta en el triangulo %d \n",(int)pointIds->GetId(n_pids),(int)cellIds->GetId(0));
	else
		fprintf(stderr,"\n El punto %d no esta en el triangulo %d \n \n",(int)pointIds->GetId(n_pids),(int)cellIds->GetId(0));

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
	static double A[]={0,0,9};
	static double x1[]={0,0,0};
	static double x2[]={0,2,0};
	static double x3[]={2,2,0};
	static double x4[]={45,1,-0.1};
	static double x5[]={60,9,-0.1};
	static double x6[]={3,2,-0.1};
	double AN[3];
	double Pn[3][3];
	static double* Po=x1;

	normalFrom3Points(x1,A,x2,Pn[0]);
	normalFrom3Points(x2,A,x3,Pn[1]);
	normalFrom3Points(x3,A,x1,Pn[2]);

	averageNormal(Pn,3,AN);
	double dP=distanceToAveragePlain(A,AN,Po);
	double dL=distanceToLine(A,x1,x2);
	//double E[3][3]={x1,x2,x3};
	double dE=distanceToEdge();

	fprintf(stderr,"Normal: %f %f %f\n\n",AN[0],AN[1],AN[2]);
	fprintf(stderr,"Distancia al plano: %f\n\n",dP);
	fprintf(stderr,"Distancia a la linea: %f\n\n",dL);
	fprintf(stderr,"Distancia al borde: %f\n\n",dE);
}

