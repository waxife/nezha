/**
 *  @file   sh_spi_cmd_help.c
 *  @brief  show spi nor flash related addr and command by HEX value
 *  $Id: sh_spi_cmd_help.c,v 1.1.1.1 2013/12/18 03:43:33 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/04/  ycshih    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <cache.h>
#include <nor.h>

command_init (sh_spi_cmd_help, "spi_cmd_help", "spi_cmd_help");

#define CMD_SPI_WRSR        (SPI_CMD_TYPE_BYTEWR | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1)
#define CMD_SPI_RDSR        (SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1) // can read io_port
#define CMD_SPI_RDID        (SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 3)

static int sh_spi_cmd_help(int argc, char **argv, void **result, void *sys_ctx)
{
 
    printf("NOR_CTRL_BASE            = 0x%x\r\n", NOR_CTRL_BASE            );
    printf("NOR_RD_PORT              = 0x%x\r\n", NOR_RD_PORT              );
    printf("NOR_WR_PORT              = 0x%x\r\n", NOR_WR_PORT              );
    printf("AUTO_RDSR_CFG            = 0x%x\r\n", AUTO_RDSR_CFG            );
    printf("IP_STATUS                = 0x%x\r\n", IP_STATUS                );
    printf("NOR_GENERAL_CTRL         = 0x%x\r\n", NOR_GENERAL_CTRL         );
    printf("NOR_TIMING_PARA          = 0x%x\r\n", NOR_TIMING_PARA          );
    printf("SPI_CMD                  = 0x%x\r\n", SPI_CMD                  );
    printf("SPI_ADDR                 = 0x%x\r\n", SPI_ADDR                 );
    printf("NOR_PPI_CMD              = 0x%x\r\n", NOR_PPI_CMD              );
    printf("PPI_ERASE_RDSR_ADDR      = 0x%x\r\n", PPI_ERASE_RDSR_ADDR      );
    printf("PPI_Pre_Idle_CMD         = 0x%x\r\n", PPI_Pre_Idle_CMD         );
    printf("PPI_Suspend_CMD          = 0x%x\r\n", PPI_Suspend_CMD          );
    printf("PPI_Resume_CMD           = 0x%x\r\n", PPI_Resume_CMD           );
    printf("PPI_Suspend_Latency_CMD  = 0x%x\r\n", PPI_Suspend_Latency_CMD  );
    printf("NOR_STATUS               = 0x%x\r\n", NOR_STATUS               );
    printf("NOR_PPI_CYCLE_1          = 0x%x\r\n", NOR_PPI_CYCLE_1          );
    printf("NOR_PPI_CYCLE_2          = 0x%x\r\n", NOR_PPI_CYCLE_2          );
    printf("NOR_PPI_CYCLE_3          = 0x%x\r\n", NOR_PPI_CYCLE_3          );
    printf("NOR_PPI_CYCLE_4          = 0x%x\r\n", NOR_PPI_CYCLE_4          );
    printf("NOR_PPI_CYCLE_5          = 0x%x\r\n", NOR_PPI_CYCLE_5          );
    printf("NOR_PPI_CYCLE_6          = 0x%x\r\n", NOR_PPI_CYCLE_6          );
    printf("NOR_PPI_CYCLE_7          = 0x%x\r\n", NOR_PPI_CYCLE_7          );

    printf("\n\nPPI_PROGRAM_DATA_CYCLE   = 0x%x\r\n", PPI_PROGRAM_DATA_CYCLE);
    printf("CMD_SPI_PIORD            = 0x%x\r\n", CMD_SPI_PIORD            );
    printf("CMD_SPI_BYTEWR           = 0x%x\r\n", CMD_SPI_BYTEWR           );
    printf("CMD_SPI_PAGEWR           = 0x%x\r\n", CMD_SPI_PAGEWR           );
    printf("CMD_SPI_WREN             = 0x%x\r\n", CMD_SPI_WREN             );
    printf("CMD_SPI_SECERASE         = 0x%x\r\n", CMD_SPI_SECERASE         );
    printf("CMD_SPI_CHIPERASE        = 0x%x\r\n", CMD_SPI_CHIPERASE        );
    printf("CMD_SPI_WRSR             = 0x%x\r\n", CMD_SPI_WRSR             );
    printf("CMD_SPI_RDSR             = 0x%x\r\n", CMD_SPI_RDSR             );
    printf("CMD_SPI_RDID             = 0x%x\r\n", CMD_SPI_RDID             );
    printf("CMD_SPI_RDID             = 0x%x\r\n", CMD_SPI_RDID             );

    return 0;

//EXIT:
//    print_usage(sh_spi_cmd_help);
//    return -1;
}


