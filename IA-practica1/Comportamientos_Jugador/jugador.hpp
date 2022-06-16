#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <queue>

using namespace std;

//Struct con las variables de estado del agente
struct Estado{
  int fil;
  int col;
  int brujula;
  bool girar_derecha, bien_situado;
  bool bikini, zapatillas, recarga;
  Action ultimaAccion;
  int pasos;
  int rotaciones;
  bool rotar;
  bool yaPosicionado;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      estadoActual.fil = estadoActual.col = 99;
      estadoActual.brujula = 0;
      estadoActual.girar_derecha = false;
      estadoActual.bien_situado = false;
      estadoActual.ultimaAccion = actIDLE;
      estadoActual.bikini = false;
      estadoActual.zapatillas = false;
      estadoActual.recarga = false;
      estadoActual.pasos = 0;
      estadoActual.rotar = true;
      estadoActual.yaPosicionado = false;
      vector<unsigned char> aux(199, '?');
      for(int i=0; i<199; i++){
        mapaAuxiliar.push_back(aux);
      }
      estadoActual.rotaciones = 0;
      filAuxiliar = colAuxiliar = 99;
      numReinicios = 0, cuentaForward = 0;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void mostrarInformacion(Sensores sensores);
    Action elegirAccion(Sensores sensores);
    void cambioDeEstado(Sensores sensores);
    void reiniciar(Sensores sensores);
  private:
    //estado actual del agente
    Estado estadoActual;
    std::vector<vector<unsigned char>> mapaAuxiliar;
    queue<Action> listaAcciones;
    int filAuxiliar, colAuxiliar;
    int numReinicios, cuentaForward;

    // Declarar aquí las variables de estado
    bool esObstaculo(unsigned char casilla);
    bool puedoAvanzar(unsigned char terrreno, unsigned char superficie);
    bool esBikini (unsigned char casilla);
    bool esZapatilla(unsigned char casilla);
    bool esRecarga(unsigned char casilla);
    void pintarMapa(Sensores sensores);
    void pintarMapaAuxiliar(Sensores sensores);
    bool seMiPosicion(Sensores sensores);
    bool puedoPasar(int fila, int columna, Sensores sensores);
    void analizaEntorno(Sensores sensores);
    bool meInteresa(unsigned char casilla);
    void pasarMapaAuxiliarAMapa(Sensores sensores);
    bool esAldeano (unsigned char superficie);
    bool esLobo(unsigned char superficie);
};

#endif
