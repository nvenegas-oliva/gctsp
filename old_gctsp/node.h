/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
#include "glob.h"

class node {
private:

    /* Estructura de búsqueda, necesaria para poder utilizar
       la función find( ) de un set de "node".
     */
    struct busca {
        //Dentro de la estructura de busqueda, implemento una cabecera de función para luego llamarla

        busca( int id ) :
        id( id ) {
        }

        bool operator()(node * node) const {
            return node->getId( ) == id;
        }

        //Esto es un atributo, sirve para referenciarlo como parámetro a la función
        int id;
    };

    //Atriutos de cada nodo
    int id;
    float x;
    float y;

public:

    //Constructor

    node( int _id, float _x, float _y ) {
        id = _id;
        x = _x;
        y = _y;
    }

    node( ) {

    }

    virtual ~node( );
    void setId( int _id );

    void setX( float _x );

    void setY( float _y );

    int getId( );

    float getX( );

    float getY( );

    //Muestra el contenido del set "vec"
    void show_set( set < int > vec );

    //Muestra las asociaciones entre nodos
    void show_associations( map < int, set < int > > associations );

    //Carga la informacion de los nodos y retorna el parámetro P
    int load( const char * file_name, set <node * > *suppliers, set <node * > *customers,
            map < int, set <int > > *associations, node *depot );

    //Muestra el contenido de un contenedor:
    void show_container( set < node* > container );
    vector<vector <float> > getSuppliers( set < node* > vec, float x, float y, vector<vector <float> > &tp );

    // Función que calcula el fitness
    float objective_function( vector<unsigned> &suppliers, set < node* > &container, node *depot, int P, map < int, set < int > > &associations, int *puntero );
    // Calcular Price
    int calcularPrice( vector<unsigned> &chrm, int P, map < int, set < int > > *associations );

    // calcular distancia de dos puntos
    float calculateDistance( float x1, float y1, float x2, float y2 );

    //graficar salida
    void graficar(string outName, vector<unsigned> &chrm, set <node * > &suppliers, set < node* > &customers, node *depot, int P, map < int, set < int > > &associations);

};
