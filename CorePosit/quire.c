//
//  quire.c
//  CorePosit
//
//  Created by CAILLOT Antoine on 31/01/2021.
//

#include "quire.h"
#include "posit.h"
#include <stdio.h>
#include <string.h>

void QSL(Quire *q, unsigned int n)
{
    uint8_t buff[Q_SIZE] = {0};
    uint16_t A = 0;
    unsigned int i;
    
    unsigned int bigShift = n / 8, smallShift = n % 8;
    
    for(i=(Q_SIZE-1); i>bigShift; i--) // move bytes
    {
        buff[i] = q->reg[i-bigShift];
    }
    
    for(i=(Q_SIZE-1); i>0; i--) // shift the bits
    {
        if(i == (Q_SIZE-1)) // trash MSB
        {
            buff[i] <<= smallShift;
        }
        else
        {
            A = buff[i];
            A <<= smallShift; // uses q 16bits register to perform the action
            buff[i+1] |= (uint8_t) ((A & 0xff00) >> 8); // apply the upper bytes
            buff[i] = (uint8_t) (A & 0x00ff); // apply the lower bytes
        }
    }
    
    memcpy(q->reg, buff, Q_SIZE);
}

void QSR(Quire *q, unsigned int n)
{
    uint8_t buff[Q_SIZE] = {0};
    uint16_t A = 0;
    unsigned int i;
    
    unsigned int bigShift = n / 8, smallShift = n % 8;
    
    for(i=0; i<Q_SIZE-bigShift; i++) // move bytes
    {
        buff[i] = q->reg[i+bigShift];
    }
    
    for(i=0; i<Q_SIZE; i++) // shift the bits
    {
        if(i == 0) // trash the LSB
        {
            buff[i] >>= smallShift;
        }
        else
        {
            A = buff[i] << 8;
            A >>= smallShift; // uses q 16bits register to perform the action
            buff[i] = (uint8_t) ((A & 0xff00) >> 8); // apply the upper bytes
            buff[i-1] |= (uint8_t) (A & 0x00ff); // apply the lower bytes
        }
    }
    
    memcpy(q->reg, buff, Q_SIZE);
}

void QADD(Quire *qA, Quire *qB)
{
    uint8_t buff[Q_SIZE] = {0};
    uint16_t A = 0, B = 0, C = 0;
    unsigned int i;
    
    for(i=0; i<Q_SIZE; i++)
    {
        A = qA->reg[i];
        B = qB->reg[i];
        A += B + C;
        C = A >> 8;
        buff[i] = (uint8_t) (A & 0x00ff);
    }
    memcpy(qA->reg, buff, Q_SIZE);
}

void QNEG(Quire *q)
{
    Quire one;
    QINV(q);
    QZERO(&one);
    one.reg[0] = 0x01;
    QADD(q, &one);
}

void QZERO(Quire *q)
{
    unsigned int i;
    for(i=0; i<Q_SIZE; i++)
        q->reg[i] = 0x00;
}

void QINV(Quire *q)
{
    uint8_t buff[Q_SIZE];
    unsigned int i;
    for(i=0; i < Q_SIZE; i++)
        buff[i] = ~q->reg[i];
    memcpy(q->reg, buff, Q_SIZE);
}

void QSUB(Quire *qA, Quire *qB)
{
    Quire negB;
    memcpy(&negB, qB, Q_SIZE);
    QNEG(&negB);
    QADD(qA, &negB);
}
