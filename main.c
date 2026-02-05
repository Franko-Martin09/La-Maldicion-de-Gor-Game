#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Para usar strings
#include <conio.h> //Para usar getch()
#include <time.h> //Usado para el numero aleatorio y para contar el tiempo de ejecución para el puntaje
#include <locale.h> //Añade paquete de idiomas
#include <windows.h> //Para usar las funciones de cambio de color, limpieza de pantalla y animacion de tetxto usamos una libreria de Windows, por lo que el juego no será compatible con otros sistemas operativos.
#include <mmsystem.h>  // Necesario para PlaySound

struct jugador { //Datos del jugador
	char nombre [20];
	unsigned short puntaje;
	short pv_max;
	short pv;
	short pm_max;
	short pm;
	short fuerza;
	//Flags para 0/1 solo para indicar si el jugador tiene los items
	char ojo;
	char esencia;
	char corazon;
};
struct contrincante { //Datos del enemigo
	char nombre [20];
	short pv_max;
	short pv;
	short fuerza;
	// flag para cargar el archivo de texto
	short codigo_imagen;
	short color;
	//Flags para debilidades elementales
	float fisico;
	float fuego;
	float hielo;
};

//Declaracion de funciones
short banner(HANDLE, struct contrincante *, struct jugador *); //Declaracion de la funcion banner
void intro(); //Funcion de la introduccion
void nombramiento(struct jugador *); //Igreso del nombre del jugador
short mapa(struct jugador *, struct contrincante *, HANDLE); //Pantalla de eleccion de destino
short combate(struct jugador *, struct contrincante *, HANDLE);
void inicializarOponenteEsqueleto(struct contrincante *);
void inicializarOponenteArbol(struct contrincante *);
void inicializarOponenteGolem(struct contrincante *);
void inicializarOponenteGOR(struct contrincante *, struct jugador *);
void printAnimado(char*, short); //Funcion para animar el texto
void ajustarTamanoConsola(HANDLE, short, short); //Funcion para cambiar el tamaño de la consola
void Carga_imagen (struct contrincante *);
void mezcla(float *a, float *b, float *c); //Mezcla de tres valores con el algoritmo Fisher-Yates
void guardarPuntos(struct jugador *, clock_t, clock_t);
void cargarPuntos();
void creditos();

