#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>

using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{

	Action accion = actIDLE;

	cambioDeEstado(sensores);
	switch(sensores.nivel){
		case 0:
			estadoActual.brujula = sensores.sentido;
			estadoActual.fil = sensores.posF;
			estadoActual.col = sensores.posC;
			estadoActual.bien_situado = true;
			estadoActual.yaPosicionado = true;
			// if (estadoActual.bien_situado && (estadoActual.ultimaAccion == 0 or estadoActual.ultimaAccion == 3))
			{
				cout << "Esta pintando\n\n"
					 << endl;
				pintarMapa(sensores);
			}
			break;
		case 1:
			estadoActual.brujula = sensores.sentido;
		case 2:
		case 3:
		case 4:
			if(!estadoActual.bien_situado)
			{
				if (sensores.terreno[0] == 'G' && !estadoActual.yaPosicionado)
				{
					pasarMapaAuxiliarAMapa(sensores);
					estadoActual.fil = sensores.posF;
					estadoActual.col = sensores.posC;
					pintarMapa(sensores);
					estadoActual.bien_situado = true;
					estadoActual.yaPosicionado = true;
				}
				else
				{
					cout << "\nPinta auxiliar" << endl;
					pintarMapaAuxiliar(sensores);
				}
			}
			else
			{
				pintarMapa(sensores);
			}
			break;
	}

	if (esBikini(sensores.terreno[0]))
	{
		this->estadoActual.bikini = true;
	}

	if (esZapatilla(sensores.terreno[0]))
	{
		this->estadoActual.zapatillas = true;
	}

	if (esRecarga(sensores.terreno[0]) && sensores.bateria < 5000){
		accion = actIDLE;
	} else {
		accion = elegirAccion(sensores);
	}
	mostrarInformacion(sensores);
	if (estadoActual.ultimaAccion == actFORWARD && accion == actFORWARD)
		cuentaForward++;

	int maxCuentaForward = 0;
	if (mapaResultado.size() > 70)
	{
		maxCuentaForward = 35;
	} else {
		maxCuentaForward = 15;
	}

	if (cuentaForward > maxCuentaForward && listaAcciones.size() == 0)
	{
		if (rand() % 2 == 0)
			accion = actTURN_L;
		else
			accion = actTURN_R;

		cuentaForward = 0;
	}

	// Determinar el efecto de la ultima accion enviada
	estadoActual.ultimaAccion = accion;
	cout << "Ultima Accion: " << estadoActual.ultimaAccion << "\n\n";
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

void ComportamientoJugador::mostrarInformacion(Sensores sensores)
{
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << " ";
	cout << "Posicion: fila " << estadoActual.fil << " columna " << estadoActual.col << " ";

	switch (sensores.sentido)
	{
	case 0:
		cout << "Norte" << endl;
		break;
	case 1:
		cout << "Este" << endl;
		break;
	case 2:
		cout << "Sur " << endl;
		break;
	case 3:
		cout << "Oeste" << endl;
		break;
	}
	cout << "Terreno: ";
	for (int i = 0; i < sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << endl;

	cout << "Superficie: ";
	for (int i = 0; i < sensores.superficie.size(); i++)
		cout << sensores.superficie[i];
	cout << endl;

	cout << "Colisión: " << sensores.colision << endl;
	cout << "Reset: " << sensores.reset << endl;
	cout << "Vida: " << sensores.vida << endl;
	cout << endl;
}

Action ComportamientoJugador::elegirAccion(Sensores sensores)
{
	// Decidir la nueva accion
	Action accion = actIDLE;
	cout << listaAcciones.size() << "\n\n";
	if (listaAcciones.size() == 0){
		analizaEntorno(sensores);
	}
	
	if (listaAcciones.size() > 0){
		accion = listaAcciones.front();
		listaAcciones.pop();
		estadoActual.rotar = false;
	}
	else if (estadoActual.rotar){
		accion = actTURN_R;
		estadoActual.rotaciones++;
		if (estadoActual.rotaciones == 4)
		{
			estadoActual.rotar = false;
		}
	} else {
		estadoActual.rotar = false;

		if (esObstaculo(sensores.terreno[2]) or esAldeano(sensores.superficie[2]) or esLobo(sensores.superficie[2]))
		{
			if (esAldeano(sensores.superficie[2])){
				if (!estadoActual.girar_derecha)
					accion = actTURN_L;
				else
					accion = actTURN_R;
			}
			else if (mapaResultado.size() > 70)
			{
				if (sensores.nivel == 0){
					if (sensores.terreno[2] == 'P')
					{
						if ((1+rand()) % 3 == 1)
							accion = actFORWARD;
						else {
							if (rand() % 2 == 0)
								accion = actTURN_L;
							else
								accion = actTURN_R;
						}
					} else {
						if (rand() % 2 == 0)
							accion = actTURN_L;
						else
							accion = actTURN_R;
					}
				}
				else if (esLobo(sensores.superficie[2]))
				{
					if (numReinicios > 3)
					{
						if (!estadoActual.girar_derecha)
							accion = actTURN_L;
						else
							accion = actTURN_R;
					}
					else
					{
						accion = actFORWARD;
					}
				}
				else
				{
					if (estadoActual.bien_situado){
						if (!estadoActual.girar_derecha)
							accion = actTURN_L;
						else
							accion = actTURN_R;
					} else {
						if (sensores.terreno[2] == 'P')
						{
							if ((1+rand() % 8) == 1)
								accion = actFORWARD;
							else {
								if (!estadoActual.girar_derecha)
									accion = actTURN_L;
								else 
									accion = actTURN_R;
							}
						}
					}
				}
			}
			else
			{
				if (!estadoActual.girar_derecha)
					accion = actTURN_L;
				else
					accion = actTURN_R;
			}
		}
		else
		{
			if ((estadoActual.bikini && sensores.terreno[2] == 'A') || (estadoActual.zapatillas && sensores.terreno[2] == 'B')
				|| puedoAvanzar(sensores.terreno[2], sensores.superficie[2]))
				accion = actFORWARD;
			
			else if (sensores.terreno[2] == 'A' || sensores.terreno[2] == 'B'){
				int contBosque = 0, contAgua = 0;
				for (int i = 0; i < sensores.terreno.size(); i++)
				{
					if (sensores.terreno[i] == 'B')
						contBosque++;
					if (sensores.terreno[i] == 'A')
						contAgua++;
				}

				if ((contBosque > 7 || contAgua > 7) && sensores.bateria > 3500){
					accion = actFORWARD;
				} else {
					if (rand() % 2 == 0)
						accion = actTURN_L;
					else
						accion = actTURN_R;
				}
			} else {
				if (rand() % 2 == 0)
					accion = actTURN_L;
				else
					accion = actTURN_R;
			}
			
		}
	}

	return accion;
}

void ComportamientoJugador::cambioDeEstado(Sensores sensores)
{
	if (sensores.reset)
		reiniciar(sensores);
	else {
		switch (estadoActual.ultimaAccion)
		{
		case actFORWARD:
			if (!sensores.colision){
				switch (estadoActual.brujula)
				{
				case 0:
					estadoActual.fil--; //Norte
					break;
				case 1:
					estadoActual.col++; //Este
					break;
				case 2:
					estadoActual.fil++; //Sur
					break;
				case 3:
					estadoActual.col--; //Oeste
					break;
				}
			}
			break;
		case actTURN_L:
			estadoActual.brujula = (estadoActual.brujula + 3) % 4;
			estadoActual.girar_derecha = (rand() % 2 == 0); //lo mas seguro es que haya que quitarlo
			break;
		case actTURN_R:
			estadoActual.brujula = (estadoActual.brujula + 1) % 4;
			estadoActual.girar_derecha = (rand() % 2 == 0); //lo mas seguro es que haya que quiitarlo
			break;
		}
	}
}

bool ComportamientoJugador::esObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')	//Añadir superficie sin agente
		return true;
	else
		return false;
}

bool ComportamientoJugador::esAldeano(unsigned char superficie){
	if (superficie == 'a')
		return true;
	else
		return false;
}

bool ComportamientoJugador::esLobo(unsigned char superficie){
	if (superficie == 'l')
		return true;
	else
		return false;
}

bool ComportamientoJugador::puedoAvanzar(unsigned char terreno, unsigned char superficie)
{
	bool puedoAvanzar = false;

	if ((terreno == 'T' || terreno == 'K' ||
		 terreno == 'D' or terreno == 'X' or
		 terreno == 'S' or terreno == 'G') &&
		(superficie == '_'))
		puedoAvanzar = true;

	return puedoAvanzar;
}

bool ComportamientoJugador::esBikini(unsigned char casilla)
{
	if (casilla == 'K')
	{
		this->estadoActual.bikini = true;
		return true;
	}
	else
		return false;
}

bool ComportamientoJugador::esZapatilla(unsigned char casilla)
{
	if (casilla == 'D')
	{
		this->estadoActual.zapatillas = true;
		return true;
	}
	else
		return false;
}

bool ComportamientoJugador::esRecarga(unsigned char casilla)
{
	if (casilla == 'X')
	{
		this->estadoActual.recarga = true;
		return true;
	}
	else
		return false;
}


void ComportamientoJugador::analizaEntorno(Sensores sensores){
	vector<int> posiciones; posiciones.clear();
	for (int i = 1; i < sensores.terreno.size(); i++){
		if (meInteresa(sensores.terreno[i])){
			posiciones.push_back(i);
			cout << "\n\nLe interesa";
		}
	}
	bool prioridadZapatillas = false, prioridadBikini = false;
	
	if (!estadoActual.zapatillas)
		prioridadZapatillas = true;
	if (!estadoActual.bikini)
		prioridadBikini = true;



	int posicion = -1;
	bool veoBikiniOZapatillas = false;
	bool veoPosicionamiento = false;
	for (int i = posiciones.size()-1; i >= 0; i--){
		//Encuentra casilla de posicionamiento
		if (sensores.terreno[posiciones[i]] == 'G' && sensores.bateria > 1000){
			posicion = posiciones[i];
			veoPosicionamiento = true;
		}
		else if(!veoPosicionamiento && sensores.bateria > 1000){
			//Encuentra bikini o zapatillas
			if (sensores.terreno[posiciones[i]] == 'K' or sensores.terreno[posiciones[i]] == 'D'){
				posicion = posiciones[i];
				veoBikiniOZapatillas = true;
				cout << "\n\nVeo zpatilla o Bikini";
			} else if (!veoBikiniOZapatillas) {
				//Encuentra recarga
				posicion = posiciones[i];
			}
		}
	}

	switch (posicion)
	{
	case 1:
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actTURN_L);
		break;
	case 3:
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actTURN_R);
		break;
	case 4:
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actTURN_L);
		break;
	case 8:
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actTURN_R);
		break;
	case 9:
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actTURN_L);
		break;
	case 15:
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actFORWARD);
		listaAcciones.push(actTURN_R);
		break;
	case -1:
		break;
	default:
		if (!esObstaculo(sensores.terreno[2]))
			listaAcciones.push(actFORWARD);
		break;
	}

}


