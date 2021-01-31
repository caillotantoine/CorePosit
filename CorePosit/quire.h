//
//  quire.h
//  CorePosit
//
//  Created by CAILLOT Antoine on 31/01/2021.
//

#ifndef quire_h
#define quire_h

#include <stdio.h>
#include <string.h>

#include "posit.h"

void QSL(Quire *q, unsigned int n); // shift the quire of n bits to the left
void QSR(Quire *q, unsigned int n);
void QADD(Quire *qA, Quire *qB); // qA = qA + qB
void QSUB(Quire *qA, Quire *qB); // qA = qA - qB
void QZERO(Quire *q); // qA = 0
void QNEG(Quire *q); // qA = -qA
void QINV(Quire *q); // qA = ~qA

#endif /* quire_h */

