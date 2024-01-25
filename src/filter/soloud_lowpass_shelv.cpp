#include "soloud.h"
#include "soloud_lowpass_shelv.h"
#include <math.h>
#include <stdio.h>
namespace SoLoud {

    // Función para calcular los parámetros del filtro
    void LowPassShelvInstance::calcShelvParams(float f, int order, float gain) {

        // Calculo de la constante K y G
        float G = pow(10, (gain)/20.0f);
        float K = tan(M_PI * f / mSamplerate);

        // Coeficiente a0 para normalizar el resto de coeficientes
        float a0;
        
        float p1 = sqrt(2 - sqrt(2));
        float p2 = sqrt(2 + sqrt(2));

        // if en función del orden del filtro
        switch (order)
        {
        case 1:
            a0 = (K + 1);
            a[0] = (K - 1) / a0;
            b[0] = (K * G + 1)/a0;
            b[1] = (K * G - 1) / a0;
            break;
        case 2:
            a0 =  (1 + sqrt(2) * K + pow(K, 2));
            a[0] = (-2 + 2  * pow(K, 2))/ a0;
            a[1] = (1 - sqrt(2) * K + pow(K, 2)) / a0;
            b[0] = (1 + sqrt(2) * sqrt(G)* K + G * pow(K, 2)) / a0;
            b[1] = (-2 + 2 * pow(K, 2)) / a0;
            b[2] = (1 - sqrt(2) * sqrt(G)* K + G * pow(K, 2)) / a0;
            break;
        case 3:
            a0 = (1 + 2 * K + 2 * pow(K, 2) + pow(K, 3));
            a[0] = (-3.0 - 2.0 * K + 2 * pow(K, 2)+ 3 * pow(K, 3)) / a0;
            a[1] = (3.0 - 2.0 * K - 2 * pow(K, 2) + 3 * pow(K, 3)) / a0;
            a[2] = (-1.0 + 2.0 * K - 2 * pow(K, 2) + pow(K, 3)) / a0;
            b[0] = (1 + 2 * K * pow(G, 1/3) + 2 * pow(G, 2/3) * pow(K, 2) + G * pow(K, 3)) / a0;
            b[1] = (-3.0 - 2.0 * K * pow(G, 1/3) + 2 * pow(G, 2/3) * pow(K, 2) + 3 * G * pow(K, 3)) / a0;
            b[2] = (3.0 - 2.0 * K * pow(G, 1/3) - 2 * pow(G, 2/3) * pow(K, 2) + 3 * G * pow(K, 3)) / a0;
            b[3] = (-1.0 + 2.0 * K * pow(G, 1/3) - 2 * pow(G, 2/3) * pow(K, 2) + G * pow(K, 3)) / a0;
            break;
        case 4:
            // K control to guarantee filter stability
            if (K < 0.1) {
                K = 0.1;
            }

            // G equal to sqrt(G) to make operations easier
            G = sqrt(G);
            a0 = (pow(K, 4) + (p1 + p2) * pow(K, 3) + (2 + p1 * p2) * pow(K, 2) + (p1 + p2) * K + 1);
            a[0] = (4 * pow(K, 4) + 2 * (p1 + p2) * pow(K, 3) - 2 * (p1 + p2) * K - 4) / a0;
            a[1] = (6 * pow(K, 4) - 2 * (2 + p1 * p2) * pow(K, 2) + 6) / a0;
            a[2] = (4 * pow(K, 4) - 2 * (p1 + p2) * pow(K, 3) + 2 * (p1 + p2) * K - 4)/a0;
            a[3] = (pow(K, 4) - (p1 + p2) * pow(K, 3) + (2 + p1 * p2) * pow(K, 2) - (p1 + p2) * K  + 1)/a0;

            b[0] = (pow(G,4) * pow(K, 4) + (p1 + p2) * pow(K, 3) * G * sqrt(G) + (2 + p1 * p2) * pow(K, 2) * G + (p1 + p2) * K * sqrt(G) + 1) / a0;
            b[1] = (4 * pow(G,4) *  pow(K, 4) + 2 * (p1 + p2) * pow(K, 3) * G * sqrt(G) - 2 * (p1 + p2) * K * sqrt(G) - 4) / a0;
            b[2] = (6 * pow(G,4) *  pow(K, 4) - 2 * (2 + p1 * p2) * pow(K, 2) * G + 6) / a0;
            b[3] = (4 * pow(G,4) *  pow(K, 4) - 2 * (p1 + p2) * pow(K, 3) * G * sqrt(G) + 2 * (p1 + p2) * K * sqrt(G) - 4) / a0;
            b[4] = (pow(G,4) * pow(K, 4) - (p1 + p2) * pow(K, 3) * G * sqrt(G) + (2 + p1 * p2) * pow(K, 2) * G - (p1 + p2) * K * sqrt(G) + 1) / a0;
            break;
        default:
            break;
        }


        // if (order == 1) {
        //     a0 = (K + 1);
        //     a[0] = (K - 1) / a0;
        //     b[0] = (K * G + 1)/a0;
        //     b[1] = (K * G - 1) / a0;
        // }
        // else if (order == 2) {
        //     a0 =  (1 + sqrt(2) * K + pow(K, 2));
        //     a[0] = (-2 + 2  * pow(K, 2))/ a0;
        //     a[1] = (1 - sqrt(2) * K + pow(K, 2)) / a0;
        //     b[0] = (1 + sqrt(2) * sqrt(G)* K + G * pow(K, 2)) / a0;
        //     b[1] = (-2 + 2 * pow(K, 2)) / a0;
        //     b[2] = (1 - sqrt(2) * sqrt(G)* K + G * pow(K, 2)) / a0;

        // }
        // else if (order == 3) {
        //     a0 = (1 + 2 * K + 2 * pow(K, 2) + pow(K, 3));
        //     a[0] = (-3.0 - 2.0 * K + 2 * pow(K, 2)+ 3 * pow(K, 3)) / a0;
        //     a[1] = (3.0 - 2.0 * K - 2 * pow(K, 2) + 3 * pow(K, 3)) / a0;
        //     a[2] = (-1.0 + 2.0 * K - 2 * pow(K, 2) + pow(K, 3)) / a0;
        //     b[0] = (1 + 2 * K * pow(G, 1/3) + 2 * pow(G, 2/3) * pow(K, 2) + G * pow(K, 3)) / a0;
        //     b[1] = (-3.0 - 2.0 * K * pow(G, 1/3) + 2 * pow(G, 2/3) * pow(K, 2) + 3 * G * pow(K, 3)) / a0;
        //     b[2] = (3.0 - 2.0 * K * pow(G, 1/3) - 2 * pow(G, 2/3) * pow(K, 2) + 3 * G * pow(K, 3)) / a0;
        //     b[3] = (-1.0 + 2.0 * K * pow(G, 1/3) - 2 * pow(G, 2/3) * pow(K, 2) + G * pow(K, 3)) / a0;

        // }
        // else if (order == 4) {
        //     // K control to guarantee filter stability
        //     if (K < 0.1) {
        //         K = 0.1;
        //     }

        //     // G equal to sqrt(G) to make operations easier
        //     G = sqrt(G);
        //     float p1 = sqrt(2 -sqrt(2));
        //     float p2 = sqrt(2 + sqrt(2));
        //     a0 = (pow(K, 4) + (p1 + p2) * pow(K, 3) + (2 + p1 * p2) * pow(K, 2) + (p1 + p2) * K + 1);
        //     a[0] = (4 * pow(K, 4) + 2 * (p1 + p2) * pow(K, 3) - 2 * (p1 + p2) * K - 4) / a0;
        //     a[1] = (6 * pow(K, 4) - 2 * (2 + p1 * p2) * pow(K, 2) + 6) / a0;
        //     a[2] = (4 * pow(K, 4) - 2 * (p1 + p2) * pow(K, 3) + 2 * (p1 + p2) * K - 4)/a0;
        //     a[3] = (pow(K, 4) - (p1 + p2) * pow(K, 3) + (2 + p1 * p2) * pow(K, 2) - (p1 + p2) * K  + 1)/a0;

        //     b[0] = (pow(G,4) * pow(K, 4) + (p1 + p2) * pow(K, 3) * G * sqrt(G) + (2 + p1 * p2) * pow(K, 2) * G + (p1 + p2) * K * sqrt(G) + 1) / a0;
        //     b[1] = (4 * pow(G,4) *  pow(K, 4) + 2 * (p1 + p2) * pow(K, 3) * G * sqrt(G) - 2 * (p1 + p2) * K * sqrt(G) - 4) / a0;
        //     b[2] = (6 * pow(G,4) *  pow(K, 4) - 2 * (2 + p1 * p2) * pow(K, 2) * G + 6) / a0;
        //     b[3] = (4 * pow(G,4) *  pow(K, 4) - 2 * (p1 + p2) * pow(K, 3) * G * sqrt(G) + 2 * (p1 + p2) * K * sqrt(G) - 4) / a0;
        //     b[4] = (pow(G,4) * pow(K, 4) - (p1 + p2) * pow(K, 3) * G * sqrt(G) + (2 + p1 * p2) * pow(K, 2) * G - (p1 + p2) * K * sqrt(G) + 1) / a0;
        // }
    }