int main () {
	//Contador de tiempo para calcular puntuaciones
	clock_t inicio = clock ();
	//Cambia al idioma español para usar los caracteres de nuestro idioma
	setlocale(LC_ALL, "spanish");
	// Inicializar semilla aleatoria
	srand(time(NULL));

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //Le pide al sistema operativo el "control" (handle) de la consola de salida, y lo guarda en hConsole
	ajustarTamanoConsola(hConsole, 121, 47); // HANDLE, columnas, filas

	//declaracion de punteros a struct:
	struct jugador *P_heroe;
	struct contrincante *P_enemigo;
	//asignacion de memoria dinamica:
	P_heroe = malloc (sizeof(struct jugador));
	P_enemigo = malloc (sizeof(struct contrincante));
	//verificación de asignacion de memoria:
	if (P_heroe == NULL || P_enemigo == NULL) {
		printf("                                               ***************************\n");
		printf("                                               *Error al asignar memoria.*\n");
		printf("                                               ***************************\n");
		return 1;
	}
	//1.inicio
	char gameOver = 0;
	//Titulo y menu principal
	short salir = banner(hConsole, P_enemigo, P_heroe);
	if (salir == 1) {
		return 0;
	}
	//Sale la introduccion
	intro();
	//Ingreso del nombre del jugador e inicializacion de sus datos
	nombramiento(P_heroe);
	//inicio de menu interactivo: eleccion de destino ->
	do {  //En caso de haber perdido una pelea se puede reintentar o terminar
		
		short resultado = mapa(P_heroe, P_enemigo, hConsole); //Lleva a la pantalla de seleccion de batalla y devuelve el resultado

		char opcion = 0;
		switch (resultado) {
			case 1: //Opción para volver a intentarlo si se pierde una batalla
				PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(300);
				printf("========================================================================================================================\n");
				printf("¿Quieres volver a intentarlo?\n");
				printf("========================================================================================================================\n");
				printf("1. Si.\n2. No.\n");
				printf("========================================================================================================================\n");
				do {
					opcion = getch();
				} while (opcion != '1' && opcion != '2');
				switch (opcion) {
					case '1':
						PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
						Sleep(300);
						printf("========================================================================================================================\n");
						printAnimado("¡Eres nuestra esperanza!.\n", 10);
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						break;
					case '2':
						printf("========================================================================================================================\n");
						printAnimado("Has perdido...\n", 10);
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						PlaySound("sound/gameover.wav", NULL, SND_FILENAME | SND_ASYNC);
						Sleep(1000);
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						gameOver = '1';
						break;
				}
				break;
			case 2: //Opción para volver a jugar si gana
				PlaySound("sound/winner.wav", NULL, SND_FILENAME | SND_ASYNC);
				system("color E0");
				printf("========================================================================================================================\n");
				printAnimado("El cuerpo del señor oscuro Gor yace derrotado entre los restos humeantes de su trono profanado. La oscuridad que\nmanchaba el cielo comienza a disiparse, y un rayo de luz celestial rompe por primera vez en años las nubes que cubrían\nel reino de Lorian. La princesa Augusta, liberada de su prisión de sombras, te observa con gratitud y lágrimas en los\nojos, consciente de que su reino ha sido salvado por la voluntad indomable de un solo héroe:\n", 0);
				printf("tú, %s.\n", P_heroe->nombre);
				printAnimado("Las tierras corruptas comienzan a sanar, el antiguo bosque florece de nuevo, y la llama sagrada del Monte Ramún se apaga\nen paz. Los ecos de la cripta susurran tu nombre, y los pueblos libres de Lorian entonan cantos en tu honor.\nEl sacrificio de los altos sacerdotes no fue en vano: tú llevaste su esperanza más allá del umbral de la muerte misma.\nPero el poder deja cicatrices… y mientras el castillo oscuro se desmorona tras de ti, una última mirada revela que el\nmal nunca desaparece del todo… sólo duerme.\n¡Gloria eterna al héroe de Lorian! ¡Que tu gesta viva por siempre en los cantares del mundo libre!\n", 0);
				printf("========================================================================================================================\n");
				printf("\n\n\n");
				system("pause");//Espera a que el usuario presione una tecla
				system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
			
				clock_t fin = clock();
				guardarPuntos(P_heroe, inicio, fin);
				PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(300);
				printf("========================================================================================================================\n");
				printAnimado("HAS GANADO!\n", 15);
				printf("========================================================================================================================\n");
				printf("\nTu puntuación es: %d\n\n", P_heroe->puntaje);
				printf("========================================================================================================================\n");
				printf("¿Quieres volver a jugar? ¡Intenta superar las máximas puntuaciones!\n");
				printf("========================================================================================================================\n");
				printf("1. Si.\n2. No.\n");
				printf("========================================================================================================================\n");
				do {
					opcion = getch();
					PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(300);
				} while (opcion != '1' && opcion != '2');
				switch (opcion) {
					case '1':
						printAnimado("¡Buena suerte!.\n", 10);
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						break;
					case '2':
						printAnimado("¡Gracias por jugar!\n", 10);
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						gameOver = '1';
						break;
				}
				system("color 07");
				break;
		}
	} while (gameOver == 0);

	//liberacion de memoria antes de finalizar:
	free(P_heroe);
	free(P_enemigo);

	return 0;
}
short banner(HANDLE hConsole, struct contrincante *P_enemigo, struct jugador *P_heroe) {

	char menu = 0;
	while (1) {
		system("cls");
		P_enemigo->codigo_imagen = 0;//La mejor forma que encontré para asignarle una direccion de donde tomar el texto del banner
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); //Cambia el color del texto a verde
		Carga_imagen(P_enemigo);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Reset color a blanco
		printf("\n\n\n\n\n\n\n\n");
		PlaySound("sound/risa.wav", NULL, SND_FILENAME | SND_ASYNC);
		Sleep(600);
		printf("========================================================================================================================\n");
		printf("|                                             ===========================                                              |\n");
		printf("|                                             | 1. Iniciar partida      |                                              |\n");
		printf("|                                             | 2. Puntuaciones         |                                              |\n");
		printf("|                                             | 3. Créditos             |                                              |\n");
		printf("|                                             | 4. Salir                |                                              |\n");
		printf("|                                             ===========================                                              |\n");
		printf("========================================================================================================================\n");
		printf("                                                                                                                  V1.0.3\n");
		do {
			menu = getch();
		} while (menu != '1' && menu != '2' && menu != '3' && menu != '4');
		switch (menu) {
			case '1':
				return 0;
				break;
			case '2':
				cargarPuntos(P_heroe);
				break;
			case '3':
				creditos();
				break;
			case '4':
				return 1;
		}
	}
}
void intro() {

	PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
	Sleep(400);
	system("cls");
	printf("========================================================================================================================\n");
	printAnimado("EL REINO\n", 15);
	printf("========================================================================================================================\n\n");
	printf("El reino de Lorian ha caído en la ruina, el señor oscuro Gor ha matado al rey y secuestrado a la princesa Augusta,\ndepende de ti, el héroe más grande del reino partir a su rescate y terminar con las oscuras ambiciones de Gor. Imbuido\npor los poderes divinos otorgados por los altos sacerdotes, obtendrás el poder de regresar al punto de partida en el\ncastillo si caes en tu gesta, por lo que la victoria depende únicamente de lo férrea que sea tu voluntad.\n¡Partid oh gran héroe! ¡Devuelve la paz a nuestra tierra!\n\n");
	printf("========================================================================================================================\n");
	printf("\n\n\n");
	system("pause");//Espera a que el usuario presione una tecla
}
void nombramiento(struct jugador *P_heroe) { //Ingreso del nombre del jugador

	PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC); //inclusion de AUDIO:
	Sleep(400);
	system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
	short longitud = 0;

	printf("========================================================================================================================\n");
	printAnimado("EL HEROE\n", 15);

	do {
		printf("========================================================================================================================\n\n");
		printAnimado("Ingresa el nombre del Héroe (20 caracteres como máximo):\n\n", 15);
		printf("========================================================================================================================\n\n");
		fflush(stdin);
		fgets(P_heroe->nombre, sizeof(P_heroe->nombre), stdin);
		longitud = strlen (P_heroe->nombre);
		if (P_heroe->nombre[longitud - 1] == '\n') { //Borra el \n del final del nombre
			P_heroe->nombre[longitud - 1] = '\0';
		}
		if (longitud <= 1) {
			PlaySound("sound/denegado.wav", NULL, SND_FILENAME | SND_ASYNC); //inclusion de AUDIO:
			Sleep(400);
			printAnimado("El Heroe debe tener un nombre...\n\n", 25);
		}
	} while (longitud <= 1);

	//inicializo valores de jugador:

}
short mapa(struct jugador *P_heroe, struct contrincante *P_enemigo, HANDLE hConsole) {

	char destino = 0; //Opciones para ubicaciones
//inicializo valores de jugador:
	P_heroe->pv_max = 150;
	P_heroe->pv = P_heroe->pv_max;
	P_heroe->pm_max = 150;
	P_heroe->pm = P_heroe->pm_max;
	P_heroe->fuerza = 10;
	P_heroe->ojo = 0;
	P_heroe->esencia = 0;
	P_heroe->corazon = 0;
	P_heroe->puntaje = 0;
	while (1) { //Iteracion para volver a la seleccion de destino cada vez que se termine una batalla (excepto la ultima, esa termina el juego)

		short salud_heroe = P_heroe->pv;
		short magia_heroe = P_heroe->pm;

		PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC); //inclusion de AUDIO:
		Sleep(400);
		system("cls");
		printf("========================================================================================================================\n");
		printAnimado("MAPA:\n", 15);
		printf("========================================================================================================================\n");
		printf("\n");
		printf("Puedes enfrentarte directamente a Gor, pero tal vez encuentres algo que te ayude en tu gesta si visitas estas\nlocalizaciones malditas... Y logras derrotar a sus guardianes.\n");
		printf("\n");
		printf("========================================================================================================================\n");
		printf("|                                                  Elige el destino.                                                   |\n");
		printf("========================================================================================================================\n");
		printf("|                                             ===========================                                              |\n");
		printf("|                                             | 1. Catacumbas prohibidas|                                              |\n");
		printf("|                                             | 2. Bosque oscuro        |                                              |\n");
		printf("|                                             | 3. Monte Ramún          |                                              |\n");
		printf("|                                             | 4. Fortaleza de Gor     |                                              |\n");
		printf("|                                             ===========================                                              |\n");
		printf("========================================================================================================================\n");
		printf("|%s|\n", P_heroe->nombre);
		printf("========================================================================================================================\n");
		printf("|%3hd/%3hd PV                                                                                                            |\n", salud_heroe, P_heroe->pv_max);
		printf("|%3hd/%3hd PM                                                                                                            |\n", magia_heroe, P_heroe->pm_max);
		printf("========================================================================================================================\n");
		
		do {
			destino = getch();
		} while (destino != '1' && destino != '2' && destino != '3' && destino != '4');

		switch (destino) {
			case '1': //Cripta
				if (P_heroe->ojo == 0) { //Flag para no entrar nuevamente al combate

					PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(400);
					system("cls");

					inicializarOponenteEsqueleto(P_enemigo);
					printf("========================================================================================================================\n");
					switch (P_enemigo->color) { //cambia el color de las imagenes de los enemigos
						case 0:
							SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Cambia el color a blanco
							break;
						case 1:
							SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); //Cambia el color del texto a verde
							break;
						case 2:
							SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); //Cambia el color del texto a rojo
							break;
					}
					Carga_imagen(P_enemigo);
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Reset color a blanco
					printf("\n========================================================================================================================\n");
					printf("Al ingresar en la antigua cripta, eres recibido por el esqueleto resucitado del antiguo fundador del reino, Edmund el\nunificador. La magia corrupta de Gor le ha devuelto la vida, pero es sólo una sombra de su antiguo ser...\n");
					PlaySound("sound/perdiste.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(300);

					short resultado = combate(P_heroe, P_enemigo, hConsole);

					if (resultado == 1) {
						return 1;
					} else {
						printf("Los restos del antiguo soberano yacen en paz nuevamente, dejando tras de sí el ojo del rey, un\nantiguo artefacto que según cuentan las leyendas, permite ver las debilidades de Gor!\n");
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
						Sleep(400);
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						P_heroe->ojo = 1; //Entrega el item y además sirve como flag para que no se pueda volver al combate si ya se ha completado
					}
				} else {
					PlaySound("sound/error.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(200);
					system("cls");
					printf("========================================================================================================================\n");
					printAnimado("Las catacumbas prohibidas ya han sido visitadas.\n", 15);
					printf("========================================================================================================================\n");
					printf("\n\n\n");
					system("pause");//Espera a que el usuario presione una tecla
					system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
				}
				break;
			case '2': //Bosque
				if (P_heroe->esencia == 0) {
					PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(400);
					system("cls");

					inicializarOponenteArbol(P_enemigo);
					printf("========================================================================================================================\n");
					switch (P_enemigo->color) { //cambia el color de las imagenes de los enemigos
						case 0:
							SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Cambia el color a blanco
							break;
						case 1:
							SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); //Cambia el color del texto a verde
							break;
						case 2:
							SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); //Cambia el color del texto a rojo
							break;
					}
					Carga_imagen(P_enemigo);
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Reset color a blanco
					printf("\n========================================================================================================================\n");
					printf("El antiguo bosque sagrado fue renombrado como bosque oscuro tras el alzamiento del castillo de Gor en sus inmediaciones.\nLa corrupción del señor demoníaco envolvió el otrora bosque rebosante de vida en un aura maligna que deformó tanto flora\ncomo fauna, volviendo cada rincón del ruinoso entorno en una trampa mortal. En el centro, reposa el antiguo árbol, que\naún bajo la influencia maligna, contiene en su centro la esencia sagrada que repele el mal, pero no la entregará sin\nluchar...\n");

					PlaySound("sound/arbol.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(300);

					short resultado = combate(P_heroe, P_enemigo, hConsole);

					if (resultado == 1) {
						return 1;
					} else {
						printf("Del hueco que se ha formado en el árbol ruin, logras extraer la esencia sagrada, lo cual amplía tu poder para exterminar\nal maligno señor oscuro.\n");
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
						Sleep(400);
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						P_heroe->esencia = 1;
					}
				} else {
					PlaySound("sound/error.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(200);
					system("cls");
					printf("========================================================================================================================\n");
					printAnimado("El bosque oscuro ya ha sido visitado.\n", 15);
					printf("========================================================================================================================\n");
					printf("\n\n\n");
					system("pause");//Espera a que el usuario presione una tecla
					system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
				}
				break;
			case '3': //Volcan
				if (P_heroe->corazon == 0) {
					PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(400);
					system("cls");

					inicializarOponenteGolem(P_enemigo);
					printf("========================================================================================================================\n");
					switch (P_enemigo->color) { //cambia el color de las imagenes de los enemigos
						case 0:
							SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Cambia el color a blanco
							break;
						case 1:
							SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); //Cambia el color del texto a verde
							break;
						case 2:
							SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); //Cambia el color del texto a rojo
							break;
					}
					Carga_imagen(P_enemigo);
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Reset color a blanco
					printf("\n========================================================================================================================\n");
					printf("En la cima del volcán conocido como Monte Ramún reposa una de las creaciones más terribles del maligno Gor: un gólem\nformado de roca volcánica y lava, custodio del artefacto conocido como el corazón de fuego. Al escalar este infierno\npuedes escuchar el retumbar de la tierra con cada uno de sus pasos y cuando vislumbras al imponente monstruo que mora en\neste entorno de pesadilla, comprendes que ninguna creación de Gor pertenece a este mundo.\n");

					PlaySound("sound/golem risa.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(300);

					short resultado = combate(P_heroe, P_enemigo, hConsole);

					if (resultado == 1) {
						return 1;
					} else {
						PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
						Sleep(400);
						printf("Tu último ataque es certero y el gólem se desmorona en un estrepitoso alud de rocas. Al fin obtienes el corazón de\nfuego, artefacto que te protegerá de la hechicería maligna de Gor.\n");
						printf("========================================================================================================================\n");
						printf("\n\n\n");
						system("pause");//Espera a que el usuario presione una tecla
						system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
						P_heroe->corazon = 1;
					}
				} else {
					PlaySound("sound/error.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(200);
					system("cls");
					printf("========================================================================================================================\n");
					printAnimado("El monte Ramún ya ha sido visitado.\n", 15);
					printf("========================================================================================================================\n");
					printf("\n\n\n");
					system("pause");//Espera a que el usuario presione una tecla
					system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
				}
				break;
			case '4': //Fortaleza de GOR
				PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(400);
				system("cls");

				inicializarOponenteGOR(P_enemigo, P_heroe);
				printf("========================================================================================================================\n");
				switch (P_enemigo->color) { //cambia el color de las imagenes de los enemigos
					case 0:
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Cambia el color a blanco
						break;
					case 1:
						SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); //Cambia el color del texto a verde
						break;
					case 2:
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); //Cambia el color del texto a rojo
						break;
				}
				Carga_imagen(P_enemigo);
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Reset color a blanco
				printf("\n========================================================================================================================\n");
				printf("Atravesando los portones del castillo maldito, una oscuridad antinatural envuelve tu ser. En lo alto del trono\nennegrecido, el señor oscuro Gor te espera, empuñando el poder de las artes prohibidas. La sangre del rey aún tiñe su\narma, y la princesa Augusta yace cautiva entre sombras.\nEsta es la última batalla. No hay retorno. Solo uno saldrá con vida.\n");

				PlaySound("sound/risa.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(1000);

				short resultado = combate(P_heroe, P_enemigo, hConsole);

				if (resultado == 1) {
					return 1;
				} else {
					PlaySound("sound/start.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(400);
					printf("Con un rugido final, Gor cae, derrotado por tu voluntad inquebrantable. Las sombras se disipan, y el castillo tiembla\ncomo si la tierra misma repudiara al tirano vencido. Has salvado al reino de Lorian.\nLa luz regresa... y con ella, la esperanza.\n");
					printf("========================================================================================================================\n");
					printf("\n\n\n");
					system("pause");//Espera a que el usuario presione una tecla
					system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
					return 2; //Al ganar la última batalla lleva a la pantalla de elección de nuevo juego o salir
				}
				break;
		}
	}
	return 0;
}
short combate(struct jugador *P_heroe, struct contrincante *P_enemigo, HANDLE hConsole) {

	short primer_turno = 1;
	char opcion = 0;
	short salud_enem = P_enemigo->pv_max;
	short salud_heroe = P_heroe->pv;
	short magia_heroe = P_heroe->pm;
	short costo_magia = 10;
	short dano = P_heroe->fuerza;
	short fuerza_enemigo = P_enemigo->fuerza;
	char log_jugador[150] = {0};
	char log_enemigo[150] = {0};


	while (1) {

		if (P_heroe->esencia != 0) { //Si el heroe tiene la escencia (derrotó al arbol) hace doble de daño.
			dano = P_heroe->fuerza * 2;
		} else {
			dano = P_heroe->fuerza;
		}
		if (P_heroe->corazon != 0) { //Si el heroe tiene el corazon (derrotó al golem) recibe la mitad del daño.
			fuerza_enemigo = P_enemigo->fuerza / 2;
		} else {
			fuerza_enemigo = P_enemigo->fuerza;
		}

		if (!primer_turno) { //En el primer turno no muestra la imagen porque ya viene de mapa()
			printf("========================================================================================================================\n");
			switch (P_enemigo->color) { //Cambia el color de las imagenes de los enemigos
				case 0:
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Cambia el color a blanco
					break;
				case 1:
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); //Cambia el color del texto a verde
					break;
				case 2:
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); //Cambia el color del texto a rojo
					break;
			}
			Carga_imagen(P_enemigo);
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //Reset color a blanco
			printf("========================================================================================================================\n");
		}

		if (log_jugador[0] != '\0') {
			printf("%s", log_jugador);
		}
		if (log_enemigo[0] != '\0') {
			printf("%s", log_enemigo);
			if (salud_enem <= 0) {
				printf("========================================================================================================================\n");
				P_heroe->pv = salud_heroe; //Se guardan los dos parametros para otros combates y mostrar los datos en el mapa
				P_heroe->pm = magia_heroe;
				return 0;
			}
			if (salud_heroe <= 0) {
				printf("========================================================================================================================\n");
				PlaySound("sound/muerte.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(1000);
				printf("\n\n\n");
				system("pause");//Espera a que el usuario presione una tecla
				system("cls");//Limpia la pantalla en Windows (usa CMD internamente)
				return 1;
			}
		}

		printf("========================================================================================================================\n");
		printf("%s: %3hd/%3hd PV\n", P_enemigo->nombre, salud_enem, P_enemigo->pv_max);
		printf("%s: %3hd/%3hd PV || %3hd/%3hd PM\n", P_heroe->nombre, salud_heroe, P_heroe->pv_max, magia_heroe, P_heroe->pm_max);
		printf("========================================================================================================================\n");
		printf("|                                                 ======================                                               |\n");
		printf("|                                                 | 1. Ataque físico   |                                               |\n");
		printf("|                                                 | 2. Magia de hielo  |                                               |\n");
		printf("|                                                 | 3. Magia de fuego  |                                               |\n");
		printf("|                                                 ======================                                               |\n");
		printf("========================================================================================================================\n");

		primer_turno = 0;
		fflush(stdin);
		opcion = getch();
		system ("cls");

		switch (opcion) { //Ataque del jugador
			case '1': //Ataque fisico
				PlaySound("sound/espada.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(300);
				dano *= P_enemigo->fisico;
				sprintf(log_jugador, "¡%s ataca con un golpe físico contra %s!\n", P_heroe->nombre, P_enemigo->nombre);
				salud_enem -= dano;
				break;

			case '2': //Ataque de hielo
				if (magia_heroe <= 0) {
					sprintf(log_jugador, "¡No hay pm suficiente!\n");
					dano = 0;
				} else {
					PlaySound("sound/ice.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(300);
					dano *= P_enemigo->hielo;
					sprintf(log_jugador, "¡%s lanza magia de hielo contra %s!\n", P_heroe->nombre, P_enemigo->nombre);
					salud_enem -= dano;
					magia_heroe -= costo_magia;
				}
				break;

			case '3': //Ataque de fuego
				if (magia_heroe <= 0) {
					sprintf(log_jugador, "¡No hay pm suficiente!\n");
					dano = 0;
				} else {
					PlaySound("sound/fire.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(300);
					dano *= P_enemigo->fuego;
					sprintf(log_jugador, "¡%s lanza magia de fuego contra %s!\n", P_heroe->nombre, P_enemigo->nombre);
					salud_enem -= dano;
					magia_heroe -= costo_magia;
				}
				break;

			default: //Boton equivocado, ataque fallido
				PlaySound("sound/error.wav", NULL, SND_FILENAME | SND_ASYNC);
				Sleep(300);
				sprintf(log_jugador, "Opción no válida ¡Tu ataque ha fallado!\n");
				dano = 0;
		}

		sprintf(log_jugador + strlen(log_jugador), "¡%s ha recibido %hd de daño!\n", P_enemigo->nombre, dano);

		if (salud_enem <= 0) {
			sprintf(log_enemigo, "¡Has derrotado a %s!\n", P_enemigo->nombre);
		} else { //Ataque del enemigo

			salud_heroe -= fuerza_enemigo;
			sprintf(log_enemigo, "¡%s ataca y %s recibe %hd de daño!\n", P_enemigo->nombre, P_heroe->nombre, fuerza_enemigo);

			if (salud_heroe <= 0) {
				sprintf(log_enemigo + strlen(log_enemigo), "Has sido derrotado por %s...\n", P_enemigo->nombre);
			}
		}
		if (P_enemigo->codigo_imagen == 4) { //Si el enemigo es Gor, llama a la funcion para cambiar sus parametros en cada turno
			inicializarOponenteGOR(P_enemigo, P_heroe);
		}
	}
	return 0;
}
void inicializarOponenteEsqueleto(struct contrincante *e) {
	strcpy(e->nombre, "Edmund");
	e->pv_max = 120;
	e->fuerza = 20;
	e->fisico = 2;
	e->fuego = 0.5;
	e->hielo = 0.5;
	e->codigo_imagen = 1;
	e->color = 0;
}
void inicializarOponenteArbol(struct contrincante *a) {
	strcpy(a->nombre, "Árbol sagrado");
	a->pv_max = 100;
	a->fuerza = 10;
	a->fisico = 1;
	a->fuego = 2;
	a->hielo = 1;
	a->codigo_imagen = 2;
	a->color = 1;
}
void inicializarOponenteGolem(struct contrincante *g) {
	strcpy(g->nombre, "Gólem de fuego");
	g->pv_max = 200;
	g->fuerza = 10;
	g->fisico = 1;
	g->fuego = 0;
	g->hielo = 2;
	g->codigo_imagen = 3;
	g->color = 2;
}
void inicializarOponenteGOR(struct contrincante *gor, struct jugador *P_heroe) {
	strcpy(gor->nombre, "Gor el terrible");
	gor->pv_max = 200;
	gor->fuerza = 20;
	gor->codigo_imagen = 4;
	mezcla(&gor->fisico, &gor->fuego, &gor->hielo); //Mezcla los valores para que las debilidades/fortalezas sean aleatorias

	if (P_heroe->ojo != 0) {
		if (gor->fisico == 2) { //El color depende de la debilidad, imitando el patron de los demas enemigos
			gor->color = 0;
		} else {
			if (gor->fuego == 2) {
				gor->color = 1;
			} else {
				if (gor->hielo == 2) {
					gor->color = 2;
				}
			}
		}
	} else {
		gor->color = 0;
	}
}
void mezcla(float *a, float *b, float *c) { //Mezcla tres valores

	short valores[3] = {0, 1, 2};
	// Fisher-Yates shuffle
	for (short i = 2; i > 0; i--) {
		short j = rand() % (i + 1);
		short temp = valores[i];
		valores[i] = valores[j];
		valores[j] = temp;
	}
	*a = valores[0];
	*b = valores[1];
	*c = valores[2];
}
void printAnimado(char* texto, short tiempo) {

	short i = 0;
	while (texto[i] != '\0') {
		printf("%c", texto[i++]);
		Sleep(tiempo);
	};
}
void Carga_imagen (struct contrincante *P_enemigo) {

	FILE *archivo;
	char linea[120]; // Buffer para almacenar una línea

	switch (P_enemigo->codigo_imagen) {

		case 0 :
			archivo = fopen("txt/Banner.txt", "rt"); // Abrir en modo lectura
			if (archivo == NULL) {
				printf("                                               ****************************\n");
				printf("                                               *Error al abrir el archivo.*\n");
				printf("                                               ****************************\n");
			}
			while (fgets(linea, sizeof(linea), archivo) != NULL) { //feof devuelve 0 si no es fin de archivo, pero usamos otro método que nos es más claro
				printf("%s", linea); // Imprimir cada línea
			}
			fclose(archivo);
			archivo = NULL;
			break;

		case 1:
			archivo = fopen("txt/Edmund.txt", "rt"); // Abrir en modo lectura
			if (archivo == NULL) {
				printf("                                               ****************************\n");
				printf("                                               *Error al abrir el archivo.*\n");
				printf("                                               ****************************\n");
			}
			while (fgets(linea, sizeof(linea), archivo) != NULL) { //feof devuelve 0 si no es fin de archivo, pero usamos otro método que nos es más claro
				printf("%s", linea); // Imprimir cada línea
			}
			fclose(archivo);
			archivo = NULL;
			break;
		case 2:
			archivo = fopen("txt/Arbol_corrupto.txt", "rt"); // Abrir en modo lectura
			if (archivo == NULL) {
				printf("                                               ****************************\n");
				printf("                                               *Error al abrir el archivo.*\n");
				printf("                                               ****************************\n");
			}
			while (fgets(linea, sizeof(linea), archivo) != NULL) { //feof devuelve 0 si no es fin de archivo, pero usamos otro método que nos es más claro
				printf("%s", linea); // Imprimir cada línea
			}
			fclose(archivo);
			archivo = NULL;
			break;
		case 3:
			archivo = fopen("txt/Golem_fuego.txt", "rt"); // Abrir en modo lectura
			if (archivo == NULL) {
				printf("                                               ****************************\n");
				printf("                                               *Error al abrir el archivo.*\n");
				printf("                                               ****************************\n");
			}
			while (fgets(linea, sizeof(linea), archivo) != NULL) { //feof devuelve 0 si no es fin de archivo, pero usamos otro método que nos es más claro
				printf("%s", linea); // Imprimir cada línea
			}
			fclose(archivo);
			archivo = NULL;
			break;
		case 4:
			archivo = fopen("txt/Gor.txt", "rt"); // Abrir en modo lectura
			if (archivo == NULL) {
				printf("                                               ****************************\n");
				printf("                                               *Error al abrir el archivo.*\n");
				printf("                                               ****************************\n");
			}
			while (fgets(linea, sizeof(linea), archivo) != NULL) { //feof devuelve 0 si no es fin de archivo, pero usamos otro método que nos es más claro
				printf("%s", linea); // Imprimir cada línea
			}
			fclose(archivo);
			archivo = NULL;
			break;
	}

}
void ajustarTamanoConsola(HANDLE hConsola, short columnas, short filas) {

	// Establecer el tamaño del búfer primero
	COORD tamBuffer;
	tamBuffer.X = columnas;
	tamBuffer.Y = filas;
	SetConsoleScreenBufferSize(hConsola, tamBuffer);

	// Establecer el tamaño de la ventana
	SMALL_RECT tamVentana = {0, 0, columnas - 1, filas - 1};
	SetConsoleWindowInfo(hConsola, TRUE, &tamVentana);
}
void guardarPuntos(struct jugador *P_heroe, clock_t inicio, clock_t fin) {

	int puntaje_max = 10000;
	short penalizacion = 15;
	int tiempo_segundos = (fin - inicio) / CLOCKS_PER_SEC;
	P_heroe->puntaje = puntaje_max - (tiempo_segundos * penalizacion);

	if (P_heroe->puntaje < 0) {
		P_heroe->puntaje = 0;
	}

	FILE *archivo = fopen("scores/puntajes.bin", "ab");
	if (archivo == NULL) {
		printf("                                               ****************************\n");
		printf("                                               *Error al abrir el archivo.*\n");
		printf("                                               ****************************\n");
		return;
	}

	fwrite(P_heroe, sizeof( struct jugador), 1, archivo);
	fclose(archivo);
}
void cargarPuntos() {
	while (1) {

		PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
		Sleep(400);
		system("cls");
		printf("========================================================================================================================\n");
		printAnimado("PUNTUACIONES MÁXIMAS\n", 15);
		printf("========================================================================================================================\n");

		FILE *archivo = fopen("scores/puntajes.bin", "rb");
		if (archivo == NULL) {
			printf("                                               ****************************\n");
			printf("                                               *Error al abrir el archivo.*\n");
			printf("                                               ****************************\n");
			system("pause");
			system("cls");
			return;
		}

// Calcular cuántos registros hay
		fseek(archivo, 0, SEEK_END);
		long tam_archivo = ftell(archivo);
		short total_jugadores = tam_archivo / sizeof(struct jugador);
		rewind(archivo); // Volver al inicio del archivo

// Crear un array dinámico para leer los jugadores
		struct jugador *jugadores = malloc(total_jugadores * sizeof(struct jugador));
		if (jugadores == NULL) {
			printf("                                               ***************************\n");
			printf("                                               *Error al asignar memoria.*\n");
			printf("                                               ***************************\n");
			fclose(archivo);
			system("pause");
			system("cls");
			return;
		}

// Leer todos los jugadores del archivo
		fread(jugadores, sizeof(struct jugador), total_jugadores, archivo);
		fclose(archivo);

// Ordenar por puntaje descendente (bubble sort, suficiente para 10)
		for (short i = 0; i < total_jugadores - 1; i++) {
			for (short j = i + 1; j < total_jugadores; j++) {
				if (jugadores[j].puntaje > jugadores[i].puntaje) {
					struct jugador temp = jugadores[i];
					jugadores[i] = jugadores[j];
					jugadores[j] = temp;
				}
			}
		}

// Mostrar los 10 mejores
		printf("Estos son los mayores héroes del reino:\n");
		printf("========================================================================================================================\n");
		short max_mostrar;
		if (total_jugadores < 1) {
			printf("Aún no hay puntajes, ¡vence a Gor al menos una vez!.\n");
		}
		if (total_jugadores < 10) {
			max_mostrar = total_jugadores;
		} else {
			max_mostrar = 10;
		}
		for (short i = 0; i < max_mostrar; i++) {
			printf("%2d. %-20s - %5d puntos\n", i + 1, jugadores[i].nombre, jugadores[i].puntaje);
		}
		printf("========================================================================================================================\n");

// Liberar memoria
		free(jugadores);

		printf("========================================================================================================================\n");
		printf("|                                             ===========================                                              |\n");
		printf("|                                             | 1. Volver               |                                              |\n");
		printf("|                                             | 2. Borrar puntuaciones  |                                              |\n");
		printf("|                                             ===========================                                              |\n");
		printf("========================================================================================================================\n");

		char menu = 0;

		do {
			menu = getch();
			PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
			Sleep(400);
		} while (menu != '1' && menu != '2');

		switch (menu) {
			case '1':
				system("cls");
				return;
				break;
			case '2':
				printf("¿Estas seguro que quieres borrar los registros? Esta acción no se puede deshacer.\n");
				printf("========================================================================================================================\n");
				printf("|                                             ===========================                                              |\n");
				printf("|                                             | 1. Si                   |                                              |\n");
				printf("|                                             | 2. No                   |                                              |\n");
				printf("|                                             ===========================                                              |\n");
				printf("========================================================================================================================\n");

				do {
					menu = getch();
					PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
					Sleep(400);
				} while (menu != '1' && menu != '2');

				switch (menu) {
					case '1':{
						FILE *archivo = fopen("scores/puntajes.bin", "wb");
						if (archivo == NULL) {
							printf("\nError de archivo.\n");
							system("pause");
							system("cls");
							return;
						}
						fclose(archivo);
						printf("Puntajes borrados.\n");
						system("pause");
						break;
					}
					case '2':
						break;
				}
				break;
		}
	}
}
void creditos() {
	PlaySound("sound/select.wav", NULL, SND_FILENAME | SND_ASYNC);
	Sleep(400);
	system("cls");
	printf("========================================================================================================================\n");
	printAnimado("LA MALDICION DE GOR:\n", 15);
	printf("========================================================================================================================\n");
	printf("\n");
	printAnimado("Creado por:\n", 10);
	printf("\n");
	printf("========================================================================================================================\n");
	printf("|                                          ==============================                                              |\n");
	printf("|                                          |   Horodeski Braian Matias  |                                              |\n");
	printf("|                                          |    Martin Franco Gabriel   |                                              |\n");
	printf("|                                          |     Villegas Alejandro     |                                              |\n");
	printf("|                                          ==============================                                              |\n");
	printf("========================================================================================================================\n");
	printAnimado("Estructura de datos y algoritmos I - Comision 322.\n", 15);
	printf("========================================================================================================================\n");
	printAnimado("UNIVERSIDAD PROVINCIAL DE EZEIZA - 2025.\n", 15);
	printf("========================================================================================================================\n");
	printf("\n\n\n");
	system("pause");
	system("cls");
	return;
}


