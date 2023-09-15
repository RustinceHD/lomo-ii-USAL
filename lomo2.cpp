
//  Edgar Cop� Rubio,  edgarcoperubio@usal.es https://github.com/edcoru
//  Juan Calles Rivas,  juan.calles@usal.es	https://github.com/RustinceHD/
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lomo2.h"

#define MAX 100
#define USR1 "i6743891"
#define USR2 "i0964742"
#define FIL 17
#define COL 75

typedef struct {
	int ID;
	char color[MAX];
	int coordX;
	int coordY;

}tipoTren;



typedef struct memoriaCompartida {
	int matriz[FIL][COL];
	int nTrenes;
	int ntrenesAcual;
	int numTrenesInter;
	BOOL bucle;
	BOOL interbloc;
	HANDLE semaforo;
	HANDLE mapa;
	HANDLE hTreads[MAX];
	DWORD threadIds[MAX];
	tipoTren arrayTrenesInter[MAX];
	tipoTren arrayTrenes[MAX];
}memoriaCompartida;
/*
typedef struct funciones{
 char* (*)(int) LOMO_getColor;
  void * LOMO_espera;
  int (*)() LOMO_trenNuevo;
  int (*)(int, int*, int*) LOMO_peticiOnAvance;
  int(*)(int, int*, int*) LOMO_avance;
  void (*)(char*) pon_error;
}funcione;
*/

int salida(int);
BOOL __stdcall handler(DWORD);
DWORD WINAPI movimiento(LPVOID);
int liberarRecursosCOmpartido(int);

int main(int argc, char* argv[]) {
	
	int i = 0, j = 0;

	if (!SetConsoleCtrlHandler(handler, TRUE)) {
		salida(-91);
	}

	int retardo, nTrenes = 0, tamMax;


	//Abrimos y comprobamos si hay error al abrir la libreria
	//HANDLE libreria = LoadLibrary(LIBLOMO);
	HINSTANCE libreria = LoadLibrary("lomo2.dll");
	if (libreria == NULL) {
		salida(-92);
	}

	//Procedemos a hacer un BINDEO de la función LOMO_generar_mapa de la libreria

	
	// (LO que devuelve () (parametro))

	int (*LOMO_generar_mapa)(const char*, const char*) = (int(*)(const char*, const char*)) GetProcAddress(libreria, "LOMO_generar_mapa");
	if (LOMO_generar_mapa == NULL) {
		salida(-92);
	}

	int (*LOMO_inicio)(int, int, const char*, const char*) = (int(*)(int, int, const char*, const char*)) GetProcAddress(libreria, "LOMO_inicio");
	if (LOMO_inicio == NULL) {
		salida(-92);
	}




	int(*LOMO_fin)() = (int(*)()) GetProcAddress(libreria, "LOMO_fin");
	if (LOMO_fin == NULL) {
		salida(-92);
	}




	VOID(*pon_error)(char*) = (VOID(*)(char*)) GetProcAddress(libreria, "pon_error");
	if (pon_error == NULL) {
		salida(-92);
	}


	if (argc > 4)
	{
		salida(-99);
	}
	if (argc == 2 && strcmp(argv[1], "--mapa") == 0)
	{
		if (-1 == LOMO_generar_mapa(USR1, USR2))
			salida(-98);
		return 0;
	}
	else if (argc == 4 && atoi(argv[1]) >= 0 && atoi(argv[3]) >= 1 && atoi(argv[3]) <= 100 && atoi(argv[2]) >= 3 && atoi(argv[2]) <= 19)
	{
		retardo = atoi(argv[1]);
		tamMax = atoi(argv[2]);
		nTrenes = atoi(argv[3]);
	}
	else
	{
		salida(-99);
	}

	HANDLE semaforo = CreateSemaphore(NULL, 0, 1, "semaforoZ");
	if (semaforo == NULL) {
		CloseHandle(semaforo);
		salida(-91);
	}


	/////////////////////////////////////////////////////////////////

	HANDLE mapa = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(memoriaCompartida), "Mem0ria");
	if(mapa==NULL){
		salida(-95);
	}

	
	memoriaCompartida *mc = (memoriaCompartida*)MapViewOfFile(mapa, FILE_MAP_ALL_ACCESS, 0,0, sizeof(memoriaCompartida));
	if(mc==NULL){
		salida(-95);
	}
	mc->bucle = TRUE;
	mc->mapa = mapa;
	mc->semaforo = semaforo;
	/////////////////////////////////////////////////////////////////
	mc->nTrenes = nTrenes;

	for (i = 0; i < FIL; i++)
	{
		for (j = 0; j < COL; j++)
		{
			mc->matriz[i][j] = -99;
		}
	}

	for (i = 0; i < nTrenes; i++) {
		mc->arrayTrenes[i].ID = -99;
	}

	LOMO_inicio(retardo, tamMax, USR1, USR2);


	DWORD threadIda;
	HANDLE tmp2;
	for (i = 0; i < nTrenes; i++) {

		mc->hTreads[i] = CreateThread(NULL, 0, movimiento, NULL, 0, &threadIda);

		mc->threadIds[i] = threadIda;
		if (mc->hTreads[i] == NULL) {
			salida(-94);
		}


	}
	mc->interbloc = FALSE;
	ReleaseSemaphore(semaforo, 1, NULL);

	WaitForMultipleObjects(nTrenes, mc->hTreads, TRUE, 60000);
	if (mc->interbloc == TRUE) {
		char c[MAX * 9];
		char coma[] = ", ";
		char final[] = " y ";
		char punt[] = ".";
		sprintf_s(c, sizeof(c), "Ntrenes[%d] :", mc->numTrenesInter+1);
		mc->numTrenesInter++;
		for (i = 0; i < mc->numTrenesInter  ; i++ ) {
			
			strcat_s(c, sizeof(c), mc->arrayTrenesInter[i].color);
			strcat_s(c, sizeof(c), coma);
			
		}
		
		
		pon_error(c);
	}
	UnmapViewOfFile(mc);
	liberarRecursosCOmpartido(0);
	
	

}

