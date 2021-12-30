/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *                 Jehan <jehan@girinstud.io>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsSBCharSetProber.h"

/********* Language model for: Turkish *********/

/**
 * Generated by BuildLangModel.py
 * On: 2016-05-04 11:05:14.017802
 **/

/* Character Mapping Table:
 * ILL: illegal character.
 * CTR: control character specific to the charset.
 * RET: carriage/return.
 * SYM: symbol (punctuation) that does not belong to word.
 * NUM: 0 - 9.
 *
 * Other characters are ordered by probabilities
 * (0 is the most common character in the language).
 *
 * Orders are generic to a language. So the codepoint with order X in
 * CHARSET1 maps to the same character as the codepoint with the same
 * order X in CHARSET2 for the same language.
 * As such, it is possible to get missing order. For instance the
 * ligature of 'o' and 'e' exists in ISO-8859-15 but not in ISO-8859-1
 * even though they are both used for French. Same for the euro sign.
 */
static unsigned char Latin9CharToOrderMap[] =
{
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,RET,CTR,CTR,RET,CTR,CTR, /* 0X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 1X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* 2X */
  NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,SYM,SYM,SYM,SYM,SYM,SYM, /* 3X */
  SYM,  0, 15, 19,  7,  1, 27, 21, 18,  6, 28,  9,  4, 11,  3, 13, /* 4X */
   23, 31,  5, 10,  8, 12, 20, 29, 30, 14, 22,SYM,SYM,SYM,SYM,SYM, /* 5X */
  SYM,  0, 15, 19,  7,  1, 27, 21, 18,  2, 28,  9,  4, 11,  3, 13, /* 6X */
   23, 31,  5, 10,  8, 12, 20, 29, 30, 14, 22,SYM,SYM,SYM,SYM,CTR, /* 7X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 8X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 9X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* AX */
  SYM,SYM,SYM,SYM,SYM, 50,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* BX */
   45, 38, 33, 43, 39, 51, 46, 24, 47, 36, 52, 48, 53, 32, 35, 54, /* CX */
   25, 40, 55, 37, 49, 56, 26,SYM, 42, 57, 41, 34, 16,  2, 17, 44, /* DX */
   45, 38, 33, 43, 39, 58, 46, 24, 47, 36, 59, 48, 60, 32, 35, 61, /* EX */
   25, 40, 62, 37, 49, 63, 26,SYM, 42, 64, 41, 34, 16,  6, 17, 65, /* FX */
};
/*X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF */

static unsigned char Latin3CharToOrderMap[] =
{
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,RET,CTR,CTR,RET,CTR,CTR, /* 0X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 1X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* 2X */
  NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,SYM,SYM,SYM,SYM,SYM,SYM, /* 3X */
  SYM,  0, 15, 19,  7,  1, 27, 21, 18,  6, 28,  9,  4, 11,  3, 13, /* 4X */
   23, 31,  5, 10,  8, 12, 20, 29, 30, 14, 22,SYM,SYM,SYM,SYM,SYM, /* 5X */
  SYM,  0, 15, 19,  7,  1, 27, 21, 18,  2, 28,  9,  4, 11,  3, 13, /* 6X */
   23, 31,  5, 10,  8, 12, 20, 29, 30, 14, 22,SYM,SYM,SYM,SYM,CTR, /* 7X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 8X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 9X */
  SYM, 66,SYM,SYM,SYM,ILL, 67,SYM,SYM,  2, 17, 25, 68,SYM,ILL, 69, /* AX */
  SYM, 70,SYM,SYM,SYM,SYM, 71,SYM,SYM,  6, 17, 25, 72,SYM,ILL, 73, /* BX */
   45, 38, 33,ILL, 39, 74, 75, 24, 47, 36, 76, 48, 77, 32, 35, 78, /* CX */
  ILL, 40, 79, 37, 49, 80, 26,SYM, 81, 82, 41, 34, 16, 83, 84, 44, /* DX */
   45, 38, 33,ILL, 39, 85, 86, 24, 47, 36, 87, 48, 88, 32, 35, 89, /* EX */
  ILL, 40, 90, 37, 49, 91, 26,SYM, 92, 93, 41, 34, 16, 94, 95,SYM, /* FX */
};
/*X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF */


/* Model Table:
 * Total sequences: 878
 * First 512 sequences: 0.9932381051376177
 * Next 512 sequences (512-1024): 0.00676189486238231
 * Rest: 1.3010426069826053e-17
 * Negative sequences: TODO
 */
