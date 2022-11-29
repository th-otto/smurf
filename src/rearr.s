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

*
*           Rearrange Line
*   Zum Ausschneiden und Freistellen von Blîcken
*   aus Standardformatbildern. Eine Bildzeile mit d0
*   Bytes wird so umgebaut, daû das erste Pixel, welches
*   in den Planes bei Bit d1 liegt, ganz nach links geschoben
*   wird. Die anderen Pixel werden nachgerÅckt.
*
*   a0: Quelladresse
*   a1: Zieladresse
*   d0: LÑnge der zu wandelnden Zeile
*   d1: Anzahl an Pixeln, um die das Bild nach links wandern soll
*

	.IFNE 0
	.globl rearrange_line
rearrange_line:

    subq    #1,d0

loop:
*   move.w  (a0),d2         /* Word(!) holen */
    move.b  (a0),d2
    lsl.w   #8,d2
    move.b  1(a0),d2
    
    lsr.w   d1,d2           /* nach rechts shiften */

    move.b  d2,(a0)+        /* Low-Byte(!) zurÅckschreiben */

    dbra    d0,loop

    rts
	.ENDC

/*
 * void rearrange_line2(char *src, char *dst, long bytes, unsigned int pixels)
 */
	.IFEQ PURE_C
/* gcc cdecl entry point */
	.globl _rearrange_line2
_rearrange_line2:
	move.l 4(a7),a0
	move.l 8(a7),a1
	move.l 12(a7),d0
#ifndef __MSHORT__
	move.w 18(a7),d1
#else
	move.w 16(a7),d1
#endif
	move.l d2,-(a7)
	.ENDC

	.globl rearrange_line2
rearrange_line2:

    subq    #1,d0

loop2:
    move.b  (a0)+,d2
    lsl.w   #8,d2
    move.b  (a0),d2
    
    lsr.w   d1,d2           /* nach rechts shiften */

    move.b  d2,(a1)+        /* Low-Byte(!) zurÅckschreiben */

    dbra    d0,loop2

	.IFEQ PURE_C
	move.l (a7)+,d2
	.ENDC

    rts