int liberarRecursosCOmpartido(int op) {
	if (op == 0) {
		HINSTANCE libreria = LoadLibrary("lomo2.dll");
		if (libreria == NULL) {
			salida(-92);
		}



		int(*LOMO_fin)() = (int(*)()) GetProcAddress(libreria, "LOMO_fin");
		if (LOMO_fin == NULL) {
			salida(-92);
		}

		LOMO_fin();

		HANDLE mapa = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Mem0ria");
		if (mapa == NULL) {
			salida(-91);
		}

		memoriaCompartida* mc = (memoriaCompartida*)MapViewOfFile(mapa, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(memoriaCompartida));
		if (mc == NULL) {
			salida(-90);
		}
		if (mc->interbloc == FALSE) {
			mc->bucle = FALSE;
			WaitForMultipleObjects(mc->nTrenes, mc->hTreads, TRUE, INFINITE);
		}
		HANDLE sem = mc->semaforo, mem = mc->mapa;

		CloseHandle(sem);
		UnmapViewOfFile(mc);
		CloseHandle(mapa);

		return 0;
	}
	else if (op == 1) {
		HANDLE mapa = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Mem0ria");
		if (mapa == NULL) {
			salida(-91);
		}
		memoriaCompartida* mc = (memoriaCompartida*)MapViewOfFile(mapa, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(memoriaCompartida));
		if (mc == NULL) {
			salida(-90);
		}
		HANDLE sem = mc->semaforo, mem = mc->mapa;

		CloseHandle(sem);
		UnmapViewOfFile(mc);
		CloseHandle(mapa);

		return 0;

	}
	return 0;
}

