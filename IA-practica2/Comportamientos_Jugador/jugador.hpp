#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <stack>
#include <queue>

struct estado {
  int fila;
  int columna;
  int orientacion;

  //Para nivel 2
  bool bikini;
  bool zapatillas;

  //Para el nivel 3
  bool hayObjetivoDefinido = false;
  bool haLlegadoObjetivo = true;

  //Para nivel 4
  bool objetivo1,objetivo2,objetivo3;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
        
      // Inicializar Variables de Estado
      hayPlan = false;
      hayplanComplemento = false;
      hayplanBateria = false;
      hayplanBikini = false;
      hayplanZapatillas = false;
      bateriaObjetivo.fila = -1;
      bateriaObjetivo.columna = -1;
      complementoObjetivo.fila = -1;
      complementoObjetivo.columna = -1;
      //
      hayplanBikiniCerca = false;
      hayplanZapatillasCerca = false;
      bikiniCercaObjetivo.fila = -1;
      bikiniCercaObjetivo.columna = -1;
      zapatillasCercaObjetivo.fila = -1;
      zapatillasCercaObjetivo.columna = -1;

      vector<unsigned char> aux(199, '?');
      for(int i=0; i<199; i++){
        mapaAuxiliar.push_back(aux);
      }

      filAuxiliar = colAuxiliar = 99;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
      hayplanComplemento = false;
      hayplanBateria = false;
      hayplanBikini = false;
      hayplanZapatillas = false;
      bateriaObjetivo.fila = -1;
      bateriaObjetivo.columna = -1;
      complementoObjetivo.fila = -1;
      complementoObjetivo.columna = -1;
      //
      hayplanBikiniCerca = false;
      hayplanZapatillasCerca = false;
      bikiniCercaObjetivo.fila = -1;
      bikiniCercaObjetivo.columna = -1;
      zapatillasCercaObjetivo.fila = -1;
      zapatillasCercaObjetivo.columna = -1;

      vector<unsigned char> aux(199, '?');
      for(int i=0; i<199; i++){
        mapaAuxiliar.push_back(aux);
      }

      filAuxiliar = colAuxiliar = 99;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan;
    Action accionAnterior;
    
    bool hayPlan;
    int giro; //Para nivel 2

    //nivel 3
    int numIteraciones = 0; 
    queue<estado> destinos;
    estado dst;

    //nivel 4
    list<estado> objetivosReto; //lista de almacenamiento de los objetivos pendientes
    stack<estado> pila_objetivosReto;//pila de almacenamiento de los objetivos pendientes 
    std::vector<vector<unsigned char>> mapaAuxiliar;
    int filAuxiliar, colAuxiliar;
    bool hayplanComplemento, hayplanBateria, hayplanBikini, hayplanZapatillas, hayplanBikiniCerca, hayplanZapatillasCerca;
    //Con estas variables de tipo estado almacenaremos el objetivo del plan (el estado del complemento o batería que se quiere alcanzar)
    // y podremos comprobar cuando hemos conseguido llegar a ese objetivo principal
    // de forma que podremos calcular un nuevo plan sin boicotear la finalidad del anterior plan elaborado
    estado complementoObjetivo, bateriaObjetivo, bikiniCercaObjetivo, zapatillasCercaObjetivo;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Costo_Uniforme(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AEstrellaReto(const estado &origen, const list<estado> &destino, list<Action> &plan);

    
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);
    int valorCasilla(estado &e, int giro);
    void pintarMapaNivel4(Sensores sensores);
    void pintarMapa(Sensores sensores);
    // void pasarMapaAuxiliarAMapa(Sensores sensores);
    bool EsObstaculo(unsigned char casilla);
    //Nivel 3
    void nuevoDestino();
    bool EsAguaOBosqueYNoTengoComplementos(unsigned char casilla);
    pair<bool,estado> IrAPorComplemento(const estado &actual, const list<estado> &objetivos);
    list<estado> ObtenerLista(stack<estado> pila);
    stack<estado> ObtenerPila(const estado &origen, list<estado> destino);
    void BorrarSiObjetivo(const estado &e,list<estado> &objetivos);
    pair<bool,estado> IrAPorBikiniCerca(const estado &actual);
    pair<bool,estado> IrAPorZapatillasCerca(const estado &actual);
    pair<bool,estado> IrAPorBateria(const estado &actual);
    pair<bool,estado> IrAPorBikini(const estado &actual);
    pair<bool,estado> IrAPorZapatillas(const estado &actual);
    void actualizarPosicion();
};

#endif
