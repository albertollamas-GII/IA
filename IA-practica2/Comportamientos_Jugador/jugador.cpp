#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <limits> //Para el minimo en funcion minimo_vector

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

	if (sensores.nivel != 4){
		actual.fila = sensores.posF;
		actual.columna = sensores.posC;
		actual.orientacion = sensores.sentido;
	}

	cout << "Fila: " << actual.fila << endl;
	cout << "Col : " << actual.columna << endl;
	cout << "Ori : " << actual.orientacion << endl;
	
		// Capturo los destinos
		cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
		objetivos.clear();
		for (int i = 0; i < sensores.num_destinos; i++)
		{
			estado aux;
			aux.fila = sensores.destino[2 * i];
			aux.columna = sensores.destino[2 * i + 1];
			objetivos.push_back(aux);
		}
	if (sensores.nivel != 3 and sensores.nivel != 4){
		if (!hayPlan){
			hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan);
			cout << "Sale pathfinding" << endl;
		}

		if (hayPlan and plan.size() > 0){	//Hay un plan no vacio
			accion = plan.front();		//Tomo la siguiente accion del plan
			plan.erase(plan.begin());	//Eliminamos la accion del plan
		} else {
			cout << "No se pudo encontrar un plan\n";
		}
	} else if (sensores.nivel == 3) {

		if (sensores.terreno[0] == 'X' and sensores.bateria < 3000)
			return actIDLE;
		
		if (sensores.terreno[0] == 'D'){
			actual.zapatillas = true;
			actual.bikini = false;
		}

		if (sensores.terreno[0] == 'K'){
			actual.zapatillas = false;
			actual.bikini = true;
		}
		
		
		if (!actual.hayObjetivoDefinido or destinos.size() == 0){
			nuevoDestino();
			actual.hayObjetivoDefinido = true;
		}

		if (actual.haLlegadoObjetivo) {
			dst = destinos.front();
			destinos.pop();
			actual.haLlegadoObjetivo = false;
		}

		if (actual.fila == dst.fila and actual.columna == dst.columna) {
			actual.haLlegadoObjetivo = true;
			if (!destinos.empty()) {
				cout << "Obteniendo nuevo destino de destinos \n";
				dst = destinos.front();
				destinos.pop();
				actual.haLlegadoObjetivo = false;
				hayPlan = false;
			} else {
				cout << "Definiendo nueevo destino\n";
				nuevoDestino();
				actual.hayObjetivoDefinido = true;
				dst = destinos.front();
				destinos.pop();
				actual.haLlegadoObjetivo = false;
				hayPlan = false;
			}
		}	

		pintarMapa(sensores);
		cout << dst.fila << " " << dst.columna << " " << endl;
		cout << mapaResultado[dst.fila][dst.columna] << endl;

		if (EsObstaculo(mapaResultado[dst.fila][dst.columna])){
			nuevoDestino();
			dst = destinos.front();
			destinos.pop();
		}

		if (!hayPlan) {
			hayPlan = pathFinding_AEstrella(actual, dst, plan);
			

			while(!hayPlan) {
				nuevoDestino();
				dst = destinos.front();
				destinos.pop();
				hayPlan = pathFinding_AEstrella(actual, dst, plan);
			}

		}
		if (plan.front() == actFORWARD and (EsObstaculo(sensores.terreno[2]) or EsAguaOBosqueYNoTengoComplementos(sensores.terreno[2]))) {
			hayPlan = pathFinding_AEstrella(actual, dst, plan);
			while(!hayPlan) {
				nuevoDestino();
				dst = destinos.front();
				destinos.pop();
				hayPlan = pathFinding_AEstrella(actual, dst, plan);
			}
		}

		if (hayPlan and plan.size() > 0){	//Hay un plan no vacio
			accion = plan.front();		//Tomo la siguiente accion del plan
			plan.erase(plan.begin());	//Eliminamos la accion del plan
		} else {
			cout << "No se pudo encontrar un plan\n";
		}
		
	} else if (sensores.nivel == 4){	
		bool esperarAldeanoOLobo = false;
		if (numIteraciones == 0){
			accion = actWHEREIS;
			numIteraciones++;
			
		} else {
			if (numIteraciones == 1) {
				actual.fila = sensores.posF;
				actual.columna = sensores.posC;
				actual.orientacion = sensores.sentido;
				cout << actual.fila << " " << actual.columna << endl;
				numIteraciones++;
			} else {
				if (accionAnterior == actWHEREIS){
					actual.fila = sensores.posF;
					actual.columna = sensores.posC;
					actual.orientacion = sensores.sentido;
				} else 
					actualizarPosicion();
				
				//Pinto el mapa auxiliar
				pintarMapaNivel4(sensores);
				//Si el actual es un objetivo, lo elimino de la lista
				BorrarSiObjetivo(actual,objetivosReto);

				//Comprobamos si hemos alcanzado alguno de estos objetivos, y actualizamos las variables correspondientes
				// puesto que aunque haya actualmente un plan, podremos dar opción a recalcular otro que nos convenga más una
				// vez alcanzados la batería o el complemento que se deseaba
				if(hayplanBateria){
					if((actual.fila == bateriaObjetivo.fila) and (actual.columna == bateriaObjetivo.columna)){
						hayplanBateria = false;
					}
				}
				if(hayplanComplemento){
					if((actual.fila == complementoObjetivo.fila) and (actual.columna == complementoObjetivo.columna)){
						hayplanComplemento = false;
					}
				}

				if(hayplanBikiniCerca){
					if((actual.fila == bikiniCercaObjetivo.fila) and (actual.columna == bikiniCercaObjetivo.columna)){
						hayplanBikiniCerca = false;
					}
				}

				if(hayplanZapatillasCerca){
					if((actual.fila == zapatillasCercaObjetivo.fila) and (actual.columna == zapatillasCercaObjetivo.columna)){
						hayplanZapatillasCerca = false;
					}
				}

				//Comprobamos si la casilla actual es de recarga de batería,
				// si lo es y conviene recargarla, esperamos hasta llegar a los 3000
				if((mapaResultado[actual.fila][actual.columna] == 'X') and ((sensores.bateria < 2600)and !(sensores.tiempo > 2700)) and ((sensores.bateria < 3000) and !(sensores.tiempo > 2700))){
						accion = actIDLE;    //esperamos para completar la recarga
				}else {

					//Si le queda poca carga y no hay un plan para ir a por una batería, localizamos una y vamos hacia ella para recargar elaborando un nuevo plan
					if(!hayplanBateria and (sensores.bateria < 300)){
						pair<bool,estado> cogerBateria;
						cogerBateria = IrAPorBateria(actual);

						if (cogerBateria.first){	//si ha localizado una bateria
							//introduzco los nuevos objetivos en objetivosReto
							for(auto it=objetivos.begin(); it!=objetivos.end(); ++it){
								objetivosReto.push_back(*it);
							}
							//en una pila, organizamos los objetivos, donde cuanto más se acerquen al tope, más cercanos al destino están
							//es decir, organizamos por cercanía al destino
							pila_objetivosReto = ObtenerPila(actual,objetivosReto);

							//Añadimos el estado de la casilla con la bateria en el tope de la pila
							pila_objetivosReto.push(cogerBateria.second);

							//y pasamos los objetivos ya ordenados a nuetra lista de objetivos
							objetivosReto = ObtenerLista(pila_objetivosReto);

							//se indica el éxito (o no) del nuevo plan
							cout << "Plan Bateria" << endl;
							hayplanBateria = pathFinding (sensores.nivel, actual, objetivosReto, plan);

							//y si ha tenido éxito recogemos el estado de nuestro objetivo
							if(hayplanBateria)
								bateriaObjetivo = cogerBateria.second;

							hayPlan = hayplanBateria;
						}

					}

					//Comprobamos si la casilla actual se trata de una casilla con bikini o zapatillas
					if(hayPlan and mapaResultado[actual.fila][actual.columna] == 'K'){
						//En caso de que no lo tuviera ya, lo indicamos
						actual.bikini = true;
						//Por lo que no puede tener zapatillas
						actual.zapatillas = false;
					}
					if(hayPlan and mapaResultado[actual.fila][actual.columna] == 'D'){
						//En caso de que no las tuviera ya, lo indicamos
						actual.zapatillas = true;
						//Por lo que no puede tener bikini
						actual.bikini = false;
					}

					//Si no tiene un plan para ir a por batería ni complementos (bikini/zapatillas), no tiene ni bikini ni zapatillas, y encuentra uno cerca, irá a por un complemento
					if(hayPlan and !hayplanBateria and !hayplanBikiniCerca and !hayplanZapatillasCerca and !actual.bikini and !actual.zapatillas and !hayplanComplemento and !hayplanBikini and !hayplanZapatillas){
						pair<bool,estado> complemento;
						complemento = IrAPorComplemento(actual,objetivos);

						//Si ha encontrado un complemento cerca, caculamos el nuevo plan
						if(complemento.first){

							//introduzco los nuevos objetivos en objetivosReto
							for(auto it=objetivos.begin(); it!=objetivos.end(); ++it){
								objetivosReto.push_back(*it);
							}

							//en una pila, organizamos los objetivos, donde, cuanto más se acerquen al tope, más cercanos al destino están
							pila_objetivosReto = ObtenerPila(actual,objetivosReto);

							//Añadimos el complemento en el tope de la pila
							pila_objetivosReto.push(complemento.second);

							//y pasamos los objetivos ya ordenados a nuetra lista de objetivos
							objetivosReto =ObtenerLista(pila_objetivosReto);
							cout << "Plan Complemento" << endl;
							//calculamos un nuevo plan
							hayplanComplemento = pathFinding (sensores.nivel, actual, objetivosReto, plan);

							if(hayplanComplemento)
								complementoObjetivo = complemento.second;

							//se indica el éxito (o no) del nuevo plan
							hayPlan = hayplanComplemento;


							//si hubiera plan de bikini, indicamos que ya no
							if(hayplanBikini){
								hayplanBikini = false;
							}

							//si hubiera plan de zapatillas, indicamos que ya no
							if(hayplanZapatillas){
								hayplanZapatillas = false;
							}
						}

					}

					//En caso de que se se necesiten unas zapatillas, si encontramos unas cerca, vamos a por ellas generando un nuevo plan
					//Para ello no debe existir un plan más prioritario
					if(hayPlan and !hayplanBateria and !actual.zapatillas and !hayplanZapatillasCerca and hayplanZapatillas and !hayplanComplemento){

						pair<bool,estado> zap;
						zap = IrAPorZapatillasCerca(actual);

						//si he encontrado unas zapatillas cerca calculo un nuevo plan
						if(zap.first){
							//introduzco los nuevos objetivos en objetivosReto
							for(auto it=objetivos.begin(); it!=objetivos.end(); ++it){
								objetivosReto.push_back(*it);
							}

							//en una pila, organizamos los objetivos, donde cuanto más se acerquen al tope, más cercanos al destino están
							pila_objetivosReto = ObtenerPila(actual,objetivosReto);

							//Añadimos el zap en el tope de la pila
							pila_objetivosReto.push(zap.second);

							//y pasamos los objetivos ya ordenados a nuetra lista de objetivos
							objetivosReto = ObtenerLista(pila_objetivosReto);

							//calculamos un nuevo plan
							cout << "Plan Zapatillas cerca" << endl;
							hayplanZapatillasCerca = pathFinding (sensores.nivel, actual, objetivosReto, plan);

							//y si ha tenido éxito recogemos el estado de nuestro objetivo
							if(hayplanZapatillasCerca)
								complementoObjetivo = zap.second;

							//se indica el éxito (o no) del nuevo plan
							hayPlan = hayplanZapatillasCerca;

							//Anulamos el plan de buscar unas zapatillas cualquiera
							hayplanZapatillas = false;

							//si hubiera plan de bikini, indicamos que ya no
							if(hayplanBikini){
								hayplanBikini = false;
							}
							//si hubiera plan de bikiniCerca, eliminamos el objetivo
							if(hayplanBikiniCerca){
								hayplanBikiniCerca = false;
								//eliminamos el objetivo de ese plan de la lista de objetivos
								BorrarSiObjetivo(bikiniCercaObjetivo,objetivosReto);
							}
						}

					}

					//En caso de que se se necesite un bikini, si encontramos uno cerca, vamos a por él generando un nuevo plan
					//Para ello no debe existir un plan más prioritario
					if(hayPlan and !hayplanBateria and !actual.bikini and !hayplanBikiniCerca and hayplanBikini and !hayplanZapatillasCerca and !hayplanComplemento){

						pair<bool,estado> bik;
						bik = IrAPorBikiniCerca(actual);

						//si he encontrado un bikini cerca calculo un nuevo plan
						if(bik.first){

							//introduzco los nuevos objetivos en objetivosReto
							for(auto it=objetivos.begin(); it!=objetivos.end(); ++it){
								objetivosReto.push_back(*it);
							}

							//en una pila, organizamos los objetivos, donde cuanto más se acerquen al tope, más cercanos al destino están
							pila_objetivosReto = ObtenerPila(actual,objetivosReto);

							//Añadimos el bik en el tope de la pila
							pila_objetivosReto.push(bik.second);

							//y pasamos los objetivos ya ordenados a nuetra lista de objetivos
							objetivosReto = ObtenerLista(pila_objetivosReto);

							//calculamos un nuevo plan
							cout << "Plan BikiniCerca" << endl;
							hayplanBikiniCerca = pathFinding (sensores.nivel, actual, objetivosReto, plan);

							//y si ha tenido éxito recogemos el estado de nuestro objetivo
							if(hayplanBikiniCerca)
								complementoObjetivo = bik.second;

							//se indica el éxito (o no) del nuevo plan
							hayPlan = hayplanBikiniCerca;

							//Anulamos el plan de buscar un bikini cualquiera
							hayplanBikini = false;

							//si hubiera plan de zapatillas, indicamos que ya no
							if(hayplanZapatillas){
								hayplanZapatillas = false;
							}
						}

					}

					// Si no hay plan, o ya ha terminado uno, construyo otro
					if (!hayPlan || (plan.size() == 0)) {
							for(auto it=objetivos.begin(); it!=objetivos.end(); ++it){
								objetivosReto.push_back(*it);
							}

							hayPlan = pathFinding (sensores.nivel, actual, objetivosReto, plan);
							cout << "El numero de objetivos es: " << objetivos.size() << endl;
					}

					if (hayPlan and plan.size()>0){ //hay un plan no vacio
						accion = plan.front();   //tomo la siguiente accion del Plan
						//si la acción siguiente en el plan es avanzar, compruebo si delante hay obstáculos,
						// y en caso de que haya, modifico la siguiente acción
						if(accion == actFORWARD){
							//comprobamos si en la casilla de delante hay un aldeano
							if(sensores.superficie[2] == 'a' or sensores.superficie[2] == 'l'){
								accion = actIDLE; //esperamos a que pase
								esperarAldeanoOLobo = true;
							}//comprobamos si en la casilla de delante hay un muro o precipicio
							else if(sensores.terreno[2]=='M' or sensores.terreno[2]=='P'){
								//si lo hay, calculamos un nuevo plan
								cout << "Muro delante" << endl;
								hayPlan = pathFinding (sensores.nivel, actual, objetivosReto, plan);
								accion = plan.front();
							}
							//comprobamos si en la casilla de delante hay bosque, en cuyo caso, si no tenemos zapatillas, buscaremos
							// las zapatillas más cercanas
							//NOTA: en el caso de la casilla de bosque he puesto que si ya hay un plan de zapatillas no se tenga en cuenta
							//aunque acabe atravesandolo sin zapatillas, puesto que consume menos batería que entrar en agua sin bikini
							//por lo que le he dado prioridad a que busque el bikini si tambien lo necesita
							else if(sensores.terreno[2] == 'B' and !hayplanBateria and !actual.zapatillas and !hayplanZapatillas){
								//calculamos un nuevo plan
								cout << "Plan Zapatillas" << endl;
								hayplanZapatillas = pathFinding (sensores.nivel, actual, objetivosReto, plan);

								//se indica el éxito (o no) del nuevo plan
								hayPlan = hayplanZapatillas;

								accion = plan.front();
							}
							//comprobamos si en la casilla de delante hay agua, en cuyo caso, si no tenemos bikini, buscaremos
							else if(sensores.terreno[2] == 'A' and !hayplanBateria and !actual.bikini and !hayplanBikiniCerca){
								//calculamos un nuevo plan
								cout << "Plan Bikini" << endl;
								hayplanBikini = pathFinding (sensores.nivel, actual, objetivosReto, plan);

								//se indica el éxito (o no) del nuevo plan
								hayPlan = hayplanBikini;

								accion = plan.front();
							}

						}

						if(!esperarAldeanoOLobo && plan.size() > 0)
							plan.erase(plan.begin());   //eliminamos la accion del plan
						
					}
					else{
						cout << "no se pudo encontrar un plan\n";
					}
				}
			}
		}
		
	}
	accionAnterior = accion;
	return accion;
}


// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
{
	switch (level)
	{
	case 0: {
		cout << "Demo\n";
		estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;
	}
	case 1: {
		cout << "Optimo numero de acciones\n";
		// Incluir aqui la llamada al busqueda en anchura
		estado un_objetivo_anchura;
		un_objetivo_anchura = destino.front();
		cout << "fila: " << un_objetivo_anchura.fila << " col: " << un_objetivo_anchura.columna << endl;
		return pathFinding_Anchura(origen, un_objetivo_anchura, plan);
		break;
	}
	case 2:{
		cout << "Optimo en coste\n";
		estado un_objetivo_cu;
		un_objetivo_cu = destino.front();
		cout << "fila: " << un_objetivo_cu.fila << " col: " << un_objetivo_cu.columna << endl;
		// return pathFinding_Costo_Uniforme(origen, un_objetivo_cu, plan);
		return pathFinding_AEstrella(origen, un_objetivo_cu, plan);
		break;
	}
	case 3: {
		cout << "Reto 1: Descubrir el mapa\n";
		return pathFinding_AEstrella(origen, dst, plan);
		cout << "No implementado aun\n";
		break;
	}
	case 4: {
		cout << "Reto 2: Maximizar objetivos\n";
		return pathFinding_AEstrellaReto(origen, destino, plan);
		cout << "No implementado aun\n";
		break;
	}
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool ComportamientoJugador::EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

struct nodo
{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la listaffss
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "i ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

// NIVEL 1
//---------------------- Implementación de la busqueda en anchura ---------------------------
//Entran los puntos origen y destino y devuelve la secuencia de acciones en la variable plan
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan" << endl;

	plan.clear();

	set<estado, ComparaEstados> Cerrados; //Lista de cerrados
	queue<nodo> Abiertos; 				  //Cola de abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while(!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		//Tomo el siguiente valor de Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.front(); //Elemento al frente de la cola
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

// NIVEL 2
//---------------------- Implementación del costo uniforme ---------------------------
//Ahora debemos controlar el consumo de bateria
/*
	Primero se realizó el nivel mediante el algoritmo de costo uniforme, sin embargo, en mapas grandes era poco eficiente
	Por ello posteriormente se realizó el nivel mediante el algoritmo A* 
	Se han dejado ambos algoritmos pero el que se utiliza es el A*. Si se desea utilizar el de costo Uniforme debe descomentar la linea X
	Para ello, he añadido al struct estado dos variables booleanas  (bikini y zapatillas)
	Ademas se ha añadido un parametro val para controlar el valor del coste acumulado
*/


/*
Algoritmo A*
*/

struct nodoValorA{
	estado st;
	list<Action> secuencia;
	int val; //Parametro para el coste acumulado 
	int heuristica; //Coste estimado (coste estimado desde el estado actual al destino)
	//Ordena según la función f(n)=g(n)+ h(n) donde:
	//g(n) <=> val
	//h(n) <=> heuristico

	//Sobrecarga del operador < para cola con prioridad permitiendonos expandir el nodo mas prometedor
	bool operator < (const nodoValorA &n) const {
		//Como nuestro nodo mas prioritario sera el de menor coste:
		if( ((this->val) + (this->heuristica)) != (n.val + n.heuristica)){
            if( ((this->val) + (this->heuristica)) > (n.val + n.heuristica) )
                return true;
            else
                return false;
	    }else
            if(secuencia.size() > n.secuencia.size()) //Si hay igualdad de coste consideraremos mas prioritario el de menos acciones
                return true;
            else
                return false;
	}
};

//Comparo teniendo en cuenta si tiene bikini/zapaillas devolviendo true si son estados diferentes
struct ComparaEstadosValorA{
	bool operator()(const estado &a, const estado &b) const{
		if ((a.fila > b.fila) or (a.fila == b.fila and a.columna > b.columna) or
			(a.fila == b.fila and a.columna == b.columna and a.orientacion > b.orientacion) or
			(a.fila == b.fila and a.columna == b.columna and a.orientacion == b.orientacion and !a.bikini and b.bikini) or
			(a.fila == b.fila and a.columna == b.columna and a.orientacion == b.orientacion and a.bikini == b.bikini and !a.zapatillas and b.zapatillas))
			return true;
		else
			return false;
	}
};

//Calculo del valor heuristico
int valorHeuristico(const estado &origen, const estado &destino) {
    return max(abs(destino.fila - origen.fila), abs(destino.columna - origen.columna));
}

//Funcion que devuelve el gasto de bateria de una casilla dependiendo de la situacion actual de nuestro personaje
int ComportamientoJugador::valorCasilla(estado &e, int accion){
	int fil = e.fila, col = e.columna;

	//Accion ha sido girar 90º izquierda o derecha
	if (accion == 0) {
		if (e.bikini){
			switch(mapaResultado[fil][col]){
				case 'A':
					return 5;
					break;
				case 'B':
					return 3;
					break;
				case 'T':
					return 2;
					break;
				default:
					return 1;
			}
		} else if (e.zapatillas){
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 500;
				break;
			case 'B':
				return 1;
				break;
			case 'T':
				return 2;
				break;
			default:
				return 1;
			}
		} else {
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 500;
				break;
			case 'B':
				return 3;
				break;
			case 'T':
				return 2;
				break;
			default:
				return 1;
			}
		}
	} else if (accion == 1) { // giro de 45º izquierda o derecha
		if (e.bikini)
		{
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 2;
				break;
			case 'B':
				return 2;
				break;
			case 'T':
				return 1;
				break;
			default:
				return 1;
			}
		}
		else if (e.zapatillas)
		{
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 300;
				break;
			case 'B':
				return 1;
				break;
			case 'T':
				return 1;
				break;
			default:
				return 1;
			}
		}
		else
		{
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 300;
				break;
			case 'B':
				return 2;
				break;
			case 'T':
				return 1;
				break;
			default:
				return 1;
			}
		}
	} else if (accion == 2) { // actForward (no gira)
		if (e.bikini)
		{
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 10;
				break;
			case 'B':
				return 100;
				break;
			case 'T':
				return 2;
				break;
			default:
				return 1;
			}
		}
		else if (e.zapatillas)
		{
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 200;
				break;
			case 'B':
				return 15;
				break;
			case 'T':
				return 2;
				break;
			default:
				return 1;
			}
		}
		else
		{
			switch (mapaResultado[fil][col])
			{
			case 'A':
				return 200;
				break;
			case 'B':
				return 100;
				break;
			case 'T':
				return 2;
				break;
			default:
				return 1;
			}
		}
	}
}

