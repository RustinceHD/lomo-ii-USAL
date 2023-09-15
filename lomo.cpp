
//  Edgar Cop� Rubio,  edgarcoperubio@usal.es https://github.com/edcoru
//  Juan Calles Rivas,  juan.calles@usal.es	https://github.com/RustinceHD/
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "lomo2.h"

#define MAX 100
#define USR1 "USAL_USER_ID_1"
#define USR2 "USAL_USER_ID_1"
#define FIL 17
#define COL 75

typedef struct data_handler {
     HANDLE hThreads[MAX];
     DWORD threadIds[MAX];
     HANDLE semaforo;
     int nTrenes;
     int nTrenesActuales;
     BOOL thread;
}dataS;
dataS dataHandler;    
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

int salida(int );
BOOL __stdcall handler (DWORD );
DWORD WINAPI movimiento (LPVOID);

int main (int argc, char *argv[]){
    dataHandler.thread = TRUE;
    int i =0 , j =0;

      if (!SetConsoleCtrlHandler(handler, TRUE)) {
       salida(-91);
    }

int retardo, nTrenes, tamMax;


//Abrimos y comprobamos si hay error al abrir la libreria
//HANDLE libreria = LoadLibrary(LIBLOMO);
HINSTANCE libreria =  LoadLibrary(  "lomo2.dll");
if(libreria == NULL){
   salida (-92);
}

//Procedemos a hacer un BINDEO de la función LOMO_generar_mapa de la libreria

//tipo void (*  
/*
FARPROC  = GEtProcAddres(libreria,"LOMO_generar_mapa")
if (LOMO_generar_mapa == NULL){
    
}
*/
// (LO que devuelve () (parametro))

    int (*LOMO_generar_mapa)(char*, char*) = (int(*)(char*, char*)) GetProcAddress(libreria, "LOMO_generar_mapa");
    if (LOMO_generar_mapa == NULL) {
        salida(-92);
    }

        int (*LOMO_inicio)(int, int, char*, char*) = (int(*)(int, int, char*, char*)) GetProcAddress(libreria, "LOMO_inicio");
    if (LOMO_inicio == NULL) {
        salida(-92);
    }

    
    

        int (*LOMO_fin) = (int(*)) GetProcAddress(libreria, "LOMO_fin");
    if (LOMO_fin == NULL) {
        salida(-92);
    }

       VOID (*pon_error)(char*) = (VOID(*)(char*)) GetProcAddress(libreria, "pon_error");
    if (pon_error == NULL) {
        salida(-92);
    }


    if (argc > 4)
    {
        salida(-99);
    }
//LOMOGENERARMAPA CAMBIAR
    if (argc == 2 && strcmp(argv[1], "--mapa") == 0)
    {
        if (-1 == LOMO_generar_mapa( USR1, USR2 ))
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
    if(semaforo==NULL){
        salida(-91);
    }

     HANDLE mapa = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, FIL * COL * sizeof(int), "Mem0ria");

    int(*matComp)[COL] =  (int(*)[COL])MapViewOfFile(mapa, FILE_MAP_ALL_ACCESS, 0, 0, FIL * COL * sizeof(int));
    
  for (i = 0; i < FIL; i++)
    {
        for (j = 0; j < COL; j++)
        {
            matComp[i][j] = 0;
           // fprintf(stderr,"%d  ", matComp[i][j]);
        }
     //   fprintf(stderr,"\n");
    }
 
/*
    funciones.LOMO_getColor=LOMO_getCrlor;
    funciones.LOMO_avance= LOMO_avance;
    funciones.LOMO.LOMO_espera=LOMO_espera;
    funciones.LOMO_peticiOnAvance=LOMO_peticiOnAvance;
    funciones.LOMO_trenNuevo=LOMO_trenNuevo;
    funciones.pon_error=pon_error;()
    dataHandler.nTrenes=nTrenes;*/
    LOMO_inicio(retardo,tamMax,USR1,USR2);


    DWORD threadIda;
    for ( i = 0; i < nTrenes; i++){
        
        dataHandler.hThreads[i] = CreateThread(NULL, 0,movimiento,NULL,0, &threadIda);
        
        dataHandler.threadIds[i] = threadIda;
        if (dataHandler.hThreads[i]==NULL){
        //PENDIENTE DER IMPLEMENTAR
        printf ("AAAAAAAA");
        }
      

    }

 ReleaseSemaphore( semaforo, 1, NULL);
    
    WaitForMultipleObjects(nTrenes, dataHandler.hThreads, TRUE, INFINITE);


    //Hacer signal BOOL ReleaseSemaphore( HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
    //Hacer wait  DWORD WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds);
    //Para usar un semaforo ya creado HANDLE OpenSemaphore( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName); 

    /*
    printf("ANTES DE LOMO INICIO\n\n");
    
    printf("DESPUeS DE LOMO INICIO \n\n");
*/    
    ////////////////////////////////////////////////////////////////////////////////
    //IMPLEMENTAR LOS HIJOS  Y MANEJAFORA 
    //////////////////////////////////////////////////////////////////////////////
  
    //////////////////////////////////////////
    //CORE MOVIMIENTO
    //////////////////////////////////////////
    /*
    while (1) {
        //hacer un wait sobre el semaforo 

dfs
        //hacer signal semafor
*/
}


