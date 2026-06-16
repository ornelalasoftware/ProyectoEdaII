#include "sistemagestionlogistico.h"
#include <fstream>
#include <iostream>
#include <cstring>


SistemaLogistico::SistemaLogistico() {

    grafo = new Grafo(15);

    cantidadCiudades=0;
    capacidadCiudades=15;
    ciudades=new Ciudad[capacidadCiudades];

    historial = nullptr;

    cargarHistorial();
}
//Constructor: cuando el sistema arranca, reserva memoria dinámica para un nuevo
//objeto Grafo que se configura inicialmente para soportar hasta 10 ciudades
//NO agrada mucho, hay que modificarlo porque se puede desbordar


SistemaLogistico::~SistemaLogistico() {

    delete grafo;
    delete[] ciudades;

    while (historial != nullptr) {

        NodoHistorial* auxiliar = historial;

        historial = historial->siguiente;

        delete auxiliar;
    }
}
//Destructor: cuando el programa se cierra, libera la memoria que ocupaba
//el objeto grafo para evitar fugas de memoria.


void SistemaLogistico::cargarMapaInicial()
{
    cantidadCiudades = 0;

    // Sincroniza exacto con las constantes (X, Y) y los ID
    agregarCiudad("Cordoba Capital", 225.0f, 145.0f);   // ID 0
    agregarCiudad("Villa Carlos Paz", 195.0f, 147.0f);  // ID 1
    agregarCiudad("Villa Maria", 310.0f, 210.0f);       // ID 2
    agregarCiudad("Bell Ville", 365.0f, 215.0f);        // ID 3
    agregarCiudad("Rio Cuarto", 180.0f, 245.0f);        // ID 4
    agregarCiudad("San Francisco", 375.0f, 135.0f);     // ID 5

    // Conexiones con kilómetros reales
    agregarRuta(1, 0, 35);   // Carlos Paz - Córdoba
    agregarRuta(0, 2, 146);  // Córdoba - Villa María
    agregarRuta(0, 3, 200);  // Córdoba - Bell Ville
    agregarRuta(2, 3, 63);   // Villa María - Bell Ville
    agregarRuta(2, 4, 136);  // Villa María - Río Cuarto
    agregarRuta(4, 3, 220);  // Río Cuarto - Bell Ville
    agregarRuta(4, 1, 215);  // Río Cuarto - Carlos Paz
    agregarRuta(3, 5, 195);  // Bell Ville - San Francisco
}


void SistemaLogistico::registrarHistorial(
    int origen,
    int destino,
    ResultadoRuta resultado)
{
    NodoHistorial* nuevoNodo =
        new NodoHistorial();

    strncpy(nuevoNodo->dato.origen, obtenerNombreCiudad(origen).c_str(), 49);
    nuevoNodo->dato.origen[49] = '\0';

    strncpy(nuevoNodo->dato.destino, obtenerNombreCiudad(destino).c_str(), 49);
    nuevoNodo->dato.destino[49] = '\0';

    nuevoNodo->dato.distancia =
        resultado.distanciaTotal;

    nuevoNodo->siguiente = historial;

    historial = nuevoNodo;
}


void SistemaLogistico::guardarHistorial() {

    std::ofstream archivo("historial.txt");

    if (!archivo.is_open()) {
        return;
    }

    NodoHistorial* aux = historial;

    while (aux != nullptr) {

        archivo
            << aux->dato.origen
            << "|"
            << aux->dato.destino
            << "|"
            << aux->dato.distancia
            << "\n";

        aux = aux->siguiente;
    }

    archivo.close();
}


void SistemaLogistico::cargarHistorial() {

    std::ifstream archivo("historial.txt");

    if (!archivo.is_open()) {
        return;
    }

    std::string origen;
    std::string destino;
    float distancia;

    while (getline(archivo, origen, '|')) {

        getline(archivo, destino, '|');

        archivo >> distancia;
        archivo.ignore();

        NodoHistorial* nuevo =
            new NodoHistorial();

        strncpy(nuevo->dato.origen, origen.c_str(), 49);
        nuevo->dato.origen[49] = '\0';

        strncpy(nuevo->dato.destino, destino.c_str(), 49);
        nuevo->dato.destino[49] = '\0';

        nuevo->dato.distancia =
            distancia;

        nuevo->siguiente = historial;

        historial = nuevo;
    }

    archivo.close();
}


