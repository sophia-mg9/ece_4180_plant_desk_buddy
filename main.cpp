
#include "mbed.h"
#include "rtos.h"
#include "SDFileSystem.h"
#include "wave_player.h"
#include "uLCD_4DGL.h"

SDFileSystem sd(p5, p6, p7, p8, "sd"); //SD card
uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;
<<<<<<< HEAD
Serial blue(p28,p27);
=======
>>>>>>> 952f06b65f49484bba6a7a50a3cb060227b2fd18

AnalogOut DACout(p18);
wave_player waver(&DACout);
Mutex uLCD_mutex;

PwmOut RGBLED_r(p23);
PwmOut RGBLED_g(p24);
PwmOut RGBLED_b(p25);

const int color_arr[] = {0x00ff00, 0x00ffff, 0xff7700, 0xffff00, 
                         0x0000ff, 0xff00dd, 0xff0000, 0x9900ff};

const float rgb_arr[8][3] = {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
                           {0.0f, 0.5333f, 1.0f}, {0.0f, 0.0f, 1.0f}, 
                           {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.1333f},
                           {0.0f, 1.0f, 1.0f}, {0.4f, 1.0f, 0.0f}};

unsigned int m_z=12434,m_w=33254;
int mode = 0;

unsigned int rnd() {
    m_z = 36969 * (m_z & 65535) + (m_z >>16);
    m_w = 18000 * (m_w & 65535) + (m_w >>16);
    return ((m_z <<16) + m_w);
}

void thread1(void const *args) {
    while(true) {
        FILE *wave_file;
        printf("\r\n\nHello, wave world!\n\r");
        Thread::wait(1000);
        wave_file=fopen("/sd/Boogie3.wav","r");
        if(wave_file==NULL) printf("file open error!\n\n\r");
        waver.play(wave_file);
        fclose(wave_file);

        Thread::wait(1000);
    }
}

void thread2(void const *args) {
    int arr[8];
    int x = 0;
    while(true) {
        for (int i = 0; i < 8; i++) {
            if (mode == 2) {
                if (x % 2 == 0) {
                    arr[i] = color_arr[0];
                } else {
                    arr[i] = 0x0;
                }
            } else if (mode == 0) {
                arr[i] = color_arr[x % 8];
            } else if (mode == 1) {
                arr[i] = color_arr[rnd() % 8];
            }
        }
        uLCD_mutex.lock();
    
        
        uLCD.filled_rectangle(0, 0, 32, 32, arr[0]);
        uLCD.filled_rectangle(0, 64, 32, 96, arr[1]);
        uLCD.filled_rectangle(32, 32, 64, 64, arr[2]);
        uLCD.filled_rectangle(32, 96, 64, 128, arr[3]);
        uLCD.filled_rectangle(64, 0, 96, 32, arr[4]);
        uLCD.filled_rectangle(64, 64, 96, 96, arr[5]);
        uLCD.filled_rectangle(96, 32, 128, 64, arr[6]);
        uLCD.filled_rectangle(96, 96, 128, 128, arr[7]);
        uLCD_mutex.unlock();
        x++;
        Thread::wait(100);
    }
}

void thread3(void const *args) {
    int arr[8];
    int x = 0;
    while(true) {
        for (int i = 0; i < 8; i++) {
            if (mode == 1) {
                if (x % 2 == 0) {
                    arr[i] = color_arr[0];
                } else {
                    arr[i] = 0x0;
                }
            } else if (mode == 2) {
                arr[i] = color_arr[x % 8];
            } else {
                arr[i] = color_arr[rnd() % 8];
            }
        }
        uLCD_mutex.lock();
        uLCD.rectangle(4, 36, 28, 60, arr[0]);
        uLCD.rectangle(4, 100, 28, 124, arr[1]);
        uLCD.rectangle(36, 4, 60, 28, arr[2]);
        uLCD.rectangle(36, 68, 60, 92, arr[3]);
        uLCD.rectangle(68, 36, 92, 60, arr[4]);
        uLCD.rectangle(68, 100, 92, 124, arr[5]);
        uLCD.rectangle(100, 4, 124, 28, arr[6]);
        uLCD.rectangle(100, 68, 124, 92, arr[7]);
        uLCD_mutex.unlock();
        Thread::wait(500); // wait 0.5s
        x++;
    }
}

//RGB LIGHT
void thread4(void const *args)
{
    int i = 0;
    while(1) {         // thread loop
        int index = i % 8;
        if (mode != 3) {
            RGBLED_r = rgb_arr[index][0];
            RGBLED_g = rgb_arr[index][1];
            RGBLED_b = rgb_arr[index][2];
        }

        i++;     // thread loop

        Thread::wait(100);
       
    }
}


int main()
{

    char bnum = 0;
    printf("Test\n");
    Thread t1(thread1);
    Thread t2(thread2);
    Thread t3(thread3);
    Thread t4(thread4);
    uLCD.baudrate(400000);

    while (true) {

        if (blue.getc()=='!') {
            if (blue.getc()=='B') { //button data
                bnum = blue.getc(); //button number
                switch (bnum) {
                    case '1': mode = 0; break;
                    case '2': mode = 1; break;
                    case '3': mode = 2; break; 
                }
            }
        }
       Thread::wait(500);
    }
}