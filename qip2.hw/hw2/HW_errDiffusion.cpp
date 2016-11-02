void gammaCorrect(ImagePtr, double, ImagePtr);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//
void
HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{
    ImagePtr Itmp;
	IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
    gammaCorrect(I1, gamma, Itmp);

	int i, x, y, e, type;
    ChannelPtr<uchar> p1, p2;
    int padding = method + 1;
    int thr = MXGRAY/2;

    short *buf[3];
    buf[0] = new short[w + 4];
    buf[1] = new short[w + 4];
    buf[2] = new short[w + 4];
    short* row0 = buf[0] + 2;
    short* row1 = buf[1] + 2;
    short* row2 = buf[2] + 2;

	double weights[4];
	if (!method){
		weights[0] = 7 / 16.;
		weights[1] = 5 / 16.;
		weights[2] = 3 / 16.;
		weights[3] = 1 / 16.;
	}
	else{
		weights[0] = 7 / 48.;
		weights[1] = 5 / 48.;
		weights[2] = 3 / 48.;
		weights[3] = 1 / 48.;
	}


    for(int ch = 0; IP_getChannel(Itmp, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        // Initialize circ buffer
        for (i=0; i<w; ++i) row0[i] = *p1++;
        for (i=0; i<w; ++i) row1[i] = *p1++;
        if (method){
            for (i=0; i<w; ++i) row2[i] = *p1++;
        }

        for (y=0; y<h; ++y){
            // Compute error
            for (x=0; x<w; ++x){
                if (*row0 < thr) *p2 = 0;
                else *p2 = MaxGray;
                e = *row0 - *p2;
				if (!method){
                    row0[1]   += e * weights[0];
                    row1[x-1] += e * weights[2];
                    row1[x]   += e * weights[1];
                    row1[x+1] += e * weights[3];
                }
                else{
                    row0[1]   += e * weights[0];
                    row0[2]   += e * weights[1];
                    row1[x-2] += e * weights[2];
                    row1[x-1] += e * weights[1];
                    row1[x]   += e * weights[0];
                    row1[x+1] += e * weights[1];
                    row1[x+2] += e * weights[2];
                    row2[x-2] += e * weights[3];
                    row2[x-1] += e * weights[2];
                    row2[x]   += e * weights[1];
                    row2[x+1] += e * weights[2];
                    row2[x+2] += e * weights[3];
                }
                *row0++;
                *p2++;
            }
            //Advance Circular buffer
            row0 = row1;
            if (!method){
                row1 = buf[y%2] + padding;
				for (i = 0; i < w; ++i) row1[i] = *p1++;
            }
            else{
                row1 = row2;
                row2 = buf[y%3] + padding;
				for (i = 0; i < w; ++i) row2[i] = *p1++;
            }
        }
    }
    delete[] buf[0];
    delete[] buf[1];
    delete[] buf[2];
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// gammaCorrect:
//
// Apply gamma correction to image I1.
// Save result in I2.
//
void
gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;

	// init lookup table
	int i, lut[MXGRAY];
	for(i=0; i<MXGRAY; ++i) 
        lut[i] = (int) (MaxGray * pow((double) i/MaxGray, (1/gamma)));

	// evaluate output: each input pixel indexes into lut[] to eval output
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
	}
}