void SistemaLogistico::inicializar() {

    std::ifstream archivoPrueba("ciudades.dat", std::ios::binary);
    if (!archivoPrueba.is_open()) {
        // Si no existe el archivo, creamos todo de cero y lo guardamos
        cargarMapaInicial();
        guardarDatos();
    } else {
        archivoPrueba.close();
        // Si el archivo existe, cargamos las ciudades del disco...
        cargarDatos();

        // Esto evita que el grafo quede vacío
        agregarRuta(1, 0, 35);
        agregarRuta(0, 2, 146);
        agregarRuta(0, 3, 200);
        agregarRuta(2, 3, 63);
        agregarRuta(2, 4, 136);
        agregarRuta(4, 3, 220);
        agregarRuta(4, 1, 215);
        agregarRuta(3, 5, 195);
    }
}

//Prepara el sistema cuando arranca
//En este caso, su única tarea es llamar a la función cargarDatos()
//para recuperar el mapa guardado en el disco.


void SistemaLogistico::agregarCiudad(std::string nombre, float x, float y) {
    // Evita duplicados exactos por nombre
    for (int i = 0; i < cantidadCiudades; i++) {
        if (ciudades[i].nombre == nombre) {
            return;
        }
    }

    // Control para que no desborde
    if (cantidadCiudades >= capacidadCiudades) {
        capacidadCiudades *= 2;
        Ciudad* nuevoArreglo = new Ciudad[capacidadCiudades];
        for (int i = 0; i < cantidadCiudades; i++) {
            nuevoArreglo[i] = ciudades[i];
        }
        delete[] ciudades;
        ciudades = nuevoArreglo;
    }

    Ciudad nueva;
    nueva.id = cantidadCiudades; // Asigna ID secuencial si es que se agraga una ciudad nueva
    strncpy(nueva.nombre, nombre.c_str(), 49);
    nueva.nombre[49] = '\0';
    nueva.x = x;
    nueva.y = y;

    ciudades[cantidadCiudades] = nueva;
    cantidadCiudades++;
}

// Para cuando cargamos desde el archivo binario
void SistemaLogistico::cargarCiudadDesdeArchivo(Ciudad ciudadAux) {
    if (cantidadCiudades >= capacidadCiudades) {
        // Podríamos tal vezzzz poner la misma lógica de agrandar el arreglo acá por seguridad,
        // pero como mapeamos 15 de capacidad inicial para 6 ciudades, no va a desbordar.
    }

    // Guarda la ciudad respetando el ID que ya tenía
    ciudades[cantidadCiudades] = ciudadAux;
    cantidadCiudades++;
}

void SistemaLogistico::agregarRuta(int origen, int destino, float km) {
    grafo->agregarRuta(origen, destino, km);
}
//el sistema logístico no sabe cómo manejar grafos directamente, así que le "delega" el
//trabajo a su objeto interno grafo usando el operador flecha (->) porque es un puntero


void SistemaLogistico::cortarRuta(int origen, int destino) {
    grafo->cortarRuta(origen, destino);
}
//igual que el anterior, le pide al grafo que rompa la conexión entre esas dos ciudades en la matriz de adyacencia


ResultadoRuta SistemaLogistico::calcularRutaOptima(int origen, int destino) {
    ResultadoRuta resultado = grafo->calcularDijkstra(origen, destino);
    //le pide al grafo que ejecute el Algoritmo de Dijkstra entre el origen y el destino
    //el camino y los kilómetros se guardan en la variable resultado.

    // Solo registramos operaciones exitosas para evitar basura en el historial
    if (resultado.distanciaTotal < 999999.0f) {
        registrarHistorial(origen, destino, resultado);
        guardarHistorial();
    }
    //método para registrar el resultado en la pila historial

    return resultado;
    //Finalmente, devuelve el resultado
}

