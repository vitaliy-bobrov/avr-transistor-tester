/* ************************************************************************
 *
 *   semiconductor tests and measurements
 *
 *   (c) 2012-2024 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz K�bbeler
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define SEMI_C

#define SHUNT_DROP_MV 150    /* voltage drop across emitter shunt resistor (mV) */

/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */



/* ************************************************************************
 *   support functions
 * ************************************************************************ */


/*
 *  manage pin designators for semiconductors
 *  - results in smaller firmware than setting the designators in
 *    each semiconductor check function
 */

void SemiPinDesignators(void)
{
  uint8_t           A = 0;         /* designator for pin A */
  uint8_t           B = 0;         /* designator for pin B */
  uint8_t           C = 0;         /* designator for pin C */

  /*
   *  set pin designators based on semiconductor type
   */

  switch (Check.Found)
  {
    case COMP_BJT:            /* BJT */
      A = 'B';                /* base */
      B = 'C';                /* collector */
      C = 'E';                /* emitter */
      break;

    case COMP_FET:            /* FET */
      A = 'G';                /* gate */
      if (Check.Type & TYPE_SYMMETRICAL)     /* symmetrical */
      {
        B = 'x';              /* drain */
        C = 'x';              /* source */
      }
      else                                   /* normal */
      {
        B = 'D';              /* drain */
        C = 'S';              /* source */
      }
      break;

    case COMP_IGBT:           /* IGBT */
      A = 'G';                /* gate */
      B = 'C';                /* collector */
      C = 'E';                /* emitter */
      break;

    case COMP_THYRISTOR:      /* Thyristor / SCR */
    case COMP_PUT:            /* PUT */
      A = 'G';                /* gate */
      B = 'A';                /* anode */
      C = 'C';                /* cathode */
      break;

    case COMP_TRIAC:          /* TRIAC */
      A = 'G';                /* gate */
      B = '2';                /* MT2 */
      C = '1';                /* MT1 */
      break;

    #ifdef SW_UJT
    case COMP_UJT:            /* UJT */
      A = 'E';                /* emitter */
      B = '2';                /* B2 */
      C = '1';                /* B1 */
      break;
    #endif
  }

  /* update pin designators */
  Semi.DesA = A;              /* designator for pin A */
  Semi.DesB = B;              /* designator for pin B */
  Semi.DesC = C;              /* designator for pin C */
}



/*
 *  measure leakage current
 *  - current through a semiconducter in non-conducting mode
 *  - result is stored in Semi.I_value & I.scale
 *
 *  requires:
 *  - mode:
 *    0 = normal
 *    1 = high current
 */

void GetLeakageCurrent(uint8_t Mode)
{
  int8_t                 Scale;         /* exponent of factor (value * 10^x) */
  uint32_t               Value;         /* current */
  uint32_t               R_Shunt;       /* shunt resistor */
  uint16_t               U_Rl;          /* voltage at Rl */


  /*
   *  set up probes:
   *  - use Rl as current shunt
   *  - probe-1 = pos / probe-2 = neg / probe-3 = HiZ
   *    Diode:    probe-1 = cathode /  probe-2 = anode
   *    NPN BJT:  probe-1 = collector / probe-2 = emitter
   *    PNP BJT:  probe-1 = emitter / probe-2 = collector
   *  - set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc
   */

  R_PORT = 0;                      /* set resistor port to Gnd */
  R_DDR = Probes.Rl_2;             /* pull down probe-2 via Rl */
  ADC_DDR = Probes.Pin_1;          /* set probe-1 to output */
  ADC_PORT = Probes.Pin_1;         /* pull-up probe-1 directly */
  U_Rl = ReadU_5ms(Probes.Ch_2);   /* get voltage at Rl */

  if (U_Rl > 3)          /* > 5�A */
  {
    /* consider internal resistance of MCU */
    R_Shunt = (uint32_t)NV.RiL;         /* in 0.1 Ohms */
    Scale = -7;                         /* 100n */
    Value = 100000;                     /* scale voltage to 10 nV */

    if ((U_Rl > 1400) && (Mode == 1))   /* > 2mA */
    {
      /*
       *  for high currents take second measurement using RiL:
       *  - only if high current mode is enabled
       *  - with 1mV ADC resolution we get down to 50�A
       *  - max. current is 5V/40Ohms = 125mA
       *  - set probes: Gnd -- probe-2 / probe-1 -- Vcc
       */

      R_DDR = 0;                        /* disable resistors */
      /* pull down probe-2 directly */
      ADC_DDR = Probes.Pin_1 | Probes.Pin_2;
      U_Rl = ReadU(Probes.Ch_2);        /* get voltage at RiL */
    }
    else                                /* keep measurement */
    {
      /*
       *  Rl:
       *  - with 1mV ADC resolution we get down to 1.4�A
       *  - max. current is 5V/720Ohms = 7mA
       */

      /* add Rl */
      R_Shunt += (R_LOW * 10);          /* in 0.1 Ohms */
    }
  }
  else                   /* < 5�A */
  {
    /*
     *  For low currents we take a second measurement using Rh:
     *  - with 1mV ADC resolution we get down to 2nA
     *  - max. current is 5V/470kOhms = 10�A
     *  - set probes: Gnd -- Rh -- probe-2 / probe-1 -- Vcc
     */

    R_DDR = Probes.Rh_2;                /* pull down probe-2 via Rh */
    U_Rl = ReadU_5ms(Probes.Ch_2);      /* get voltage at Rh */

    /* neglect MCU's internal resistance */
    R_Shunt =  R_HIGH;
    Scale = -9;                         /* 1n */
    Value = 1000000;                    /* scale voltage to 1 nV */
  }

  /* clean up */
  ADC_DDR = 0;           /* set ADC port to HiZ mode */
  ADC_PORT = 0;          /* set ADC port low */
  R_DDR = 0;             /* set resistor port to HiZ mode */
  R_PORT = 0;            /* set resistor port low */

  /* calculate current */
  Value *= U_Rl;                   /* scale voltage */
  Value /= R_Shunt;                /* I = U/R */

  /* save result */
  Semi.I_value = Value;
  Semi.I_scale = Scale;
}



/* ************************************************************************
 *   diodes
 * ************************************************************************ */


/*
 *  look for a specific diode
 *
 *  required:
 *  - probe ID for Anode
 *  - probe ID for Cathode
 *
 *  returns:
 *  - NULL if no matching diode was found
 *  - pointer to Diode_Type if matching diode was found
 */

Diode_Type *SearchDiode(uint8_t A, uint8_t C)
{
  Diode_Type        *Diode;        /* pointer to diode */
  uint8_t           n;             /* counter */

  n = Check.Diodes;           /* number of diodes */
  Diode = &Diodes[0];         /* pointer to first diode */

  while (n > 0)
  {
    if ((A == Diode->A) && (C == Diode->C))
    {
      break;
    }

    n--;                 /* next diode */
    Diode++;
  }

  if (n == 0)            /* no match found */
  {
    Diode = NULL;        /* reset pointer */
  }

  return Diode;
}



/*
 *  check for diode
 */

