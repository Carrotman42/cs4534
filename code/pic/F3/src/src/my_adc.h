#ifndef MY_ADC_H
#define	MY_ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

    void init_adc();
    unsigned int adc_get_data();

#ifdef	__cplusplus
}
#endif

#endif	/* MY_ADC_H */