//Algoritmo A*
bool ComportamientoJugador::pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan){
	//Borro la lista
	cout << "Calculando plan" << endl;
	plan.clear();
	set<estado, ComparaEstadosValorA> Cerrados; //Lista de cerrados
	/*
	En esta cola de prioridad el elemento de menos valor se situa al frente
	*/
	priority_queue<nodoValorA> Abiertos;		   //Cola de abiertos

	nodoValorA current;
	current.st.fila = origen.fila;
	current.st.columna = origen.columna;
	current.st.orientacion = origen.orientacion;
	current.st.bikini = false;
	current.st.zapatillas = false;
	current.secuencia.empty();
	current.val = 0; //Para el nodo inicial el coste acumulado es 0
	int currentCasVal = 0;

	if (mapaResultado[current.st.fila][current.st.columna] == 'K')
		current.st.bikini = true;

	if (mapaResultado[current.st.fila][current.st.columna] == 'D')
		current.st.zapatillas = true;
	
	current.heuristica = valorHeuristico(origen, destino);
	Abiertos.push(current);
	
	while(!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna)){
		Abiertos.pop();

		if (Cerrados.find(current.st) == Cerrados.end()){
			Cerrados.insert(current.st);
			if (mapaResultado[current.st.fila][current.st.columna] == 'K'){
				current.st.bikini = true;
				current.st.zapatillas = false;
			}
			if (mapaResultado[current.st.fila][current.st.columna] == 'D'){
				current.st.zapatillas = true;
				current.st.bikini = false;
			}

			// Generar descendiente de girar a la derecha 90 grados
			nodoValorA hijoTurnR = current;
			hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
			if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
			{
				giro = 0;
				currentCasVal = valorCasilla(current.st, giro);
				hijoTurnR.val += currentCasVal;
				hijoTurnR.heuristica = valorHeuristico(hijoTurnR.st, destino);
				hijoTurnR.secuencia.push_back(actTURN_R);
				Abiertos.push(hijoTurnR);
			}

			// Generar descendiente de girar a la derecha 45 grados
			nodoValorA hijoSEMITurnR = current;
			hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
			if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
			{
				giro = 1;
				currentCasVal = valorCasilla(current.st, giro);
				hijoSEMITurnR.val += currentCasVal;
				hijoSEMITurnR.heuristica = valorHeuristico(hijoSEMITurnR.st, destino);
				hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
				Abiertos.push(hijoSEMITurnR);
			}

			// Generar descendiente de girar a la izquierda 90 grados
			nodoValorA hijoTurnL = current;
			hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
			if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
			{
				giro = 0;
				currentCasVal = valorCasilla(current.st, giro);
				hijoTurnL.val += currentCasVal;
				hijoTurnL.heuristica = valorHeuristico(hijoTurnL.st, destino);
				hijoTurnL.secuencia.push_back(actTURN_L);
				Abiertos.push(hijoTurnL);
			}

			// Generar descendiente de girar a la izquierda 45 grados
			nodoValorA hijoSEMITurnL = current;
			hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
			if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
			{
				giro = 1;
				currentCasVal = valorCasilla(current.st, giro);
				hijoSEMITurnL.val += currentCasVal;
				hijoSEMITurnL.heuristica = valorHeuristico(hijoSEMITurnL.st, destino);
				hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
				Abiertos.push(hijoSEMITurnL);
			}

			// Generar descendiente de avanzar
			nodoValorA hijoForward = current;
			if (!HayObstaculoDelante(hijoForward.st))
			{
				if (Cerrados.find(hijoForward.st) == Cerrados.end())
				{
					giro = 2;
					currentCasVal = valorCasilla(current.st, giro);
					hijoForward.val += currentCasVal;
					hijoForward.heuristica = valorHeuristico(hijoForward.st, destino);
					hijoForward.secuencia.push_back(actFORWARD);
					Abiertos.push(hijoForward);
				}
			}
		}

		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";
	cout << current.st.fila << " " << destino.fila;
	cout << endl << current.st.columna << " " << destino.columna << endl;
	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

//Costo uniforme
struct nodoValor{
	estado st;
	list<Action> secuencia;
	int val; //Parametro para el costo uniforme (coste acumulado)

	//Sobrecarga del operador < para cola con prioridad permitiendonos expandir el nodo mas prometedor
	bool operator < (const nodoValor &nodo) const {
		//Como nuestro nodo mas prioritario sera el de menor coste:
		if (this->val != nodo.val){
			if (this->val > nodo.val)
				return true;
			else 
				return false;
		} else {
			// Si hay igualdad de coste consideraremos mas prioritario el de menos acciones
			if (secuencia.size() > nodo.secuencia.size())
				return true;
			else
				return false;
		}
	}
};

//Comparo teniendo en cuenta si tiene bikini/zapaillas devolviendo true si son estados diferentes
struct ComparaEstadosValor{
	bool operator()(const estado &a, const estado &b) const{
		if ((a.fila > b.fila) or (a.fila == b.fila and a.columna > b.columna) or
			(a.fila == b.fila and a.columna == b.columna and a.orientacion > b.orientacion) or
			(a.fila == b.fila and a.columna == b.columna and a.orientacion == b.orientacion and !a.bikini and b.bikini) or
			(a.fila == b.fila and a.columna == b.columna and a.orientacion == b.orientacion and a.bikini == b.bikini and !a.zapatillas and b.zapatillas))
			return true;
		else
			return false;
	}
};


//Algoritmo Costo Uniforme
bool ComportamientoJugador::pathFinding_Costo_Uniforme(const estado &origen, const estado &destino, list<Action> &plan){
	//Borro la lista
	cout << "Calculando plan" << endl;
	plan.clear();
	set<estado, ComparaEstadosValor> Cerrados; //Lista de cerrados
	/*
	En esta cola de prioridad el elemento de menos valor se situa al frente
	*/
	priority_queue<nodoValor> Abiertos;		   //Cola de abiertos
	nodoValor current;
	current.st.fila = origen.fila;
	current.st.columna = origen.columna;
	current.st.orientacion = origen.orientacion;
	current.st.bikini = false;
	current.st.zapatillas = false;
	current.secuencia.empty();
	current.val = 0; //Para el nodo inicial el coste acumulado es 0
	int currentCasVal = 0;

	if (mapaResultado[current.st.fila][current.st.columna] == 'K')
		current.st.bikini = true;

	if (mapaResultado[current.st.fila][current.st.columna] == 'D')
		current.st.zapatillas = true;

	Abiertos.push(current);
	
	while(!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna)){
		Abiertos.pop();

		if (Cerrados.find(current.st) == Cerrados.end()){
			Cerrados.insert(current.st);
			if (mapaResultado[current.st.fila][current.st.columna] == 'K'){
				current.st.bikini = true;
				current.st.zapatillas = false;
			}
			if (mapaResultado[current.st.fila][current.st.columna] == 'D'){
				current.st.zapatillas = true;
				current.st.bikini = false;
			}

			// Generar descendiente de girar a la derecha 90 grados
			nodoValor hijoTurnR = current;
			hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
			if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
			{
				giro = 0;
				currentCasVal = valorCasilla(current.st, giro);
				hijoTurnR.val += currentCasVal;
				hijoTurnR.secuencia.push_back(actTURN_R);
				Abiertos.push(hijoTurnR);
			}

			// Generar descendiente de girar a la derecha 45 grados
			nodoValor hijoSEMITurnR = current;
			hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
			if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
			{
				giro = 1;
				currentCasVal = valorCasilla(current.st, giro);
				hijoSEMITurnR.val += currentCasVal;
				hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
				Abiertos.push(hijoSEMITurnR);
			}

			// Generar descendiente de girar a la izquierda 90 grados
			nodoValor hijoTurnL = current;
			hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
			if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
			{
				giro = 0;
				currentCasVal = valorCasilla(current.st, giro);
				hijoTurnL.val += currentCasVal;
				hijoTurnL.secuencia.push_back(actTURN_L);
				Abiertos.push(hijoTurnL);
			}

			// Generar descendiente de girar a la izquierda 45 grados
			nodoValor hijoSEMITurnL = current;
			hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
			if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
			{
				giro = 1;
				currentCasVal = valorCasilla(current.st, giro);
				hijoSEMITurnL.val += currentCasVal;
				hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
				Abiertos.push(hijoSEMITurnL);
			}

			// Generar descendiente de avanzar
			nodoValor hijoForward = current;
			if (!HayObstaculoDelante(hijoForward.st))
			{
				if (Cerrados.find(hijoForward.st) == Cerrados.end())
				{
					giro = 2;
					currentCasVal = valorCasilla(current.st, giro);
					hijoForward.val += currentCasVal;
					hijoForward.secuencia.push_back(actFORWARD);
					Abiertos.push(hijoForward);
				}
			}
		}

		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";
	cout << current.st.fila << " " << destino.fila;
	cout << endl << current.st.columna << " " << destino.columna << endl;
	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}



//---------------------- Funciones auxiliares para los siguientes niveles -------------------------
void ComportamientoJugador::pintarMapa(Sensores sensores)
{
	int c = sensores.posC, f = sensores.posF;

	mapaResultado[f][c] = sensores.terreno[0];

	switch (sensores.sentido)
	{
	case 0: // norte
		mapaResultado[f - 1][c - 1] = sensores.terreno[1];
		mapaResultado[f - 1][c] = sensores.terreno[2];
		mapaResultado[f - 1][c + 1] = sensores.terreno[3];
		mapaResultado[f - 2][c - 2] = sensores.terreno[4];
		mapaResultado[f - 2][c - 1] = sensores.terreno[5];
		mapaResultado[f - 2][c] = sensores.terreno[6];
		mapaResultado[f - 2][c + 1] = sensores.terreno[7];
		mapaResultado[f - 2][c + 2] = sensores.terreno[8];
		mapaResultado[f - 3][c - 3] = sensores.terreno[9];
		mapaResultado[f - 3][c - 2] = sensores.terreno[10];
		mapaResultado[f - 3][c - 1] = sensores.terreno[11];
		mapaResultado[f - 3][c] = sensores.terreno[12];
		mapaResultado[f - 3][c + 1] = sensores.terreno[13];
		mapaResultado[f - 3][c + 2] = sensores.terreno[14];
		mapaResultado[f - 3][c + 3] = sensores.terreno[15];
		break;
	
	case 1: // noreste
		mapaResultado[f][c + 1] = sensores.terreno[3];
		mapaResultado[f][c + 2] = sensores.terreno[8];
		mapaResultado[f][c + 3] = sensores.terreno[15];

		mapaResultado[f - 1][c] = sensores.terreno[1];
		mapaResultado[f - 1][c + 1] = sensores.terreno[2];
		mapaResultado[f - 1][c + 2] = sensores.terreno[7];
		mapaResultado[f - 1][c + 3] = sensores.terreno[14];

		mapaResultado[f - 2][c] = sensores.terreno[4];
		mapaResultado[f - 2][c + 1] = sensores.terreno[5];
		mapaResultado[f - 2][c + 2] = sensores.terreno[6];
		mapaResultado[f - 2][c + 3] = sensores.terreno[13];

		mapaResultado[f - 3][c] = sensores.terreno[9];
		mapaResultado[f - 3][c + 1] = sensores.terreno[10];
		mapaResultado[f - 3][c + 2] = sensores.terreno[11];
		mapaResultado[f - 3][c + 3] = sensores.terreno[12];
		
		break;

	case 2: // este
		mapaResultado[f - 1][c + 1] = sensores.terreno[1];
		mapaResultado[f][c + 1] = sensores.terreno[2];
		mapaResultado[f + 1][c + 1] = sensores.terreno[3];
		mapaResultado[f - 2][c + 2] = sensores.terreno[4];
		mapaResultado[f - 1][c + 2] = sensores.terreno[5];
		mapaResultado[f][c + 2] = sensores.terreno[6];
		mapaResultado[f + 1][c + 2] = sensores.terreno[7];
		mapaResultado[f + 2][c + 2] = sensores.terreno[8];
		mapaResultado[f - 3][c + 3] = sensores.terreno[9];
		mapaResultado[f - 2][c + 3] = sensores.terreno[10];
		mapaResultado[f - 1][c + 3] = sensores.terreno[11];
		mapaResultado[f][c + 3] = sensores.terreno[12];
		mapaResultado[f + 1][c + 3] = sensores.terreno[13];
		mapaResultado[f + 2][c + 3] = sensores.terreno[14];
		mapaResultado[f + 3][c + 3] = sensores.terreno[15];
		break;

	case 3: // sureste
		mapaResultado[f+1][c] = sensores.terreno[3];
		mapaResultado[f+2][c] = sensores.terreno[8];
		mapaResultado[f+3][c] = sensores.terreno[15];

		mapaResultado[f][c + 1] = sensores.terreno[1];
		mapaResultado[f + 1][c + 1] = sensores.terreno[2];
		mapaResultado[f + 2][c + 1] = sensores.terreno[7];
		mapaResultado[f + 3][c + 1] = sensores.terreno[14];

		mapaResultado[f][c + 2] = sensores.terreno[4];
		mapaResultado[f + 1][c + 2] = sensores.terreno[5];
		mapaResultado[f + 2][c + 2] = sensores.terreno[6];
		mapaResultado[f + 3][c + 2] = sensores.terreno[13];
		
		mapaResultado[f][c + 3] = sensores.terreno[9];
		mapaResultado[f + 1][c + 3] = sensores.terreno[10];
		mapaResultado[f + 2][c + 3] = sensores.terreno[11];
		mapaResultado[f + 3][c + 3] = sensores.terreno[12];
		
		break;

	case 4: // sur
		mapaResultado[f + 1][c + 1] = sensores.terreno[1];
		mapaResultado[f + 1][c] = sensores.terreno[2];
		mapaResultado[f + 1][c - 1] = sensores.terreno[3];
		mapaResultado[f + 2][c + 2] = sensores.terreno[4];
		mapaResultado[f + 2][c + 1] = sensores.terreno[5];
		mapaResultado[f + 2][c] = sensores.terreno[6];
		mapaResultado[f + 2][c - 1] = sensores.terreno[7];
		mapaResultado[f + 2][c - 2] = sensores.terreno[8];
		mapaResultado[f + 3][c + 3] = sensores.terreno[9];
		mapaResultado[f + 3][c + 2] = sensores.terreno[10];
		mapaResultado[f + 3][c + 1] = sensores.terreno[11];
		mapaResultado[f + 3][c] = sensores.terreno[12];
		mapaResultado[f + 3][c - 1] = sensores.terreno[13];
		mapaResultado[f + 3][c - 2] = sensores.terreno[14];
		mapaResultado[f + 3][c - 3] = sensores.terreno[15];
		break;

	case 5: // suroeste
		mapaResultado[f][c - 1] = sensores.terreno[3];
		mapaResultado[f][c - 2] = sensores.terreno[8];
		mapaResultado[f][c - 3] = sensores.terreno[15];

		mapaResultado[f + 1][c] = sensores.terreno[1];
		mapaResultado[f + 1][c - 1] = sensores.terreno[2];
		mapaResultado[f + 1][c - 2] = sensores.terreno[7];
		mapaResultado[f + 1][c - 3] = sensores.terreno[14];

		mapaResultado[f + 2][c] = sensores.terreno[4];
		mapaResultado[f + 2][c - 1] = sensores.terreno[5];
		mapaResultado[f + 2][c - 2] = sensores.terreno[6];
		mapaResultado[f + 2][c - 3] = sensores.terreno[13];
		
		mapaResultado[f + 3][c] = sensores.terreno[9];
		mapaResultado[f + 3][c - 1] = sensores.terreno[10];
		mapaResultado[f + 3][c - 2] = sensores.terreno[11];
		mapaResultado[f + 3][c - 3] = sensores.terreno[12];
		
		break;
	case 6: // oeste
		cout << "oeste" << endl;
		mapaResultado[f + 1][c - 1] = sensores.terreno[1];
		mapaResultado[f][c - 1] = sensores.terreno[2];
		mapaResultado[f - 1][c - 1] = sensores.terreno[3];
		mapaResultado[f + 2][c - 2] = sensores.terreno[4];
		mapaResultado[f + 1][c - 2] = sensores.terreno[5];
		mapaResultado[f][c - 2] = sensores.terreno[6];
		mapaResultado[f - 1][c - 2] = sensores.terreno[7];
		mapaResultado[f - 2][c - 2] = sensores.terreno[8];
		mapaResultado[f + 3][c - 3] = sensores.terreno[9];
		mapaResultado[f + 2][c - 3] = sensores.terreno[10];
		mapaResultado[f + 1][c - 3] = sensores.terreno[11];
		mapaResultado[f][c - 3] = sensores.terreno[12];
		mapaResultado[f - 1][c - 3] = sensores.terreno[13];
		mapaResultado[f - 2][c - 3] = sensores.terreno[14];
		mapaResultado[f - 3][c - 3] = sensores.terreno[15];
		break;
	case 7: // noroeste
		mapaResultado[f - 1][c] = sensores.terreno[3];
		mapaResultado[f - 2][c] = sensores.terreno[8];
		mapaResultado[f - 3][c] = sensores.terreno[15];

		mapaResultado[f][c - 1] = sensores.terreno[1];
		mapaResultado[f - 1][c - 1] = sensores.terreno[2];
		mapaResultado[f - 2][c - 1] = sensores.terreno[7];
		mapaResultado[f - 3][c - 1] = sensores.terreno[14];

		mapaResultado[f][c - 2] = sensores.terreno[4];
		mapaResultado[f - 1][c - 2] = sensores.terreno[5];
		mapaResultado[f - 2][c - 2] = sensores.terreno[6];
		mapaResultado[f - 3][c - 2] = sensores.terreno[13];
		
		mapaResultado[f][c - 3] = sensores.terreno[9];
		mapaResultado[f - 1][c - 3] = sensores.terreno[10];
		mapaResultado[f - 2][c - 3] = sensores.terreno[11];
		mapaResultado[f - 3][c - 3] = sensores.terreno[12];
		
		break;
	}
}

void ComportamientoJugador::pintarMapaNivel4(Sensores sensores)
{
	
	int c = actual.columna, f = actual.fila;

	mapaResultado[f][c] = sensores.terreno[0];

	switch (actual.orientacion)
	{
	
	case 0: // norte
		mapaResultado[f - 1][c - 1] = sensores.terreno[1];
		mapaResultado[f - 1][c] = sensores.terreno[2];
		mapaResultado[f - 1][c + 1] = sensores.terreno[3];
		mapaResultado[f - 2][c - 2] = sensores.terreno[4];
		mapaResultado[f - 2][c - 1] = sensores.terreno[5];
		mapaResultado[f - 2][c] = sensores.terreno[6];
		mapaResultado[f - 2][c + 1] = sensores.terreno[7];
		mapaResultado[f - 2][c + 2] = sensores.terreno[8];
		mapaResultado[f - 3][c - 3] = sensores.terreno[9];
		mapaResultado[f - 3][c - 2] = sensores.terreno[10];
		mapaResultado[f - 3][c - 1] = sensores.terreno[11];
		mapaResultado[f - 3][c] = sensores.terreno[12];
		mapaResultado[f - 3][c + 1] = sensores.terreno[13];
		mapaResultado[f - 3][c + 2] = sensores.terreno[14];
		mapaResultado[f - 3][c + 3] = sensores.terreno[15];
		break;
	
	case 1: // noreste
		mapaResultado[f][c + 1] = sensores.terreno[3];
		mapaResultado[f][c + 2] = sensores.terreno[8];
		mapaResultado[f][c + 3] = sensores.terreno[15];

		mapaResultado[f - 1][c] = sensores.terreno[1];
		mapaResultado[f - 1][c + 1] = sensores.terreno[2];
		mapaResultado[f - 1][c + 2] = sensores.terreno[7];
		mapaResultado[f - 1][c + 3] = sensores.terreno[14];

		mapaResultado[f - 2][c] = sensores.terreno[4];
		mapaResultado[f - 2][c + 1] = sensores.terreno[5];
		mapaResultado[f - 2][c + 2] = sensores.terreno[6];
		mapaResultado[f - 2][c + 3] = sensores.terreno[13];

		mapaResultado[f - 3][c] = sensores.terreno[9];
		mapaResultado[f - 3][c + 1] = sensores.terreno[10];
		mapaResultado[f - 3][c + 2] = sensores.terreno[11];
		mapaResultado[f - 3][c + 3] = sensores.terreno[12];
		
		break;

	case 2: // este
		mapaResultado[f - 1][c + 1] = sensores.terreno[1];
		mapaResultado[f][c + 1] = sensores.terreno[2];
		mapaResultado[f + 1][c + 1] = sensores.terreno[3];
		mapaResultado[f - 2][c + 2] = sensores.terreno[4];
		mapaResultado[f - 1][c + 2] = sensores.terreno[5];
		mapaResultado[f][c + 2] = sensores.terreno[6];
		mapaResultado[f + 1][c + 2] = sensores.terreno[7];
		mapaResultado[f + 2][c + 2] = sensores.terreno[8];
		mapaResultado[f - 3][c + 3] = sensores.terreno[9];
		mapaResultado[f - 2][c + 3] = sensores.terreno[10];
		mapaResultado[f - 1][c + 3] = sensores.terreno[11];
		mapaResultado[f][c + 3] = sensores.terreno[12];
		mapaResultado[f + 1][c + 3] = sensores.terreno[13];
		mapaResultado[f + 2][c + 3] = sensores.terreno[14];
		mapaResultado[f + 3][c + 3] = sensores.terreno[15];
		break;

	case 3: // sureste
		mapaResultado[f+1][c] = sensores.terreno[3];
		mapaResultado[f+2][c] = sensores.terreno[8];
		mapaResultado[f+3][c] = sensores.terreno[15];

		mapaResultado[f][c + 1] = sensores.terreno[1];
		mapaResultado[f + 1][c + 1] = sensores.terreno[2];
		mapaResultado[f + 2][c + 1] = sensores.terreno[7];
		mapaResultado[f + 3][c + 1] = sensores.terreno[14];

		mapaResultado[f][c + 2] = sensores.terreno[4];
		mapaResultado[f + 1][c + 2] = sensores.terreno[5];
		mapaResultado[f + 2][c + 2] = sensores.terreno[6];
		mapaResultado[f + 3][c + 2] = sensores.terreno[13];
		
		mapaResultado[f][c + 3] = sensores.terreno[9];
		mapaResultado[f + 1][c + 3] = sensores.terreno[10];
		mapaResultado[f + 2][c + 3] = sensores.terreno[11];
		mapaResultado[f + 3][c + 3] = sensores.terreno[12];
		
		break;

	case 4: // sur
		mapaResultado[f + 1][c + 1] = sensores.terreno[1];
		mapaResultado[f + 1][c] = sensores.terreno[2];
		mapaResultado[f + 1][c - 1] = sensores.terreno[3];
		mapaResultado[f + 2][c + 2] = sensores.terreno[4];
		mapaResultado[f + 2][c + 1] = sensores.terreno[5];
		mapaResultado[f + 2][c] = sensores.terreno[6];
		mapaResultado[f + 2][c - 1] = sensores.terreno[7];
		mapaResultado[f + 2][c - 2] = sensores.terreno[8];
		mapaResultado[f + 3][c + 3] = sensores.terreno[9];
		mapaResultado[f + 3][c + 2] = sensores.terreno[10];
		mapaResultado[f + 3][c + 1] = sensores.terreno[11];
		mapaResultado[f + 3][c] = sensores.terreno[12];
		mapaResultado[f + 3][c - 1] = sensores.terreno[13];
		mapaResultado[f + 3][c - 2] = sensores.terreno[14];
		mapaResultado[f + 3][c - 3] = sensores.terreno[15];
		break;

	case 5: // suroeste
		mapaResultado[f][c - 1] = sensores.terreno[3];
		mapaResultado[f][c - 2] = sensores.terreno[8];
		mapaResultado[f][c - 3] = sensores.terreno[15];

		mapaResultado[f + 1][c] = sensores.terreno[1];
		mapaResultado[f + 1][c - 1] = sensores.terreno[2];
		mapaResultado[f + 1][c - 2] = sensores.terreno[7];
		mapaResultado[f + 1][c - 3] = sensores.terreno[14];

		mapaResultado[f + 2][c] = sensores.terreno[4];
		mapaResultado[f + 2][c - 1] = sensores.terreno[5];
		mapaResultado[f + 2][c - 2] = sensores.terreno[6];
		mapaResultado[f + 2][c - 3] = sensores.terreno[13];
		
		mapaResultado[f + 3][c] = sensores.terreno[9];
		mapaResultado[f + 3][c - 1] = sensores.terreno[10];
		mapaResultado[f + 3][c - 2] = sensores.terreno[11];
		mapaResultado[f + 3][c - 3] = sensores.terreno[12];
		
		break;
	case 6: // oeste
		cout << "oeste" << endl;
		mapaResultado[f + 1][c - 1] = sensores.terreno[1];
		mapaResultado[f][c - 1] = sensores.terreno[2];
		mapaResultado[f - 1][c - 1] = sensores.terreno[3];
		mapaResultado[f + 2][c - 2] = sensores.terreno[4];
		mapaResultado[f + 1][c - 2] = sensores.terreno[5];
		mapaResultado[f][c - 2] = sensores.terreno[6];
		mapaResultado[f - 1][c - 2] = sensores.terreno[7];
		mapaResultado[f - 2][c - 2] = sensores.terreno[8];
		mapaResultado[f + 3][c - 3] = sensores.terreno[9];
		mapaResultado[f + 2][c - 3] = sensores.terreno[10];
		mapaResultado[f + 1][c - 3] = sensores.terreno[11];
		mapaResultado[f][c - 3] = sensores.terreno[12];
		mapaResultado[f - 1][c - 3] = sensores.terreno[13];
		mapaResultado[f - 2][c - 3] = sensores.terreno[14];
		mapaResultado[f - 3][c - 3] = sensores.terreno[15];
		break;
	case 7: // noroeste
		mapaResultado[f - 1][c] = sensores.terreno[3];
		mapaResultado[f - 2][c] = sensores.terreno[8];
		mapaResultado[f - 3][c] = sensores.terreno[15];

		mapaResultado[f][c - 1] = sensores.terreno[1];
		mapaResultado[f - 1][c - 1] = sensores.terreno[2];
		mapaResultado[f - 2][c - 1] = sensores.terreno[7];
		mapaResultado[f - 3][c - 1] = sensores.terreno[14];

		mapaResultado[f][c - 2] = sensores.terreno[4];
		mapaResultado[f - 1][c - 2] = sensores.terreno[5];
		mapaResultado[f - 2][c - 2] = sensores.terreno[6];
		mapaResultado[f - 3][c - 2] = sensores.terreno[13];
		
		mapaResultado[f][c - 3] = sensores.terreno[9];
		mapaResultado[f - 1][c - 3] = sensores.terreno[10];
		mapaResultado[f - 2][c - 3] = sensores.terreno[11];
		mapaResultado[f - 3][c - 3] = sensores.terreno[12];
		
		break;
	}
}




// NIVEL 3
//---------------------- Reto 1: Descubrir el mayor porcentaje del mapa ---------------------------
void ComportamientoJugador::nuevoDestino(){
	// srand(time(NULL));
	estado aux;
	aux.fila = (3 + rand()) % mapaResultado.size();
	aux.columna = (3 + rand()) % mapaResultado.size();
	destinos.push(aux);

}

bool ComportamientoJugador::EsAguaOBosqueYNoTengoComplementos(unsigned char casilla) {

	if (!actual.zapatillas and casilla == 'B')
		return true;
	
	if (!actual.bikini and casilla == 'A')
		return true;
	
	return false;
}

// NIVEL 4
//---------------------- Reto 2 (Maximizar número de objetivos)  ---------------------------
//Comprobar si el estado "e" pertenece a una lista "objetivos" y si pertenece se borra de dicha lista
void ComportamientoJugador::BorrarSiObjetivo(const estado &e,list<estado> &objetivos){
    bool borrado = false;
    for(auto it=objetivos.begin(); it!=objetivos.end() and !borrado; ++it){
        if(e.fila == (*it).fila and e.columna == (*it).columna){
            objetivos.erase(it);
            borrado = true;
        }
    }
}

//Búsqueda de batería en el terreno conocido
//Devuelve true si ha encontrado la batería, y por tanto irá a por ella. En estado almacena su casilla correspondiente.
pair<bool,estado> ComportamientoJugador::IrAPorBateria(const estado &actual){
    pair<bool,estado> salida;
    salida.first = false;
    for(int fil = 3; fil< mapaResultado.size() && !salida.first; fil++){
        for(int col = 3; col< mapaResultado[0].size() && !salida.first; col++){
            if(mapaResultado[fil][col] == 'X'){
                salida.second.fila = fil;
                salida.second.columna = col;
                salida.first = true;
            }
        }
    }

    return salida;
}

//Búsqueda de bikini en el terreno conocido
//Devuelve true si ha encontrado el bikini, y por tanto irá a por el. En estado almacena su casilla correspondiente.
pair<bool,estado> ComportamientoJugador::IrAPorBikini(const estado &actual){
    pair<bool,estado> salida;
    salida.first = false;
    for(int fil = 3; fil< mapaResultado.size() && !salida.first; fil++){
        for(int col = 3; col< mapaResultado[0].size() && !salida.first; col++){
            if(mapaResultado[fil][col] == 'K'){
                salida.second.fila = fil;
                salida.second.columna = col;
                salida.first = true;
            }
        }
    }

    return salida;
}

//Búsqueda de zapatillas en el terreno conocido
//Devuelve true si ha encontrado las zapatillas, y por tanto irá a por ellas. En estado almacena su casilla correspondiente.
pair<bool,estado> ComportamientoJugador::IrAPorZapatillas(const estado &actual){
    pair<bool,estado> salida;
    salida.first = false;
    for(int fil = 3; fil< mapaResultado.size() and !salida.first; fil++){
        for(int col = 3; col< mapaResultado[0].size() and !salida.first; col++){
            if(mapaResultado[fil][col] == 'D'){
                salida.second.fila = fil;
                salida.second.columna = col;
                salida.first = true;
            }
        }
    }

    return salida;
}


//Devuelve true si ha decidido ir a por un complemento porque se encuentra cerca, y el estado de ese complemento, que corresponde al estado del objetivo alcanzado una vez se elabore el plan.
//Con esto investigamos, si lo conocemos, el rango de 4x4 casillas que rodean a la actual
pair<bool,estado> ComportamientoJugador::IrAPorComplemento(const estado &actual, const list<estado> &objetivos){
    bool ir = false;
    pair<bool,estado> salida;

	//si no lleva ya bikini
	if(!actual.bikini && !ir){
		//si está a una distancia de máximo 4 casillas de la actual,y no se sale del mapa, iremos a por él
		for(int i = 1; i <= 4 && !ir; i++){
			for (int j = 1; j <= 4 && !ir; j++){
				if((actual.fila-i >= 0) and !ir){ //si la fila no se sale por arriba del mapa
					if(mapaResultado[actual.fila-i][actual.columna] == 'K'){
						salida.second.fila = actual.fila-i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila-i][actual.columna-j] == 'K'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){ //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila-i][actual.columna+j] == 'K'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}

				if((actual.columna-j >= 0) and !ir){ //si la columna no se sale por la izquierda del mapa
					if(mapaResultado[actual.fila][actual.columna-j] == 'K'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna-j;
						ir = true;
					}
				}

				if((actual.columna+j < mapaResultado[0].size()) and !ir){ //si la columna no se sale por la derecha del mapa
					if(mapaResultado[actual.fila][actual.columna+j] == 'K'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna+j;
						ir = true;
					}
				}


				if((actual.fila+i < mapaResultado.size()) and !ir){ //si la fila no se sale por abajo del mapa
					if(mapaResultado[actual.fila+i][actual.columna] == 'K'){
						salida.second.fila = actual.fila+i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila+i][actual.columna-j] == 'K'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){  //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila+i][actual.columna+j] == 'K'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}
			}
		}
	}
	//si no lleva ya zapatillas
	else if(!actual.zapatillas && !ir){
		//si está a una distancia de máximo 4 casillas de la actual, iremos a por ellas
		for(int i = 1; i <= 4 && !ir; i++){
			for (int j = 1; j <= 4 && !ir; j++){
				if((actual.fila-i >= 0) and !ir){ //si la fila no se sale por arriba del mapa
					if(mapaResultado[actual.fila-i][actual.columna] == 'D'){
						salida.second.fila = actual.fila-i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila-i][actual.columna-j] == 'D'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){ //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila-i][actual.columna+j] == 'D'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}

				if((actual.columna-j >= 0) and !ir){ //si la columna no se sale por la izquierda del mapa
					if(mapaResultado[actual.fila][actual.columna-j] == 'D'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna-j;
						ir = true;
					}
				}

				if((actual.columna+j < mapaResultado[0].size()) and !ir){ //si la columna no se sale por la derecha del mapa
					if(mapaResultado[actual.fila][actual.columna+j] == 'D'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna+j;
						ir = true;
					}
				}


				if((actual.fila+i < mapaResultado.size()) and !ir){ //si la fila no se sale por abajo del mapa
					if(mapaResultado[actual.fila+i][actual.columna] == 'D'){
						salida.second.fila = actual.fila+i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila+i][actual.columna-j] == 'D'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){  //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila+i][actual.columna+j] == 'D'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}
			}
		}
	}
    salida.first = ir;

    return salida;
}


//Devuelve true si ha encontrado un bikini cerca, y el estado de ese complemento, que corresponde al estado del objetivo alcanzado una vez se elabore el plan.
//Con esto investigamos, si lo conocemos, el rango de 4x4 casillas que rodean a la actual
pair<bool,estado> ComportamientoJugador::IrAPorBikiniCerca(const estado &actual){
    bool ir = false;
    pair<bool,estado> salida;

	//si no lleva ya bikini
	if(!actual.bikini && !ir){
		//si está a una distancia de máximo 4 casillas de la actual,y no se sale del mapa, iremos a por él
		for(int i = 1; i <= 4 && !ir; i++){
			for (int j = 1; j <= 4 && !ir; j++){
				if((actual.fila-i >= 0) and !ir){ //si la fila no se sale por arriba del mapa
					if(mapaResultado[actual.fila-i][actual.columna] == 'K'){
						salida.second.fila = actual.fila-i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila-i][actual.columna-j] == 'K'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){ //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila-i][actual.columna+j] == 'K'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}

				if((actual.columna-j >= 0) and !ir){ //si la columna no se sale por la izquierda del mapa
					if(mapaResultado[actual.fila][actual.columna-j] == 'K'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna-j;
						ir = true;
					}
				}

				if((actual.columna+j < mapaResultado[0].size()) and !ir){ //si la columna no se sale por la derecha del mapa
					if(mapaResultado[actual.fila][actual.columna+j] == 'K'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna+j;
						ir = true;
					}
				}


				if((actual.fila+i < mapaResultado.size()) and !ir){ //si la fila no se sale por abajo del mapa
					if(mapaResultado[actual.fila+i][actual.columna] == 'K'){
						salida.second.fila = actual.fila+i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila+i][actual.columna-j] == 'K'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){  //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila+i][actual.columna+j] == 'K'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}
			}
		}
	}
    salida.first = ir;

    return salida;
}

//Devuelve true si ha encontrado unas zapatillas cerca, y el estado de ese complemento, que corresponde al estado del objetivo alcanzado una vez se elabore el plan.
//Con esto investigamos, si lo conocemos, el rango de 4x4 casillas que rodean a la actual
pair<bool,estado> ComportamientoJugador::IrAPorZapatillasCerca(const estado &actual){
    bool ir = false;
    pair<bool,estado> salida;

	if(!actual.zapatillas && !ir){
		//si está a una distancia de máximo 4 casillas de la actual, iremos a por ellas
		for(int i = 1; i <= 4 && !ir; i++){
			for (int j = 1; j <= 4 && !ir; j++){
				if((actual.fila-i >= 0) and !ir){ //si la fila no se sale por arriba del mapa
					if(mapaResultado[actual.fila-i][actual.columna] == 'D'){
						salida.second.fila = actual.fila-i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila-i][actual.columna-j] == 'D'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){ //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila-i][actual.columna+j] == 'D'){
							salida.second.fila = actual.fila-i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}

				if((actual.columna-j >= 0) and !ir){ //si la columna no se sale por la izquierda del mapa
					if(mapaResultado[actual.fila][actual.columna-j] == 'D'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna-j;
						ir = true;
					}
				}

				if((actual.columna+j < mapaResultado[0].size()) and !ir){ //si la columna no se sale por la derecha del mapa
					if(mapaResultado[actual.fila][actual.columna+j] == 'D'){
						salida.second.fila = actual.fila;
						salida.second.columna = actual.columna+j;
						ir = true;
					}
				}


				if((actual.fila+i < mapaResultado.size()) and !ir){ //si la fila no se sale por abajo del mapa
					if(mapaResultado[actual.fila+i][actual.columna] == 'D'){
						salida.second.fila = actual.fila+i;
						salida.second.columna = actual.columna;
						ir = true;
					}else if(actual.columna-j >= 0){ //si la columna no se sale por la izquierda del mapa
						if(mapaResultado[actual.fila+i][actual.columna-j] == 'D'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna-j;
							ir = true;
						}
					}else if(actual.columna+j < mapaResultado[0].size()){  //si la columna no se sale por la derecha del mapa
						if(mapaResultado[actual.fila+i][actual.columna+j] == 'D'){
							salida.second.fila = actual.fila+i;
							salida.second.columna = actual.columna+j;
							ir = true;
						}
					}
				}
			}
		}
	}
    salida.first = ir;

    return salida;
}

//Devuelve una lista con los elementos de la pila, donde el tope será el primer elemento de la lista
list<estado> ComportamientoJugador::ObtenerLista(stack<estado> pila){
	list<estado> salida;
		estado aux;
		while(!pila.empty()){
			aux = pila.top();
			//añado el elemento en el tope de la pila al final de la lista
			salida.push_back(aux);
			//y eliminamos el elemnto en el tope de la pila
			pila.pop();
		}
		return salida;
}

//Devuelve una pila (con los elementos de la lista) donde el elemento más cercano al origen (heuristica nivel 2) se encuentra en el tope
stack<estado> ComportamientoJugador::ObtenerPila(const estado &origen, list<estado> destino){
	int dist_max;
    int distancia;
    estado aux;
    stack<estado> salida;
    list<estado>::iterator pos,it;
    while(!destino.empty()){
        dist_max=-1;
        distancia = 0;
        for(it=destino.begin(); it!=destino.end(); ++it){
            distancia = valorHeuristico(origen,(*it));
            if(distancia > dist_max){
                aux.fila = (*it).fila;
                aux.columna = (*it).columna;
                aux.orientacion = (*it).orientacion;
                aux.bikini = (*it).bikini;
                aux.zapatillas = (*it).zapatillas;
                aux.objetivo1 = (*it).objetivo1;
                aux.objetivo2 = (*it).objetivo2;
                aux.objetivo3 = (*it).objetivo3;
                dist_max = distancia;
                pos = it;
            }
        }
        //Borramos (de la lista de destinos) el destino más alejado del origen. De forma que no lo volvamos a tener en cuenta
        destino.erase(pos);
        //y lo añadimos al tope de la pila
        salida.push(aux);
    }

    return salida;
}

//Comprobar si un nodo pertenece a alguno de los objetivos
void EsObjetivo(estado &e,const list<estado> &objetivos) {
    int objetivoi = 1;
    for(auto it=objetivos.cbegin(); it!=objetivos.cend(); ++it){
            if(e.fila == (*it).fila and e.columna == (*it).columna){
                if(objetivoi == 1){
                    e.objetivo1 = true;
                }
                if(objetivoi == 2){
                    e.objetivo2 = true;
                }
                if(objetivoi == 3){
                    e.objetivo3 = true;
                }
            }
            objetivoi++;
    }
}

//Auxiliar para calcular el mínimo elemento de un vector
int minimo_vector(const vector<int> &v){
    int min=std::numeric_limits<int>::max();
    for(auto it=v.cbegin(); it!=v.cend(); ++it){
        if((*it) < min){
            min = (*it);
        }
    }
    return min;
}

    //Con esta función calcularemos el mínimo de los caminos posibles a los destinos aún no alcanzados
    //Este será el valor heurístico (coste estimado) para el nodo
int Heuristico(const estado &actual,const estado &o1,const estado &o2,const estado &o3){
    vector<int> v;
    int h;
    //Si no hemos alcanzado ningún objetivo
    if(!actual.objetivo1 and !actual.objetivo2 and !actual.objetivo3){
        //distancia(a,o1) + distancia(o1,o2) + distancia(o2,o3)
        int a123 = valorHeuristico(actual,o1) + valorHeuristico(o1,o2) + valorHeuristico(o2,o3);
        v.push_back(a123);
        //distancia(a,o1) + distancia(o1,o3) + distancia(o3,o2)
        int a132 = valorHeuristico(actual,o1) + valorHeuristico(o1,o3) + valorHeuristico(o3,o2);
        v.push_back(a132);

        //distancia(a,o2) + distancia(o2,o1) + distancia(o1,o3)
        int a213 = valorHeuristico(actual,o2) + valorHeuristico(o2,o1) + valorHeuristico(o1,o3);
        v.push_back(a213);
        //distancia(a,o2) + distancia(o2,o3) + distancia(o3,o1)
        int a231 = valorHeuristico(actual,o2) + valorHeuristico(o2,o3) + valorHeuristico(o3,o1);
        v.push_back(a231);

        //distancia(a,o3) + distancia(o3,o1) + distancia(o1,o2)
        int a312 = valorHeuristico(actual,o3) + valorHeuristico(o3,o1) + valorHeuristico(o1,o2);
        v.push_back(a312);
        //distancia(a,o3) + distancia(o3,o2) + distancia(o2,o1)
        int a321 = valorHeuristico(actual,o3) + valorHeuristico(o3,o2) + valorHeuristico(o2,o1);
        v.push_back(a321);

    }
    //Si hemos alcanzado el objetivo 1
    else if(actual.objetivo1 and !actual.objetivo2 and !actual.objetivo3){
        //distancia(a,o2) + distancia(o2,o3)
        int a23 = valorHeuristico(actual,o2) + valorHeuristico(o2,o3);
        v.push_back(a23);
        //distancia(a,o3) + distancia(o3,o2)
        int a32 = valorHeuristico(actual,o3) + valorHeuristico(o3,o2);
        v.push_back(a32);
    }
    //Si hemos alcanzado el objetivo 2
    else if(!actual.objetivo1 and actual.objetivo2 and !actual.objetivo3){
        //distancia(a,o1) + distancia(o1,o3)
        int a13 = valorHeuristico(actual,o1) + valorHeuristico(o1,o3);
        v.push_back(a13);
        //distancia(a,o3) + distancia(o3,o1)
        int a31 = valorHeuristico(actual,o3) + valorHeuristico(o3,o1);
        v.push_back(a31);
    }
    //Si hemos alcanzado el objetivo 3
    else if(!actual.objetivo1 and !actual.objetivo2 and actual.objetivo3){
        //distancia(a,o1) + distancia(o1,o2)
        int a12 = valorHeuristico(actual,o1) + valorHeuristico(o1,o2);
        v.push_back(a12);
        //distancia(a,o2) + distancia(o2,o1)
        int a21 = valorHeuristico(actual,o2) + valorHeuristico(o2,o1);
        v.push_back(a21);
    }
    //Si hemos alcanzado los objetivos 1 y 2
    else if(actual.objetivo1 and actual.objetivo2 and !actual.objetivo3){
        //distancia(a,o3) --> distancia al objetivo 3
        int a3 = valorHeuristico(actual,o3);
        v.push_back(a3);
    }
    //Si hemos alcanzado los objetivos 1 y 3
    else if(actual.objetivo1 and !actual.objetivo2 and actual.objetivo3){
        //distancia(a,o2) --> distancia al objetivo 2
        int a2 = valorHeuristico(actual,o2);
        v.push_back(a2);
    }
    //Si hemos alcanzado los objetivos 2 y 3
    else if(!actual.objetivo1 and actual.objetivo2 and actual.objetivo3){
        //distancia(a,o1) --> distancia al objetivo 1
        int a1 = valorHeuristico(actual,o1);
        v.push_back(a1);
    }
    //Si hemos alcanzado los 3 objetivos
    else{
        v.push_back(0);
    }
    h = minimo_vector(v);
    return h;
}

//Va actualizando la posicion del jugador ya que la obtenemos al principio con actwhereis y despues no la conocemos por lo que la actualizamos manualmente
void ComportamientoJugador::actualizarPosicion() {
	int fil = actual.fila, col = actual.columna;
	switch (accionAnterior) {
		case actFORWARD:
			switch (actual.orientacion){
				case 0:
					fil--;
					break;
				case 1:
					fil--;
					col++;
					break;
				case 2:
					col++;
					break;
				case 3:
					fil++;
					col++;
					break;
				case 4:
					fil++;
					break;
				case 5:
					fil++;
					col--;
					break;
				case 6:
					col--;
					break;
				case 7:
					fil--;
					col--;
					break;
			}
		break;

		case actTURN_L:
			actual.orientacion = (actual.orientacion + 6) % 8;
		break;

		case actTURN_R:
			actual.orientacion = (actual.orientacion + 2) % 8;
		break;

		case actSEMITURN_L:
			actual.orientacion = (actual.orientacion + 7) % 8;
		break;

		case actSEMITURN_R:
			actual.orientacion = (actual.orientacion + 1) % 8;
		break;
	}

	actual.fila = fil;
	actual.columna = col;
}

// Entran los puntos origen y la lista de destinos y devuelve la secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_AEstrellaReto(const estado &origen, const list<estado> &destino, list<Action> &plan) {
    //Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstadosValorA> Cerrados; // Lista de Cerrados
	priority_queue<nodoValorA> Abiertos; // Cola de Abiertos
    //En esta cola de prioridad el elemento de menor valor (costo)
    // se va a situar en el frente de la misma

    nodoValorA current;
	current.st.fila = origen.fila;
	current.st.columna = origen.columna;
	current.st.orientacion = origen.orientacion;
	current.st.bikini = current.st.zapatillas = current.st.objetivo1 = current.st.objetivo2 = current.st.objetivo3 = false;
	current.secuencia.empty();
    current.val = 0; //para el nodo inicial el coste acumulado es 0
    int currentCasVal = 0;

    //Comprobamos si el origen se trata de una casilla con bikini o zapatillas
    if(mapaResultado[current.st.fila][current.st.columna] == 'K'){
        current.st.bikini = true;
    }
    if(mapaResultado[current.st.fila][current.st.columna] == 'D'){
        current.st.zapatillas = true;
    }

    //Obtengo los 3 primeros estados objetivos, que son los que siempre buscaremos
    int objetivoi = 1;
    estado obj1,obj2,obj3;
    for(auto it=destino.cbegin(); it!=destino.cend(); ++it){
            if(objetivoi == 1){
                obj1 = (*it);
            }else if(objetivoi == 2){
                obj2 = (*it);
            }else if(objetivoi == 3){
                obj3 = (*it);
            }
            objetivoi++;
    }

    //si destino tiene alguna casilla de las 3 primeras con un valor no válido, se le da el valor de la casilla actual
    if(obj1.fila < 0 || obj1.fila >= mapaResultado.size() || obj1.columna < 0 || obj1.columna >= mapaResultado[0].size()){
        obj1.fila = origen.fila;
        obj1.columna = origen.columna;
    }
    if(obj2.fila < 0 || obj2.fila >= mapaResultado.size() || obj2.columna < 0 || obj2.columna >= mapaResultado[0].size()){
        obj2.fila = origen.fila;
        obj2.columna = origen.columna;
    }
    if(obj3.fila < 0 || obj3.fila >= mapaResultado.size() || obj3.columna < 0 || obj3.columna >= mapaResultado[0].size()){
        obj3.fila = origen.fila;
        obj3.columna = origen.columna;
    }

    //por si algún objetivo ha cambiado, utilizaremos una nueva lista donde los valores de los 3 primeros destinos son válidos
    objetivoi = 1;
    list<estado> destino_revisado;
    for(auto it=destino.cbegin(); it!=destino.cend(); ++it){
        if(objetivoi == 1){
                destino_revisado.push_back(obj1);
            }else if(objetivoi == 2){
                destino_revisado.push_back(obj2);
            }else if(objetivoi == 3){
                destino_revisado.push_back(obj3);
            }else{
                destino_revisado.push_back(*it);
            }
            objetivoi++;

    }

    //Comprobamos si el origen se trata de una casilla objetivo
    EsObjetivo(current.st,destino_revisado);

    //Calculamos su valor heurístico
    current.heuristica=Heuristico(current.st,obj1,obj2,obj3);

	Abiertos.push(current);


    while(!Abiertos.empty() and (!current.st.objetivo1 or !current.st.objetivo2 or !current.st.objetivo3)){
		Abiertos.pop();

		if (Cerrados.find(current.st) == Cerrados.end()){
			Cerrados.insert(current.st);
			if (mapaResultado[current.st.fila][current.st.columna] == 'K'){
				current.st.bikini = true;
				current.st.zapatillas = false;
			}
			if (mapaResultado[current.st.fila][current.st.columna] == 'D'){
				current.st.zapatillas = true;
				current.st.bikini = false;
			}

			// Generar descendiente de girar a la derecha 90 grados
			nodoValorA hijoTurnR = current;
			hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
			if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
			{
				giro = 0;
				currentCasVal = valorCasilla(current.st, giro);
				hijoTurnR.val += currentCasVal;
				hijoTurnR.heuristica = Heuristico(hijoTurnR.st, obj1,obj2,obj3);
				hijoTurnR.secuencia.push_back(actTURN_R);
				Abiertos.push(hijoTurnR);
			}

			// Generar descendiente de girar a la derecha 45 grados
			nodoValorA hijoSEMITurnR = current;
			hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
			if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
			{
				giro = 1;
				currentCasVal = valorCasilla(current.st, giro);
				hijoSEMITurnR.val += currentCasVal;
				hijoSEMITurnR.heuristica = Heuristico(hijoSEMITurnR.st, obj1,obj2,obj3);
				hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
				Abiertos.push(hijoSEMITurnR);
			}

			// Generar descendiente de girar a la izquierda 90 grados
			nodoValorA hijoTurnL = current;
			hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
			if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
			{
				giro = 0;
				currentCasVal = valorCasilla(current.st, giro);
				hijoTurnL.val += currentCasVal;
				hijoTurnL.heuristica = Heuristico(hijoTurnL.st, obj1,obj2,obj3);
				hijoTurnL.secuencia.push_back(actTURN_L);
				Abiertos.push(hijoTurnL);
			}

			// Generar descendiente de girar a la izquierda 45 grados
			nodoValorA hijoSEMITurnL = current;
			hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
			if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
			{
				giro = 1;
				currentCasVal = valorCasilla(current.st, giro);
				hijoSEMITurnL.val += currentCasVal;
				hijoSEMITurnL.heuristica = Heuristico(hijoSEMITurnL.st, obj1,obj2,obj3);
				hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
				Abiertos.push(hijoSEMITurnL);
			}

			// Generar descendiente de avanzar
			nodoValorA hijoForward = current;
			if (!HayObstaculoDelante(hijoForward.st))
			{
				if (Cerrados.find(hijoForward.st) == Cerrados.end())
				{
					giro = 2;
					currentCasVal = valorCasilla(current.st, giro);
					hijoForward.val += currentCasVal;
					hijoForward.heuristica = Heuristico(hijoForward.st, obj1,obj2,obj3);
					hijoForward.secuencia.push_back(actFORWARD);
					Abiertos.push(hijoForward);
				}
			}
		}

		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}

		//Comprobamos si se trata de una casilla objetivo
        EsObjetivo(current.st,destino);
	}

  cout << "Terminada la busqueda\n";

	//Comprobamos que en el plan hemos podido alcanzar los 3 objetivos
    if (current.st.objetivo1 or current.st.objetivo2 or current.st.objetivo3){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}
