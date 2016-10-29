void blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
    if (filterW % 2 == 0) filterW++;
    if (filterH % 2 == 0) filterH++;
    
    IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();

	int type, i;
	ChannelPtr<uchar> p1, p2;

    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
        // blur1D for row        
        if (filterW > 1){
            for (i=0; i<h; ++i){
                blur1D(p1+w*i, w, 1, filterW, p2+w*i);
            }
        }
        else IP_copyChannel(I1, ch, I2, ch);
        // blur1D for column
        if (filterH > 1){
            for (i=0; i<w; ++i){
                blur1D(p2+i, h, w, filterH, p2+i);
            }
        }
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur1D:
//
// Blur src with 1D box filter of width ww.
//
void
blur1D(ChannelPtr<uchar> src, int len, int stride, int ww, ChannelPtr<uchar> dst)
{
    int i;
    int sum = 0;
    int buffer_length = len + ww - 1;
    int midpoint = ww/2;
	int firstpixel = src[0];
	int lastpixel = src[(len - 1)*stride];
	uchar * buf = new uchar[buffer_length];


	for (i = 0; i < midpoint     ; ++i) buf[i] = firstpixel;
    for (     ; i<len+midpoint   ; ++i) buf[i] = src[(i-midpoint)*stride];
	for (     ; i < buffer_length; ++i) buf[i] = lastpixel;

    for (i=0; i<ww; i++) sum += buf[i];
	dst[0] = sum / ww;
    for (i=1; i<len; i++){
        sum += (buf[i+ww-1] - buf[i-1]);
		dst[i*stride] = sum / ww;
    }
	delete[] buf;
}