    void LowPassShelvInstance::filter(float* aBuffer, unsigned int aSamples, double aTime, unsigned int aChannel, float aSamplerate, int order) {
        // Si estamos en el primer canal, se actualizan los parámetros solo si han cambiado.
        if (aChannel == 0) {
            updateParams(aTime);
            if (mParamChanged & ((1 << LowPassShelv::FREQ_C) | (1 << LowPassShelv::ORDER) | (1 << LowPassShelv::GAIN)) || aSamplerate != mSamplerate)
            {
                mSamplerate = aSamplerate;
                calcShelvParams(mParam[LowPassShelv::FREQ_C], mParam[LowPassShelv::ORDER], mParam[LowPassShelv::GAIN]);
            }
            mParamChanged = 0;
        }
        float n;
        for (int i = 0; i < aSamples; i++) {
            
            // Se calcula n  en función del orden
            switch (order) {
                case 1:
                    n = b[0] * aBuffer[i] + b[1] * x[0][aChannel];
                    n -= a[0] * y[0][aChannel];
                    break;
                case 2:
                    n = b[0] * aBuffer[i] + b[1] * x[0][aChannel] + b[2] * x[1][aChannel];
                    n = n - a[0] * y[0][aChannel] - a[1] * y[1][aChannel];
                    break;
                case 3:
                    n = b[0] * aBuffer[i] + b[1] * x[0][aChannel] + b[2] * x[1][aChannel] + b[3] * x[2][aChannel];
                    n = n - a[0] * y[0][aChannel] - a[1] * y[1][aChannel] - a[2] * y[2][aChannel]; //y[n]
                    break;
                case 4:
                    n = b[0] * aBuffer[i] + b[1] * x[0][aChannel] + b[2] * x[1][aChannel] + b[3] * x[2][aChannel] + b[4] * x[3][aChannel];
                    n = n - a[0] * y[0][aChannel] - a[1] * y[1][aChannel] - a[2] * y[2][aChannel] - a[3] * y[3][aChannel]; //y[n]
                    break;
                default:
                    n = aBuffer[i];
                    break;
            }

            // Se actualizan los valores de las muestras de retardo para la siguiente muestra
            x[3][aChannel] = x[2][aChannel];    // x[n-4] = x[n-3]
            x[2][aChannel] = x[1][aChannel];    // x[n-3] = x[n-2]
            x[1][aChannel] = x[0][aChannel];    // x[n-2] = x[n-1]
            x[0][aChannel] = aBuffer[i];        // x[n-1] = x[n];

            y[3][aChannel] = y[2][aChannel];    // y[n-4] = y[n-3]
            y[2][aChannel] = y[1][aChannel];    // y[n-3] = y[n-2]
            y[1][aChannel] = y[0][aChannel];    // y[n-2] = y[n-1]
            y[0][aChannel] = n;                 // y[n-1] = y[n]
            
            // Se asigna el valor a la muestra de salida
            aBuffer[i] = n;
        }
    }

    
    // Constructor de la instancia del filtro, en el que se inicializan los parámetros,
    // los buffer de memoria se inicializan a 0 y se calculan los coeficientes
    LowPassShelvInstance::LowPassShelvInstance(LowPassShelv* aParent) {
        mParent = aParent;
        initParams(4);

        mParam[LowPassShelv::FREQ_C] = aParent->mFreqC;
        mParam[LowPassShelv::ORDER] = aParent->mOrder;
        mParam[LowPassShelv::GAIN] = aParent->mGain;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                x[i][j] = 0.0f;
                y[i][j] = 0.0f;
            }
        }

        calcShelvParams(mParam[LowPassShelv::FREQ_C], mParam[LowPassShelv::ORDER], mParam[LowPassShelv::GAIN]);
    }

    // Función para filtrar cada canal
    void LowPassShelvInstance::filterChannel(float* aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int /*aChannels*/) {
        mSamplerate = aSamplerate;
        filter(aBuffer, aSamples, aTime, aChannel, aSamplerate, mParam[LowPassShelv::ORDER]);
    }

    // Destructor de la instancia
    LowPassShelvInstance::~LowPassShelvInstance() {
    }

    // Constructor de la clase filtro, se setean los parámetros por defecto
    LowPassShelv::LowPassShelv() {
        setParams(100.0f, 1, 0);
    }

    // Función para setear los parámetros del filtro
    result LowPassShelv::setParams(float freqC, int order, float gain) {
        if ((freqC > 20000.0f || freqC < 40.0f || order <= 0 || order >= 4))
            return INVALID_PARAMETER;

        mFreqC = freqC;
        mOrder = order;
        mGain = gain;
        return 0;
    }
    
    // Función estandar que devuelve el número de parámetros del filtro
    int LowPassShelv::getParamCount() {
        return 4;
    }

    // Función estandar que devuelve el nombre de los parámetros del filtro
    const char* LowPassShelv::getParamName(unsigned int aParamIndex) {
        if (aParamIndex > 3)
            return 0;
        const char* names[4] = {
            "Wet",
            "Cut Frequency",
            "Order",
            "Gain"
        };
        return names[aParamIndex];
    }
    
    // Función estandar que devuelve el tipo de los parámetros del filtro
    unsigned int LowPassShelv::getParamType(unsigned int aParamIndex) {
        if(aParamIndex == 2) {
            return INT_PARAM;
        }
        return FLOAT_PARAM;
    }

    // Función estandar que devuelve el máximo de los parámetros del filtro
    float LowPassShelv::getParamMax(unsigned int aParamIndex)
    {
        if (aParamIndex == 1) {
            return 20000.0f;
        }
        else if (aParamIndex == 2) {
            return 4.0f;
        }
        else if (aParamIndex == 3) {
            return 24.0f;
        }
        return 1.0f;
    }
    
    // Función estandar que devuelve el mínimo de los parámetros del filtro
    float LowPassShelv::getParamMin(unsigned int aParamIndex)
    {
        if (aParamIndex == 1) {
            return 40.0f;
        }
        else if (aParamIndex == 2) {
            return 1.0f;
        }
        else if (aParamIndex == 3) {
            return -24.0f;
        }
        return 0.0f;
    }

    // Función que devuelve una instancia de este filtro
    LowPassShelvInstance* LowPassShelv::createInstance() {
        return new LowPassShelvInstance(this);
    }

}