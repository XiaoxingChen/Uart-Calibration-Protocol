#ifndef CALIVAR_H
#define CALIVAR_H
#include <stdint.h>
namespace clv   //calibration variables
{
    enum
    {
        IDX0 = 0,
        IDX1 = 1,
        IDX2 = 2,
        IDX3 = 3,
        CPUUSAGE,
        MOTORPOS,
        MOTORVEL,
        MOTORACC,
		COILCUR,
        PWMDUTY,
        ROTATEDIR,
        CTRLMODE,
        TARVAL,
        VCTRL_KP,
        VCTRL_KI,
        VCTRL_KD,
		PCTRL_KP,
        PCTRL_KI,
        PCTRL_KD
    };

    const uint8_t KP = 0;
    const uint8_t KI = 1;
    const uint8_t KD = 2;

    const uint8_t VCTRL_BASE = 12;


}

#endif // CALIVAR_H
