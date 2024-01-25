#ifndef soloud_lowpass_shelv_h
#define soloud_lowpass_shelv_h

#include "soloud.h"

namespace SoLoud {

    // Clase filtro
    class LowPassShelv;

    // Clase instancia filtro
    class LowPassShelvInstance : public FilterInstance
    {
        // Buffers de memoria para las muestras de retraso
        // Son una matriz de 4 x 2: 4 muestras de retraso | 2 canales
        // x -> Muestras de retraso de entrada
        // y -> Muestras de retraso de salida
        float x[4][2];
        float y[4][2];

        // Arrays de coeficientes b y a
        // Para a hay una posición menos, ya que a0 solo se usa en el cálculo de los coeficientes
        float b[5];
        float a[4];

        // Frecuencia de muestreo
        float mSamplerate;

        // Función para calcular los coeficientes 
        void calcShelvParams(float f, int order, float gain);

        // Función filtro
        void filter(float* aBuffer, unsigned int aSamples, double aTime, unsigned int aChannel, float aSamplerate, int order);
    
        LowPassShelv *mParent;
    public:
        // Función para filtrar cada canal
        virtual void filterChannel(float* aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int /*aChannels*/);
        
        // Constructor de la instancia
        LowPassShelvInstance(LowPassShelv* aParent);

        // Destructor de la instancia
        virtual ~LowPassShelvInstance();
    };

    class LowPassShelv : public Filter {
    public:
        // Indice de cada parámetro
        enum FILTERPARAMS {
            WET = 0,
            FREQ_C,
            ORDER,
            GAIN
        };

        // Parámetros concretos del filtro (wet es común a todos)
        float mFreqC;
        int mOrder;
        float mGain;


        // Función estandar que devuelve el número de parámetros del filtro
        virtual int getParamCount();
        // Función estandar que devuelve el nombre de los parámetros del filtro
        virtual const char* getParamName(unsigned int aParamIndex);
        // Función estandar que devuelve el tipo de los parámetros del filtro
        virtual unsigned int getParamType(unsigned int aParamIndex);
        // Función estandar que devuelve el máximo de los parámetros del filtro
        virtual float getParamMax(unsigned int aParamIndex);
        // Función estandar que devuelve el mínimo de los parámetros del filtro
        virtual float getParamMin(unsigned int aParamIndex);
        // Función que devuelve una instancia de este filtro
        virtual LowPassShelvInstance* createInstance();

        // Constructor del filtro
        LowPassShelv();

        // Función para setear los parámetrsos del filtro
        result setParams(float FreqC, int order, float gain);
    };
}

#endif