void CheckDiode(void)
{
  Diode_Type        *Diode;        /* pointer to diode */
  uint16_t          U1_Rl;         /* Vf #1 with Rl pull-up */
  uint16_t          U1_Rh;         /* Vf #1 with Rh pull-up */
  uint16_t          U1_Zero;       /* Vf #1 zero */
  uint16_t          U2_Rl;         /* Vf #2 with Rl pull-down */
  uint16_t          U2_Rh;         /* Vf #2 with Rh pull-down */
  uint16_t          U2_Zero;       /* Vf #2 zero */
  uint16_t          U_Diff;        /* Vf difference */

  wdt_reset();                          /* reset watchdog */

  DischargeProbes();                    /* try to discharge probes */
  if (Check.Found == COMP_ERROR) return;     /* skip on error */

  /*
   *  DUT could be:
   *  - simple diode
   *  - intrinsic diode of a MOSFET or another component
   *  - flyback diode of a BJT
   *  - small resistor (< 3k) or inductor
   *  - capacitor (> around 22�F)
   *
   *  Solution:
   *  - Vf of a diode rises with the current within some limits (about twice
   *    for Si and Schottky). Ge, Z-diodes and LEDs are hard to determine.
   *    Therefore it might be better to filter out other components.
   *  - For a MOSFET's intrinsic diode we have to make sure that the MOSFET
   *    is not conducting to be able to get Vf of the protection diode.
   *    So we discharge the gate and run the measurements twice for assumed
   *    p and n channel FETs.
   *  - Take care about the internal voltage drop of the MCU at the cathode
   *    for high test currents (Rl).
   *  - Filter out resistors via the voltage divider used:
   *    k = Rh / (Rl + Ri_H + Ri_L)
   *    U_Rh = U_Rl / (k - (k - 1) U_Rl / Vcc)
   *    U_Rl = k U_Rh / (1 + (k - 1) U_Rh / Vcc)
   *  - Filter out caps by checking the voltage before and after measurement
   *    with Rh. In 15ms a 22�F cap would be charged from 0 to 7mV, a larger
   *    cap would have a lower voltage. We have also to consider other effects
   *    which could increase the voltage.
   *
   *  Hints:
   *  - Rl drives a current of about 7mA. That's not the best current for
   *    measuring Vf. The current for Rh is about 10.6�A.
   *    Most DMMs use 1mA.
   */


  /*
   *  Vf #1, supporting a possible p-channel MOSFET
   */

  /* we assume: probe-1 = A / probe2 = C */
  /* set probes: Gnd -- probe-2 / probe-1 -- HiZ */
  ADC_PORT = 0;
  ADC_DDR = Probes.Pin_2;               /* pull down cathode directly */
  /* R_DDR is set to HiZ by DischargeProbes() */
  U1_Zero = ReadU(Probes.Ch_1);         /* get voltage at anode */

  /* measure voltage across DUT (Vf) with Rh */
  /* set probes: Gnd -- probe-2 / probe-1 -- Rh -- Vcc */
  R_DDR = Probes.Rh_1;                  /* enable Rh for probe-1 */
  R_PORT = Probes.Rh_1;                 /* pull up anode via Rh */
  PullProbe(Probes.Rl_3, PULL_10MS | PULL_UP);     /* discharge gate */
  U1_Rh = ReadU_5ms(Probes.Ch_1);       /* get voltage at anode */
                                        /* neglect voltage at cathode */

  /* measure voltage across DUT (Vf) with Rl */
  /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc */
  R_DDR = Probes.Rl_1;                  /* enable Rl for probe-1 */
  R_PORT = Probes.Rl_1;                 /* pull up anode via Rl */
  PullProbe(Probes.Rl_3, PULL_10MS | PULL_UP);     /* discharge gate */
  U1_Rl = ReadU_5ms(Probes.Ch_1);       /* get voltage at anode */
  U1_Rl -= ReadU(Probes.Ch_2);          /* substract voltage at cathode */


  DischargeProbes();                    /* try to discharge probes */
  if (Check.Found == COMP_ERROR) return;     /* skip on error */


  /*
   *  Vf #2, supporting a possible n-channel MOSFET
   */

  /* we assume: probe-1 = A / probe2 = C */
  /* set probes: Gnd -- probe-2 / probe-1 -- HiZ */
  ADC_PORT = 0;
  ADC_DDR = Probes.Pin_2;               /* pull down cathode directly */
  /* R_DDR is set to HiZ by DischargeProbes() */
  U2_Zero = ReadU(Probes.Ch_1);         /* get voltage at anode */

  /* measure voltage across DUT (Vf) with Rh */
  /* set probes: Gnd -- Rh -- probe-2 / probe-1 -- Vcc */
  ADC_DDR = 0;                          /* set to HiZ to prepare change */
  ADC_PORT = Probes.Pin_1;              /* pull up anode directly */
  ADC_DDR = Probes.Pin_1;               /* enable output */
  R_PORT = 0;                           /* pull down cathode via Rh */
  R_DDR = Probes.Rh_2;                  /* enable Rh for probe-2 */
  PullProbe(Probes.Rl_3, PULL_10MS | PULL_DOWN);   /* discharge gate */
  U2_Rh = ReadU_5ms(Probes.Ch_1);       /* get voltage at anode */
  U_Diff = ReadU(Probes.Ch_2);          /* get voltage at cathode */
  if (U2_Rh >= U_Diff)                  /* prevent underrun */
  {
    U2_Rh -= U_Diff;                    /* V_f = U_Anode - U_Cathode */
  }
  else                                  /* maybe large inductance */
  {
    U2_Rh = 0;                          /* simply zero */
  }

  /* measure voltage across DUT (Vf) with Rl */
  /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */
  R_DDR = Probes.Rl_2;                  /* pull down cathode via Rl */
  PullProbe(Probes.Rl_3, PULL_10MS | PULL_DOWN);   /* discharge gate */
  U2_Rl = ReadU_5ms(Probes.Ch_1);       /* get voltage at anode */
  U2_Rl -= ReadU(Probes.Ch_2);          /* substract voltage at cathode */

  ADC_DDR = 0;                     /* stop pulling up */


  /*
   *  process results
   */

  /* choose between both measurements */
  if (U1_Rl > U2_Rl)          /* the higher voltage wins */
  {
    U_Diff = U1_Rl - U2_Rl;   /* difference of U_Rls */
    U2_Rl = U1_Rl;
    U2_Rh = U1_Rh;
    U2_Zero = U1_Zero;
  }
  else                        /* keep 2nd data set */
  {
    U_Diff = U2_Rl - U1_Rl;   /* difference of U_Rls */
  }


  /*
   *  check difference of U_Rl measurements
   *  - nearly zero for diodes, BJTs etc.
   *  - about a diode drop for enhancement-mode MOSFETs
   *  - >1000mV for depletion-mode FETs partly conducting
   */

  if (U_Diff > 1000) return;       /* depletion-mode FET */


  /*
   *  U_Rh < 5mV for
   *  - resistor < 470 Ohm
   *  - very large cap
   *
   *  The lowest Vf measured with Rh is 9mV for a GY100 (Germanium).
   */

  if (U2_Rh <= 5) return;          /* small resistor or very large cap */


  /*
   *  U_Zero <= 2 for resistor or diode
   *  U_Zero > 2 for cap or diode
   *  if U_Zero > 2 then U_Rh - U_Zero < 100 for cap
   *
   *  Hints:
   *  If U_Zero > 10 and U_Rh is about U_Zero it's a large cap.
   *  As larger the cap as lower U_Rl (charging time 15ms).
   */

  if (U2_Rh > U2_Zero)             /* prevent underrun */
  {
    U_Diff = U2_Rh - U2_Zero;      /* calculate difference */
  }
  else
  {
    U_Diff = U2_Zero - U2_Rh;      /* calculate difference */
  }

  if ((U2_Zero > 2) && (U_Diff < 100))  /* capacitor */
  {
    return;
  }


  /*
   *  Check for a resistor:
   *  - Measure current in forward and reverse direction. Both currents
   *    should be the same for a resistor.
   *  - For U_Rh > 40mV we don't need to check for a resistor.
   */

  if (U2_Rh < 40)             /* resistor (< 3k) */
  {
    /* get current in forward direction */
    /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */
    R_PORT = 0;
    R_DDR = Probes.Rl_2;                  /* pull down cathode via Rl */
    ADC_PORT = Probes.Pin_1;
    ADC_DDR = Probes.Pin_1;               /* pull up anode directly */
    U1_Rl = ReadU_5ms(Probes.Ch_2);       /* get voltage at Rl */

    /* get current in reverse direction */
    /* set probes: Gnd -- Rl -- probe-1 / probe-2 -- Vcc */
    R_DDR = Probes.Rl_1;                  /* pull down anode via Rl */
    ADC_PORT = Probes.Pin_2;
    ADC_DDR = Probes.Pin_2;               /* pull up cathode directly */
    U1_Rh = ReadU_5ms(Probes.Ch_1);       /* get voltage at Rl */

    /* check if both currents match */
    U_Diff = U1_Rl / 20;           /* 5% */
    U1_Zero = U1_Rl - U_Diff;      /* 95% */
    U2_Zero = U1_Rl + U_Diff;      /* 105% */

    if ((U1_Rh > U1_Zero) && (U1_Rh < U2_Zero))   /* within limits */
    {
      /* could also be an anti-parallel diode with very low Vf */
      return;            /* resistor */
    }
  }


  /*
   *  if U_Rl (Vf) is between 0.15V and 4.64V it's a diode
   */

  if ((U2_Rl > 150) && (U2_Rl < 4640))
  {
    /* if we haven't found any other component yet */
    if ((Check.Found == COMP_NONE) ||
        (Check.Found == COMP_RESISTOR))
    {
      Check.Found = COMP_DIODE;
      /* Check.Type = TYPE_STANDARD; */
      /* we don't set Check.Done in case we'll find something different */
    }

    /* save data */
    Diode = &Diodes[Check.Diodes];
    Diode->A = Probes.ID_1;   /* probe ID for Anode */
    Diode->C = Probes.ID_2;   /* probe ID for Cathode */
    Diode->V_f = U2_Rl;       /* Vf for high measurement current */
    Diode->V_f2 = U2_Rh;      /* Vf for low measurement current */
    Check.Diodes++;
  }
}



/* ************************************************************************
 *   BJTs and FETs
 * ************************************************************************ */


/*
 *  verify MOSFET by checking the body diode
 */

void VerifyMOSFET(void)
{
  uint8_t           Anode;
  uint8_t           Cathode;
  Diode_Type        *Diode;             /* pointer to diode */

  /* set expected body diode */
  if (Check.Type & TYPE_N_CHANNEL)      /* n-channel */
  {
    Anode = Semi.C;      /* anode at source */
    Cathode = Semi.B;    /* cathode at drain */
  }
  else                                  /* p-channel */
  {
    Anode = Semi.B;      /* anode at drain */
    Cathode = Semi.C;    /* cathode at source */
  }

  /* search for a diode with reversed polarity */
  Diode = SearchDiode(Cathode, Anode);  /* search for reversed diode */
  if (Diode != NULL)                    /* got it */
  {
    /* this can't be a MOSFET, so let's reset */
    Check.Found = COMP_NONE;
    Check.Type = 0;
    Check.Done &= ~(DONE_SEMI);    /* unset flag */
  }
}



/*
 *  measure the gate threshold voltage of a depletion-mode MOSFET
 *
 *  requires:
 *  - Type: n-channel or p-channel
 */

void GetGateThreshold(uint8_t Type)
{
  int32_t           Ugs = 0;       /* gate threshold voltage / Vth */
  uint8_t           Drain_Rl;      /* Rl register bits for drain */
  uint8_t           Drain_ADC;     /* ADC port register bits for drain */
  uint8_t           PullMode;      /* pull-up/down mode */
  uint8_t           Counter;       /* loop counter */

  /*
   *  init variables
   */

  if (Type & TYPE_N_CHANNEL)       /* n-channel */
  {
    /* we assume: probe-1 = D / probe-2 = S / probe-3 = G */
    /* probe-2 is still pulled down directly */
    /* probe-1 is still pulled up via Rl */

    Drain_Rl =  Probes.Rl_1;
    Drain_ADC = Probes.Pin_1;
    PullMode = PULL_10MS | PULL_DOWN;
  }
  else                             /* p-channel */
  {
    /* we assume: probe-1 = S / probe-2 = D / probe-3 = G */
    /* probe-2 is still pulled down via Rl */
    /* probe-1 is still pulled up directly */

    Drain_Rl =  Probes.Rl_2;
    Drain_ADC = Probes.Pin_2;
    PullMode = PULL_10MS | PULL_UP;
  }


  /*
   *  For low reaction times we use the ADC directly.
   */

  /* sanitize register bits for drain to prevent a never-ending loop */
  Drain_ADC &= ((1 << TP1) | (1 << TP2) | (1 << TP3));
  ADMUX = Probes.Ch_3 | ADC_REF_VCC;    /* select probe-3 for ADC input */
                                        /* and use Vcc as reference */
  #ifndef ADC_LARGE_BUFFER_CAP
    /* buffer cap: 1nF or none at all */
    wait100us();                   /* time for voltage stabilization */
  #else
    /* buffer cap: 100nF */
    wait10ms();                    /* time for voltage stabilization */
  #endif

  /* sample 10 times */
  for (Counter = 0; Counter < 10; Counter++)
  {
    wdt_reset();                         /* reset watchdog */

    /* discharge gate via Rl for 10 ms */
    PullProbe(Probes.Rl_3, PullMode);

    /* pull up/down gate via Rh to slowly charge gate */
    R_DDR = Drain_Rl | Probes.Rh_3;

    /* wait until FET conducts */
    /* any problem will cause a watchdog timeout */
    if (Type & TYPE_N_CHANNEL)          /* n-channel */
    {
      /* FET conducts when the voltage at the drain reaches low level */
      while (ADC_PIN & Drain_ADC);
    }
    else                                /* p-channel */
    {
      /* FET conducts when the voltage at the drain reaches high level */
      while (!(ADC_PIN & Drain_ADC));
    }

    R_DDR = Drain_Rl;                   /* set probe-3 to HiZ mode */

    /* get voltage of gate */
    ADCSRA |= (1 << ADSC);              /* start ADC conversion */
    while (ADCSRA & (1 << ADSC));       /* wait until conversion is done */

    /* add ADC reading */
    if (Type & TYPE_N_CHANNEL)          /* n-channel */
    {
      Ugs += ADCW;                        /* Ugs = U_g */
    }
    else                                /* p-channel */
    {
      Ugs -= (1023 - ADCW);               /* Ugs = - (Vcc - U_g) */
    }
  }

  /* calculate V_th */
  Ugs /= 10;                     /* average of 10 samples */
  Ugs *= Cfg.Vcc;                /* convert to voltage */
  Ugs /= 1024;                   /* using 10 bit resolution */

  /* save data */
  Semi.U_2 = (int16_t)Ugs;       /* gate threshold voltage (in mV) */

  /* update reference source for next ADC run */
  Cfg.Ref = ADC_REF_VCC;         /* we've used Vcc as reference */
}