static char TurkishLangModel[] =
{
  3,3,3,3,3,3,2,3,3,3,3,3,3,2,3,3,2,3,3,3,3,3,3,3,3,3,0,3,2,3,3,0,0,0,0,0,
  3,3,3,3,3,3,0,3,3,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,3,3,0,3,3,3,3,2,0,0,0,0,
  3,3,2,3,3,3,0,3,3,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,3,3,0,3,2,2,2,2,0,0,0,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,2,3,0,2,3,2,2,0,0,2,0,2,0,
  3,3,3,3,3,2,3,3,3,3,3,3,3,3,3,3,3,0,3,3,3,3,2,3,3,0,2,3,2,2,0,0,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,2,3,3,2,2,2,3,0,0,2,
  2,2,0,3,3,3,3,3,3,3,3,3,0,2,3,2,0,3,2,3,2,2,3,3,3,3,0,2,0,0,2,0,0,0,0,0,
  3,3,3,2,3,3,3,3,2,2,2,2,3,3,3,2,3,0,2,2,2,2,2,0,0,0,3,2,2,3,0,0,2,0,0,2,
  3,3,3,2,3,3,3,2,3,3,3,3,3,3,3,3,3,0,3,2,3,2,3,2,3,0,3,3,2,2,0,0,2,0,0,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,2,3,2,2,2,2,2,3,0,3,2,0,2,0,0,0,3,3,0,
  3,3,3,3,3,3,3,2,3,3,3,3,3,3,3,3,3,0,3,3,3,2,2,3,3,0,3,3,2,3,0,0,2,0,0,0,
  3,3,3,2,3,3,3,3,2,2,3,3,3,3,3,3,3,2,3,3,0,2,3,3,0,0,3,2,0,2,2,0,2,2,0,2,
  3,3,3,3,3,3,0,3,3,3,3,3,2,2,3,3,0,3,3,3,3,3,3,3,3,3,0,3,0,2,2,0,0,0,0,0,
  3,3,3,3,3,3,0,3,3,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,3,3,0,3,3,3,2,0,0,0,0,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,2,3,2,3,3,2,0,2,0,3,2,0,2,0,0,0,0,0,0,
  3,3,3,3,3,3,3,3,2,2,2,2,3,3,2,2,3,0,2,2,2,0,2,0,0,0,3,0,2,2,0,0,0,0,0,0,
  0,2,0,3,3,3,0,3,3,3,3,3,0,2,3,3,0,3,2,3,3,0,3,3,3,3,2,3,0,0,0,0,0,0,0,0,
  3,3,3,2,3,2,3,2,3,3,2,3,3,2,0,3,3,0,2,2,2,3,0,2,3,0,2,2,0,0,0,0,0,0,2,0,
  3,3,3,3,3,3,3,2,3,2,3,3,3,3,3,3,3,0,0,0,2,2,2,2,3,0,0,2,0,2,0,0,0,2,0,2,
  3,3,3,2,3,3,3,2,3,3,0,2,3,3,2,2,3,0,3,2,0,2,2,0,0,0,0,2,0,0,0,2,2,0,0,0,
  3,3,3,2,3,3,2,2,0,2,2,2,3,3,3,0,2,2,0,2,3,2,2,0,0,0,0,2,0,0,0,0,2,0,0,0,
  3,3,3,2,3,3,3,2,2,2,3,2,3,3,2,2,3,0,3,0,0,2,0,2,0,0,3,2,0,2,0,0,0,2,0,0,
  3,3,3,0,3,2,3,3,2,2,2,3,3,3,3,2,3,0,2,3,0,3,2,0,0,0,2,0,0,2,0,0,0,0,0,0,
  3,3,3,2,3,3,3,2,3,2,3,3,3,3,2,3,2,0,3,0,0,2,2,3,2,0,0,2,0,0,0,0,0,0,0,0,
  3,3,3,0,3,3,3,2,3,2,0,3,2,3,2,2,3,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,
  3,3,3,0,3,3,3,3,0,0,0,3,3,2,0,2,3,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,3,3,3,0,3,3,3,3,3,0,0,3,2,0,0,2,2,2,2,3,2,3,3,0,2,0,0,0,0,0,0,0,0,
  3,3,3,0,3,3,3,0,3,2,2,0,3,3,3,0,2,0,0,2,0,3,2,2,2,0,2,3,0,0,0,0,0,0,0,2,
  3,3,3,0,2,0,2,0,0,0,0,0,3,3,2,0,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,
  3,3,3,2,2,2,0,0,2,2,2,0,2,3,2,0,2,0,2,0,0,0,0,0,0,0,0,2,0,2,0,0,0,0,0,0,
  2,2,2,2,2,0,0,0,2,0,0,0,0,2,0,2,0,0,2,0,2,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,
  0,0,2,0,0,0,0,0,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  2,0,0,2,0,0,0,0,0,0,2,0,0,2,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,2,2,2,0,0,2,2,0,2,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,2,0,2,0,0,2,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


SequenceModel Latin9_TurkishModel =
{
  Latin9CharToOrderMap,
  TurkishLangModel,
  36,
  (float)0.9932381051376177,
  PR_FALSE,
  "ISO-8859-9"
};

SequenceModel Latin3_TurkishModel =
{
  Latin3CharToOrderMap,
  TurkishLangModel,
  36,
  (float)0.9932381051376177,
  PR_FALSE,
  "ISO-8859-3"
};
