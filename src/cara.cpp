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

void normalFrom3Points(const double L[3],const double R[3],const double A[3],double N[3]){
	//Normal = (p2-p1)x(p3-p1) <- multiplicacion cruzada
	//regla de la mano derecha -> izquierda x derecha = -; derecha x izquierda = +;

	double v1[3],v2[3];
	for(int i=0;i<3;i++){v1[i]=R[i]-A[i];v2[i]=L[i]-A[i];}
	vtkMath::Cross(v1,v2,N);// <- multiplicacio en creu, resultat a N
	//vtkMath::Normalize(N);//no se normaliza porque no da buen resultado
}

void averageNormal(const double Normals[][3],const int num,double avgN[3]){
	/*sumando todas las normales obtenemos un vector de igual direccion
	al que tendria la base de la piramide*/

	avgN[0]=avgN[1]=avgN[2]=0;//averageNormal=0
	for(int i=0;i<num;i++){
		avgN[0]+=Normals[i][0];avgN[1]+=Normals[i][1];avgN[2]+=Normals[i][2];
	}
	vtkMath::Normalize(avgN);//normaliza la suma de todas las normales
}

void averagePoint(const double Points[][3],const int num,double avgP[3]){
	/*sumando todos los puntos i normalizando obtenemos un punto
	por donde pasa el plano medio*/

	avgP[0]=avgP[1]=avgP[2]=0;//averagePoint=0
	for(int i=0;i<num;i++){
		avgP[0]+=Points[i][0];avgP[1]+=Points[i][1];avgP[2]+=Points[i][2];
	}
	for(int i=0;i<3;i++){avgP[i]/=(double)num;}
}

void AveragePlane(double A[3],double Points[][3],const unsigned int num,
	double avgN[3],double avgP[3]){
	
	double Normals[num][3];//variable local

	//calcular la normal de cada cara del la piramide poco regular
	for(unsigned int i=0;i<num-1;i++){
		normalFrom3Points(Points[i],Points[i+1],A,Normals[i]);
	}
	normalFrom3Points(Points[num-1],Points[0],A,Normals[num-1]);//cerrar circulo

	//se suman todas las anteriores normales i se normaliza
	averageNormal(Normals,num,avgN);
	//sumando todos los puntos i se normaliza
	averagePoint(Points,num,avgP);

	//informacion para debugar, quitar
	/*fprintf(stderr,"Punto A[%f,%f,%f]\nNormal[%f,%f,%f] Punto Medio[%f,%f,%f]\n\n",
		A[0],A[1],A[2],avgN[0],avgN[1],avgN[2],avgP[0],avgP[1],avgP[2]);*/
}

double distanceToAveragePlane(double A[3],double Points[][3],const unsigned int num){
	/*calcula el plano medio (a partir de [num] puntos)
	i mide la distancia de A hasta este*/
	double Normals[num][3],PlaneNormal[3],PlanePoint[3];//variables locales

	AveragePlane(A,Points,num,PlaneNormal,PlanePoint);

	return vtkPlane::DistanceToPlane(A,PlaneNormal,PlanePoint);
}

double distanceToLine(double A[3],double P1[3],double P2[3]){
	return sqrt(vtkLine::DistanceToLine(A,P1,P2));
	//Para comparar no hace falta hacer la raiz cuadrada.
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
	static double A[]={1,2,3};//Punto a eliminar o no
	//los otros puntos, forman la base
	static double x1[]={0,0,0};
	static double x2[]={0,0,1};
	static double x3[]={0,0,2};
	static double x4[]={0,1,2};
	static double x5[]={0,2,2};
	static double x6[]={0,2,1};
	static double x7[]={0,2,0};
	static double x8[]={0,1,0};
	const int num=8;
	double AllPoints[num][3];
	static double* Po=x1;
	for(int i=0;i<3;i++){
		AllPoints[0][i]=x1[i];
		AllPoints[1][i]=x2[i];
		AllPoints[2][i]=x3[i];
		AllPoints[3][i]=x4[i];
		AllPoints[4][i]=x5[i];
		AllPoints[5][i]=x6[i];
		AllPoints[6][i]=x7[i];
		AllPoints[7][i]=x8[i];
	}

	double dP=distanceToAveragePlane(A,AllPoints,num);
	double dL=distanceToLine(A,x1,x2);

	fprintf(stderr,"Distancia al plano: %f\n\n",dP);
	fprintf(stderr,"Distancia a la linea: %f\n\n",dL);
}
