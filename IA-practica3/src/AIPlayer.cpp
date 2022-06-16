# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 4;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;
    
    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    
    // switch (id) {
    //     case 0:
    //         thinkAleatorio(c_piece,id_piece,dice);
    //         break;

    //     case 1:
    //         thinkAleatorioMasInteligente(c_piece,id_piece,dice);
    //         break;

    //     case 2:
    //         thinkFichaMasAdelantada(c_piece,id_piece,dice);
    //         break;

    //     case 3:
    //         thinkMejorOpcion(c_piece,id_piece,dice);
    //         break;
    // }
    
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    // valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiHeuristica);
    // cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiHeuristica);
            break;
        // case 1:
        //     valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
        //     break;
        // case 2:
        //     valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
        //     break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    
}

void AIPlayer::thinkAleatorio(color & c_piece, int & id_piece, int & dice) const{
    

    // El color de ficha que se va a mover
    c_piece = actual->getCurrentColor();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<int> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableDices(c_piece);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(c_piece, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if(current_pieces.size() > 0){
        id_piece = current_pieces[rand() % current_pieces.size()];
    }
    else{
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color & c_piece, int & id_piece, int & dice) const {
    // El color de ficha que se va a mover
    c_piece = actual->getCurrentColor();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<int> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableDices(c_piece);
    
    //En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;

    for (int i = 0; i < current_dices.size(); i++) {
        //Se obtiene el vector de fichas que se pueden mover para el dado elegido
        current_pieces = actual->getAvailablePieces(c_piece, current_dices[i]);

        //Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas
        if (current_pieces.size() > 0)
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
    }

    //Si no tengo ningun dado que pueda mover fichas, paso turnno con un dado al azar (la macro SKIP_TURN me permite nno mover)
    if (current_dices_que_pueden_mover_ficha.size() == 0) {
        dice = current_dices[rand() % current_dices.size()];
        id_piece = SKIP_TURN;

        //En caso contrario elijo un dado de forma aleatoria entre los que pueden mover ficha
    } else {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];

        //Se obtiene el vector de fichas que se pueden mover para el dado elegido
        current_pieces = actual->getAvailablePieces(c_piece, dice);
        //Muevo una ficha al azar entre las que se pueden mover
        id_piece = current_pieces[rand() % current_pieces.size()];
    }
}

void AIPlayer::thinkFichaMasAdelantada(color & c_piece, int & id_piece, int & dice) const {
    //Elijo el dado haciendo lo mismo que el jugador anterior
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    //Tras llamar a esta funcion, ya tengo en dice el numero de dado que quiero usar.
    //Ahora en vez de mover una filla al azar, voy a mover la que este mas adelantada
    //(equivalentemente, la mas cercana a la meta)

    vector<int> current_pieces = actual->getAvailablePieces(c_piece, dice);

    int id_ficha_mas_adelantada = -1;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); i++) {
        //distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        int distancia_meta = actual -> distanceToGoal(c_piece, current_pieces[i]);
        if (distancia_meta < min_distancia_meta) {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = current_pieces[i];
        }
    }

    //Si no he encontrado ninguna ficha, paso turno
    if (id_ficha_mas_adelantada == -1)
        id_piece = SKIP_TURN;
    else
    //Muevo la ficha mas adelantada
        id_piece = id_ficha_mas_adelantada;
}

void AIPlayer::thinkMejorOpcion(color & c_piece, int & id_piece, int & dice) const{
    //Vamos a mirar a todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual

    //gennerateNextMove va iterando sobre cada hijo. L e paso la accion del ultimo movimiento sobre
    //el que he iterado y me devolvera el siguiente. Inicialmente, cuando aun no he hecho ningun movimiento, lo inicializo asi
    color last_c_piece = none; //El color de la ultima ficha que se movio
    int last_id_piece = -1;   //El id de la ultima ficha que se movio
    int last_dice = -1;       //El dado que se uso en el ultimo movimiento

    //Cuando ya he recorrido todos los hijos, la funcion devuelve el estado actual. De esta forma puedo saber
    //cuando paro de iterar

    Parchis siguiente_hijo = actual->generateNextMove(last_c_piece,last_id_piece,last_dice);
    bool me_quedo_con_esta_accion = false;

    while(!(siguiente_hijo == *actual) && !me_quedo_con_esta_accion) {
        if (siguiente_hijo.isEatingMove() or //Si como ficha
            siguiente_hijo.isGoalMove() or   //Si con este movimiento llego a la meta, o
            (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador)) { //Si con este movimiento gano la partida
            me_quedo_con_esta_accion = true;
        } else {
            //genero el siguiente hijo
            siguiente_hijo = actual->generateNextMove(last_c_piece, last_id_piece, last_dice);
        }
    }

    //Si he encontrado una accion que me interesa, la guardo en las variables pasadas por referencia
    if (me_quedo_con_esta_accion) {
        c_piece = last_c_piece;
        id_piece = last_id_piece;
        dice = last_dice;
    } else {
        //muevo la ficha mas adelantada como antes
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
    }
}


