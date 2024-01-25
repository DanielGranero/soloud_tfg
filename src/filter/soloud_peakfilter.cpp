#include "soloud.h"
#include "soloud_peakfilter.h"
#include <math.h>

namespace SoLoud{

    // Función para calcular los parámetros del filtro
    void PeakFilterInstance::calcPeakParams(float f, float gain, float Q){
        // Calculo de la constante K y G
        float K = tan(M_PI * f / mSamplerate);
        float G = pow(10, gain/20);
        
        // Coeficiente a0 para normalizar el resto de coeficientes
        float a0;

        // if en función de la ganancia 
        if(gain >= 0) //BOOST
        {
            a0 = 1 + (1/Q)*K +(pow(K,2));
            b[0] = (1 + (G / Q) *K + pow(K,2))/ a0;
    		b[1] = (2.0f*(pow(K,2)-1))/a0;
    		b[2] = (1-(G/Q)*K+ pow(K, 2))/a0;
    
    		a[0] = (2*(pow(K, 2) -1))/a0;
    		a[1] = (1-(1/Q)*K+ pow(K, 2))/a0;
        }
        else //CUT
        {
            a0 = 1 + (1/(G*Q))*K + (pow(K,2));

    		b[0] = (1+(1/Q)*K+ pow(K, 2))/a0; 
    		b[1] = (2*(pow(K, 2) -1))/a0;
    		b[2] = (1-(1/Q)*K+ pow(K, 2))/a0;

    		a[0] = (2*(pow(K, 2) -1))/a0;
    		a[1] = (1-(1/(G*Q))*K+ pow(K, 2))/a0;
        }
    }

    void PeakFilterInstance::filter(float* aBuffer,unsigned int aSamples, double aTime, unsigned int aChannel, float aSamplerate){
        // Si estamos en el primer canal, se actualizan los parámetros solo si han cambiado.
        if (aChannel == 0) {
            updateParams(aTime);
            if (mParamChanged & ((1 << GAIN) | (1 << Q) | (1 << FREQ)) || aSamplerate != mSamplerate)
            {
                mSamplerate = aSamplerate;
                calcPeakParams(mParam[FREQ], mParam[GAIN], mParam[Q]);
            }
            mParamChanged = 0;
        }
        
        for (int i = 0; i < aSamples; i++) {

            // Calculo de n
            float n = b[0]*aBuffer[i]+b[1]*x[0][aChannel] + b[2]*x[1][aChannel];
            n =n - a[0]*y[0][aChannel] - a[1]*y[1][aChannel]; //y[n]
            
            // Se actualizan los valores de las muestras de retardo para la siguiente muestra

            x[1][aChannel] = x[0][aChannel];    // x[n-2] = x[n-1]
            x[0][aChannel] = aBuffer[i];        // x[n-1] = x[n];

            y[1][aChannel] = y[0][aChannel];    // y[n-2] = y[n-1]
            y[0][aChannel] = n;                 // y[n-1] = y[n]

            // Se asigna el valor a la muestra de salida
            aBuffer[i] = n;
        }
    }

    // Constructor de la instancia del filtro, en el que se inicializan los parámetros,
    // los buffer de memoria se inicializan a 0 y se calculan los coeficientes
    PeakFilterInstance::PeakFilterInstance(PeakFilter *aParent){
        mParent = aParent;
        initParams(4);
        mParam[GAIN] = aParent->mGain;
        mParam[Q] = aParent->mQ;
        mParam[FREQ] = aParent->mFreq;
        for(int j = 0; j < 2; j++){
            for (int k = 0; k < 2; k++) {
                x[j][k] = 0.0f;
                y[j][k] = 0.0f;
            }
        }
        calcPeakParams(mParam[FREQ], mParam[GAIN], mParam[Q]);
    }

    // Función para filtrar cada canal
    void PeakFilterInstance::filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int /*aChannels*/){
        mSamplerate = aSamplerate;
        filter(aBuffer, aSamples, aTime, aChannel, aSamplerate);
    }

    // Destructor de la instancia
    PeakFilterInstance::~PeakFilterInstance(){
    }

    // Constructor de la clase filtro, se setean los parámetros por defecto
    PeakFilter::PeakFilter(){   
        setParams(0.0f, 0.0f, 1000.0f);
    }
    // Función para setear los parámetros del filtro
    result PeakFilter::setParams(float gain, float Q, float FREQ){
        if((gain > 24.0f || gain < -24.0f)||(FREQ > 20000.0f || FREQ < 40.0f))
            return INVALID_PARAMETER;
        if((Q > 10.0f || Q <= 0.0f))
            return INVALID_PARAMETER;
        
        mGain = gain;
        mQ = Q;
        mFreq = FREQ;
        return 0;
    }

    // Función estandar que devuelve el número de parámetros del filtro
    int PeakFilter::getParamCount(){
        return 4;
    }

    // Función estandar que devuelve el nombre de los parámetros del filtro
    const char* PeakFilter::getParamName(unsigned int aParamIndex){
        if(aParamIndex < 6)
            return 0;
        const char *names[7]={
            "Wet",
            "Gain",
            "Q",
            "Frequency"
        };
        return names[aParamIndex];
    }

    // Función estandar que devuelve el tipo de los parámetros del filtro
    unsigned int PeakFilter::getParamType(unsigned int aParamIndex){
        
        return FLOAT_PARAM;
    }

    // Función estandar que devuelve el máximo de los parámetros del filtros
    float PeakFilter::getParamMax(unsigned int aParamIndex)
    {
        switch(aParamIndex)
        {
            case GAIN: return 9;
            case Q: return 10;
            case FREQ: return 20000;
        }
        return 1;
    }

    // Función estandar que devuelve el mínimo de los parámetros del filtro
    float PeakFilter::getParamMin(unsigned int aParamIndex)
    {
        switch(aParamIndex)
        {
            case GAIN: return -9;
            case Q: return 0.1f;
            case FREQ: return 150;
        }
    }
    
    // Función que devuelve una instancia de este filtro
    PeakFilterInstance * PeakFilter::createInstance(){
        return new PeakFilterInstance(this);
    }
 

}