DWORD WINAPI movimiento (LPVOID lpParam){
    int i, j;
    HINSTANCE libreria =  LoadLibrary(  "lomo2.dll");
if(libreria == NULL){
   salida (-92);
}
   
   HANDLE semaforo = OpenSemaphore( SEMAPHORE_ALL_ACCESS , FALSE,"semaforoZ" );
    if(semaforo==NULL){
        salida(-91);
    }

   HANDLE mapa = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Mem0ria");
    if(semaforo==NULL){
        salida(-91);
    }
    int (*matMem)[COL] = (int (*)[COL]) MapViewOfFile(mapa, FILE_MAP_ALL_ACCESS, 0, 0, FIL * COL * sizeof(int));

    
    char* (*LOMO_getColor)(int) = (char*(*)(int)) GetProcAddress(libreria, "LOMO_getColor");
    if (LOMO_getColor == NULL) {
        salida(-92);
    }

        VOID (*LOMO_espera)(int, int) = (VOID(*)(int, int)) GetProcAddress(libreria, "LOMO_espera");
    if (LOMO_espera == NULL) {
        salida(-92);
    }

        int (*LOMO_trenNuevo)() = (int(*)()) GetProcAddress(libreria, "LOMO_trenNuevo");
    if (LOMO_trenNuevo == NULL) {
        salida(-92);
    }

        int (*LOMO_peticiOnAvance)(int, int *, int*) = (int(*)(int, int *, int *)) GetProcAddress(libreria, "LOMO_peticiOnAvance");
    if (LOMO_peticiOnAvance == NULL) {
        salida(-92);
    }

        int (*LOMO_avance)(int, int *, int *) = (int(*)(int, int *, int *)) GetProcAddress(libreria, "LOMO_avance");
    if (LOMO_avance == NULL) {
        salida(-92);
    }

       int id = LOMO_trenNuevo();

       int posX,posY, xcab, ycab, xcola,ycola;
       
        if (id == -1){
            // ERROR PENDIENTE DE IMPLEMENTAR
        }
       
        
        while(dataHandler.thread){
            WaitForSingleObject( semaforo, INFINITE );

        

            LOMO_peticiOnAvance(id, &xcab, &ycab);
            if (matMem[ycab][xcab] == 0){
                matMem[ycab][xcab] = 1;
            xcola = xcab;
            ycola = ycab; 

            LOMO_avance(id, &xcola, &ycola );
            if (ycola >= 0 && xcola >= 0){
                matMem[ycola][xcola] = 0;
            }
            //LOMO_espera()
        
           // printf("%i\n", i);
            }
           ReleaseSemaphore( semaforo, 1, NULL);
        }
         for (i = 0; i < FIL; i++)
    {
        for (j = 0; j < COL; j++)
        {
           // matMem[i][j] = 0;
            fprintf(stderr,"%d  ", matMem[j][i]);
        }
        fprintf(stderr,"\n");
    }
 
        
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
        //handler(1);
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
        fprintf(stderr,"ERROR AL ARRANCAR LA MANEJADORA\n");
        exit(-91);
         break;
    default:
        break;
    }
}


BOOL __stdcall handler (DWORD eventoControl){
    switch (eventoControl){
        case CTRL_C_EVENT:
        //esto es solo el proceso del padre cuando hay control + c

        //liberar recursos ipc (segmento de memoria y semaforo)
        dataHandler.thread = FALSE;
        fprintf (stdout, "CONTROL + C REGISTRADO");
        return TRUE;
        break;   
        default:
        fprintf (stdout, "ERROR DE ALGO RARO SE HA LLAMADO A LA MANEJADORA SIN CONTROL + C REGISTRADO"); 
        return FALSE;
    }


}