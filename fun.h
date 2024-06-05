#ifndef __MINESWEEPER_H__
#define __MINESWEEPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <locale.h>
#define _CRT_SECURE_NO_DEPRECATE
#define DELIMITER "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

union type_char {
    char number;
    struct {
        unsigned char bit1 : 1;
        unsigned char bit2 : 1;
        unsigned char bit3 : 1;
        unsigned char bit4 : 1;
        unsigned char bit5 : 1;
        unsigned char bit6 : 1;
        unsigned char bit7 : 1;
        unsigned char bit8 : 1;
    } bits;
};

typedef enum {
    ENROLLMENT,
    DROPOUT,
    REINSTATEMENT
} EventType;

typedef struct {
    int mark;
    char* discipline;
} Mark;

typedef struct {
    EventType type;
    char date[11];
} Event;

typedef struct {
    unsigned long credit_card_number;
    char* full_name;
    Mark* marks;
    int num_marks;
    Event* events;
    int num_events;
} Student;

typedef struct {
    char* discipline;
    float average;
    int count;
} DisciplineAverage;

int main();
void task1();
void task2();

#endif
