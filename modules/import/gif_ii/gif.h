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
 * The Initial Developer of the Original Code is
 * Christian Eyrich
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/*--------- Logical Screen Descriptor -------*/
typedef struct 
{
	unsigned short width;
	unsigned short height;
	unsigned char packed_fields;
	unsigned char background_color;
	unsigned char aspect_ratio;
} LSCRDES;

/*--------- Graphic Control Extension -------*/
typedef struct 
{
	unsigned char packed_fields;
	unsigned short delay_time;
	unsigned char transparent_color;
} GC_EXTENSION;

/*--------- Comment Extension -------*/
typedef struct 
{
	short commentlength;
	char *comment;
} COM_EXTENSION;

/*---------- Image Descriptor -------*/
typedef struct 
{
	unsigned char separator;
	unsigned short left_pos;
	unsigned short top_pos;
	unsigned short image_width;
	unsigned short image_height;
	unsigned char packed_fields;
} IMAGE_DES;


/*------------ Color Table ----------*/
typedef struct
{
	short depth;
	short size;
	unsigned char rgb_values[256 * 3];
} COLTAB;