/*
 *  measure hFE of BJT in common collector circuit (emitter follower)
 *
 *  requires:
 *  - Type: NPN or PNP
 *
 *  returns:
 *  - hFE
 */

uint32_t Get_hFE_C(uint8_t Type)
{
  uint32_t          hFE;           /* return value */
  #ifndef NO_HFE_C_RL
  uint32_t          hFE2;          /* temp. hFE value */
  #endif
  uint16_t          U_R_e;         /* voltage across emitter resistor */
  uint16_t          U_R_b;         /* voltage across base resistor */
  uint16_t          Ri;            /* internal resistance of MCU */
  #ifdef SW_HFE_CURRENT
  int32_t           I_e;           /* emitter current */
  #endif


  /*
   *  measure hFE for a BJT in common collector circuit
   *  (emitter follower):
   *  - hFE = (I_e - I_b) / I_b
   *  - measure the voltages across the resistors and calculate the currents
   *    (resistor values are well known)
   *  - hFE = ((U_R_e / R_e) - (U_R_b / R_b)) / (U_R_b / R_b)
   */

  #ifndef NO_HFE_C_RL

  /*
   *  First use Rl as base resistor, and then Rh if base voltage is quite low.
   *  Keep higher hFE value.
   */

  /*
   *  set up probes and get voltages using Rl as base resistor
   */

  if (Type == TYPE_NPN)            /* NPN */
  {
    /* we assume: probe-1 = C / probe-2 = E / probe-3 = B */
    /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- Rl -- Vcc */
    ADC_DDR = Probes.Pin_1;             /* set probe-1 to output */
    ADC_PORT = Probes.Pin_1;            /* pull up collector directly */
    R_DDR = Probes.Rl_2 | Probes.Rl_3;  /* select Rl for probe-2 & Rl for probe-3 */
    R_PORT = Probes.Rl_3;               /* pull up base via Rl */

    U_R_e = ReadU_5ms(Probes.Ch_2);          /* U_R_e = U_e */
    U_R_b = Cfg.Vcc - ReadU(Probes.Ch_3);    /* U_R_b = Vcc - U_b */

    #ifdef SW_HFE_CURRENT
    I_e = (int32_t)U_R_e;               /* U_R_e (mV) */
    Ri = NV.RiL;                        /* RiL (0.1 Ohms) */
    #endif
  }
  else                             /* PNP */
  {
    /* we assume: probe-1 = E / probe-2 = C / probe-3 = B */
    /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / Gnd -- Rl -- probe-3 */
    ADC_PORT = 0;                       /* set ADC port low */
    ADC_DDR = Probes.Pin_2;             /* pull down collector directly */
    R_PORT = Probes.Rl_1;               /* pull up emitter via Rl */
    R_DDR = Probes.Rl_1 | Probes.Rl_3;  /* pull down base via Rl */

    U_R_e = Cfg.Vcc - ReadU_5ms(Probes.Ch_1);     /* U_R_e = Vcc - U_e */
    U_R_b = ReadU(Probes.Ch_3);                   /* U_R_b = U_b */

    #ifdef SW_HFE_CURRENT
    I_e = -(int32_t)U_R_e;              /* negative U_R_e (mV) */
    Ri = NV.RiH;                        /* RiH (0.1 Ohms) */
    #endif
  }


  /*
   *  Both resistors are about the same (R_e = R_b):
   *  - hFE = ((U_R_e / R_e) - (U_R_b / R_b)) / (U_R_b / R_b)
   *  -     = (U_R_e - U_R_b) / U_R_b
   */

  if (U_R_b == 0) U_R_b = 1;            /* prevent division by zero */

  hFE = (uint32_t)((U_R_e - U_R_b) / U_R_b);

  #ifdef SW_HFE_CURRENT
  /* calculate emitter current: I_e = U_R_e / (Rl + Ri) */
  /* NPN: Ri = RiL / PNP: Ri = RiH */
  I_e *= 10000;                    /* scale to 0.1 �V (for �A and Ri's 0.1 Ohms */
  Ri += (R_LOW * 10);              /* Ri + Rl (in 0.1 Ohms) */
  I_e /= Ri;                       /* / (Rl + Ri) */
  Semi.U_2 = (int16_t)I_e;         /* save I_e (in �A) */
  #endif


  /*
   *  for a possible high gain BJT or Darlington change the base resistor
   *  from Rl to Rh and measure again
   */

  if (U_R_b <= 15)            /* I_b <= 21 �A */
  {
    if (Type == TYPE_NPN)          /* NPN */
    {
      /* change probes: probe-3 -- Rh -- Vcc */
      R_DDR = Probes.Rl_2 | Probes.Rh_3;     /* select Rl for probe-2 & Rh for probe-3 */
      R_PORT = Probes.Rh_3;                  /* pull up base via Rh */

      U_R_e = ReadU_5ms(Probes.Ch_2);             /* U_R_e = U_e */
      U_R_b = Cfg.Vcc - ReadU(Probes.Ch_3);       /* U_R_b = Vcc - U_b */

      #ifdef SW_HFE_CURRENT
      I_e = (int32_t)U_R_e;             /* U_R_e (mV) */
      #endif

      Ri = NV.RiL;                      /* get internal resistance */
    }
    else                           /* PNP */
    {
      /* change probes: Gnd -- Rh -- probe-3 */
      R_DDR = Probes.Rl_1 | Probes.Rh_3;     /* pull down base via Rh */

      U_R_e = Cfg.Vcc - ReadU_5ms(Probes.Ch_1);   /* U_R_e = Vcc - U_e */
      U_R_b = ReadU(Probes.Ch_3);                 /* U_R_b = U_b */

      #ifdef SW_HFE_CURRENT
      I_e = -(int32_t)U_R_e;            /* negative U_R_e (mV) */
      #endif

      Ri = NV.RiH;                      /* get internal resistance */
    }

    /*
     *  Since I_b is so small vs. I_e we'll neglect it and use
     *  hFE = I_e / I_b
     *      = (U_R_e / R_e) / (U_R_b / R_b)
     *      = (U_R_e * R_b) / (U_R_b * R_e)
     */

    if (U_R_b == 0) U_R_b = 1;          /* prevent division by zero */
    hFE2 = U_R_e * R_HIGH;              /* U_R_e * R_b */
    hFE2 /= U_R_b;                      /* / U_R_b */
    hFE2 *= 10;                         /* upscale to 0.1 */
    Ri += (R_LOW * 10);                 /* R_e = Ri + Rl (in 0.1 Ohms) */
    hFE2 /= Ri;                         /* / R_e in 0.1 Ohms */

    /* keep the higher hFE */
    if (hFE2 > hFE)                     /* hFE_Rh higher */
    {
      hFE = hFE2;                       /* update hFE */
      #ifdef SW_HFE_CURRENT
      /* calculate emitter current: I_e = U_R_e / (Rl + Ri) */
      /* NPN: Ri = RiL / PNP: Ri = RiH */
      I_e *= 10000;                /* scale to 0.1 �V (for �A and Ri's 0.1 Ohms */
      I_e /= Ri;                   /* / (Rl + Ri) */
      Semi.U_2 = (int16_t)I_e;     /* update I_e (in �A) */
      #endif
    }
  }

  #else

  /*
   *  Skip measurement with Rl as base resistor and use only Rh. A poorly
   *  designed tester clone causes U_R_b to be too low when using Rl. :(
   */

  /*
   *  set up probes and get voltages using Rh as base resistor
   */

  if (Type == TYPE_NPN)            /* NPN */
  {
    /* we assume: probe-1 = C / probe-2 = E / probe-3 = B */
    /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- Rh -- Vcc */
    ADC_DDR = Probes.Pin_1;             /* set probe-1 to output */
    ADC_PORT = Probes.Pin_1;            /* pull up collector directly */
    R_DDR = Probes.Rl_2 | Probes.Rh_3;  /* select Rl for probe-2 & Rh for probe-3 */
    R_PORT = Probes.Rh_3;               /* pull up base via Rh */

    U_R_e = ReadU_5ms(Probes.Ch_2);               /* U_R_e = U_e */
    U_R_b = Cfg.Vcc - ReadU(Probes.Ch_3);         /* U_R_b = Vcc - U_b */

    #ifdef SW_HFE_CURRENT
    I_e = (int32_t)U_R_e;               /* U_R_e (mV) */
    #endif

    Ri = NV.RiL;                        /* get internal resistance */
  }
  else                             /* PNP */
  {
    /* we assume: probe-1 = E / probe-2 = C / probe-3 = B */
    /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / Gnd -- Rh -- probe-3 */
    ADC_PORT = 0;                       /* set ADC port low */
    ADC_DDR = Probes.Pin_2;             /* pull down collector directly */
    R_DDR = Probes.Rl_1 | Probes.Rh_3;  /* pull down base via Rh */
    R_PORT = Probes.Rl_1;               /* pull up emitter via Rl */

    U_R_e = Cfg.Vcc - ReadU_5ms(Probes.Ch_1);     /* U_R_e = Vcc - U_e */
    U_R_b = ReadU(Probes.Ch_3);                   /* U_R_b = U_b */

    #ifdef SW_HFE_CURRENT
    I_e = -(int32_t)U_R_e;              /* negative U_R_e (mV) */
    #endif

    Ri = NV.RiH;                        /* get internal resistance */
  }

  /*
   *  Since I_b is so small vs. I_e we'll neglect it and use
   *  hFE = I_e / I_b
   *      = (U_R_e / R_e) / (U_R_b / R_b)
   *      = (U_R_e * R_b) / (U_R_b * R_e)
   */

  if (U_R_b == 0) U_R_b = 1;            /* prevent division by zero */
  hFE = U_R_e * R_HIGH;                 /* U_R_e * R_b */
  hFE /= U_R_b;                         /* / U_R_b */
  hFE *= 10;                            /* upscale to 0.1 */
  Ri += (R_LOW * 10);                   /* R_e = Ri + Rl (in 0.1 Ohms) */
  hFE /= Ri;                            /* / R_e in 0.1 Ohms */

  #ifdef SW_HFE_CURRENT
  /* calculate emitter current: I_e = U_R_e / (Rl + Ri) */
  /* NPN: Ri = RiL / PNP: Ri = RiH */
  I_e *= 10000;                    /* scale to 0.1 �V (for �A and Ri's 0.1 Ohms */
  I_e /= Ri;                       /* / (Rl + Ri) */
  Semi.U_2 = (int16_t)I_e;         /* save I_e (in �A) */
  #endif

  #endif

  return hFE;
}



