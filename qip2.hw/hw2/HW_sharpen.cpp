extern void HW_blur  (ImagePtr, int, int, ImagePtr);
extern void HW_blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//
void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;

    HW_blur(I1, size, size, I2);

    int type, value;
    ChannelPtr<uchar> p1, p2, endd;

	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;){
            value = factor * ((int)*p1 - *p2) + *p1;
            *p2++ = CLIP(value, 0, MaxGray);
            *p1++;
        }
	}
}
