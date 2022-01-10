/*********************************************************************/
/* Copyright 2009, 2010 The University of Texas at Austin.           */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include <stdio.h>
#include "common.h"
#include "arm_sve.h"

#ifndef UNIT
#define INV(a) (ONE / (a))
#else
#define INV(a) (ONE)
#endif

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, BLASLONG offset, FLOAT *b){

  BLASLONG i, ii, j, jj;

  FLOAT *ao;

  jj = offset;
  int js = 0;
#ifdef DOUBLE
  svint64_t index = svindex_s64(0LL, lda);
  svbool_t pn = svwhilelt_b64(js, n);
  int n_active = svcntp_b64(svptrue_b64(), pn);
#else
  svint32_t index = svindex_s32(0, lda);
  svbool_t pn = svwhilelt_b32(js, n);
  int n_active = svcntp_b32(svptrue_b32(), pn);
#endif
  do {

    ao = a;

    i = 0;
    ii = 0;
    do {

      if (ii == jj) {
        for (int j = 0; j < n_active; j++) {
          for (int k = 0; k < j; k++) {
            *(b + j * n_active + k) = *(a + k * lda + j);
          }
          *(b + j * n_active + j) = INV(*(a + j * lda + j));
        }
      }

      if (ii > jj) {
        for (int j = 0; j < n_active; j++) {
          svfloat64_t aj_vec = svld1_gather_index(pn, ao, index);
          svst1(pn, b, aj_vec);
          ao++;
        }

      }

      b += n_active * n_active;

      i += n_active;
      ii += n_active;
    } while (i < m);


    a += n_active * lda;
    jj += n_active;

    js += n_active;
#ifdef DOUBLE
    pn = svwhilelt_b64(js, n);
    n_active = svcntp_b64(svptrue_b64(), pn);
  } while (svptest_any(svptrue_b64(), pn));
#else
    pn = svwhilelt_b32(js, n);
    n_active = svcntp_b32(svptrue_b32(), pn);
  } while (svptest_any(svptrue_b32(), pn));
#endif

return 0;
}
