#ifndef soloud_peakfilter_h
#define soloud_peakfilter_h

#include "soloud.h"

namespace SoLoud{
    // Clase filtro
    class  PeakFilter;

    // Clase instancia filtro
    class  PeakFilterInstance : public FilterInstance
    {

        enum FILTERPARAMS{
            WET = 0,
            GAIN,
            Q,
            FREQ
        };
        // Buffers de memoria para las muestras de retraso
        // Son una matriz de 2 x 2: 2 muestras de retraso | 2 canales
        // x -> Muestras de retraso de entrada
        // y -> Muestras de retraso de salida
        float x[2][2];
        float y[2][2];

                // Arrays de coeficientes b y a
        // Para a hay una posición menos, ya que a0 solo se usa en el cálculo de los coeficientes
        float b[3];
        float a[2];

        // Frecuencia de muestreo
        float mSamplerate;

        // Función para calcular los coeficientes 
        void calcPeakParams(float f, float G, float Q);

        // Función filtro
        void filter(float* aBuffer, unsigned int aSamples, double aTime, unsigned int aChannel, float aSamplerate);

        PeakFilter *mParent;
    public:

        // Función para filtrar cada canal
        virtual void filterChannel(float* aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int /*aChannels*/);
        
        // Constructor de la instancia
        PeakFilterInstance( PeakFilter * aParent);

        // Destructor de la instancia
        virtual ~PeakFilterInstance();
    };
    
    class PeakFilter : public Filter{
        
    public:
        // Indice de cada parámetro
        enum FILTERPARAMS{
            WET = 0,
            GAIN,
            Q,
            FREQ
        };

        // Parámetros concretos del filtro (wet es común a todos)
        float mGain;
        float mQ;
        float mFreq;
        //RELLENAR CON DECLARACIÓN DE LOS PARÁMETROS


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

        
        virtual PeakFilterInstance *createInstance();
        PeakFilter();
        result setParams(float G, float Q, float f);
    };
}

#endif