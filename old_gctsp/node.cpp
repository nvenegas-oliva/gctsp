/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
#include "glob.h"
#include "node.h"

node::~node( )
{

}

void node::setId( int _id )
{
    id = _id;
}

void node::setX( float _x )
{
    x = _x;
}

void node::setY( float _y )
{
    y = _y;
}

int node::getId( )
{
    return id;
}

float node::getX( )
{
    return x;
}

float node::getY( )
{
    return y;
}

//Carga la informacion de los nodos y retorna el parámetro P

int node::load( const char * file_name,
                set <node * > *suppliers,
                set <node * > *customers,
                map < int, set <int > > *associations,
                node *depot )
{
    ifstream file( file_name );
    //verifico apertura de archivo
    assert( file.is_open( ) );
    //Variables auxiliares
    string line;
    int counter = 0, bit = 0;
    float a[3];
    int P = 0;
    //proceso:
    while ( getline( file, line ) ) {

        if ( line.size( ) == 1 ) {
            counter++;
            continue;
        }

        //Asociaciones:
        if ( counter == 12 ) {
            set < int > vec;
            bit = 0;
            //Id del nodo customer
            int id;
            //Id del nodo asociado a customer
            int a_id;
            stringstream tmp( line );
            while ( tmp.good( ) ) {
                if ( bit > 0 ) {
                    tmp >> a_id;
                    //Asocio:
                    if ( bit > 1 ) {
                        vec.insert( a_id );
                    }
                }
                if ( bit == 0 ) {
                    tmp >> id;
                }
                bit++;
            }
            associations->insert( make_pair( id, vec ) );
        }

        //Customers:
        if ( counter == 10 ) {
            bit = 0;
            stringstream tmp( line );
            node *n = new node( );
            while ( tmp.good( ) ) {
                tmp >> a[bit];
                bit++;
            }
            n->setId( ( int ) a[0] );
            n->setX( a[1] );
            n->setY( a[2] );
            customers->insert( n );
        }

        //Suppliers:
        if ( counter == 8 ) {
            bit = 0;
            stringstream tmp( line );
            node *n = new node( );

            while ( tmp.good( ) ) {
                tmp >> a[bit];
                bit++;
            }
            n->setId( ( int ) a[0] );
            n->setX( a[1] );
            n->setY( a[2] );

            //Inserto al contenedor:
            suppliers->insert( n );
        }

        //Nodo depósito
        if ( counter == 7 ) {
            bit = 0;
            stringstream tmp( line );
            while ( tmp.good( ) ) {
                tmp >> a[bit];
                bit++;
            }
            depot->setId( ( int ) a[0] );
            depot->setX( a[1] );
            depot->setY( a[2] );
        }

        //linea 4: customers a cubrir (parámetro p) --> importante
        if ( counter == 4 ) {
            // cout << "Customers to cover: " << line << endl;
            P = atoi( line.c_str( ) );
            counter++;
        }

        //linea 3: numero de customers
        if ( counter == 3 ) { // cout << "Number of customers: " << line << endl;
            counter++;
        }

        //linea 2: numeros de suppliers
        if ( counter == 2 ) {
            // cout << "Number of suppliers: " << line << endl;
            counter++;
        }

        //linea 1: Cantidad de nodos
        if ( counter == 1 ) { // cout << "Total nodes: " << line << endl;
            counter++;
        }

        //linea 0: factible o no
        if ( counter == 0 ) {
            // if( line.compare(0,1,"1") == 0 )
            //   {
            //     cout << "Feasible" << endl;
            //   }
            counter++;
        }

    }
    file.close( );
    return P;
}

void node::show_container( set < node* > container )
{

    cout << "Showing the content: " << endl;

    for ( set < node* >::iterator iter = container.begin( );
            iter != container.end( );
            iter++ ) {
        cout << "Id = " << ( *iter )->getId( ) << " ";
        cout << "X = " << ( *iter )->getX( ) << " ";
        cout << "Y = " << ( *iter )->getY( ) << endl;
    }

}

void node::show_set( set < int > vec )
{
    for ( set <int>::iterator iter = vec.begin( );
            iter != vec.end( );
            iter++ ) {
        cout << *iter << " ";
    }
    cout << endl;
}

vector< vector <float> > node::getSuppliers( set < node* > vec, float x, float y, vector<vector <float> > &tp )
{
    set < node * >::iterator iterA;

    tp[0][0] = x;
    tp[0][1] = y;

    for ( int i = 0; i < vec.size( ); i++ ) {
        iterA = find_if( vec.begin( ), vec.end( ), busca( i + 1 ) );

        float x1 = ( *iterA )->getX( );
        float y1 = ( *iterA )->getY( );

        // para TSP
        tp[i + 1][0] = x1;
        tp[i + 1][1] = y1;

        // cout << tp[i][0] << " ";
        // cout << tp[i][1] << endl;

    }
    return tp;
}