/*
 *  check for BJT, enhancement-mode MOSFET and IGBT
 *  - sets hFE test circuit type in Semi.Flags
 *
 *  requires:
 *  - BJT_Type: NPN or PNP (also used for FET channel type)
 *  - U_Rl: voltage across Rl pulled down
 */

void CheckTransistor(uint8_t BJT_Type, uint16_t U_Rl)
{
  uint8_t           FET_Type;      /* MOSFET type */
  uint8_t           Flag = 0;      /* flag */
  uint16_t          U_R_c;         /* voltage across collector resistor */
  uint16_t          U_R_b;         /* voltage across base resistor */
  uint16_t          BJT_Level;     /* voltage threshold for BJT */
  uint16_t          FET_Level;     /* voltage threshold for FET */
  uint16_t          Ri;            /* internal resistance of MCU */
  uint32_t          hFE_C;         /* hFE (common collector) */
  uint32_t          hFE_E;         /* hFE (common emitter) */
  #ifdef SW_HFE_CURRENT
  int32_t           I_c;           /* collector current */
  #endif

  /*
   *  init, set probes and measure
   */

  BackupProbes();             /* save current probe IDs */

  if (BJT_Type == TYPE_NPN)   /* NPN / n-channel */
  {
    BJT_Level = 2557;         /* voltage across base resistor (5.44�A) */
    FET_Level = 3400;         /* voltage across drain resistor (4.8mA) */
    FET_Type = TYPE_N_CHANNEL;

    /*
     *  we assume
     *  - BJT: probe-1 = C / probe-2 = E / probe-3 = B
     *  - FET: probe-1 = D / probe-2 = S / probe-3 = G
     *  probes already set to:
     *    Gnd -- probe-2 / probe-1 -- Rl -- Vcc / (probe-3 -- Rl -- Vcc)
     *  drive base/gate via Rh instead of Rl
     */

    R_DDR = Probes.Rl_1 | Probes.Rh_3;       /* enable Rl for probe-1 & Rh for probe-3 */
    R_PORT = Probes.Rl_1 | Probes.Rh_3;      /* pull up collector via Rl and base via Rh */
    wait50ms();                              /* wait to skip gate charging of a FET */
    U_R_c = Cfg.Vcc - ReadU(Probes.Ch_1);    /* U_R_c = Vcc - U_c */
    U_R_b = Cfg.Vcc - ReadU(Probes.Ch_3);    /* U_R_b = Vcc - U_b */

    #ifdef SW_HFE_CURRENT
    I_c = (int32_t)U_R_c;          /* U_R_c (mV) */
    #endif

    Ri = NV.RiH;                   /* RiH (0.1 Ohms) */
  }
  else                        /* PNP / p-channel */
  {
    BJT_Level = 977;          /* voltage across base resistor (2.1�A) */
    FET_Level = 2000;         /* voltage across drain resistor (2.8mA) */
    FET_Type = TYPE_P_CHANNEL;

    /*
     *  we assume
     *  - BJT: probe-1 = E / probe-2 = C / probe-3 = B
     *  - FET: probe-1 = S / probe-2 = D / probe-3 = G
     *  probes already set to:
     *    Gnd -- Rl - probe-2 / probe-1 -- Vcc / (probe-3 -- Rl -- Gnd)
     *  drive base/gate via Rh instead of Rl
     */

    R_DDR = Probes.Rl_2 | Probes.Rh_3;  /* pull down base via Rh */
    U_R_c = ReadU_5ms(Probes.Ch_2);     /* U_R_c = U_c */
    U_R_b = ReadU(Probes.Ch_3);         /* U_R_b = U_b */

    #ifdef SW_HFE_CURRENT
    I_c = -(int32_t)U_R_c;         /* negative U_R_c (mV) */
    #endif

    Ri = NV.RiL;                   /* RiL (0.1 Ohms) */
  }


  /*
   *  distinguish BJT from enhancement-mode MOSFET or IGBT
   */

  if (U_R_b > BJT_Level)      /* U_R_b exceeds minimum level of BJT */
  {
    /*
     *  A voltage drop across the base resistor Rh means that a current
     *  is flowing constantly. So this can't be a FET.
     *
     *  Problem:
     *  A reversed collector and emitter also passes the tests, but with a
     *  lower hFE. So we take the results of the test run with the higher
     *  hFE. A freewheeling diode might prevent one test run.
     *  Another possible solution: V_f for collector junction is slightly
     *  lower than for emitter junction (larger area for collector -> lower R).
     */

    if (Check.Found == COMP_BJT)        /* second test run */
    {
      /*
       *  If the type is different from the one in the first run, we have
       *  a parasitic BJT (caused by a freewheeling diode on the same substrate).
       */

      if (!(Check.Type & BJT_Type))     /* different type */
      {
        Check.Type |= TYPE_PARASITIC;   /* set flag */
      }

      /*
       *  If we have an NPN and the collector/emitter pins are the same for both
       *  test runs we could have a TRIAC which wasn't detected (sufficient gate
       *  trigger current, but insufficient holding current).
       */

      else if (BJT_Type == TYPE_NPN)   /* same type and NPN */
      {
        if (Semi.B == Probes.ID_1)     /* same pin for collector */
        {
          /* possibly a TRIAC with a too high holding current */
          Check.Found = COMP_ERROR;     /* signal error */
          Check.Type = TYPE_DETECTION;  /* detection problem */
        }
      }

      Check.Done |= DONE_SEMI;          /* no more tests needed */
    }
    else                                /* first test run */
    {
      Check.Found = COMP_BJT;           /* seems to be a BJT */
      Check.Type = BJT_Type;            /* save BJT type */
    }


    /*
     *  Calculate hFE via voltages and known resistors:
     *  - common emitter circuit
     *  - hFE = I_c / I_b
     *        = (U_R_c / R_c) / (U_R_b / R_b)
     *        = (U_R_c * R_b) / (U_R_b * R_c)
     *  - consider leakage current:
     *    I_c = I_c_conducting - I_c_leak
     *        = (U_R_c_conducting / R_c) - (U_R_c_leak / R_c)
     *        = (U_R_c_conducting - U_R_c_leak) / R_c
     *    -> U_R_c = U_R_c_conducting - U_R_c_leak
     *             = U_R_c_conducting - U_Rl
     */

    if (U_R_c > U_Rl) U_R_c -= U_Rl;    /* - U_Rl (leakage) */
    hFE_E = U_R_c * R_HIGH;             /* U_R_c * R_b */
    hFE_E /= U_R_b;                     /* / U_R_b */
    hFE_E *= 10;                        /* upscale to 0.1 */
    Ri += (R_LOW * 10);                 /* R_c = Ri + Rl (in 0.1 Ohms) */
    hFE_E /= Ri;                        /* / R_c in 0.1 Ohms */

    Flag = HFE_COMMON_EMITTER;          /* common emitter circuit */

    #ifdef SW_HFE_CURRENT
    /* calculate collector current: I_c = U_R_c / (Rl + Ri) */
    /* NPN: Ri = RiH / PNP: Ri = RiL */
    I_c *= 10000;                  /* scale to 0.1 �V (for �A and Ri's 0.1 Ohms */
    I_c /= Ri;                     /* / (Rl + Ri) */
    #endif

    /* get hFE for common collector circuit */
    hFE_C = Get_hFE_C(BJT_Type);

    /* keep higher hFE of both test circuits */
    if (hFE_C > hFE_E)                  /* hFE_C larger */
    {
      hFE_E = hFE_C;                    /* take hFE_C */
      Flag = HFE_COMMON_COLLECTOR;      /* common collector circuit */
      #ifdef SW_HFE_CURRENT
      I_c = (int32_t)Semi.U_2;          /* take I_e from hFE_C measurement */
      #endif
    }

    /* update logic */
    FET_Type = 0;             /* use as update flag */

    if (hFE_E > Semi.F_1)     /* hFE not set yet or higher than old one */
    {
      FET_Type = 1;           /* update BJT data */

      #ifdef SW_REVERSE_HFE
      Semi.F_2 = Semi.F_1;    /* update reverse hFE with old value */
                              /* old value is 0 for first BJT test run */
      #endif
    }
    #ifdef SW_REVERSE_HFE
    else                      /* hFE lower than old one */
    {
      Semi.F_2 = hFE_E;       /* update reverse hFE with current value */
    }
    #endif


    /*
     *  parasitic BJT
     */

    /* measure Base-Emitter capacitance */
    if (BJT_Type == TYPE_NPN)           /* NPN */
    {
      MeasureCap(Probes.ID_2, Probes.ID_3, 0);    /* E-B */
    }
    else                                /* PNP */
    {
      MeasureCap(Probes.ID_3, Probes.ID_1, 0);    /* B-E */
    }

    RestoreProbes();               /* restore original probe IDs */

    if (Check.Type & TYPE_PARASITIC)    /* parasitic BJT */
    {
      /*
       *  Detect the correct type:
       *  - The correct type has a larger Base-Emitter capacitance.
       */

      if (CmpValue(Semi.C_value, Semi.C_scale, Caps[0].Value, Caps[0].Scale) == 1)
      {
        /* C_BE of old type is larger -> keep the old type */
        FET_Type = 0;                   /* don't update data */
      }
      else
      {
        /* C_BE of old type is lower -> take the new type */
        FET_Type = 1;                             /* update data */
        Check.Type = BJT_Type | TYPE_PARASITIC;   /* update type */
      }
    }


    /*
     *  update data
     */

    if (FET_Type > 0)              /* update requested */
    {
      GetLeakageCurrent(0);             /* get leakage current */

      /* save data */
      Semi.F_1 = hFE_E;                 /* hFE */
      Semi.Flags = Flag;                /* hFE circuit type */
      #ifdef SW_HFE_CURRENT
      Semi.U_3 = (int16_t)I_c;          /* I_c/I_e (in �A) */
      #endif
      Semi.C_value = Caps[0].Value;     /* B-E/E-B capacitance */
      Semi.C_scale = Caps[0].Scale;
      Semi.A = Probes.ID_3;             /* probe ID for base */

      /* update Collector and Emitter pins */
      if (BJT_Type == TYPE_NPN)    /* NPN */
      {
        Semi.B = Probes.ID_1;      /* probe ID for collector */
        Semi.C = Probes.ID_2;      /* probe ID for emitter */

        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_BJT_NPN;  /* set symbol ID */
        #endif
      }
      else                         /* PNP */
      {
        Semi.B = Probes.ID_2;      /* probe ID for collector */
        Semi.C = Probes.ID_1;      /* probe ID for emitter */

        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_BJT_PNP;  /* set symbol ID */
        #endif
      }
    }
  }
  else if ((U_Rl < 97) && (U_R_c > FET_Level))    /* MOSFET */
  {
    /*
     *  If there's
     *  - just a small leakage current (< 0.1mA) in non-conducting mode
     *  - a large U_R_c (= large current) when conducting
     *  - a low U_R_b (= very low gate current)
     *  we got a FET or an IGBT.
     */

    /*
     *  The drain source channel of a MOSFET is modeled as a resistor
     *  while an IGBT acts more like a diode. So we measure the voltage drop
     *  across the conducting path. A MOSFET got a low voltage drop based on
     *  it's R_DS_on and the current. An IGBT got a much higher voltage drop.
     */

    FET_Level = ReadU(Probes.Ch_1) - ReadU(Probes.Ch_2);

    if (FET_Level < 250)      /* MOSFET */
    {
      Check.Found = COMP_FET;
      Check.Type = FET_Type | TYPE_ENHANCEMENT | TYPE_MOSFET;

      /*
       *  get R_DS_on:
       *  - R = U / I
       *    for n-ch:
       *      U = U_DS = U_D - U_S = FET_Level
       *      I = U_Rl / Rl = (5V - U_D) / Rl = U_R_c / Rl
       *    for p-ch:
       *      U = U_SD = U_S - U_D = FET_Level
       *      I = U_Rl / Rl = U_D / Rl = U_R_c / Rl
       *  -> R_DS_on = FET_Level * Rl / U_R_c
       */

      BJT_Level = R_LOW * 10;                /* Rl in 0.1 Ohms */

      if (FET_Type == TYPE_N_CHANNEL)        /* n-channel */
      {
        BJT_Level += NV.RiH;                 /* Rl + RiH (0.1 Ohms) */

        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_MOSFET_ENH_N;  /* set symbol ID */
        #endif
      }
      else                                   /* p-channel */
      {
        BJT_Level += NV.RiL;                 /* Rl + RiL (0.1 Ohms) */

        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_MOSFET_ENH_P;  /* set symbol ID */
        #endif
      }

      FET_Level *= 10;                            /* scale to 0.1 mV */
      hFE_C = (uint32_t)FET_Level * BJT_Level;    /* U_DS * Rl (0.1 Ohms * 0.1 mV) */
      hFE_C /= U_R_c;                             /* / U_Rl (0.01 Ohms) */
      FET_Level = (uint16_t)hFE_C;                /* todo: check for upper limit */

      /* probe offset */
      #ifdef R_MULTIOFFSET
      uint8_t            n;
      /* get index number for Source/Drain probe pair */
      n = GetOffsetIndex(Probes.ID_1, Probes.ID_2);
      BJT_Level = NV.RZero[n];
      #else
      BJT_Level = NV.RZero;
      #endif

      if (FET_Level > BJT_Level)        /* large enough */
      {
        FET_Level -= BJT_Level;         /* subtract probe resistance */
      }
      else                              /* too small */
      {
        FET_Level = 0;                  /* set to zero */
      }

      Semi.U_1 = FET_Level;             /* save R_DS_on (0.01 Ohms) */
    }
    else                      /* IGBT */
    {
      Check.Found = COMP_IGBT;
      Check.Type = FET_Type | TYPE_ENHANCEMENT;

      #ifdef SW_SYMBOLS
      if (FET_Type == TYPE_N_CHANNEL)        /* n-channel */
      {
        Check.Symbol = SYMBOL_IGBT_ENH_N;    /* set symbol ID */
      }
      else                                   /* p-channel */
      {
        Check.Symbol = SYMBOL_IGBT_ENH_P;    /* set symbol ID */
      }
      #endif
    }

    GetGateThreshold(FET_Type);    /* measure gate threshold voltage */

    Check.Done |= DONE_SEMI;       /* transistor detected */

    /* save data */
    Semi.A = Probes.ID_3;           /* probe ID for gate */

    if (FET_Type == TYPE_N_CHANNEL)     /* n-channel */
    {
      Semi.B = Probes.ID_1;        /* probe ID for drain */
      Semi.C = Probes.ID_2;        /* probe ID for source */
    }
    else                                /* p-channel */
    {
      Semi.B = Probes.ID_2;        /* probe ID for drain */
      Semi.C = Probes.ID_1;        /* probe ID for source */
    }

    /* Gate-Source capacitance */
    MeasureCap(Semi.A, Semi.C, 0);      /* measure capacitance */
    Semi.C_value = Caps[0].Value;       /* save value */
    Semi.C_scale = Caps[0].Scale;
    RestoreProbes();                    /* restore original probe IDs */
  }
}



