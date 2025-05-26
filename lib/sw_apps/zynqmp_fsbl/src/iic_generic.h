#ifndef IIC_GENERIC_H_
#define IIC_GENERIC_H_

#include <xiicps.h>

int iic_write_register(XIicPs *IicPsInstance, u8 device_addr, u8 reg_addr, const u8 data[], int n_bytes);
int iic_read_register(XIicPs *IicPsInstance, u8 device_addr, u8 reg_addr, u8 data[], int n_bytes);

int iic_mux_set(XIicPs *IicPsInstance, uint8_t active_bus);

#endif