void node::show_associations( map < int, set < int > > associations )
{

    cout << "Showing the associations: " << endl;

    for ( map < int, set < int > >::iterator iter = associations.begin( );
            iter != associations.end( );
            iter++ ) {
        cout << "Node id = " << iter->first << endl;
        cout << "Associated to: ";
        show_set( iter->second );
    }

}

/* Función que devuelve el costo de cubrir el P utilizando
   un  recorrido de suppliers aleatorio.
 */
float node::objective_function( vector<unsigned> &suppliers, set < node* > &container,
                                node *depot,
                                int P,
                                map < int, set < int > > &associations, int *puntero )
{
    float cost = 0, costTemp; //Costo en distancia acumulado.
    //Coordenadas necesarias para calcular la distancia.
    float x1, y1, x2, y2;
    //Variables auxiliares
    //Iteradores:
    set < node * >::iterator iterA;
    set < node * >::iterator iterB;

    //PRIMERO se calcula d(depot, suppliers[0]), es decir, primer arco.
    x1 = ( depot )->getX( );
    y1 = ( depot )->getY( );
    //cout << "\nDEPOT: " << " x = "  << x1 << " y = " << y1 << endl;

    //cout << "suppliers[0] = " << suppliers[0] << endl;

    iterA = find_if( container.begin( ), container.end( ), busca( suppliers[0] ) );
    x2 = ( *iterA )->getX( );
    y2 = ( *iterA )->getY( );
    //cout << "\tID = " << (*iterA)->getId() << " x = "  << x2 << " y = " << y2 << endl;

    costTemp = calculateDistance( x1, y1, x2, y2 );
    //cout << "\td(0, " << suppliers[0] << ") = " << costTemp << endl;
    cost += costTemp;

    set <int> covers; //Clientes cubiertos.

    //Se recuperan las coordenadas para luego calcular la distancia
    for ( int i = 0; i < suppliers.size( ) - 1; i++ ) //NO se llega al final para evitar desbordamiento: d(N, N+1)
    {
        //cout << "suppliers["<< i << "] = " << suppliers[i] << endl;

        //Se inicializan los iteradores, necesarios para el unorderer_set.
        iterA = find_if( container.begin( ), container.end( ), busca( suppliers[i] ) );
        iterB = find_if( container.begin( ), container.end( ), busca( suppliers[i + 1] ) );

        x1 = ( *iterA )->getX( );
        y1 = ( *iterA )->getY( );
        x2 = ( *iterB )->getX( );
        y2 = ( *iterB )->getY( );


        map < int, set < int > >::iterator it;
        set < int > vec;
        it = associations.find( suppliers[i] );
        vec = it->second;
        // show_set(vec);
        for ( set <int>::iterator iter = vec.begin( ); iter != vec.end( ); iter++ )
            covers.insert( *iter );

        it = associations.find( suppliers[i + 1] );
        vec = it->second;
        // show_set(vec);
        for ( set <int>::iterator iter = vec.begin( ); iter != vec.end( ); iter++ )
            covers.insert( *iter );

        // show_set(covers);


        // for(int j = 0; j < associations[(*iterB)->getId()].size(); j++)
        // 	covers.insert(associations[(*iterB)->getId()][j]);

        // for(int j = 0; j < covers.size(); j++)
        // 	cout << covers[i << endl;


        // exit(0);


        //cout << "\tIDA = " << (*iterA)->getId() << " x = "  << x1 << " y = " << y1 << endl;
        //cout << "\tIDB = " << (*iterB)->getId() << " x = "  << x2 << " y = " << y2 << endl;

        costTemp = calculateDistance( x1, y1, x2, y2 );
        //cout << "\td(" << suppliers[i] << ", " << suppliers[i + 1] << ") = " << costTemp << endl;
        cost += costTemp;

        // covers += vec.size( );
        //cout << "\tCOVERS = " << covers << "/" << P << endl;

        if ( covers.size( ) >= P ) {
            //cout << "\nTHE CUSTOMERS ARE COVERS !!" << endl;

            //Sólo queda sumar la distancia d(i+1, 0)
            x1 = ( depot )->getX( );
            y1 = ( depot )->getY( );
            //cout << "\nDEPOT: " << " x = "  << x1 << " y = " << y1 << endl;
            //cout << "suppliers["<< i + 1 << "] = " << suppliers[i + 1] << endl;
            //cout << "\tIDB = " << (*iterB)->getId() << " x = "  << x2 << " y = " << y2 << endl;

            costTemp = calculateDistance( x1, y1, x2, y2 );
            //cout << "\td(" << suppliers[i + 1] << " , 0) = " << costTemp << endl;
            cost += costTemp;
            *puntero = i + 1;
            break;
        }
    }
    /* Sumado a lo anterior, falta la distancia entre
       d( nodo 0, suppliers[0] ) y d( suppliers[último i que supera el P] , 0 )
    */
    return cost;
}

