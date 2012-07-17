#ifndef ADC_H
#define ADC_H

//! Disable ADC, reset channels
void adc_reset(void);

//! Add an ADC input
void adc_add(unsigned char pin);

//! Start ADC capturing
void adc_start(void);

//! Returns ADC value
unsigned short adc_get(unsigned char channel);

//! Returns the number of configured channels
unsigned char adc_num(void);

#endif // ADC_H