/*
 *  check for a depletion mode FET (self conducting)
 *  - JFET or dep. mode MOSFET
 *
 *  requires:
 *  - U_Rl: voltage across Rl pulled down (Gate HiZ)
 */

void CheckDepletionModeFET(uint16_t U_Rl)
{
  uint16_t          Offset;        /* offset voltage */
  uint16_t          U_1;           /* voltage #1 */
  uint16_t          U_2;           /* voltage #2 */
  uint16_t          Diff_1 = 0;    /* voltage difference #1 */
  uint16_t          Diff_2 = 0;    /* voltage difference #2 */
  uint8_t           Flag = 0;      /* control and signal flag */

  /* local constants */
  #define SIGNAL_NONE    0         /* no signal set */
  #define SIGNAL_OTHER   1         /* some other component */
  #define SIGNAL_FET     2         /* depletion-mode FET */


  /*
   *  required probe setup (by calling function):
   *  - Gnd -- Rl -- probe-2 / probe-1 -- Vcc
   *
   *  MOSFETs require a single pass detection because of the intrinsic
   *  flyback diode. JFETs don't got that diode, but some rare types
   *  come with an integrated flyback diode.
   */


  /*
   *  select detection offset based on U_Rl
   */

  if (U_Rl > 490)         /* > 700�A (was 92mV/130�A) */
  {
    /*
     *  Standard JFET or depletion mode MOSFET
     *
     *  The high threshold allows us to skip most germanium BJTs with a high
     *  leakage current (I_CEO), but not all.
     */

    Offset = 488;         /* 0.7mA */
  }
  else                    /* > 21�A (15mV) */
  {
    /* JFET with low I_DSS */
    Offset = 40;          /* 57�A */
  }


  /*
   *  check if we got an n-channel JFET or depletion-mode MOSFET
   *  - JFETs are depletion-mode only
   */

  /* we assume: probe-1 = D / probe-2 = S / probe-3 = G */
  /* probes already set to: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */

  /* get source voltage when gate is pulled down */
  /* create a negative V_GS via voltage drop across Rl at source */
  /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- Rh -- Gnd */
  R_DDR = Probes.Rl_2 | Probes.Rh_3;    /* pull down gate via Rh */
  U_1 = ReadU_20ms(Probes.Ch_2);        /* voltage at source */

  /* get source voltage when gate is pulled up */
  /* create a slightly positive V_GS via voltage drop across Rl at source */
  /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- Rh -- Vcc */
  R_PORT = Probes.Rh_3;                 /* pull up gate via Rh */
  U_2 = ReadU_20ms(Probes.Ch_2);        /* voltage at source */
  Diff_1 = U_2 - U_1;                   /* source voltage difference */
  /* underun doesn't matter because of next "if" condition */


  /*
   *  If the source voltage is higher (more current), when the gate is driven
   *  by a positive voltage vs. connected to ground, we got a depletion-mode
   *  n-channel FET.
   */

  if (U_2 > (U_1 + Offset))   /* exceeds required offset */
  {
    /*
     *  n-channel depletion-mode FET
     *  we assume: probe-1 = D / probe-2 = S / probe-3 = G
     */

    /*
     *  Check for Darlington BJT plus EMI issues causing a high U_Rl.
     *  - with base pulled down we should have a low leakage current
     *  - assumes: probe-1 = C / probe-2 = E / probe-3 = B
     */

    if (U_1 <= 5)             /* emitter shunt <= 5mV (7�A) */
    {
      Flag = SIGNAL_OTHER;    /* most likely a Darlington BJT */
    }


    /*
     *  Check if we might have a Germanium BJT with a high leakage current:
     *  - the base-emitter junction of a BJT should pass a constant current with
     *    a low V_BE
     *  - measure V_BE
     *  - voltage drop across RiL at emitter is about 150mV
     *  - assumes: probe-1 = C / probe-2 = E / probe-3 = B
     *  - NPN
     */

    /* get base voltage when base is pulled up by Rl */
    /* set probes: Gnd -- probe-2 / probe-3 -- Rl -- Vcc / probe-1 -- HiZ */
    R_DDR = Probes.Rl_3;                     /* switch to Rl for base */
    R_PORT = Probes.Rl_3;                    /* pull up base via Rl */
    ADC_PORT = 0;
    ADC_DDR = Probes.Pin_2;                  /* pull down emitter directly */
    U_1 = ReadU_20ms(Probes.Ch_3);           /* get voltage at base */
    ADC_DDR = 0;

    if ((U_1 < 450 + SHUNT_DROP_MV) &&        /* low V_BE < (450mV + 150mV) */
        (U_1 > 100))                          /* but not too low 100mV */
    {
      Flag = SIGNAL_OTHER;                    /* most likely a Ge BJT */
    }


    /*
     *  Check if we might have a Schottky-clamped BJT with a Schottky diode
     *  between base and collector:
     *  - measure V_f of Schottky diode
     *  - assumes: probe-1 = C / probe-2 = E / probe-3 = B
     *  - NPN
     */

    /* get base voltage when base is pulled up by Rl */
    /* set probes: Gnd -- probe-1 / probe-3 -- Rl -- Vcc / probe-2 -- HiZ */
    ADC_DDR = Probes.Pin_1;                  /* pull down collector directly */
    U_1 = ReadU_20ms(Probes.Ch_3);           /* get voltage at base */
    U_2 = ReadU(Probes.Ch_1);                /* get voltage at collector */
    ADC_DDR = 0;

    if (U_1 > U_2)          /* valid voltages */
    {
      U_1 -= U_2;           /* V_f = U_B - U_C */

      if (U_1 < 450)        /* low V_f < 450mV */
      {
        Flag = SIGNAL_OTHER;       /* most likely a Schottky BJT */
      }
    }


    /*
     *  Detect drain and source by a 2nd measurement with reversed
     *  drain and source pins.
     */

    /* we simulate: probe-1 = S / probe-2 = D / probe-3 = G */

    /* get source voltage when gate is pulled down */
    /* set probes: Gnd -- Rl -- probe-1 / probe-2 -- Vcc / probe-3 -- Rh -- Gnd */
    ADC_PORT = Probes.Pin_2;            /* set ADC port to high */
    ADC_DDR = Probes.Pin_2;             /* pull up drain directly */
    R_DDR = Probes.Rl_1 | Probes.Rh_3;  /* enable Rl for source and Rh for gate */
    R_PORT = 0;                         /* pull down source via Rl / pull down gate via Rh */
    U_1 = ReadU_20ms(Probes.Ch_1);      /* voltage at source */

    /* get source voltage when gate is pulled up */
    /* set probes: Gnd -- Rl -- probe-1 / probe-2 -- Vcc / probe-3 -- Rh -- Vcc */
    R_PORT = Probes.Rh_3;               /* pull up gate via Rh */
    U_2 = ReadU_20ms(Probes.Ch_1);      /* voltage at source */
    Diff_2 = U_2 - U_1;                 /* source voltage difference */

    #if 0
    /* todo: prevent underun? */
    if (U_2 > U_1)                      /* prevent underun */
    {
      Diff_2 = U_2 - U_1;               /* source voltage difference */
    }
    else                                /* underun */
    {
      Diff_2 = 0;                       /* set zero */
    }
    #endif


    /*
     *  Compare gate voltages to distinguish JFET from MOSFET
     */

    /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / probe-3 -- Rh - Vcc */
    ADC_PORT = 0;                        /* set ADC port to low */
    ADC_DDR = Probes.Pin_2;              /* pull down source directly */
    R_DDR = Probes.Rl_1 | Probes.Rh_3;   /* enable Rl for drain & Rh for gate */
    R_PORT = Probes.Rl_1 | Probes.Rh_3;  /* pull up drain via Rl / pull up gate via Rh */

    U_1 = ReadU_20ms(Probes.Ch_3);       /* get voltage at gate */

    if (U_1 > 3911)           /* MOSFET */
    {
      /* n channel depletion-mode MOSFET */
      Check.Type = TYPE_N_CHANNEL | TYPE_DEPLETION | TYPE_MOSFET;
      #ifdef SW_SYMBOLS
      Check.Symbol = SYMBOL_MOSFET_DEP_N;    /* set symbol ID */
      #endif

      Flag = SIGNAL_FET;      /* signal match */
    }
    else                      /* JFET */
    {
      if (Flag == 0)          /* not a BJT */
      {
        /* n channel JFET (depletion-mode only) */
        Check.Type = TYPE_N_CHANNEL | TYPE_DEPLETION | TYPE_JFET;
        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_JFET_N;        /* set symbol ID */
        #endif

        Flag = SIGNAL_FET;    /* signal match */
      }
    }
  }


  /*
   *  check if we got a p-channel JFET or depletion-mode MOSFET
   *  - JFETs are depletion-mode only
   */

  if (Flag != SIGNAL_FET)     /* no n-ch dep-mode FET found */
  {
    Flag = SIGNAL_NONE;                 /* reset control flag */

    /*  we assume: probe-1 = S / probe-2 = D / probe-3 = G */

    /* get source voltage when gate is pulled up */
    /* create a positive V_GS via voltage drop across Rl at source */
    /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / probe-3 -- Rh -- Vcc */
    ADC_PORT = 0;                       /* set ADC port to Gnd */
    ADC_DDR = Probes.Pin_2;             /* pull down drain directly */
    R_DDR = Probes.Rl_1 | Probes.Rh_3;  /* enable Rl for probe-1 & Rh for probe-3 */
    R_PORT = Probes.Rl_1 | Probes.Rh_3; /* pull up source via Rl / pull up gate via Rh */
    U_1 = ReadU_20ms(Probes.Ch_1);      /* get voltage at source */

    /* get source voltage when gate is pulled down */
    /* create a slightly negative V_GS via voltage drop across Rl at source */
    /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / probe-3 -- Rh -- Gnd */
    R_PORT = Probes.Rl_1;               /* pull down gate via Rh */
    U_2 = ReadU_20ms(Probes.Ch_1);      /* get voltage at source */
    Diff_1 = U_1 - U_2;                 /* source voltage difference */
    /* underun doesn't matter because of next "if" condition */


    /*
     *  If the source voltage is higher (less current), when the gate is driven
     *  by a positive voltage vs. connected to ground, we got a depletion-mode
     *  p-channel FET.
     */

    if (U_1 > (U_2 + Offset))      /* exceeds required offset */
    {
      /*
       *  p-channel depletion-mode FET
       *  we assume: probe-1 = S / probe-2 = D / probe-3 = G
       */

      /*
       *  Check for Darlington BJT plus EMI issues causing a high U_Rl.
       *  - with base pulled up we should have a low leakage current
       *  - assumes: probe-1 = E / probe-2 = C / probe-3 = B
       */

      U_2 = Cfg.Vcc - 12;          /* threshold: Vcc - 12mV */
        /* meant to be -5, but more leeway because of Vref=Vcc */
      if (U_1 >= U_2)              /* emitter shunt <= 12mV (17�A) */
      {
        Flag = SIGNAL_OTHER;       /* most likely a Darlington BJT */
      }


      /*
       *  Check if we might have a Germanium BJT with a high leakage current:
       *  - the base-emitter junction of a BJT should pass a constant current with
       *    a low V_BE
       *  - measure V_BE
       *  - voltage drop across RiH at emitter is about 150mV
       *  - assumes: probe-1 = E / probe-2 = C / probe-3 = B
       *  - PNP
       */

      /* get base voltage when base is pulled down by Rl */
      /* set probes: Gnd -- Rl -- probe-3 / probe-1 -- Vcc / probe-2 -- HiZ */
      R_DDR = Probes.Rl_3;                   /* switch to Rl */
      R_PORT = 0;                            /* pull down base by Rl */
      ADC_DDR = Probes.Pin_1;
      ADC_PORT = Probes.Pin_1;               /* pull up emitter directly */
      U_1 = ReadU_20ms(Probes.Ch_3);         /* get voltage at base */
      ADC_PORT = 0;

      /* low V_BE < (450mV + 150mV) (V_BE = Vcc - V_B) */
      if ((U_1 > Cfg.Vcc - 450 - SHUNT_DROP_MV) &&
          (U_1 < Cfg.Vcc - 100))       /* but not too low  (Vcc - 100mV) */
      {
        Flag = SIGNAL_OTHER;       /* most likely a Ge BJT */
      }


      #if 0
      /*
       *  Check if we might have a Schottky-clamped BJT with a Schottky diode
       *  between base and collector:
       *  - measure V_f of Schottky diode
       *  - assumes: probe-1 = E / probe-2 = C / probe-3 = B
       *  - PNP
       *  - this check doesn't seem to be necessary
       */

      /* get base voltage when base is pulled down by Rl */
      /* set probes: Gnd -- Rl -- probe-3 / probe-2 -- Vcc / probe-1 -- HiZ */
      ADC_DDR = Probes.Pin_2;
      ADC_PORT = Probes.Pin_2;               /* pull up collector directly */
      U_1 = ReadU_20ms(Probes.Ch_3);         /* get voltage at base */
      U_2 = ReadU(Probes.Ch_2);              /* get voltage at collector */
      ADC_DDR = 0;

      if (U_2 > U_1)          /* valid voltages */
      {
        U_2 -= U_1;           /* V_f = U_C - U_B */

        if (U_2 < 450)        /* low V_f < 450mV */
        {
          Flag = SIGNAL_OTHER;     /* most likely a Schottky BJT */
        }
      }
      #endif


      /*
       *  Detect drain and source by a 2nd measurement with reversed
       *  drain and source pins.
       */

      /* we simulate: probe-1 = D / probe-2 = S / probe-3 = G */

      /* get source voltage when gate is pulled up */
      /* set probes: Gnd -- probe-1 / probe-2 -- Rl -- Vcc / probe-3 -- Rh -- Vcc */
      ADC_DDR = Probes.Pin_1;             /* pull down drain directly */
      R_DDR = Probes.Rl_2 | Probes.Rh_3;  /* enable Rl for source & Rh for gate */
      R_PORT = Probes.Rl_2 | Probes.Rh_3; /* pull up source via Rl / pull up gate via Rh */
      U_1 = ReadU_20ms(Probes.Ch_2);      /* get voltage at source */

      /* get source voltage when gate is pulled down */
      /* set probes: Gnd -- probe-1 / probe-2 -- Rl -- Vcc / probe-3 -- Rh -- Gnd */
      R_PORT = Probes.Rl_2;               /* pull down gate via Rh */
      U_2 = ReadU_20ms(Probes.Ch_2);      /* get voltage at source */
      Diff_2 = U_1 - U_2;                 /* source voltage difference */

      #if 0
      /* todo: prevent underun? */
      if (U_1 > U_2)                      /* prevent underun */
      {
        Diff_2 = U_1 - U_2;               /* source voltage difference */
      }
      else                                /* underun */
      {
        Diff_2 = 0;                       /* set zero */
      }
      #endif

      /* consider intrinsic or optional flyback diode (causes low Diff_2) */
      if (Diff_2 < (Offset / 2))          /* diode */
      {
        /* swap Diff_1 and Diff_2 */
        /* to fix reversed D/S issue caused by diode */
        U_1 = Diff_1;
        Diff_1 = Diff_2;
        Diff_2 = U_1;
      }


      /*
       *  Compare gate voltages to distinguish JFET from MOSFET
       */

      /* set probes: probe-1 -- Vcc / probe-2 -- Rl -- Gnd / probe-3 -- Rh -- Gnd */
      R_PORT = 0;                       /* pull down drain via Rl & gate via Rh */
      ADC_PORT = Probes.Pin_1;          /* pull up source directly */
      ADC_DDR = Probes.Pin_1;           /* enable pull up for source */
      /* gate is still pulled down via Rh */
      U_1 = ReadU_20ms(Probes.Ch_3);    /* get voltage at gate */

      if (U_1 < 977)               /* MOSFET */
      {
        /* p-channel depletion-mode MOSFET */
        Check.Type =  TYPE_P_CHANNEL | TYPE_DEPLETION | TYPE_MOSFET;
        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_MOSFET_DEP_P;  /* set symbol ID */
        #endif

        Flag = SIGNAL_FET;         /* signal match */
      }
      else                         /* JFET */
      {
        if (Flag == SIGNAL_NONE)   /* not a BJT */
        {
          /* p-channel JFET (depletion-mode only) */
          Check.Type = TYPE_P_CHANNEL | TYPE_DEPLETION | TYPE_JFET;
          #ifdef SW_SYMBOLS
          Check.Symbol = SYMBOL_JFET_P;      /* set symbol ID */
          #endif

          Flag = SIGNAL_FET;       /* signal match */
        }
      }
    }
  }


  /*
   *  on match process and save data
   */

  if (Flag == SIGNAL_FET)     /* found depletion-mode FET */
  {
    /* common stuff */
    Check.Found = COMP_FET;        /* it's a FET */
    Check.Done |= DONE_SEMI;       /* transistor detection done */
    Semi.A = Probes.ID_3;          /* probe ID for gate */

    /*
     *  drain & source pinout
     *  - n-ch: larger voltage difference wins
     *  - p-ch: smaller voltage difference wins
     *    But we take the larger one and indirectly reverse D/S pins
     *    (double reverse) to shorten code.
     */

    if (Diff_1 > Diff_2)      /* n-ch: drain and source as assumed */
    {
      Semi.B = Probes.ID_1;        /* probe ID for drain */
      Semi.C = Probes.ID_2;        /* probe ID for source */
    }
    else                      /* n-ch: drain and source reversed */
    {
      Semi.B = Probes.ID_2;        /* probe ID for drain */
      Semi.C = Probes.ID_1;        /* probe ID for source */
    }


    /*
     *  drain & source symmetry
     *  - if both voltage differences are about the same we got a
     *    symmetrical FET
     */

    U_2 = Diff_1 / 50;             /* 2% of Diff_1 */
    U_1 = Diff_1 - U_2;            /* 98% */
    U_2 += Diff_1;                 /* 102% */
    if ((Diff_2 >= U_1) && (Diff_2 <= U_2))   /* within +/- 2% */
    {
      Check.Type |= TYPE_SYMMETRICAL;
    }


    /*
     *  I_DSS (V_GS = 0V)
     */

    /* use: probe-1 = D / probe-2 = S / probe-3 = G */
    UpdateProbes(Semi.B, Semi.C, Semi.A);    /* drain, source, gate */
    ADC_DDR = Probes.Pin_2 | Probes.Pin_3 ;  /* enable direct pull of source & gate */
    R_DDR = Probes.Rl_1;                     /* enable Rl for drain */

    if (Check.Type & TYPE_N_CHANNEL)    /* n-channel */
    {
      /* low-side switching */
      /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / probe-3 -- Gnd */
      ADC_PORT = 0;                     /* pull down source and gate directly */
      R_PORT = Probes.Rl_1;             /* pull up drain via Rl */
      U_1 = Cfg.Vcc - ReadU_20ms(Probes.Ch_1);    /* voltage at drain */
      U_2 = (R_LOW * 10) + NV.RiH;      /* current sense resistor (0.1 Ohms) */
    }
    else                                /* p-channel */
    {
      /* high-side switching */
      /* set probes: Gnd -- Rl -- probe-1 / probe-2 -- Vcc / probe-3 -- Vcc */
      ADC_PORT = Probes.Pin_2 | Probes.Pin_3;   /* pull up source & gate directly */
      R_PORT = 0;                       /* pull down drain via Rl */
      U_1 = ReadU_20ms(Probes.Ch_1);    /* voltage at drain */
      U_2 = (R_LOW * 10) + NV.RiL;      /* current sense resistor (0.1 Ohms) */
    }

    /* I = U / R_sense */
    Semi.I_value = (uint32_t)U_1 * 10000;    /* U (0.1 �V) */
    Semi.I_value /= U_2;                     /* I = U / R (�A) */
    Semi.I_scale = -6;                       /* �A */


    /*
     *  V_GS(off)
     *  - based on idea from Pieter-Tjerk de Boer (PA3FWM)
     */

    /* keep probes as above: probe-1 drain / probe-2 source / probe-3 gate */
    R_DDR = Probes.Rh_2 | Probes.Rh_3;       /* enable Rh for source and gate */
    ADC_DDR = Probes.Pin_1;                  /* pull drain directly */

    if (Check.Type & TYPE_N_CHANNEL)    /* n-channel */
    {
      /* set probes: probe-1 -- Vcc / Gnd -- Rh -- probe-2 / Gnd -- Rh -- probe-3 */
      R_PORT = 0;                            /* pull down source and gate via Rh */
      ADC_PORT = Probes.Pin_1;               /* pull up drain directly */
    }
    else                                /* p-channel */
    {
      /* set probes: Gnd -- probe-1 / probe-2 -- Rh -- Vcc / probe-3 -- Rh -- Vcc */
      R_PORT = Probes.Rh_2 | Probes.Rh_3;    /* pull up source and gate via Rh */
      ADC_PORT = 0;                          /* pull down drain directly */
    }

    /* get voltages at source and gate */
    wait10ms();                         /* setting time */
    U_1 = ReadU(Probes.Ch_2);           /* voltage at source */
    U_2 = ReadU(Probes.Ch_3);           /* voltage at gate */

    /* U_GS(off) = U_G - U_S */
    Semi.U_3 = U_2 - U_1;               /* U_G - U_S (mV) */

    if (Check.Type & TYPE_N_CHANNEL)    /* n-channel */
    {
      /* U_GS(off) should be negative and not exceed -4.8V (too close to Vcc) */
      if ((Semi.U_3 > 0) || (Semi.U_3 < -4800))
      {
        Semi.U_3 = 0;                   /* invalid */
      }
    }
    else                                /* p-channel */
    {
      /* U_GS(off) should be positive and not exceed 4.8V (too close to Vcc) */
      if ((Semi.U_3 < 0) || (Semi.U_3 > 4800))
      {
        Semi.U_3 = 0;                   /* invalid */
      }
    }
  }

  /* clean up local constants */
  #undef SIGNAL_NONE
  #undef SIGNAL_OTHER
  #undef SIGNAL_FET
}



