#ifndef __DIGITAL_FILTER_H_
#define __DIGITAL_FILTER_H_

unsigned int filter_unsigned_median(unsigned int *buff, unsigned int size);
unsigned int filter_unsigned_avr(unsigned int *buff, unsigned int size);
unsigned int filter_unsigned_median_with_avr(unsigned int *buff,
                unsigned int size);
unsigned int filter_unsigned_elimination(unsigned int *buff, unsigned int size,
                unsigned int prev_result);
unsigned int filter_unsigned_limit(unsigned int prev_value,
                unsigned int current_value, unsigned int limit_value);
unsigned int filter_unsigned_limit_with_elimination(unsigned int prev_result,
                unsigned int current_result, unsigned int limit_value);
#endif /* __DIGITAL_FILTER_H_ */