DWORD WINAPI movimiento(LPVOID lpParam) {
	int i, j, antY;
	int trensID[100];
	HINSTANCE libreria = LoadLibrary("lomo2.dll");
	if (libreria == NULL) {
		salida(-92);
	}

	HANDLE semaforo = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "semaforoZ");
	if (semaforo == NULL) {
		salida(-91);
	}

	HANDLE mapa = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Mem0ria");
	if (mapa == NULL) {
		salida(-91);
	}

	memoriaCompartida *mc  = (memoriaCompartida*) MapViewOfFile(mapa, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(memoriaCompartida) );
	if(mc==NULL){
		salida(-89);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////

	char* (*LOMO_getColor)(int) = (char* (*)(int)) GetProcAddress(libreria, "LOMO_getColor");
	if (LOMO_getColor == NULL) {
		salida(-92);
	}

	VOID(*LOMO_espera)(int, int) = (VOID(*)(int, int)) GetProcAddress(libreria, "LOMO_espera");
	if (LOMO_espera == NULL) {
		salida(-92);
	}

	int (*LOMO_trenNuevo)() = (int(*)()) GetProcAddress(libreria, "LOMO_trenNuevo");
	if (LOMO_trenNuevo == NULL) {
		salida(-92);
	}

	int (*LOMO_peticiOnAvance)(int, int*, int*) = (int(*)(int, int*, int*)) GetProcAddress(libreria, "LOMO_peticiOnAvance");
	if (LOMO_peticiOnAvance == NULL) {
		salida(-92);
	}
	VOID(*pon_error)(char*) = (VOID(*)(char*)) GetProcAddress(libreria, "pon_error");
	if (pon_error == NULL) {
		salida(-92);
	}

	int (*LOMO_avance)(int, int*, int*) = (int(*)(int, int*, int*)) GetProcAddress(libreria, "LOMO_avance");
	if (LOMO_avance == NULL) {
		salida(-92);
	}

	int id = LOMO_trenNuevo();

	int posX, posY, xcab, ycab = 0, xcola, ycola;

	if (id == -1) {
		// ERROR PENDIENTE DE IMPLEMENTAR
		salida(-88);
	}
	//arrayMcomp->color = LOMO_getColor(id);
	strcpy_s(mc->arrayTrenes[id].color, LOMO_getColor(id));

	while (mc->bucle) {

		LOMO_peticiOnAvance(id, &xcab, &ycab);
		WaitForSingleObject(semaforo, INFINITE);
		if (mc->interbloc == FALSE) {

		antY = ycab;


		if (mc->matriz[ycab][xcab] == -99) {
			mc->matriz[ycab][xcab] = id;
			mc->arrayTrenes[id].ID = -99;

			LOMO_avance(id, &xcola, &ycola);
			LOMO_espera(ycab, antY);
			fprintf(stdout, "ID DEL TREN %d", id);
			if (ycola >= 0 && xcola >= 0) {
				mc->matriz[ycola][xcola] = -99;
			}

		}
		else {

			if (ycab >= 0 && xcab >= 0 && mc->interbloc == FALSE) {
				//fprintf(stderr, "[Comprobando interbloqueo tren %d]\n",id);
				int val = mc->arrayTrenes[id].ID = mc->matriz[ycab][xcab];

				int i = 0;
				mc->arrayTrenesInter[i] = mc->arrayTrenes[id];
				i++;
				while (val != -99 && val != id && i < mc->nTrenes + 1) {



					mc->arrayTrenesInter[i] = mc->arrayTrenes[val];
					mc->numTrenesInter = i;
					i++;
					val = mc->arrayTrenes[val].ID;
				}
				if (id == val) {

					char c[900];
					sprintf_s(c, sizeof(c), "El número de trenes interbloqueados es %d y el id es %i su color es %s", i , id, mc->arrayTrenesInter[id].color);
					pon_error(c);
					mc->interbloc = TRUE;
					mc->bucle = FALSE;
				}
				else {
					tipoTren aux;
					memset(mc->arrayTrenesInter, 0, sizeof(tipoTren) * MAX);
					mc->numTrenesInter = 0;
				}

			}
		}
	}
		ReleaseSemaphore(semaforo, 1, NULL);
	}

	UnmapViewOfFile(mc);
	return 0;

}

int salida(int error)
{
	switch (error)
	{
	case -99:
		printf("Error al pasa los argumentos ponga <--mapa> para ver el código html del mapa");
		exit(1);
		break;
	case -98:
		printf("Error en la creación del mapa ");
		exit(-98);
		break;
	case -97:
		perror("Error en la creación de los recursos ipc");
		// liberar el buzón
		exit(-97);
		break;
	case -96:
		perror("Error en la creación del semáforo");
		exit(-96);
		break;
	case -95:
		perror("ERROR A LA HORA DE RESERVAR MEMORIA DINÁMICA");
		exit(-95);
		break;
	case -94:
		perror("ERROR A LA HORA DE CREAR LOS HIJOS");
		liberarRecursosCOmpartido(0);
		exit(-94);
		break;
	case -93:
		printf("Error al abrir el archivo.\n");
		exit(-93);
		break;
	case -92:
		printf("Error al cargar la libreria\n");
		exit(-92);
		break;
	case -91:
		fprintf(stderr, "ERROR AL ARRANCAR LA MANEJADORA\n");
		exit(-91);
		break;
	case -90:
		fprintf(stderr, "ERROR al abrir la memoria compartida en la función de liberar recursos\n");
		exit(-90);
		break;
	case -89:
		fprintf(stderr, "ERROR AL ABRIR LA MEMORIA COMPARTIDA EN LA FUNCION DE MOVIMIENTO\n");
		liberarRecursosCOmpartido(0);
		exit(-89);
		break;
	case -88:
		fprintf(stderr, "ERROR EN LA CREACIÖN DEL TREN\n");
		exit(-88);
		break;
	default:
		break;
	}
}


BOOL __stdcall handler(DWORD eventoControl) {


	switch (eventoControl) {
	case CTRL_C_EVENT:
		//esto es solo el proceso del padre cuando hay control + c

		

		//liberar recursos ipc (segmento de memoria y semaforo)
		liberarRecursosCOmpartido(0);
		fprintf(stdout, "CONTROL + C REGISTRADO");
		return TRUE;
		break;
	default:
		fprintf(stdout, "ERROR DE ALGO RARO SE HA LLAMADO A LA MANEJADORA SIN CONTROL + C REGISTRADO");
		return FALSE;
	}


}