/* ************************************************************************
 *   special devices
 * ************************************************************************ */


/*
 *  check for Thyristor and TRIAC
 *
 *  returns:
 *  - 1 if component was found
 *  - 0 if component wasn't found
 */

uint8_t CheckThyristorTriac(void)
{
  uint8_t           Flag = 0;      /* return value */
  uint16_t          U_1;           /* voltage #1 */
  uint16_t          U_2;           /* voltage #2 */
  uint16_t          V_GT;          /* gate trigger voltage */

  /*
   *  check for a Thyristor (SCR) or TRIAC
   *  - A thyristor conducts also after the gate is discharged as long
   *    as the load current stays alive and doesn't reverse polarity.
   *  - A TRIAC is similar to a pair of anti-parallel thyristors but is
   *    triggered also by negative gate current.
   *  - The tester might not provide enough drive current, so it can't
   *    detect all types.
   */

  /*
   *  probes need to be set already to:
   *    Gnd -- probe-2 / probe-1 -- Rl -- Vcc / probe-3 -- Rl -- Vcc
   */

  /*
   *  we assume:
   *  - SCR: probe-1 = A / probe-2 = C / probe-3 = G
   *  - TRIAC: probe-1 = MT2 / probe-2 = MT1 / probe-3 = G
   */

  /* V_GT (gate trigger voltage) */
  U_1 = ReadU(Probes.Ch_3);             /* voltage at gate */
  U_2 = ReadU(Probes.Ch_2);             /* voltage at cathode */
  V_GT = U_1 - U_2;                     /* = Ug - Uc */

  /* discharge gate and check load current */
  PullProbe(Probes.Rl_3, PULL_10MS | PULL_DOWN);    /* discharge gate */
  U_1 = ReadU_5ms(Probes.Ch_1);         /* get voltage at anode */

  /* simulate short loss of current and check load current again */
  R_PORT = 0;                           /* pull down anode */
  wait5ms();
  R_PORT = Probes.Rl_1;                 /* and pull up anode again */
  U_2 = ReadU_5ms(Probes.Ch_1);         /* get voltage at anode (below Rl) */

  /* voltages at anode match behaviour of Thyristor or TRIAC */
  if ((U_1 < 1600) && (U_2 > 4400))
  {
    /*
     *  Now we check if the DUT is a Thyristor or a TRIAC:
     *  - set Gate to HiZ and reverse Anode & Cathode
     *  - check if DUT doesn't conduct
     *  - trigger gate by pulling it down to Gnd for a short moment
     *  - check if DUT passes current
     *  A Thyrister shouldn't conduct but a TRIAC should (Q3 operation mode).
     */

    /* reverse Anode and Cathode (MT2 and MT1) */
    /* set probes: Gnd -- probe-1 / probe-2 -- Rl -- Vcc  */
    R_DDR = 0;                          /* disable all probe resistors */
    R_PORT = 0;
    ADC_PORT = Probes.Pin_2;            /* pull up Cathode directly */
    wait5ms();
    R_DDR = Probes.Rl_1;                /* pull down Anode via Rl */
    /* probe-3 = gate is in HiZ mode */

    /* check if DUT doesn't conduct */
    U_1 = ReadU_5ms(Probes.Ch_1);       /* get voltage at Anode */

    if (U_1 <= 244)      /* voltage at Anode is low (no current) */
    {
      /* trigger the gate with a negative current (TRIAC: Q3) */
      PullProbe(Probes.Rl_3, PULL_10MS | PULL_DOWN);

      /* check for conduction */
      U_1 = ReadU_5ms(Probes.Ch_1);     /* get voltage at Anode */

      if (U_1 < 733)         /* no current -> Thyristor */
      {
        Check.Found = COMP_THYRISTOR;   /* we found a Thyristor */
        Check.Done |= DONE_SEMI;        /* detected component */
        #ifdef SW_SYMBOLS
        Check.Symbol = SYMBOL_SCR;      /* set symbol ID */
        #endif

        Flag = 2;                       /* save data and signal success */
      }
      else                   /* got current -> TRIAC */
      {
        /*
         *  To verify the TRIAC we stop the current flow for a moment and check
         *  the voltage at MT2 again. The TRIAC shouldn't conduct anymore.
         */

        /* drop load current for a moment */
        R_PORT = Probes.Rl_1;           /* pull up MT2 via Rl */
        wait5ms();
        R_PORT = 0;                     /* and pull down MT2 via Rl */

        /* and check load current again */
        U_2 = ReadU_5ms(Probes.Ch_1);   /* get voltage at MT2 */

        if (U_2 <= 244)       /* no current */
        {
          if (Check.Found == COMP_TRIAC)     /* second test run */
          {
            Check.Done |= DONE_SEMI;         /* no more tests needed */
          }

          Check.Found = COMP_TRIAC;     /* found TRIAC */
          #ifdef SW_SYMBOLS
          Check.Symbol = SYMBOL_TRIAC;  /* set symbol ID */
          #endif

          /*
           *  TRIAC could be in Q3 or Q4 operation mode. If G and MT1 are swapped
           *  the TRIAC would pass the former check in Q4 mode but the current
           *  through MT2 would be a little bit lower. Other issues are that TRIACs
           *  usually have different trigger currents for different operation
           *  quadrants and that some don't support Q4. So we perform up to two test
           *  runs and prefer the one with the higher voltage at MT2.
           */

          if (U_1 > (uint16_t)Semi.F_1)      /* first run or higher current */
          {
            Semi.F_1 = U_1;        /* update reference value */
            Flag = 2;              /* save data and signal success */
          }
          else                     /* wrong pinout */
          {
            Flag = 1;              /* signal success, but don't save data */
          }
        }
      }
    }
  }

  if (Flag == 2)         /* save data and signal success */
  {
    /* save data */
    Semi.A = Probes.ID_3;     /* probe ID for Gate */
    Semi.B = Probes.ID_1;     /* probe ID for Anode/MT2 */
    Semi.C = Probes.ID_2;     /* probe ID for Cathode/MT1 */
    Semi.U_1 = V_GT;          /* gate trigger voltage (in mV) */

    Flag = 1;                 /* signal success */
  }

  return Flag;
}



