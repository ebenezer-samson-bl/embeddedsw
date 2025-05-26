#include "xparameters.h"
#include "p49v6965a000nlgi.h"
#include "max31785.h"

#include "xfsbl_error.h"
#include "xfsbl_debug.h"

#include "iic_generic.h"

#define IIC_SCLK_RATE 100000

#define ETH_SWITCH_BUS 3
#define ETH_SWITCH_ADDR 0x5f

#define debug(...) do { XFsbl_Printf(DEBUG_DETAILED, "[debug] " __VA_ARGS__); } while(0)
#define error(...) do { XFsbl_Printf(DEBUG_PRINT_ALWAYS, "[ERROR] " __VA_ARGS__); } while(0)
#define info(...) do { XFsbl_Printf(DEBUG_PRINT_ALWAYS, "[info] " __VA_ARGS__); } while(0)

u32 bv2_peripheral_init()
{
    u32 Status, cStatus = 0;

    XIicPs IicPsInstance;
    XIicPs_Config *Config;

    info("Setting up Buzz Virtual 2 hardware...\n\r");

    /*
     * Initialize the IIC driver so that it's ready to use
     * Look up the configuration in the config table, then initialize it.
     */
    Config = XIicPs_LookupConfig(XPAR_XIICPS_0_DEVICE_ID);
    if (NULL == Config) {
        XFsbl_Printf(DEBUG_PRINT_ALWAYS, "FATAL: Could not get I2C config for peripheral initialization.\n\r");
        return XST_FAILURE;
    }

    Status = XIicPs_CfgInitialize(&IicPsInstance, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        XFsbl_Printf(DEBUG_PRINT_ALWAYS, "FATAL: Could not initialize I2C bus for peripheral initialization.\n\r");
        return XST_FAILURE;
    }

    /*
     * Perform a self-test.
     */
    Status = XIicPs_SelfTest(&IicPsInstance);
    if (Status != XST_SUCCESS) {
        XFsbl_Printf(DEBUG_PRINT_ALWAYS, "FATAL: I2C self test failed prior to peripheral initialization.\n\r");
        return XST_FAILURE;
    }

    /*
     * Set the IIC serial clock rate.
     */
    XIicPs_SetSClk(&IicPsInstance, IIC_SCLK_RATE);

    debug("Setting up P49V6901 clock generator...\n\r");
    Status = clk_gen_cnf(&IicPsInstance);

    if(Status == XST_SUCCESS)
    {
        debug("... done\n\r");
    }
    else
    {
        error("... FAILED WITH STATUS %d\n\r", Status);
    }

    debug("Setting up MAX31785 fan controller...\n\r");
    cStatus = max31785_init(&IicPsInstance);
    Status |= cStatus;

    if(cStatus == XST_SUCCESS)
    {
        debug("... done\n\r");
    }
    else
    {
        error("... FAILED WITH STATUS %d\n\r", cStatus);
    }

    debug("Setting correct RGMII mode for KSZ9563 port 3...\n\r");

    if((Status = iic_mux_set(&IicPsInstance, ETH_SWITCH_BUS)) != 0)
    {
        error("Could not switch to I2C bus %d\n\r", ETH_SWITCH_BUS);
        return Status;
    }

    // Register 0x3001, enable RGMII ingress delay, disable RGMII IBS (default values otherwise)
    if((Status = iic_write_register(&IicPsInstance, ETH_SWITCH_ADDR, 0x33, (u8[]){0x01, 0xdb}, 2)) != 0)
    {
        error("Could not set RGMII mode register\n\r");
        return Status;
    }

    debug("... done\n\r");

    info("... done\n\r");


    return Status;
}