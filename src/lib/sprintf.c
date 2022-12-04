/*
 * doprnt.c
 *
 *  Created on: 29.05.2013
 *      Author: mfro
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')


struct _mem_stream { char *xstring; char *xestring; };


#define ONLY_INTEGER_IO

#define INF	32766						/* should be bigger than any field to print */

static char snil[] = "(nil)";

static int doprnt(int (*addchar) (int, void *), void *stream, const char *sfmt, va_list ap)
{
	char buf[128];
	char *bp;
	const char *f;

#ifndef ONLY_INTEGER_IO
	long double flt;
#endif
	long l;
	unsigned long u;
#ifdef STDIO_WITH_LONG_LONG
	long long ll;
	unsigned long long ull;
#endif
	int i;
	int fmt;
	int num = 0;

	f = sfmt;
	for (; *f; f++)
	{
		if (*f != '%')
		{
			/* then just out the char */
			num += (*addchar) ((unsigned char) *f, stream);
		} else
		{
			unsigned char pad = ' ';
			const char *numbers;
			int flush_left = 0;
			int f_width = 0;
			int prec = INF;
			int hash = 0;
			int do_long = 0;
			int sign = 0;

			f++;						/* skip the % */

			if (*f == '-')
			{							/* minus: flush left */
				flush_left = 1;
				f++;
			}

			if (*f == '0')
			{
				/* padding with 0 rather than blank */
				pad = '0';
				f++;
			}
			if (*f == '*')
			{
				/* field width */
				f_width = va_arg(ap, int);

				f++;
			} else if (ISDIGIT((unsigned char) *f))
			{
				f_width = atoi(f);
				while (ISDIGIT((unsigned char) *f))
					f++;				/* skip the digits */
			}

			if (*f == '.')
			{							/* precision */
				f++;
				if (*f == '*')
				{
					prec = va_arg(ap, int);

					f++;
				} else if (ISDIGIT((unsigned char) *f))
				{
					prec = atoi(f);
					while (ISDIGIT((unsigned char) *f))
						f++;			/* skip the digits */
				}
			}

			if (*f == '#')
			{							/* alternate form */
				hash = 1;
				f++;
			}

			if (*f == 'l')
			{							/* long format */
				do_long++;
				f++;
				if (*f == 'l')
				{
					do_long++;
					f++;
				}
			} else if (*f == 'L')
			{
				do_long = 1;
				f++;
			} else if (*f == 'z')
			{
				do_long = sizeof(size_t) > sizeof(int);
				f++;
			}

			fmt = (unsigned char) *f;
			bp = buf;
			switch (fmt)
			{							/* do the format */
			case 'i':
			case 'd':
				switch (do_long)
				{
				case 0:
					l = (long) (va_arg(ap, int));

					break;
#ifdef STDIO_WITH_LONG_LONG
				case 2:
					ll = va_arg(ap, long long);

					if (ll > LONG_MAX || ll < LONG_MIN)
					{					/* use 64 bit arithmetic only if needed */
						if (ll < 0)
						{
							sign = 1;
							ll = -ll;
						}
						do
						{
							*bp++ = (char) (ll % 10) + '0';
						} while ((ll /= 10) > 0);
						l = 0;
					} else
						l = (long) ll;
					break;
#endif
				case 1:
				default:
					l = va_arg(ap, long);

					break;
				}
				if (bp == buf)
				{
					if (l < 0)
					{
						sign = 1;
						l = -l;
					}
					do
					{
						*bp++ = (char) (l % 10) + '0';
					} while ((l /= 10) > 0);
				}
				if (sign)
					*bp++ = '-';
				f_width = f_width - (int) (bp - buf);
				if (!flush_left)
					while (f_width-- > 0)
						num += (*addchar) (pad, stream);
				for (bp--; bp >= buf; bp--)
					num += (*addchar) ((unsigned char) *bp, stream);
				if (flush_left)
					while (f_width-- > 0)
						(*addchar) (' ', stream);
				break;

#ifndef ONLY_INTEGER_IO
			case 'f':
			case 'e':
			case 'g':
				/* this is actually more than stupid, but does work for now */
				if (do_long)
					flt = va_arg(ap, long double);
				else
					flt = va_arg(ap, double);	/* beware: va_arg() extends float to double! */

				if (flt < 0)
				{
					sign = 1;
					flt = -flt;
				}
				{
					int quotient, remainder;

					quotient = (int) flt;
					remainder = (flt - quotient) * 10E5;

					for (i = 0; i < 6; i++)
					{
						*bp++ = (char) (remainder % 10) + '0';
						remainder /= 10;
					}
					*bp++ = '.';
					do
					{
						*bp++ = (char) (quotient % 10) + '0';
					} while ((quotient /= 10) > 0);
					if (sign)
						*bp++ = '-';
					f_width = f_width - (int) (bp - buf);
					if (!flush_left)
						while (f_width-- > 0)
							num += (*addchar) (pad, stream);
					for (bp--; bp >= buf; bp--)
						num += (*addchar) ((unsigned char) *bp, stream);
					if (flush_left)
						while (f_width-- > 0)
							(*addchar) (' ', stream);
				}
				break;
#endif

			case 'p':
				do_long = 1;
				hash = 1;
				fmt = 'x';
				/* fall through */
			case 'o':
			case 'x':
			case 'X':
			case 'u':
				numbers = "0123456789abcdef";
				switch (do_long)
				{
				case 0:
					u = va_arg(ap, unsigned int);

					break;
#ifdef STDIO_WITH_LONG_LONG
				case 2:
					ull = va_arg(ap, unsigned long long);

					if (ull > ULONG_MAX)
					{					/* use 64 bit arithmetic only if needed */
						switch (fmt)
						{
						case 'u':
						default:
							do
							{
								*bp++ = numbers[ull % 10];
							} while ((ull /= 10) > 0);
							break;
						case 'o':
							do
							{
								*bp++ = numbers[ull % 8];
							} while ((ull /= 8) > 0);
							break;
						case 'X':
							numbers = "0123456789ABCDEF";
							/* fall through */
						case 'x':
							do
							{
								*bp++ = numbers[ull % 16];
							} while ((ull /= 16) > 0);
							break;
						}
					} else
					{
						u = (unsigned long) ull;
					}
					break;
#endif
				case 1:
				default:
					u = va_arg(ap, unsigned long);
					break;
				}
				if (bp == buf)
				{
					switch (fmt)
					{
					case 'u':
					default:
						do
						{
							*bp++ = numbers[u % 10];
						} while ((u /= 10) > 0);
						break;
					case 'o':
						do
						{
							*bp++ = numbers[u % 8];
						} while ((u /= 8) > 0);
						break;
					case 'X':
						numbers = "0123456789ABCDEF";
						/* fall through */
					case 'x':
						do
						{
							*bp++ = numbers[u % 16];
						} while ((u /= 16) > 0);
						break;
					}
				}
				if (hash)
				{
					switch (fmt)
					{
					case 'X':
					case 'x':
						*bp++ = 'x';
						/* fall through */
					case 'o':
						*bp++ = '0';
						break;
					}
				}
				i = f_width - (int) (bp - buf);
				if (!flush_left)
					while (i-- > 0)
						num += (*addchar) (pad, stream);
				for (bp--; bp >= buf; bp--)
					num += (*addchar) ((unsigned char) *bp, stream);
				if (flush_left)
					while (i-- > 0)
						num += (*addchar) (' ', stream);
				break;

			case 'c':
				i = va_arg(ap, int);
				num += (*addchar) (i, stream);
				break;

			case 'S':
			case 's':
				bp = va_arg(ap, char *);

				if (!bp)
					bp = snil;
				f_width = f_width - strlen(bp);
				if (!flush_left)
					while (f_width-- > 0)
						num += (*addchar) (pad, stream);
				for (i = 0; *bp && i < prec; i++)
				{
					num += (*addchar) ((unsigned char) *bp, stream);
					bp++;
				}
				if (flush_left)
					while (f_width-- > 0)
						num += (*addchar) (' ', stream);
				break;

			case '%':
				num += (*addchar) ('%', stream);
				break;

			default:
				break;
			}
		}
	}
	return num;
}


static int __addchar(int c, void *_stream)
{
	struct _mem_stream *stream = (struct _mem_stream *)_stream;
	
	if (stream->xstring >= stream->xestring)
		stream->xstring++;
	else
		*stream->xstring++ = (char) c;

	return 1;
}


int vsprintf(char *str, const char *format, va_list va)
{
	struct _mem_stream stream;

	stream.xstring = str;
	stream.xestring = (char *)0x7fffffffl;

	doprnt(__addchar, &stream, format, va);
	*stream.xstring = '\0';

	return stream.xstring - str;
}


int sprintf(char *str, const char *format, ...)
{
	int ret;
	va_list va;
	va_start(va, format);
	
	ret = vsprintf(str, format, va);
	va_end(va);
	return ret;
}