/*
 *  check for PUT (Programmable Unijunction Transistor)
 */

void CheckPUT(void)
{
  uint16_t          U_1;           /* voltage #1 */
  uint16_t          U_2;           /* voltage #2 */
  uint16_t          U_3;           /* voltage #3 */

  /*
   *  Conduction should have been checked before and we assume
   *  that the PUT is still conducting. Probes should be already
   *  set to: Gnd -- Rl -- probe-2 / probe-1 -- Vcc
   *
   *  we assume: probe-1 = A / probe-2 = C / probe-3 = G
   *
   *  hint:
   *  TRIAC will pass this test too, but PUT won't pass TRIAC check
   *  todo: if Check.Found == TRIAC -> return?
   */

  /* get Uf */
  U_2 = ReadU(Probes.Ch_1);          /* voltage at Anode */
  U_3 = U_2;
  U_1 = ReadU(Probes.Ch_2);          /* voltage at Cathode */
  U_2 -= U_1;                        /* Uf = U_A - U_C */

  /* get offset voltage V_T */
  U_1 = ReadU(Probes.Ch_3);          /* voltage at Gate */
  U_3 -= U_1;                        /* V_T = U_A - U_G */


  /*
   *  If this is a PUT, it should stop conduction when we pull up
   *  the gate carefully.
   */

  /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- Rh -- Vcc */
  ADC_DDR = 0;                          /* set HiZ */
  ADC_PORT = Probes.Pin_1;              /* pull up Anode directly */
  R_PORT = Probes.Rh_3;
  R_DDR = Probes.Rl_2 | Probes.Rh_3;    /* pull up Gate, pull down Cathode */
  MilliSleep(50);                       /* wait */
  ADC_DDR = Probes.Pin_1;               /* enable pull-up for Anode */
  U_1 = ReadU(Probes.Ch_2);             /* get voltage at Rl */

  if (U_1 < 10)             /* no conduction */
  {
    Check.AltFound = COMP_PUT;     /* found PUT */
    /* we don't set Comp.Done in case we'll find some other transistor */

    #ifdef SW_SYMBOLS
    Check.AltSymbol = SYMBOL_PUT;  /* set symbol ID */
    #endif

    /* save data */
    AltSemi.A = Probes.ID_3;       /* probe ID for Gate */
    AltSemi.B = Probes.ID_1;       /* probe ID for Anode */
    AltSemi.C = Probes.ID_2;       /* probe ID for Cathode */
    AltSemi.U_1 = U_2;             /* U_f */
    AltSemi.U_2 = U_3;             /* V_T */
  }
}