int node::calcularPrice( vector<unsigned> &cromosoma, int P, map < int, set < int > > *associations )
{
    set < int > covers;
    set < unsigned >::iterator iter;

    for ( vector<unsigned>::iterator iter = cromosoma.begin( );
          iter != cromosoma.end( );
          iter++ )
        {
            map<int, set<int>>::iterator it;
            set < int > vec;
            //Contando las clientes asociados al supplierTemp.
            it = associations->find( *iter );
            vec = it->second;

            for ( set <int>::iterator i = vec.begin( ); i != vec.end( ); i++ )
                covers.insert( *i );

            // Se actualiza hasta donde cubre ahora
            if ( covers.size( ) >= P )
                break;
        }
    return covers.size( );
}

float node::calculateDistance( float x1, float y1, float x2, float y2 )
{
    float dx, dy;
    dx = pow( x2 - x1, 2 );
    dy = pow( y2 - y1, 2 );
    return sqrt( dx + dy );
}


void node::graficar(string outName, vector<unsigned> &chrm, set <node * > &suppliers,
                    set < node* > &customers, node *depot, int P, map < int, set < int > > &associations)
{
    vector<string> color;
    string line;
    //Load color list
    ifstream file( "colors.txt" );
    //verifico apertura de archivo
    assert( file.is_open( ) );
    //Variables auxiliares
    while ( getline( file, line ) ) {
        color.push_back(line);
    }
    file.close( );

    ofstream Graphics(outName);
    Graphics << "graph [ hierarchic 1 directed 1" << endl;

    // printed depot
    Graphics << "node [ id " << ( depot )->getId( ) *10 << " graphics [ x " << ( depot )->getX( ) *10 << " y " << ( depot )->getY( )
             << " w 11  h 11 type \"rectangle\" fill  \"#FF6600\"] LabelGraphics [text \"" << ( depot )->getId( )  << "\" fontSize 7 ] ]" << endl;
    // printed suppliers
    int index_color = 0;
    for ( set < node* >::iterator iter = suppliers.begin( );
          iter != suppliers.end( );
          iter++ )
        {
            Graphics << "node [ id " << ( *iter )->getId( ) << " "
                     << "graphics [ x " << ( *iter )->getX( ) *10  << " " << "y " << ( *iter )->getY( ) * 10 << " "
                     << "w 11  h 11 type \"roundrectangle\" fill  \"" + color[index_color++] + "\" ] LabelGraphics"
                     << " " << "[text " << " " << "\"" << ( *iter )->getId( ) << "\"" << " " << "fontSize 7 ] ]" << endl;
        }
    // printed customers
    for ( set < node* >::iterator iter = customers.begin( );
          iter != customers.end( );
          iter++ )
        {
            Graphics << "node [ id " << ( *iter )->getId( ) << " "
                     << "graphics [ x " << ( *iter )->getX( ) *10  << " " << "y " << ( *iter )->getY( ) * 10 << " "
                     << "w 9  h 9 type \"triangle\"] LabelGraphics"
                     << " " << "[text " << " " << "\"" << ( *iter )->getId( ) << "\"" << " " << "fontSize 5 ] ]" << endl;
        }
    // printed route
    Graphics << "edge [ source " << 0 << " target " << chrm[0] << " graphics [ fill \" #FF0000"
             <<  "\" targetArrow \"standard\" ] ]" << endl;
    for ( int i = 0; i < P; i++ ) //NO se llega al final para evitar desbordamiento: d(N, N+1)
        {
            Graphics << "edge [ source " << chrm[i] << " target " << chrm[i+1] << " graphics [ fill \" #FF0000"
                     <<  "\" targetArrow \"standard\" ] ]" << endl;
        }

    Graphics << "edge [ source " << chrm[P] << " target " << 0 << " graphics [ fill \" #FF0000"
             << "\" targetArrow \"standard\" ] ]" << endl;
    Graphics << "]";
    Graphics.close();

}