bool ComportamientoJugador::meInteresa(unsigned char casilla){
	
	if (casilla == 'G' or casilla == 'K' or casilla == 'D' or casilla == 'X')
		return true;
	return false;

}


void ComportamientoJugador::pasarMapaAuxiliarAMapa(Sensores sensores){
	filAuxiliar = estadoActual.fil - sensores.posF;
	colAuxiliar = estadoActual.col - sensores.posC;
	int x = 0, y = 0;
	
	for (int i = filAuxiliar; i < (filAuxiliar + mapaResultado.size()); i++){
		for (int j = colAuxiliar; j < (colAuxiliar + mapaResultado[x].size()); j++){
			cout << x << " " << y << "\n\n";
			mapaResultado[x][y] = mapaAuxiliar[i][j];
			y++;
		}
		y = 0;
		x++;
	}
}

void ComportamientoJugador::reiniciar(Sensores sensores){
	estadoActual.bikini = false;
	estadoActual.zapatillas = false;
	estadoActual.rotar = true;
	estadoActual.rotaciones = 0;
	estadoActual.brujula = 0;
	mapaAuxiliar.clear();
	vector<unsigned char> aux(199, '?');
	for (int i = 0; i < 199; i++)
	{
		mapaAuxiliar.push_back(aux);
	}
	estadoActual.fil = estadoActual.col = filAuxiliar = colAuxiliar = 99;
	numReinicios++;
	estadoActual.bien_situado = false;
	while (listaAcciones.size() > 0)
	{
		listaAcciones.pop();
	}
	cout << "\nReinicia" << endl;
}