// MÉTODOS DE PERSISTENCIA: GUARDAR Y CARGAR DATOS -----> acá esta Juan!!!


void SistemaLogistico::guardarDatos() {
    std::ofstream archivo("ciudades.dat", std::ios::binary);
    //Abre (o crea) un archivo llamado ciudades.dat en modo de escritura binaria (std::ios::binary)
    //ofstream (Output File Stream): Se usa exclusivamente para escribir/guardar datos en un archivo (crea el archivo si no existe)
    //Cuando se abre un archivo con fstream, por defecto se maneja en modo .txt
    //pero para guardar estructuras como el struct o la matriz utilizamos el modificador ios::binary

    if (!archivo.is_open()) {
        std::cout << "Error al guardar archivo.\n";
        return;
    }//chequea que el archivo se abra correctamente, sino tira error

    for (int i = 0; i < cantidadCiudades; i++) {
        archivo.write(
            reinterpret_cast<const char*>(&ciudades[i]),//reinterpreta al struct como char y le manda por referencia la direccion
            sizeof(Ciudad) //calcula tamaño en bytes de Ciudad
            );
        //Al hacer todo esto la ciudad se guarda en el archivo .dat como una copia identica o "foto"
    }

    archivo.close();

    guardarHistorial();

    std::cout << "Datos guardados correctamente.\n";
}


void SistemaLogistico::cargarDatos() {
    std::ifstream archivo("ciudades.dat", std::ios::binary);
    //ifstream (Input File Stream): Se usa exclusivamente para leer/cargar datos desde un archivo hacia tu programa.
    //Abre el archivo ciudades.dat en modo de lectura binaria

    if (!archivo.is_open()) {
        return;
    }
    //archivo.is_open(): Es un método propio de los flujos de fstream
    //Si el archivo no existe, sale en silencio sin lanzar errores

    // Se reinicia el contador de ciudades a 0
    cantidadCiudades = 0;

    Ciudad ciudadAuxiliar; //auxiliar para guardar datos que se usan en el bucle que sigue

    while (
        archivo.read(
            reinterpret_cast<char*>(&ciudadAuxiliar),
            sizeof(Ciudad)
            )
        ) {
        // Usamos cargarCiudadDesdeArchivo para respetar la secuencia binaria del disco
        cargarCiudadDesdeArchivo(ciudadAuxiliar);
    }
    //bucle de lectura: archivo.read extrae del archivo el número exacto de bytes
    //que componen una Ciudad y los vuelca en la variable temporal ciudadAuxiliar
    //Si todo sale bien, mete esa ciudad en el vector ciudades
    //el ciclo se detiene automáticamente cuando se llega al final del archivo (EOF)

    archivo.close();
}

// HISTORIAL

void SistemaLogistico::mostrarHistorial() {

    std::cout << "\n===== HISTORIAL =====\n";

    // Puntero auxiliar para recorrer la lista enlazada simple sin perder la direccion original
    NodoHistorial* auxiliar = historial;

    while (auxiliar != nullptr) {

        std::cout
            << "Origen: "
            << auxiliar->dato.origen
            << "\n";

        std::cout
            << "Destino: "
            << auxiliar->dato.destino
            << "\n";

        std::cout
            << "Distancia: "
            << auxiliar->dato.distancia
            << " km\n";

        std::cout
            << "---------------------\n";

        auxiliar = auxiliar->siguiente;
    }
}


Grafo& SistemaLogistico::getGrafo() {
    return *grafo;
}
//Getter: Como grafo es un puntero interno (Grafo*), este método devuelve el objeto
//"desapuntado" (utilizando el asterisco *) en forma de referencia (Grafo&),
//permitiendo que otras partes del programa interactúen con la matriz sin romper el
//encapsulamiento. Esto es para evitar que alguien lo pueda modificar y romperlo


std::string SistemaLogistico::obtenerNombreCiudad(int id) {
    //verificamos que el ID sea válido y exista
    if (id >= 0 && id < cantidadCiudades) {
        return ciudades[id].nombre;
    }
    return "Ciudad Desconocida";//si no encuentra nada
}