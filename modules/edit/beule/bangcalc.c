BANG_INTER(SMURF_PIC *bild, short mx, short my, short HOEHE, short RADIUS, short RANDS)
{
	uint8_t *bt;
	uint8_t *bf;
	uint8_t *bildto;
	short bb, hh;
	short i, j;
	long bb3;
	double rad, hrad, mrad;
	short sx = 0;
	short sy = 0;
	short ex = bild->pic_width;
	short ey = bild->pic_height;
	double bfak;

	if (mx - RADIUS > 0)
		sx = mx - RADIUS;
	if (my - RADIUS > 0)
		sy = my - RADIUS;
	if (mx + RADIUS + 1 < ex)
		ex = mx + RADIUS + 1;
	if (my + RADIUS + 1 < ey)
		ey = my + RADIUS + 1;

	bb = bild->pic_width;
	hh = bild->pic_height;
	bildto = bt = (uint8_t *)Malloc((long) (ex - sx) * (ey - sy) * 3);
	if (bildto == 0)
		return 0;

	bf = bild->pic_data;

	bb3 = bb * 3;

	rad = (double) RADIUS;
	hrad = (double) HOEHE / RADIUS;
	mrad = (double) M_PI / RADIUS;

	bfak = (double) 128 / (ey - sy);

	if (RANDS)
	{
		for (i = sy; i < ey; i++)
		{
			double yd;
			short yd2;

			if ((i & BUSYCALL) == 0)
				services->busybox((short) ((i - sy) * bfak));

			yd2 = (my - i);
			yd = (double) yd2 *yd2;

			for (j = sx; j < ex; j++)
			{
				double mm2;
				short xd2;

				xd2 = (mx - j);
				mm2 = sqrt((double) xd2 * xd2 + yd);
				if (mm2 > rad)
				{
					uint8_t *x;
					x = bf + (long) j * 3 + (long) i * bb3;

					*bt++ = *x++;
					*bt++ = *x++;
					*bt++ = *x;
				} else
				{
					double xx, yy;
					double bh;

					bh = sin(mm2 * mrad) * hrad;
					xx = j + xd2 * bh - 0.5;
					yy = i + yd2 * bh - 0.5;
					if (xx < 0 || yy < 0 || xx >= bb - 1 || yy >= hh - 1)
					{
						*bt++ = 0;
						*bt++ = 0;
						*bt++ = 0;
					} else
					{
						uint8_t *x;
						double rot, gruen, blau;
						double xx1, yy1, xx2, yy2, xy;

						xx1 = fabs(xx - trunc(xx));
						xx2 = fabs(1 - xx1);
						yy1 = fabs(yy - trunc(yy));
						yy2 = fabs(1 - yy1);

						x = bf + (long) trunc(xx) * 3 + (long) trunc(yy) * bb3;
						xy = xx2 * yy2;
						rot = (*x++) * xy;
						gruen = (*x++) * xy;
						blau = (*x++) * xy;

						xy = xx1 * yy2;
						rot += (*x++) * xy;
						gruen += (*x++) * xy;
						blau += (*x) * xy;

						x += bb3 - 5;
						xy = xx2 * yy1;
						rot += (*x++) * xy;
						gruen += (*x++) * xy;
						blau += (*x++) * xy;

						xy = xx1 * yy1;
						rot += (*x++) * xy;
						gruen += (*x++) * xy;
						blau += (*x) * xy;

						*bt++ = (uint8_t) rot;
						*bt++ = (uint8_t) gruen;
						*bt++ = (uint8_t) blau;
					}
				}
			}
		}
	} else
	{
		/* RANDS */
		for (i = sy; i < ey; i++)
		{
			double yd;
			short yd2;

			if ((i & BUSYCALL) == 0)
				services->busybox((short) ((i - sy) * bfak));

			yd2 = (my - i);
			yd = (double) yd2 *yd2;

			for (j = sx; j < ex; j++)
			{
				double mm2;
				short xd2;

				xd2 = (mx - j);
				mm2 = sqrt((double) xd2 * xd2 + yd);
				if (mm2 > rad)
				{
					uint8_t *x;
					x = bf + (long) j * 3 + (long) i * bb3;

					*bt++ = *x++;
					*bt++ = *x++;
					*bt++ = *x;
				} else
				{
					double xx, yy;
					double bh;

					bh = sin(mm2 * mrad) * hrad;
					xx = j + xd2 * bh - 0.5;
					yy = i + yd2 * bh - 0.5;
					if (xx < 0)
						xx = 0;
					if (yy < 0)
						yy = 0;
					if (xx >= bb - 1)
						xx = bb - 1.001;
					if (yy >= hh - 1)
						yy = hh - 1.001;
					{
						uint8_t *x;
						double rot, gruen, blau;
						double xx1, yy1, xx2, yy2, xy;

						xx1 = fabs(xx - trunc(xx));
						xx2 = fabs(1 - xx1);
						yy1 = fabs(yy - trunc(yy));
						yy2 = fabs(1 - yy1);

						x = bf + (long) trunc(xx) * 3 + (long) trunc(yy) * bb3;
						xy = xx2 * yy2;
						rot = (*x++) * xy;
						gruen = (*x++) * xy;
						blau = (*x++) * xy;

						xy = xx1 * yy2;
						rot += (*x++) * xy;
						gruen += (*x++) * xy;
						blau += (*x) * xy;

						x += bb3 - 5;
						xy = xx2 * yy1;
						rot += (*x++) * xy;
						gruen += (*x++) * xy;
						blau += (*x++) * xy;

						xy = xx1 * yy1;
						rot += (*x++) * xy;
						gruen += (*x++) * xy;
						blau += (*x) * xy;

						*bt++ = (uint8_t) rot;
						*bt++ = (uint8_t) gruen;
						*bt++ = (uint8_t) blau;
					}
				}
			}
		}
	}
	{
		bf = (uint8_t *) bild->pic_data + (long) sy * bb3 + (long) sx * 3;

		bt = bildto;
		for (i = sy; i < ey; i++)
		{
			memcpy(bf, bt, (long) (ex - sx) * 3);
			bt += (long) (ex - sx) * 3;
			bf += (long) bb3;
		}
	}
	Mfree(bildto);
	return -1;
}


