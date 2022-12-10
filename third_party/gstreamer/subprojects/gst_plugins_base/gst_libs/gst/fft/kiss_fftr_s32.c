/*
 *  Copyright (c) 2003-2004, Mark Borgerding. All rights reserved.
 *  This file is part of KISS FFT - https://github.com/mborgerding/kissfft
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  See COPYING file for more information.
 */

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/kiss_fftr_s32.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/_kiss_fft_guts_s32.h"

struct kiss_fftr_s32_state
{
  kiss_fft_s32_cfg substate;
  kiss_fft_s32_cpx *tmpbuf;
  kiss_fft_s32_cpx *super_twiddles;
#ifdef USE_SIMD
  void *pad;
#endif
};

kiss_fftr_s32_cfg
kiss_fftr_s32_alloc (int nfft, int inverse_fft, void *mem, size_t * lenmem)
{
  int i;
  kiss_fftr_s32_cfg st = NULL;
  size_t subsize = 0, memneeded;

  g_return_val_if_fail ((nfft & 1) == 0, NULL);
  nfft >>= 1;

  kiss_fft_s32_alloc (nfft, inverse_fft, NULL, &subsize);
  memneeded =
      ALIGN_STRUCT (sizeof (struct kiss_fftr_s32_state)) +
      ALIGN_STRUCT (subsize) + sizeof (kiss_fft_s32_cpx) * (nfft * 3 / 2);

  if (lenmem == NULL) {
    st = (kiss_fftr_s32_cfg) KISS_FFT_S32_MALLOC (memneeded);
  } else {
    if (*lenmem >= memneeded)
      st = (kiss_fftr_s32_cfg) mem;
    *lenmem = memneeded;
  }
  if (!st)
    return NULL;

  st->substate = (kiss_fft_s32_cfg) (((char *) st) + ALIGN_STRUCT (sizeof (struct kiss_fftr_s32_state)));       /*just beyond kiss_fftr_s32_state struct */
  st->tmpbuf =
      (kiss_fft_s32_cpx *) (((char *) st->substate) + ALIGN_STRUCT (subsize));
  st->super_twiddles = st->tmpbuf + nfft;
  kiss_fft_s32_alloc (nfft, inverse_fft, st->substate, &subsize);

  for (i = 0; i < nfft / 2; ++i) {
    double phase =
        -3.14159265358979323846264338327 * ((double) (i + 1) / nfft + .5);
    if (inverse_fft)
      phase *= -1;
    kf_cexp (st->super_twiddles + i, phase);
  }
  return st;
}

void
kiss_fftr_s32 (kiss_fftr_s32_cfg st, const kiss_fft_s32_scalar * timedata,
    kiss_fft_s32_cpx * freqdata)
{
  /* input buffer timedata is stored row-wise */
  int k, ncfft;
  kiss_fft_s32_cpx fpnk, fpk, f1k, f2k, tw, tdc;

  g_return_if_fail (!st->substate->inverse);

  ncfft = st->substate->nfft;

  /*perform the parallel fft of two real signals packed in real,imag */
  kiss_fft_s32 (st->substate, (const kiss_fft_s32_cpx *) timedata, st->tmpbuf);
  /* The real part of the DC element of the frequency spectrum in st->tmpbuf
   * contains the sum of the even-numbered elements of the input time sequence
   * The imag part is the sum of the odd-numbered elements
   *
   * The sum of tdc.r and tdc.i is the sum of the input time sequence. 
   *      yielding DC of input time sequence
   * The difference of tdc.r - tdc.i is the sum of the input (dot product) [1,-1,1,-1... 
   *      yielding Nyquist bin of input time sequence
   */

  tdc.r = st->tmpbuf[0].r;
  tdc.i = st->tmpbuf[0].i;
  C_FIXDIV (tdc, 2);
  CHECK_OVERFLOW_OP (tdc.r, +, tdc.i);
  CHECK_OVERFLOW_OP (tdc.r, -, tdc.i);
  freqdata[0].r = tdc.r + tdc.i;
  freqdata[ncfft].r = tdc.r - tdc.i;
#ifdef USE_SIMD
  freqdata[ncfft].i = freqdata[0].i = _mm_set1_ps (0);
#else
  freqdata[ncfft].i = freqdata[0].i = 0;
#endif

  for (k = 1; k <= ncfft / 2; ++k) {
    fpk = st->tmpbuf[k];
    fpnk.r = st->tmpbuf[ncfft - k].r;
    fpnk.i = -st->tmpbuf[ncfft - k].i;
    C_FIXDIV (fpk, 2);
    C_FIXDIV (fpnk, 2);

    C_ADD (f1k, fpk, fpnk);
    C_SUB (f2k, fpk, fpnk);
    C_MUL (tw, f2k, st->super_twiddles[k - 1]);

    freqdata[k].r = HALF_OF (f1k.r + tw.r);
    freqdata[k].i = HALF_OF (f1k.i + tw.i);
    freqdata[ncfft - k].r = HALF_OF (f1k.r - tw.r);
    freqdata[ncfft - k].i = HALF_OF (tw.i - f1k.i);
  }
}

void
kiss_fftri_s32 (kiss_fftr_s32_cfg st, const kiss_fft_s32_cpx * freqdata,
    kiss_fft_s32_scalar * timedata)
{
  /* input buffer timedata is stored row-wise */
  int k, ncfft;

  g_return_if_fail (st->substate->inverse);

  ncfft = st->substate->nfft;

  st->tmpbuf[0].r = freqdata[0].r + freqdata[ncfft].r;
  st->tmpbuf[0].i = freqdata[0].r - freqdata[ncfft].r;
  C_FIXDIV (st->tmpbuf[0], 2);

  for (k = 1; k <= ncfft / 2; ++k) {
    kiss_fft_s32_cpx fk, fnkc, fek, fok, tmp;
    fk = freqdata[k];
    fnkc.r = freqdata[ncfft - k].r;
    fnkc.i = -freqdata[ncfft - k].i;
    C_FIXDIV (fk, 2);
    C_FIXDIV (fnkc, 2);

    C_ADD (fek, fk, fnkc);
    C_SUB (tmp, fk, fnkc);
    C_MUL (fok, tmp, st->super_twiddles[k - 1]);
    C_ADD (st->tmpbuf[k], fek, fok);
    C_SUB (st->tmpbuf[ncfft - k], fek, fok);
#ifdef USE_SIMD
    st->tmpbuf[ncfft - k].i *= _mm_set1_ps (-1.0);
#else
    st->tmpbuf[ncfft - k].i *= -1;
#endif
  }
  kiss_fft_s32 (st->substate, st->tmpbuf, (kiss_fft_s32_cpx *) timedata);
}
