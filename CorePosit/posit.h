//
//  posit.h
//  CorePosit
//
//  Created by CAILLOT Antoine on 31/01/2021.
//

#ifndef posit_h
#define posit_h

#include <stdio.h>
#include <strings.h>

#define P_SIZE  16
#define P_ES    3

#define Q_SIZE ((P_SIZE * P_SIZE) >> 1)/8

typedef struct{
    uint8_t reg[Q_SIZE];
} Quire;

#if P_SIZE == 8
typedef uint8_t Posit;
#endif
#if P_SIZE == 16
typedef uint16_t Posit;
#endif
#if P_SIZE == 32
typedef uint32_t Posit;
#endif


typedef struct {
    uint8_t sign:1,
            exp:7;
    int8_t regime;
#if P_SIZE > 16
    uint32_t frac;
#else
    uint16_t frac;
#endif
} PositDecod;

#endif /* posit_h */
