/*	Author: Heng Tan    
 *  Partner(s) Name: 
 *	Lab Section: 024
 *	Assignment: Lab 10  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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
unsigned char lock = 0;
unsigned char check = 0;
// ======== End Shared Variables ====================

// ======== Tasks ===================================

//startEnter
enum enter_States {enter_wait, enter_one, enter_two, enter_three, enter_four, enter_five};

int enterSMTick(int state){
    static unsigned char x;
    
    static int lastState;
    x = GetKeypadKey();
    
    switch(state){
        case enter_wait: 
            if(x == '#'){
                state = enter_wait;
                check = 1;
            } else if (check == 1 && x == '\0'){
                state = enter_one;
                check = 0;
            } 
            break;
        case enter_one: 
            if(x == '1'){
                state = enter_one;
                check = 1;
            } else if (check == 1 && x == '\0'){
                state = enter_two;
                check = 0;
            } else if (x != '1' && x != '\0'){
                state = enter_wait;
                check = 0;
            } 
            break;
        case enter_two: 
            if(x == '2'){
                state = enter_two;
                check = 1;
            } else if (check == 1 && x == '\0'){
                state = enter_three;
                check = 0;
            } else if (x != '2' && x != '\0'){
                state = enter_wait;
                check = 0;
            } 
            break;
        case enter_three: 
            if(x == '3'){
                state = enter_three;
                check = 1;
            } else if (check == 1 && x == '\0'){
                state = enter_four;
                check = 0;
            } else if (x != '3' && x != '\0'){
                state = enter_wait;
                check = 0;
            } 
            break;
        case enter_four: 
            if(x == '4'){
                state = enter_four;
                check = 1;
            } else if (check == 1 && x == '\0'){
                state = enter_five;
                check = 0;
            } else if (x != '4' && x != '\0'){
                state = enter_wait;
                check = 0;
            } 
            break;
        case enter_five: 
            if(x == '5'){
                state = enter_five;
                check = 1;
            } else if (check == 1 && x == '\0'){
                state = enter_wait;
                lock = 1;
                check = 0;
            } else if (x != '5' && x != '\0'){
                state = enter_wait;
                check = 0;
            }
            break;
        default: state = enter_wait; break;
    }

    switch(state){
        case enter_wait: break;
        case enter_one: break;
        case enter_two: break;
        case enter_three: break;
        case enter_four: break;
        case enter_five: break;
    }
    return state;
}

//displaySMTick
enum display_States {display_display};

int displaySMTick(int state){
    static unsigned char output;
    switch(state){
        case display_display: state = display_display; break;
        default: state = display_display; break;
    }

    switch(state){
        case display_display:
            if(lock){
                PORTB = 0x01;
            } else {
                PORTB = 0x00;
            }
            //PORTB = output;
            break;
    }
    return state;
}

//displaySMTick
enum button_States {button_wait};

int ButtonSMTick(int state){
    switch(state){
        case button_wait: state = button_wait; break;
        default: state = button_wait; break;
    }

    switch(state){
        case button_wait:
            if(((~PINB) & 0x80) == 0x80){
                lock = 0x00;
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
    DDRB = 0x7F; PORTB = 0x80;
    DDRC = 0xF0; PORTC = 0x0F;

    static task task1, task2, task3, task4;
    task *tasks[] = { &task1, &task2, &task3};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    tasks[0]->state = start;
    tasks[0]->period = 50;
    tasks[0]->elapsedTime = tasks[0]->period;
    tasks[0]->TickFct = &enterSMTick;

    tasks[1]->state = start;
    tasks[1]->period = 50;
    tasks[1]->elapsedTime = tasks[1]->period;
    tasks[1]->TickFct = &displaySMTick;

    tasks[2]->state = start;
    tasks[2]->period = 50;
    tasks[2]->elapsedTime = 50;
    tasks[2]->TickFct = &ButtonSMTick;

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
