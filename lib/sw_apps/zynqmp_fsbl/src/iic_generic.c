#include "iic_generic.h"

int iic_write_register(XIicPs *IicPsInstance, u8 device_addr, u8 reg_addr, const u8 data[], int n_bytes)
{
    u8 buf[n_bytes + 1];
    buf[0] = reg_addr;
    if(data != NULL && n_bytes > 0)
        memcpy(buf + 1, data, n_bytes);

    // TODO: Timeout !
    while(XIicPs_BusIsBusy(IicPsInstance));

    return XIicPs_MasterSendPolled(IicPsInstance, buf, n_bytes + 1, device_addr);
}

int iic_read_register(XIicPs *IicPsInstance, u8 device_addr, u8 reg_addr, u8 data[], int n_bytes)
{
    int Status = 0;

    // TODO: Timeout !
    while(XIicPs_BusIsBusy(IicPsInstance));

    XIicPs_SetOptions(IicPsInstance, XIICPS_REP_START_OPTION);
    Status = XIicPs_MasterSendPolled(IicPsInstance, &reg_addr, 1, device_addr);

    XIicPs_ClearOptions(IicPsInstance, XIICPS_REP_START_OPTION);
    Status |= XIicPs_MasterRecvPolled(IicPsInstance, data, n_bytes, device_addr);

    return Status;
}

#define IIC_MUX_ADDR 0x77

int iic_mux_set(XIicPs *IicPsInstance, uint8_t active_bus)
{
    return iic_write_register(IicPsInstance, IIC_MUX_ADDR, (1 << active_bus), NULL, 0);
}