double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

//Implementacion de la poda alfa beta
double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const{
    //Si el juego ha terminado o hemos llegado hasta la profundidad maxima
    if (actual.gameOver() || profundidad == profundidad_max) {
        //Devolvemos la valoracion del estado
        return heuristic(actual, jugador);
    } else {
       int dice_aux = -1;
       int id_piece_aux = -1;
       color c_piece_aux = none;

    //Calculamos el siguiente movimiento de manera descendiente, es decir, empezando por el 6,5,4,...
       Parchis nextMove = actual.generateNextMoveDescending(c_piece_aux,id_piece_aux,dice_aux);
       
       //Obtenemos los valores de alpha y beta cuando corresponda
       while(!(nextMove == actual)) {

           if (this->jugador != actual.getCurrentPlayerId()) {
               double valor_actual = Poda_AlfaBeta(nextMove,jugador, profundidad +1 , profundidad_max, c_piece_aux, id_piece_aux, dice_aux, alpha, beta, heuristic);
               
               if (valor_actual < beta) 
                   beta = valor_actual;
               if (alpha >= beta)
                   break;                
           } else {
               double valor_actual = Poda_AlfaBeta(nextMove,jugador, profundidad +1 , profundidad_max, c_piece_aux, id_piece_aux, dice_aux, alpha, beta, heuristic);
               
               double aux = alpha;
               if (valor_actual > alpha) 
                   alpha = valor_actual;

               if (profundidad == 0 && alpha != aux){
                   c_piece = c_piece_aux;
                   dice = dice_aux;
                   id_piece = id_piece_aux;
               }
               
               if (alpha >= beta)
                   break;  
           }

           nextMove = actual.generateNextMoveDescending(c_piece_aux,id_piece_aux,dice_aux);
       }

    //Devolvemos alpha o beta
       if (jugador == actual.getCurrentPlayerId())
           return alpha;
       else
           return beta;
    }
}


//-------------------- HEURISTICAS --------------------
double AIPlayer::MiHeuristica(const Parchis &estado, int jugador) {
    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    if (jugador == ganador)
        return gana;
    else if (oponente == ganador)
        return pierde;
    else {
        double puntuacion_jugador = 0, puntuacion_oponente = 0;

        puntuacion_jugador = puntuacion(estado, jugador);
        puntuacion_oponente = puntuacion(estado, oponente);        
        return puntuacion_jugador - puntuacion_oponente;
    }
}


double AIPlayer::puntuacion(const Parchis &estado, int jugador) {
    double puntuacion = 0;
    vector<color> colores = estado.getPlayerColors(jugador);

    // Recorro mis colores y las fichas de cada color
    for (int i = 0; i < colores.size(); i++)
    {
        color c = colores[i];
        // Recorro las fichas de cada color
        int fichas_en_meta = estado.piecesAtGoal(c);
        int bonus = 0;
        if (fichas_en_meta == 3) {
            bonus += 9*30;
        } else if (fichas_en_meta == 2) {
            bonus += 9*20;
        } else if (fichas_en_meta == 1) {
            bonus += 9*10;
        }

        if (estado.piecesAtHome(c) > 0){
            bonus -= 999;
        } else if (estado.piecesAtHome(c) > 1) {
            bonus -= 999;
        }

        puntuacion += bonus;
        for (int j = 0; j < num_pieces; j++)
        {
            int distancia_a_meta = estado.distanceToGoal(c, j);
            puntuacion -= distancia_a_meta * 2;
            if (estado.isSafePiece(c,j)) {
                puntuacion += 300;
            } else 
                puntuacion++;

            if (estado.isEatingMove()) {
                puntuacion += 350;
                if (estado.isGoalMove()) {
                    puntuacion += 200;
                }
            } else 
                puntuacion++;

            if (estado.isGoalMove()) {
                puntuacion += 500;
            } else 
                puntuacion++;

            if (estado.gameOver() and estado.getWinner() == jugador) {
                puntuacion += gana;
            }
            
            
            
        }
    }

    return puntuacion;
}