void ComportamientoJugador::pintarMapa(Sensores sensores)
{
	int c = estadoActual.col, f = estadoActual.fil;

	mapaResultado[f][c] = sensores.terreno[0];

	switch (estadoActual.brujula)
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

	case 1: // este
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

	case 2: // sur
		cout << f << " " << c << endl;
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

	case 3: // oeste
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
	}
}

void ComportamientoJugador::pintarMapaAuxiliar(Sensores sensores)
{
	int c = estadoActual.col, f = estadoActual.fil;

	mapaAuxiliar[f][c] = sensores.terreno[0];

	switch (estadoActual.brujula)
	{
	case 0: // norte
		mapaAuxiliar[f - 1][c - 1] = sensores.terreno[1];
		mapaAuxiliar[f - 1][c] = sensores.terreno[2];
		mapaAuxiliar[f - 1][c + 1] = sensores.terreno[3];
		mapaAuxiliar[f - 2][c - 2] = sensores.terreno[4];
		mapaAuxiliar[f - 2][c - 1] = sensores.terreno[5];
		mapaAuxiliar[f - 2][c] = sensores.terreno[6];
		mapaAuxiliar[f - 2][c + 1] = sensores.terreno[7];
		mapaAuxiliar[f - 2][c + 2] = sensores.terreno[8];
		mapaAuxiliar[f - 3][c - 3] = sensores.terreno[9];
		mapaAuxiliar[f - 3][c - 2] = sensores.terreno[10];
		mapaAuxiliar[f - 3][c - 1] = sensores.terreno[11];
		mapaAuxiliar[f - 3][c] = sensores.terreno[12];
		mapaAuxiliar[f - 3][c + 1] = sensores.terreno[13];
		mapaAuxiliar[f - 3][c + 2] = sensores.terreno[14];
		mapaAuxiliar[f - 3][c + 3] = sensores.terreno[15];
		break;

	case 1: // este
		mapaAuxiliar[f - 1][c + 1] = sensores.terreno[1];
		mapaAuxiliar[f][c + 1] = sensores.terreno[2];
		mapaAuxiliar[f + 1][c + 1] = sensores.terreno[3];
		mapaAuxiliar[f - 2][c + 2] = sensores.terreno[4];
		mapaAuxiliar[f - 1][c + 2] = sensores.terreno[5];
		mapaAuxiliar[f][c + 2] = sensores.terreno[6];
		mapaAuxiliar[f + 1][c + 2] = sensores.terreno[7];
		mapaAuxiliar[f + 2][c + 2] = sensores.terreno[8];
		mapaAuxiliar[f - 3][c + 3] = sensores.terreno[9];
		mapaAuxiliar[f - 2][c + 3] = sensores.terreno[10];
		mapaAuxiliar[f - 1][c + 3] = sensores.terreno[11];
		mapaAuxiliar[f][c + 3] = sensores.terreno[12];
		mapaAuxiliar[f + 1][c + 3] = sensores.terreno[13];
		mapaAuxiliar[f + 2][c + 3] = sensores.terreno[14];
		mapaAuxiliar[f + 3][c + 3] = sensores.terreno[15];
		break;

	case 2: // sur
		cout << f << " " << c << endl;
		mapaAuxiliar[f + 1][c + 1] = sensores.terreno[1];
		mapaAuxiliar[f + 1][c] = sensores.terreno[2];
		mapaAuxiliar[f + 1][c - 1] = sensores.terreno[3];
		mapaAuxiliar[f + 2][c + 2] = sensores.terreno[4];
		mapaAuxiliar[f + 2][c + 1] = sensores.terreno[5];
		mapaAuxiliar[f + 2][c] = sensores.terreno[6];
		mapaAuxiliar[f + 2][c - 1] = sensores.terreno[7];
		mapaAuxiliar[f + 2][c - 2] = sensores.terreno[8];
		mapaAuxiliar[f + 3][c + 3] = sensores.terreno[9];
		mapaAuxiliar[f + 3][c + 2] = sensores.terreno[10];
		mapaAuxiliar[f + 3][c + 1] = sensores.terreno[11];
		mapaAuxiliar[f + 3][c] = sensores.terreno[12];
		mapaAuxiliar[f + 3][c - 1] = sensores.terreno[13];
		mapaAuxiliar[f + 3][c - 2] = sensores.terreno[14];
		mapaAuxiliar[f + 3][c - 3] = sensores.terreno[15];
		break;

	case 3: // oeste
		cout << "oeste" << endl;
		mapaAuxiliar[f + 1][c - 1] = sensores.terreno[1];
		mapaAuxiliar[f][c - 1] = sensores.terreno[2];
		mapaAuxiliar[f - 1][c - 1] = sensores.terreno[3];
		mapaAuxiliar[f + 2][c - 2] = sensores.terreno[4];
		mapaAuxiliar[f + 1][c - 2] = sensores.terreno[5];
		mapaAuxiliar[f][c - 2] = sensores.terreno[6];
		mapaAuxiliar[f - 1][c - 2] = sensores.terreno[7];
		mapaAuxiliar[f - 2][c - 2] = sensores.terreno[8];
		mapaAuxiliar[f + 3][c - 3] = sensores.terreno[9];
		mapaAuxiliar[f + 2][c - 3] = sensores.terreno[10];
		mapaAuxiliar[f + 1][c - 3] = sensores.terreno[11];
		mapaAuxiliar[f][c - 3] = sensores.terreno[12];
		mapaAuxiliar[f - 1][c - 3] = sensores.terreno[13];
		mapaAuxiliar[f - 2][c - 3] = sensores.terreno[14];
		mapaAuxiliar[f - 3][c - 3] = sensores.terreno[15];
		break;
	}
	cout << "\nSale pintar auxiliar" << endl;
}