BANG(SMURF_PIC *bild, short mx, short my, short HOEHE, short RADIUS, short RANDS)
{
	uint8_t *bt;
	uint8_t *bf;
	uint8_t *bildto;
	uint8_t null[3] = { 0, 0, 0 };
	short bb, hh;
	short i, j;
	long bb3;
	double rad, hrad, mrad;
	short sx = 0;
	short sy = 0;
	short ex = bild->pic_width;
	short ey = bild->pic_height;
	double bfak;

	if (mx - RADIUS > 0)
		sx = mx - RADIUS;
	if (my - RADIUS > 0)
		sy = my - RADIUS;
	if (mx + RADIUS + 1 < ex)
		ex = mx + RADIUS + 1;
	if (my + RADIUS + 1 < ey)
		ey = my + RADIUS + 1;

	bb = bild->pic_width;
	hh = bild->pic_height;
	bildto = bt = (uint8_t *)Malloc((long) (ex - sx) * (ey - sy) * 3);
	if (bildto == 0)
		return 0;

	bf = bild->pic_data;

	bb3 = bb * 3;

	rad = (double) RADIUS;
	hrad = (double) HOEHE / RADIUS;
	mrad = (double) M_PI / RADIUS;

	bf += (long) sy * bb3;

	bfak = (double) 128 / (ey - sy);

	if (RANDS)
	{
		for (i = sy; i < ey; i++)
		{
			double yd;
			short yd2;

			if ((i & BUSYCALL) == 0)
				services->busybox((short) ((i - sy) * bfak));

			bf += (long) sx * 3;

			yd2 = (my - i);
			yd = (double) yd2 *yd2;

			for (j = sx; j < ex; j++)
			{
				uint8_t *x;
				double mm2;
				short xd2;

				xd2 = (mx - j);
				mm2 = sqrt((double) xd2 * xd2 + yd);
				if (mm2 > rad)
				{
					*bt++ = *bf++;
					*bt++ = *bf++;
					*bt++ = *bf++;
				} else
				{
					short xx, yy;
					double bh;

					bh = sin(mm2 * mrad) * hrad;
					xx = xd2 * bh;
					yy = yd2 * bh;
					if (xx + j < 0 || xx + j >= bb || yy + i < 0 || yy + i >= hh)
					{
						x = null;
					} else
					{
						x = bf + (long) xx * 3 + (long) yy * bb3;
					}
					*bt++ = *x++;
					*bt++ = *x++;
					*bt++ = *x;
					bf += 3;
				}
			}
			bf += (long) (bb - ex) * 3;
		}
	} else
	{
		/* RANDS */
		for (i = sy; i < ey; i++)
		{
			double yd;
			short yd2;

			if ((i & BUSYCALL) == 0)
				services->busybox((short) (i * bfak));

			bf += (long) sx * 3;

			yd2 = (my - i);
			yd = (double) yd2 *yd2;

			for (j = sx; j < ex; j++)
			{
				uint8_t *x;
				double mm2;
				short xd2;

				xd2 = (mx - j);
				mm2 = sqrt((double) xd2 * xd2 + yd);
				if (mm2 > rad)
				{
					*bt++ = *bf++;
					*bt++ = *bf++;
					*bt++ = *bf++;
				} else
				{
					short xx, yy;
					double bh;

					bh = sin(mm2 * mrad) * hrad;
					xx = xd2 * bh;
					yy = yd2 * bh;
					if (xx + j < 0)
						xx = -j;
					if (xx + j >= bb)
						xx = bb - j - 1;
					if (yy + i < 0)
						yy = -i;
					if (yy + i >= hh)
						yy = hh - i - 1;
					x = bf + (long) xx * 3 + (long) yy * bb3;

					*bt++ = *x++;
					*bt++ = *x++;
					*bt++ = *x;
					bf += 3;
				}
			}
			bf += (long) (bb - ex) * 3;
		}
	}
	{
		bf = (uint8_t *) bild->pic_data + (long) sy * bb3 + (long) sx * 3;

		bt = bildto;
		for (i = sy; i < ey; i++)
		{
			memcpy(bf, bt, (long) (ex - sx) * 3);
			bt += (long) (ex - sx) * 3;
			bf += (long) bb3;
		}
	}
	Mfree(bildto);
	return -1;
}
