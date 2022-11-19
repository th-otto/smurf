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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

*******************************************************************
*                           DITMACRO.S
*
*   Makrodefinitionen fr die Assembler-Ditherroutinen
*
*   05.10.96 Olaf   
*
*******************************************************************



*-------------------------------------------------------------
*   Busybox-Aufruf
*-------------------------------------------------------------

	/* smurf_struct->services->busybox(128 - (int) ((y << 7) / height)) */
	.MACRO   _Busybox

    movem.l d0-d2/a0-a1,-(sp)  /*  register retten */
    move.l busybox,a0      /* busybox-adresse */
    moveq #0,d1
    move.w 20+4(sp),d1        /* Wert... */
    lsl.l #7,d1            /* ...ausrechnen */
    move.w height,d0
    divu.w d0,d1
    move.w #128,d0
    sub.w d1,d0
    jsr (a0)                    /* busybox */
    movem.l (sp)+,d0-d2/a0-a1  /*  und regs zurck */

	.ENDM
