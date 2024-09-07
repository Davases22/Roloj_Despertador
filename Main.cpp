#include "Delay.h"
#include "GLcd.h"
#include "Nokia5110.h"
#include "Port.h"
#include "Spi.h"
#include "Teclado.h"
#include "Timer.h"
#include "cmsis_os.h"

//----VARIABLES TECLADO----/
Teclado Te;
Port F, C;
unsigned char R,R2;
int n, i;
bool allowInput = false; // Flag para permitir la entrada después de "="
bool alarmaActiva = false;
char Text[50];
char Text2[50] = "00:00:00"; // Char para guardar la hora
char Text3[50] = "00:00:00"; // Char para guardar la hora de alarma
char Dec[17] = {' ', '1', '4', '7', '*', '2', '5', '8', '0',
                '3', '6', '9', '#', 'A', 'B', 'C', 'D'};

//----VARIABLES TIMER----/
int horas = 8, minutos = 11, segundos = 0;
int alarm_horas = 0, alarm_minutos = 0, alarm_segundos = 0;
Timer T; // Timer

//----VARIABLES NOKIA5110----/
Nokia5110 Nokia;
Spi S;           // Objeto SPI
Pin DC, RST, CE, PinMotor; // Pines asociados

								//Mapa de bits Figura Reloj
 unsigned char reloj [504]={
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0xC0, 0xE0, 0xF0, 0x70, 0x78, 0x38, 0x3C, 0x1C, 0x1C, 0x1E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E,
0x1E, 0x1C, 0x1C, 0x3C, 0x38, 0x78, 0x70, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xE0, 0xFC, 0xFE, 0x3F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x1F, 0xFF,
0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x0F, 0x7F, 0xFF, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xC0, 0xF0, 0xFF, 0x7F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1E, 0x1C,
0x3C, 0x38, 0x78, 0x70, 0x70, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0x70, 0x70, 0x70,
0x38, 0x3C, 0x1C, 0x1E, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// Función para mostrar la pantalla inicial
void mostrarPantallaInicial() {
    Nokia.BorrarPantalla();
    Nokia.PrintTexto(30, 0, "RELOJ");
    Nokia.PrintTexto(10, 10, "-----------");
    Nokia.PrintTexto(8, 15, "|");
    Nokia.PrintTexto(72, 15, "|");
    Nokia.PrintTexto(10, 20, "-----------");
    char buffer[9] = {};
    sprintf(buffer, "%02d:%02d:%02d", horas, minutos, segundos);
    Nokia.PrintTexto(22, 15, buffer);
    Nokia.PrintTexto(0, 32, "--------------");
    Nokia.PrintTexto(40, 37, "|");
    Nokia.PrintTexto(40, 41, "|");
    Nokia.PrintTexto(2, 40, "PROGRA");
    Nokia.PrintTexto(47, 40, "CONFIG");
    Nokia.Refrescar();
}

// Permite configurar la hora de la alarma y hora actual
void escribirHora() {
    R = Te.GetTecla();
    if (R) {
        if (Dec[R] == '#' && !allowInput) {
            Nokia.BorrarPantalla();
            Nokia.PrintTexto(5, 0, "Configuracion");
            Nokia.PrintTexto(10, 10, "-----------");
            Nokia.PrintTexto(8, 15, "|");
            Nokia.PrintTexto(72, 15, "|");
            Nokia.PrintTexto(10, 20, "-----------");
            Nokia.PrintTexto(0, 27, "Cambiar hora()");
            Nokia.PrintTexto(22, 39, Text2);
            Nokia.Refrescar();

            // Permitir entrada de nuevos datos
            n = 0;
            Text2[0] = 0; // Reiniciar Text2 para permitir nuevas entradas
            allowInput = true;
        } else if (allowInput && n < 8) {
            if (n == 2 || n == 5) {
                Text2[n] = ':'; // Inserta ':' después de los primeros dos números
                n++;
            }
            Text2[n] = Dec[R];
            n++;
            Text2[n] = 0;
            Nokia.SetColor(0);
            Nokia.RectanguloRelleno(0, 39, 75, 45);
            Nokia.SetColor(1);
            Nokia.PrintTexto(22, 39, Text2);
            Nokia.Refrescar();

            if (n >= 8) {
                allowInput = false; // Desactivar la entrada de datos después de 8 caracteres (HH:MM:SS)
            }
        }
    }

    if (Dec[R] == 'D') {
        int h, m, s;
        if (sscanf(Text2, "%02d:%02d:%02d", &h, &m, &s) == 3) {
            if (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60) {
                horas = h;
                minutos = m;
                segundos = s;
                mostrarPantallaInicial(); // Actualizar la pantalla con la nueva hora
            } else {
                Nokia.BorrarPantalla();
                Nokia.PrintTexto(5, 0, "Hora invalida");
                Delay_ms(500);
                n = 0;
                Text2[0] = 0; // Reiniciar Text2 para permitir nuevas entradas
                allowInput = true; // Permitir reingreso
                mostrarPantallaInicial();
            }
        }
    }

    if (Dec[R] == '*') {
        Text3[0] = 0; // Reiniciar Text3 para permitir nuevas entradas
        allowInput = true; // Permitir reingreso
        Nokia.BorrarPantalla();
        Nokia.PrintTexto(5, 0, "Configuracion");
        Nokia.PrintTexto(10, 10, "-----------");
        Nokia.PrintTexto(8, 15, "|");
        Nokia.PrintTexto(72, 15, "|");
        Nokia.PrintTexto(10, 20, "-----------");
        Nokia.PrintTexto(0, 27, "Poner alarma()");
        Nokia.PrintTexto(22, 39, Text3);
        Nokia.Refrescar();

        i = 0;
        allowInput = true;
        while (allowInput && i < 8) {
            R = Te.GetTecla();
            if (R && Dec[R] != '*') {
                if (i == 2 || i == 5) {
                    Text3[i] = ':'; // Inserta ':' después de los primeros dos números
                    i++;
                }
                Text3[i] = Dec[R];
                i++;
                Text3[i] = 0;
                Nokia.SetColor(0);
                Nokia.RectanguloRelleno(0, 39, 75, 45);
                Nokia.SetColor(1);
                Nokia.PrintTexto(22, 39, Text3);
                Nokia.Refrescar();

                if (i >= 8) {
                    allowInput = false; // Desactivar la entrada de datos después de 8 caracteres (HH:MM:SS)
                }

                while (Te.GetTecla() != 0) {
                    osDelay(10);
                }
            }
        }

        int h, m, s;
        if (sscanf(Text3, "%02d:%02d:%02d", &h, &m, &s) == 3) {
            if (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60) {
                alarm_horas = h;
                alarm_minutos = m;
                alarm_segundos = s;
                alarmaActiva = true;
                Nokia.BorrarPantalla();
                Nokia.PrintTexto(25, 4, "ALARMA");
                Nokia.PrintTexto(20, 30, "GUARDADA");
                Delay_ms(600);
                mostrarPantallaInicial(); // Volver a la pantalla inicial
            } else {
                Nokia.BorrarPantalla();
                Nokia.PrintTexto(5, 0, "Hora de alarma invalida");
                Delay_ms(500);
                mostrarPantallaInicial();
            }
        }
    }

    while (Te.GetTecla() != 0);
}

void Interrupcion(void) {
    Nokia.SetColor(0);
    Nokia.RectanguloRelleno(22, 15, 67, 22); // Borrar área del tiempo
    Nokia.SetColor(1);
    Nokia.Refrescar();

    if (++segundos >= 60) {
        segundos = 0;
        if (++minutos >= 60) {
            minutos = 0;
            if (++horas >= 24) {
                horas = 0;
            }
        }
    }

    char buffer[9] = {};
    sprintf(buffer, "%02d:%02d:%02d", horas, minutos, segundos);
    Nokia.PrintTexto(22, 15, buffer);
    Nokia.Refrescar();

    // Verificar si la hora actual coincide con la hora de la alarma
    if (horas == alarm_horas && minutos == alarm_minutos && segundos == alarm_segundos && alarmaActiva) {
        Nokia.BorrarPantalla(); // Borrar pantalla completamente antes de mostrar el mensaje de alarma
        Nokia.PrintTexto(5, 0, "Alarma!");
			  PinMotor=1;
        // Se transmite los 504 bytes del mapa de bits  
//        for(int X=0;X<504;X++)S=Imagen[X];
			  Nokia.Imagen(reloj);
        Nokia.Refrescar();

        // Esperar hasta que se presione el botón 'A' para cancelar la alarma
        while (1) {
            R = Te.GetTecla();
            if (Dec[R] == 'A') {
                alarmaActiva = false;
							  PinMotor=0;

                // Borrar pantalla y mostrar mensaje de alarma cancelada
                Nokia.BorrarPantalla(); 
                Nokia.PrintTexto(20, 15, "Alarma");
                Nokia.PrintTexto(15, 30, "cancelada");
                Nokia.Refrescar();
                Delay_ms(500);

                // Redibujar la interfaz de reloj
                Nokia.BorrarPantalla(); // Borrar pantalla antes de redibujar la interfaz de reloj
                Nokia.PrintTexto(30, 0, "RELOJ");
                Nokia.PrintTexto(10, 10, "-----------");
                Nokia.PrintTexto(8, 15, "|");
                Nokia.PrintTexto(72, 15, "|");
                Nokia.PrintTexto(10, 20, "-----------");
                sprintf(buffer, "%02d:%02d:%02d", horas, minutos, segundos);
                Nokia.PrintTexto(22, 15, buffer);
                Nokia.PrintTexto(0, 32, "--------------");
                Nokia.PrintTexto(40, 37, "|");
                Nokia.PrintTexto(40, 41, "|");
                Nokia.PrintTexto(2, 40, "PROGRA");
                Nokia.PrintTexto(47, 40, "CONFIG");
                Nokia.Refrescar();
                break;
            }
        }
    }
}

// Variables hilos
osThreadId Tarea1ID; // Identificador Teclado
osThreadId Tarea2ID; // Identificador Pantalla

void Tarea1Teclado(void const *arg) {
    while (1) {
        R2 = Te.WaitTecla();
        osSignalSet(Tarea2ID, 0x01); // Señal de que se pulsó una tecla
    }
}

void Tarea2Pantalla(void const *arg) {
    // Mostrar pantalla inicial
    mostrarPantallaInicial();
    osEvent E;
    while (1) {
        E = osSignalWait(0, osWaitForever);
        if (E.value.signals & 0x01) {
            escribirHora(); // Escribir hora o manejar la entrada del teclado
        }
    }
}

int main() {
    // Inicialización del hardware y objetos
	  osKernelInitialize();
    S.Iniciar(PA7, PA6, PA5, 1000000);
    Nokia.SetSpi(&S);
    Nokia.IniciarGLCD();
    Nokia.BorrarPantalla();
    Nokia.SetColor(1);
	
	  T.SetTimer(2);
    T = 1;
    T = Interrupcion;
	
	  PinMotor.DigitalOut(PC9);
    osThreadDef(Tarea1Teclado, osPriorityNormal, 1, 0);
    osThreadDef(Tarea2Pantalla, osPriorityNormal, 1, 0);
    Tarea1ID = osThreadCreate(osThread(Tarea1Teclado), NULL);
    Tarea2ID = osThreadCreate(osThread(Tarea2Pantalla), NULL);
	
	  C.DigitalOut(PB0, PB1, PA10, PB3);
    F.DigitalIn(PB5, PB4, PB10, PA8);
	  F.PullUp();
    Te.SetBusPort(&F, &C);
    
    osKernelStart();
    while (1);{
			osThreadYield();
		}
		return 0;
}