#ifdef SW_UJT

/*
 *  check for UJT (Unijunction Transistor)
 */

void CheckUJT(void)
{
  uint8_t           Flag = 0;      /* control flag */
  uint16_t          U_1;           /* voltage */
  uint16_t          U_2;           /* voltage */
  uint16_t          U_3;           /* voltage */

  /*
   *  A UJT has a resistance of about 4-12 kOhms between B2 and B1, and
   *  a PN junction between E and B2/B1 (same N-type semi).
   *  E is nearer B2, i.e. R_B2 < R_B1 when not conducting.
   *
   *  we assume: probe-1 = B2 / probe-2 = B1 / probe-3 = E
   */


  /*
   *  Measure R_BB
   *  - At this point we shouldn't have found any resistors.
   */

  if (Check.Resistors == 0)        /*�no resistors found yet */
  {
    CheckResistor();               /* measure R_BB (between probe-1 and probe-2) */

    if (Check.Resistors == 1)      /* got resistance */
    {
      /* check for 3 - 15 kOhms */
      if (CmpValue(Resistors[0].Value, Resistors[0].Scale, 3000, 0) == 1)
      {
        if (CmpValue(Resistors[0].Value, Resistors[0].Scale, 15000, 0) == -1)
        {
          Flag = 1;      /* more checks required */
        }
      }

      Check.Resistors = 0;         /* reset counter */
    }
  }


  /*
   *  Check for pn junction:
   *  - excludes a potentiometer (3-15 k) with the wiper turned to one end
   *  - the Emitter reverse current is just a few �A (< 20 �A)
   */

  if (Flag)                   /* next check */
  {
    /* measure Emitter reverse current (Rl + RiL as shunt) */
    /* set probes: probe-1 -- Vcc / probe-2 -- HiZ / Gnd -- Rl -- probe-3 */
    R_PORT = 0;
    R_DDR = Probes.Rl_3;                /* pull down E via Rl */
    ADC_PORT = Probes.Pin_1;
    ADC_DDR = Probes.Pin_1;             /* pull up B2 directly */
    U_1 = ReadU_5ms(Probes.Ch_3);       /* voltage at E */

    if (U_1 > 14)                       /* > 14mV (20�A) */
    {
      /* reverse current is too large */
      Flag = 0;                         /* can't be a UJT */
    }
  }


  /*
   *  Check if we can switch the UJT.
   */

  if (Flag)                   /* next check */
  {
    /* switched off, only R_BB */
    /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- HiZ */
                                        /* R_PORT is already 0 */
    R_DDR = Probes.Rl_2;                /* pull down B1 via Rl */
                                        /* B2 is already pulled up directly */
    U_1 = ReadU_5ms(Probes.Ch_2);       /* voltage at B1 */

    /* switched on */
    /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc / probe-3 -- Rl - Vcc */
    R_PORT = Probes.Rl_3;
    R_DDR = Probes.Rl_2 | Probes.Rl_3;  /* and pull up E via Rl */
    U_2 = ReadU_5ms(Probes.Ch_2);       /* voltage at B1 */

    /* compare currents (voltage drop across Rl) */
    if (U_2 > U_1)            /* I_on > I_off */
    {
      U_3 = U_2 - U_1;        /* get difference */
      if (U_3 > 1000)         /* more than 1V (1.4mA) */
      {
        /*
         *  This seems to be an UJT, but we need a second test run with
         *  reversed B1-B2 to find the proper pinout.
         */

        if (Check.AltFound == COMP_UJT) /* second test run */
        {
          Check.Done |= DONE_ALTSEMI;   /* no more tests needed */

          /* compare voltage drops for B1/B2 detection */
          if (AltSemi.U_1 > U_2)        /* larger voltage wins */
          {
            Flag = 0;         /* keep pinout of first run */
          }
        }

        if (Flag)             /* update data */
        {
          Check.AltFound = COMP_UJT;         /* it's an UJT */
          #ifdef SW_SYMBOLS
          Check.AltSymbol = SYMBOL_UJT;      /* set symbol ID */
          #endif

          /* save data */
          AltSemi.A = Probes.ID_3;      /* probe ID for Emitter */
          AltSemi.B = Probes.ID_1;      /* probe ID for B2 */
          AltSemi.C = Probes.ID_2;      /* probe ID for B1 */
          AltSemi.U_1 = U_2;            /* voltage drop when switched on */
          /* R_BB is stored in Resistors[0] */
        }
      }
    }
  }
}

#endif



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef SEMI_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
