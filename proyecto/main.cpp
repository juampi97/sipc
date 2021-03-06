#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;



int main(int argc, char** argv)
{

	Mat frame, bgmask, out_frame;



	//Abrimos la webcam

	VideoCapture cap;
	cap.open(0);
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la c�mara\n");
		return -1;
	}
        int cont = 0;
        while (frame.empty()&& cont < 2000 ) {

                cap >> frame;
                ++cont;
        }
        if (cont >= 2000) {
                printf("No se ha podido leer un frame v�lido\n");
                exit(-1);
        }

	// Creamos las ventanas que vamos a usar en la aplicaci�n

	namedWindow("Reconocimiento");
	namedWindow("Fondo");

        // creamos el objeto para la substracci�n de fondo
	MyBGSubtractorColor back(cap);
	// creamos el objeto para el reconocimiento de gestos
	HandGesture rec_gesto;
	// iniciamos el proceso de obtenci�n del modelo del fondo
	back.LearnModel();

	for (;;)
	{
		cap >> frame;
		//flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Le�do frame vac�o\n");
			continue;
		}
		int c = cvWaitKey(40);
		if ((char)c == 'q') break;

		// obtenemos la m�scara del fondo con el frame actual
		back.ObtainBGMask(frame,bgmask);
                // CODIGO 2.1
                // limpiar la m�scara del fondo de ruido
                //...
		int dilat = 1;
                Mat element = getStructuringElement(MORPH_RECT,Size(2 * dilat + 1, 2 * dilat + 1), Point(dilat, dilat));

		erode(bgmask,dst,element);
		dilate(dst,bgmask,element);
		medianBlur(bgmask,bgmask,5);

		// deteccion de las caracter�sticas de la mano
		rec_gesto.FeaturesDetection(bgmask,frame);
                // mostramos el resultado de la sobstracci�n de fondo

                // mostramos el resultado del reconocimento de gestos

		imshow("Reconocimiento", frame);
		imshow("Fondo",bgmask);


	}

	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	cap.release();
	return 0;
}
