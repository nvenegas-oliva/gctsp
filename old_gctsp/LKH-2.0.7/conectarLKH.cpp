/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>


// LKH
extern "C" int lkh_dll(int argc, char *argv[], int *MejorTour, float **coordenadas, int n);
using namespace std;


/* solo para probar */
int optimoSolucion;
int Numciudades;
float **coord;


/**
 * Funcion que lee los datos de una instancia y los pasa a las variables.
 *
 **/
int leerInstancia(string ruta_instancia)
{
  FILE *fp;
  int i;

  if((fp = fopen (ruta_instancia.c_str(), "r")) == NULL)
    {
      cout << "No se pudo cargar " << ruta_instancia << endl;
      exit(0);
    }

  if(fscanf(fp, "OPTIMO:%d\t\n", &optimoSolucion) == EOF)
    cout << "TSP: no pudo leer los datos (optimo) de la instancia " << ruta_instancia << endl;
  //Lee la cantidad de ciudades
  if(fscanf(fp, "DIMENSION:%d\t\n", &Numciudades) == EOF)
    cout << "TSP: no pudo leer los datos (dimension) de la instancia " << ruta_instancia << endl;

  coord = (float **) malloc(Numciudades * sizeof(float *));
  for(i = 0; i < Numciudades; i++)
    coord[i] = (float *) malloc(2 * sizeof(float));


  i = 0;
  //carga lista de secuencia de ciudades.
  while (!feof(fp)) {
    int lala;
    if (i >= Numciudades)
      break;

    if(fscanf(fp, "%d\t%f\t%f\n", &lala, &coord[i][0], &coord[i][1]) == EOF)
      cout << "TSP: no pudo leer los datos (nombre ciudad, x, y) de la instancia " << ruta_instancia << endl;

    /* printf("%d %lf %lf\n", tsp->NombreCiudad, tsp->x, tsp->y); */

    i++;
  }

  fclose(fp);

  return Numciudades;
}

int main(int argc, char *argv[])
{
  stringstream stream2;
  stringstream stream3;
  stream2 << "Giant_Parameters" << 1 <<".txt";
  stream3 << "Giant_Tour" << 1 <<".txt";
  string parametro2 = stream2.str();
  string parametro3 = stream3.str();
  ofstream Parameters((parametro2).c_str());
  Parameters << "PROBLEM_FILE = lala.tsp" << endl;
  Parameters << "TRACE_LEVEL = 0" << endl;
  Parameters << "GAIN23 = NO" << endl;
  Parameters << "RUNS = 1" << endl;
  Parameters << "OUTPUT_TOUR_FILE " << parametro3 << endl;
  Parameters.close();
  char dll_exe[] = "lkh_dll";
  char dll_par[1024];
  strcpy(dll_par, ((parametro2).c_str()));
  char *dll_argv[] = { dll_exe , dll_par };
  /* cout << dll_argv[1]; */

  for(unsigned k = 0; k < 1; k++)
    {

      int dimension = leerInstancia("kroC100.tsp");

      /* ruta final */
      int *tour = (int *) malloc(dimension * sizeof(int));

      printf("intento %d\n", k);

      for(unsigned i = 0; i < dimension; i++)
        cout << coord[i][0] << " " << coord[i][0] << endl;

      printf("resultado %d\n", lkh_dll(2 , dll_argv, tour, coord, dimension));


      for(int i = 0; i < dimension; i++)
        printf("%d ", tour[i]);
      printf("\n");
      free(tour);
      for(int i = 0; i < Numciudades; i++)
        free(coord[i]);
      free(coord);

    }
  return 0;
}
