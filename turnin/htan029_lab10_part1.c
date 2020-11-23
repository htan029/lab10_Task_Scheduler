/*	Author: Heng Tan    
 *  Partner(s) Name: 
 *	Lab Section: 024
 *	Assignment: Lab 10  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Demo Link: https://youtu.be/okenoFqQwSg
 */
#include <avr/io.h>
#include <keypad.h>
#include <timer.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

////////////////////////////////////////////////////
// ======== task struct ===================

typedef struct task {
  signed char state; // Current state of the task
  unsigned long int period; // Rate at which the task should tick
  unsigned long int elapsedTime; // Time since task's previous tick
  int (*TickFct)(int); // Function to call for task's tick
} task;

// ======== End Task scheduler data structure =======

// ======== Shared Variables ========================
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;
// ======== End Shared Variables ====================

// ======== Tasks ===================================


//displaySMTick
enum display_States {display_display};

int displaySMTick(int state){
    unsigned char x;
    switch(state){
        case display_display: state = display_display; break;
        default: state = display_display; break;
    }

    switch(state){
        case display_display:
            x = GetKeypadKey();
            switch(x){
                case '\0': PORTB = 0x1F; break;
                case '1': PORTB = 0x81; break;
                case '2': PORTB = 0x82; break;
                case '3': PORTB = 0x83; break;
                case '4': PORTB = 0x84; break;
                case '5': PORTB = 0x85; break;
                case '6': PORTB = 0x86; break;
                case '7': PORTB = 0x87; break;
                case '8': PORTB = 0x88; break;
                case '9': PORTB = 0x89; break;
                case 'A': PORTB = 0x8A; break;
                case 'B': PORTB = 0x8B; break;
                case 'C': PORTB = 0x8C; break;
                case 'D': PORTB = 0x8D; break;
                case '*': PORTB = 0x8E; break;
                case '0': PORTB = 0x80; break;
                case '#': PORTB = 0x8F; break;
                default: PORTB = 0x1B; break;
            }
            break;
    }
    return state;
}

// ======= task struct end ==================
////////////////////////////////////////////////////

// ======= find GCD =========================
unsigned long int findGCD(unsigned long int a, unsigned long int b){
    unsigned long int c;
    while (1){
        c= a%b;
        if(c==0){return b;}
        a = b; 
        b = c;
    }
    return 0;
}
// ====== end find GCD ======================

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;

    static task task1, task2, task3, task4;
    task *tasks[] = { &task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    tasks[0]->state = start;
    tasks[0]->period = 50;
    tasks[0]->elapsedTime = tasks[1]->period;
    tasks[0]->TickFct = &displaySMTick;

    unsigned short i;
    unsigned long gcd = tasks[0]->period;
    for(i = 1; i < numTasks; i++){
        gcd = findGCD(gcd, tasks[i]->period);
    }

    TimerSet(gcd);
    TimerOn();
    
    /* Insert your solution below */
    while (1) {
        for(i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += gcd;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
