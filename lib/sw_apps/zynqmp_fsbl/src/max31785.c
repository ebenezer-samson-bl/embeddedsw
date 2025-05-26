#include "max31785.h"

#include "xfsbl_error.h"
#include "xfsbl_debug.h"

#define IIC_SLAVE_ADDR 0x55

#define IIC_MUX_SLAVE_SELECT 1

#define debug(...) do { XFsbl_Printf(DEBUG_PRINT_ALWAYS, __VA_ARGS__); } while(0)
#define error(...) do { XFsbl_Printf(DEBUG_PRINT_ALWAYS, __VA_ARGS__); } while(0)

u32 max31785_init(XIicPs *IicPsInstance)
{
    u32 Status = 0;

    if((Status = iic_mux_set(IicPsInstance, IIC_MUX_SLAVE_SELECT)) != 0)
    {
        error("ERROR: Could not switch to I2C bus %d\n\r", IIC_MUX_SLAVE_SELECT);
        return Status;
    }

    for(u8 i = 0; i < 4; i++)
    {
        if((Status = iic_write_register(IicPsInstance, IIC_SLAVE_ADDR, 0, &i, 1)) != 0)
        {
            error("ERROR: Could not select page %d\n\r", (int)i);
            return Status;
        }

        // 100% PWM
        if((Status = iic_write_register(IicPsInstance, IIC_SLAVE_ADDR, 0x3B, (u8[]){0xFF, 0x7F}, 2)) != 0)
        {
            error("ERROR: Could not set 100% PWM on channel %d\n\r", (int)i);
            return Status;
        }

        // Fan config: 25 kHz PWM frequency, no fault response
        if((Status = iic_write_register(IicPsInstance, IIC_SLAVE_ADDR, 0xF1, (u8[]){0xE0, 0xE3}, 2)) != 0)
        {
            error("ERROR: Could not set fan config on channel %d\n\r", (int)i);
            return Status;
        }

        // Enable fan, PWM mode
        if((Status = iic_write_register(IicPsInstance, IIC_SLAVE_ADDR, 0x3A, (u8[]){0x80}, 1)) != 0)
        {
            error("ERROR: Could not enable fan on channel %d\n\r", (int)i);
            return Status;
        }
    }

    return Status;
}