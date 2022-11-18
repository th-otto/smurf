/*
 * ***** BEGIN LICENSE BLOCK *****
 *
 * The contents of this file are subject to the GNU General Public License
 * Version 2 (the "License"); you may not use this file except in compliance
 * with the GPL. You may obtain a copy of the License at
 * http://www.gnu.org/copyleft/gpl.html or the file GPL.TXT from the program
 * or source code package.
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
 * the specific language governing rights and limitations under the GPL.
 *
 * The Original Code is Therapy Seriouz Software code.
 *
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

* Pixel Packed to Interleaved Bitplane - Transformer
* fÅr SMURF
* a0=16 Bytes Buffer, Bilddaten
* a1= Ziel-Adresse
* d0= Anzahl Bitplanes
*
* 13.11.95 Olaf Piesche
*


*****************************************************************
* 16 Packed-Pixel aus a0 ins Standardformat nach a1 schreiben   *
* in d1 planes                                                  *
* void setpix162(char *source, char *paddr, int num_plane, long bitplanelen);
*****************************************************************
/* gcc cdecl entry point */
	.IFEQ PURE_C
	.globl _setpix162
_setpix162:
	move.l 4(a7),a0
	move.l 8(a7),a1
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.l 16(a7),d1
#else
	move.w 12(a7),d0
	move.l 14(a7),d1
#endif
	.ENDC

	.globl setpix162
setpix162:
    movem.l     d2-d5,-(sp)        /* save regs */
    moveq   #16,d4
    subq.w  #1,d0
    moveq   #0,d5

loopplane2:
    moveq #0,d3        /* Word to write */
    moveq #15,d2       /* Bit val */

    loopp:
        btst.b  d5,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
        beq.b   loopp1         /* nope! */
        bset.l  d2,d3       /* yep! set d6th Bit in d7! */
    loopp1:
        dbra    d2,loopp

    suba.l d4,a0           /* goto buffer start */
    move.w  d3,(a1)     /* write finished word. */
    adda.l d1,a1       /* next plane */
    addq.w #1,d5

    dbra d0,loopplane2     /* do this for #d0 Planes. */


	movem.l (sp)+,d2-d5
	rts
