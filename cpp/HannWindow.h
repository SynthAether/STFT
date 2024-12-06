#ifndef _H_HANN_WINDOW_
#define _H_HANN_WINDOW_

#include <cmath>
#include <cstdio>

class HannWindow {
  int cnt = 0;
private:
    // c standard defind 
    //#define M_PI (3.14159265358979323846)::

    // MATLAB 'pi'
    const double MATLAB_pi= 3.141592653589793;
    double *hann;
    int shift_size;
    int frame_size;


public:
    inline HannWindow(int _frame_size, int _shift_size);
    inline ~HannWindow();
    // 2D
    inline void Process(double ** buf, int channels);
    // 1D - multi channel
    inline void Process(double * buf, int channels);
    // 1D - single channel 
    inline void Process(double * buf);
    // 2D
    inline void WindowWithScaling(double ** buf, int channels);
    // 1D - multi channel
    inline void WindowWithScaling(double * buf, int channels);
    // 1D - single channel 
    inline void WindowWithScaling(double * buf);
};

inline HannWindow::HannWindow(int _frame_size, int _shift_size) {
    int i;
    double tmp = 0;

    shift_size = _shift_size;
    frame_size = _frame_size;

    hann = new double[frame_size];

    if (frame_size / shift_size == 4) {
		/* Ver 1 */
	/*
	switch (frame_size / shift_size) {
	case 4:
		hann[0] = 0.0;
		for (i = 1; i < frame_size; ++i)
			hann[i] = 0.5 * (1.0 - cos(2.0 * M_PI * (double)i / (double)frame_size));
		tmp = sqrt((double)2 / 3);
		for (i = 1; i < frame_size; i++)
			hann[i] *= tmp;
		break;
	case 2:
		for (i = 0; i < frame_size; i++)
			hann[i] = sin(M_PI * (i + 0.5) / frame_size);
		break;
	}
	*/

	/* Ver 2 */
	// win = hanning(frame_size,'periodic');
		for (i = 0; i < frame_size; i++)
			hann[i] = 0.5 * (1.0 - cos(2.0 * MATLAB_pi * (i / (double)frame_size)));

		// win = win./sqrt(sum(win.^2)/shift_size);
	    /* Where does it come from?
		for (i = 0; i < frame_size; i++)
			tmp += hann[i] * hann[i];
		tmp /= shift_size;
		tmp = std::sqrt(tmp);

		for (i = 0; i < frame_size; i++)
			hann[i] /= tmp;
	    */
    }
    else if (frame_size / shift_size == 2) {
        // win = sin(pi * ([0:1:nwin-1]'+0.5) /nwin ); %1/2 shift
        for (i = 0; i < frame_size; i++)
            hann[i] = sin(MATLAB_pi * ((double)i + 0.5) / (double)frame_size);
            
    }
    // no Overlap-Save
    else if(shift_size == frame_size){
      for (int i = 0; i < frame_size; i++) {
        hann[i] = 1;
      }
    }
    else {
		printf("Error: frame_size / shift_size != 2 or 4\n");
		exit(1);
	}

    

    
}

inline HannWindow::~HannWindow() { delete[] hann; }

inline void HannWindow::Process(double **buffer,
                                   int channels) {
  cnt++;
    int i, j;
    for (i = 0; i < channels; i++) {
#pragma omp parallel for
        for (j = 0; j < frame_size; j++) {
            buffer[i][j] *= hann[j];
        }
    }
}
inline void HannWindow::Process(double *buffer,
                                   int channels) {
    int i, j;
    for (i = 0; i < channels; i++) {
#pragma omp parallel for
        for (j = 0; j < frame_size; j++) {
            buffer[i*(frame_size+2) + j] *= hann[j];
        }
    }
}

inline void HannWindow::Process(double *buffer){
    int j;
        for (j = 0; j < frame_size; j++) {
           buffer[j] *= hann[j];
        }
}

inline void HannWindow::WindowWithScaling(double **buffer,
                                   int channels) {
    int i, j;
    for (i = 0; i < channels; i++) {
#pragma omp parallel for
        for (j = 0; j < frame_size; j++) {
            buffer[i][j] *= hann[j];
            buffer[i][j] /= 32767.0;
        }
    }
}
inline void HannWindow::WindowWithScaling(double *buffer){
    int j;
        for (j = 0; j < frame_size; j++) {
            buffer[j] *= hann[j];
            buffer[j] /= 32767.0;
        }
}
inline void HannWindow::WindowWithScaling(double *buffer,
                                   int channels) {
    int i, j;
    for (i = 0; i < channels; i++) {
#pragma omp parallel for
        for (j = 0; j < frame_size; j++) {
            buffer[ i*(frame_size+2) + j] *= hann[j];
            buffer[ i*(frame_size+2) + j] /= 32767.0;
        }
    }
}

#endif
