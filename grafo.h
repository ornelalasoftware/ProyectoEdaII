#ifndef GRAFO_H
#define GRAFO_H

#include <vector>
#include <string>

const float INF = 999999.0f;

;struct ResultadoRuta {
    int* camino;
    int cantidadCiudades;
    float distanciaTotal;
    std::string descripcionTexto;
};

class Grafo {
private:
    float** matriz;
    int n;

public:
    Grafo(int numCiudades);
    ~Grafo();

    void agregarRuta(int origen, int destino, float km);
    void cortarRuta(int origen, int destino);
    float obtenerPeso(int origen, int destino) { return matriz[origen][destino]; }
    // Método "Getter" para encapsulamiento: Permite a la interfaz gráfica
    // consultar los kilómetros (peso) entre dos ciudades de la matriz privada,
    // sin alterar sus datos. Devuelve la distancia real o INF si la ruta está cortada.

    ResultadoRuta calcularDijkstra(int inicio, int fin);
};

#endif