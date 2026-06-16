#ifndef SISTEMAGESTIONLOGISTICO_H
#define SISTEMAGESTIONLOGISTICO_H

#include "grafo.h"
#include "ciudad.h"
#include <string>

struct RegistroHistorial {
    char origen[50];
    char destino[50];
    float distancia;
};

struct NodoHistorial {
    RegistroHistorial dato;
    NodoHistorial* siguiente;
};

class SistemaLogistico {
private:
    Grafo* grafo;

    Ciudad* ciudades;
    int cantidadCiudades;
    int capacidadCiudades;

    NodoHistorial* historial;

public:
    SistemaLogistico();
    ~SistemaLogistico();

    void inicializar();
    void cargarMapaInicial();

    void agregarCiudad(std::string nombre, float x, float y);
    void cargarCiudadDesdeArchivo(Ciudad ciudadAux);
    void agregarRuta(int origen, int destino, float km);
    void cortarRuta(int origen, int destino);

    int getCantidadCiudades() { return cantidadCiudades; }

    ResultadoRuta calcularRutaOptima(int origen, int destino);

    std::string obtenerNombreCiudad(int id);

    void registrarHistorial(int origen, int destino, ResultadoRuta resultado);
    void guardarHistorial();
    void cargarHistorial();

    void guardarDatos();
    void cargarDatos();
    void mostrarHistorial();

    Grafo& getGrafo();
};

#endif