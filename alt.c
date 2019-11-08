/*
 * Peripheral port select registers Microchip PIC32MZ microcontroller.
 *
 * Copyright (C) 2019 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include "gpio.h"

#define INT1R           0x1404
#define INT2R           0x1408
#define INT3R           0x140C
#define INT4R           0x1410
#define T2CKR           0x1418
#define T3CKR           0x141C
#define T4CKR           0x1420
#define T5CKR           0x1424
#define T6CKR           0x1428
#define T7CKR           0x142C
#define T8CKR           0x1430
#define T9CKR           0x1434
#define IC1R            0x1438
#define IC2R            0x143C
#define IC3R            0x1440
#define IC4R            0x1444
#define IC5R            0x1448
#define IC6R            0x144C
#define IC7R            0x1450
#define IC8R            0x1454
#define IC9R            0x1458
#define OCFAR           0x1460
#define U1RXR           0x1468
#define U1CTSR          0x146C
#define U2RXR           0x1470
#define U2CTSR          0x1474
#define U3RXR           0x1478
#define U3CTSR          0x147C
#define U4RXR           0x1480
#define U4CTSR          0x1484
#define U5RXR           0x1488
#define U5CTSR          0x148C
#define U6RXR           0x1490
#define U6CTSR          0x1494
#define SDI1R           0x149C
#define SS1R            0x14A0
#define SDI2R           0x14A8
#define SS2R            0x14AC
#define SDI3R           0x14B4
#define SS3R            0x14B8
#define SDI4R           0x14C0
#define SS4R            0x14C4
#define SDI5R           0x14CC
#define SS5R            0x14D0
#define SDI6R           0x14D8
#define SS6R            0x14DC
#define C1RXR           0x14E0
#define C2RXR           0x14E4
#define REFCLKI1R       0x14E8
#define REFCLKI3R       0x14F0
#define REFCLKI4R       0x14F4

#define RPA14R          0x1538
#define RPA15R          0x153C
#define RPB0R           0x1540
#define RPB1R           0x1544
#define RPB2R           0x1548
#define RPB3R           0x154C
#define RPB5R           0x1554
#define RPB6R           0x1558
#define RPB7R           0x155C
#define RPB8R           0x1560
#define RPB9R           0x1564
#define RPB10R          0x1568
#define RPB14R          0x1578
#define RPB15R          0x157C
#define RPC1R           0x1584
#define RPC2R           0x1588
#define RPC3R           0x158C
#define RPC4R           0x1590
#define RPC13R          0x15B4
#define RPC14R          0x15B8
#define RPD0R           0x15C0
#define RPD1R           0x15C4
#define RPD2R           0x15C8
#define RPD3R           0x15CC
#define RPD4R           0x15D0
#define RPD5R           0x15D4
#define RPD6R           0x15D8
#define RPD7R           0x15DC
#define RPD9R           0x15E4
#define RPD10R          0x15E8
#define RPD11R          0x15EC
#define RPD12R          0x15F0
#define RPD14R          0x15F8
#define RPD15R          0x15FC
#define RPE3R           0x160C
#define RPE5R           0x1614
#define RPE8R           0x1620
#define RPE9R           0x1624
#define RPF0R           0x1640
#define RPF1R           0x1644
#define RPF2R           0x1648
#define RPF3R           0x164C
#define RPF4R           0x1650
#define RPF5R           0x1654
#define RPF8R           0x1660
#define RPF12R          0x1670
#define RPF13R          0x1674
#define RPG0R           0x1680
#define RPG1R           0x1684
#define RPG6R           0x1698
#define RPG7R           0x169C
#define RPG8R           0x16A0
#define RPG9R           0x16A4

static uint32_t *volatile pps_base;           // PPS registers mapped here

static void pps_init()
{
    const int PPS_ADDR = 0x1f801000;
    extern int gpio_mem_fd;

    // Map a page of memory to the PPS address
    pps_base = (uint32_t*) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
        gpio_mem_fd, PPS_ADDR);
    if ((int32_t)pps_base < 0) {
        printf("PPS mmap failed: %s\n", strerror(errno));
        exit(-1);
    }
}

static uint32_t read_sfr(int offset)
{
    if (!pps_base)
        pps_init();

    uint32_t value = pps_base[offset & 0xfff];
    //printf("[%04x] -> %08x\n", offset, value);
    return value & 0xf;
}

static void write_sfr(int offset, uint32_t value)
{
    if (!pps_base)
        pps_init();

    pps_base[offset & 0xfff] = value;
    printf("%08x -> [%04x]\n", value, offset);
}

static gpio_mode_t mode_output_group1(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U3TX;
    case 2:  return MODE_U4RTS;
    case 3:  return 0;          // Reserved
    case 4:  return 0;          // Reserved
    case 5:  return MODE_SDO1;
    case 6:  return MODE_SDO2;
    case 7:  return MODE_SDO3;
    case 8:  return 0;          // Reserved
    case 9:  return MODE_SDO5;
    case 10: return MODE_SS6O;
    case 11: return MODE_OC3;
    case 12: return MODE_OC6;
    case 13: return MODE_REFCLKO4;
    case 14: return MODE_C2OUT;
    case 15: return MODE_C1TX;
    }
}

static gpio_mode_t mode_output_group2(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U1TX;
    case 2:  return MODE_U2RTS;
    case 3:  return MODE_U5TX;
    case 4:  return MODE_U6RTS;
    case 5:  return MODE_SDO1;
    case 6:  return MODE_SDO2;
    case 7:  return MODE_SDO3;
    case 8:  return MODE_SDO4;
    case 9:  return MODE_SDO5;
    case 10: return 0;          // Reserved
    case 11: return MODE_OC4;
    case 12: return MODE_OC7;
    case 13: return 0;          // Reserved
    case 14: return 0;          // Reserved
    case 15: return MODE_REFCLKO1;
    }
}

static gpio_mode_t mode_output_group3(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U3RTS;
    case 2:  return MODE_U4TX;
    case 3:  return 0;          // Reserved
    case 4:  return MODE_U6TX;
    case 5:  return MODE_SS1O;
    case 6:  return 0;          // Reserved
    case 7:  return MODE_SS3O;
    case 8:  return MODE_SS4O;
    case 9:  return MODE_SS5O;
    case 10: return MODE_SDO6;
    case 11: return MODE_OC5;
    case 12: return MODE_OC8;
    case 13: return 0;          // Reserved
    case 14: return MODE_C1OUT;
    case 15: return MODE_REFCLKO3;
    }
}

static gpio_mode_t mode_output_group4(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U1RTS;
    case 2:  return MODE_U2TX;
    case 3:  return MODE_U5RTS;
    case 4:  return MODE_U6TX;
    case 5:  return 0;          // Reserved
    case 6:  return MODE_SS2O;
    case 7:  return 0;          // Reserved
    case 8:  return MODE_SDO4;
    case 9:  return 0;          // Reserved
    case 10: return MODE_SDO6;
    case 11: return MODE_OC2;
    case 12: return MODE_OC1;
    case 13: return MODE_OC9;
    case 14: return 0;          // Reserved
    case 15: return MODE_C2TX;
    }
}

gpio_mode_t gpio_get_output_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('D',2):  return mode_output_group1(read_sfr(RPD2R));
    case GPIO_PIN('G',8):  return mode_output_group1(read_sfr(RPG8R));
    case GPIO_PIN('F',4):  return mode_output_group1(read_sfr(RPF4R));
    case GPIO_PIN('F',1):  return mode_output_group1(read_sfr(RPF1R));
    case GPIO_PIN('B',9):  return mode_output_group1(read_sfr(RPB9R));
    case GPIO_PIN('B',10): return mode_output_group1(read_sfr(RPB10R));
    case GPIO_PIN('B',5):  return mode_output_group1(read_sfr(RPB5R));
    case GPIO_PIN('C',1):  return mode_output_group1(read_sfr(RPC1R));
    case GPIO_PIN('D',14): return mode_output_group1(read_sfr(RPD14R));
    case GPIO_PIN('G',1):  return mode_output_group1(read_sfr(RPG1R));
    case GPIO_PIN('A',14): return mode_output_group1(read_sfr(RPA14R));
    case GPIO_PIN('D',6):  return mode_output_group1(read_sfr(RPD6R));

    case GPIO_PIN('D',3):  return mode_output_group2(read_sfr(RPD3R));
    case GPIO_PIN('G',7):  return mode_output_group2(read_sfr(RPG7R));
    case GPIO_PIN('F',5):  return mode_output_group2(read_sfr(RPF5R));
    case GPIO_PIN('D',11): return mode_output_group2(read_sfr(RPD11R));
    case GPIO_PIN('F',0):  return mode_output_group2(read_sfr(RPF0R));
    case GPIO_PIN('B',1):  return mode_output_group2(read_sfr(RPB1R));
    case GPIO_PIN('E',5):  return mode_output_group2(read_sfr(RPE5R));
    case GPIO_PIN('B',3):  return mode_output_group2(read_sfr(RPB3R));
    case GPIO_PIN('C',4):  return mode_output_group2(read_sfr(RPC4R));
    case GPIO_PIN('G',0):  return mode_output_group2(read_sfr(RPG0R));
    case GPIO_PIN('A',15): return mode_output_group2(read_sfr(RPA15R));
    case GPIO_PIN('D',7):  return mode_output_group2(read_sfr(RPD7R));

    case GPIO_PIN('D',9):  return mode_output_group3(read_sfr(RPD9R));
    case GPIO_PIN('B',8):  return mode_output_group3(read_sfr(RPB8R));
    case GPIO_PIN('B',15): return mode_output_group3(read_sfr(RPB15R));
    case GPIO_PIN('D',4):  return mode_output_group3(read_sfr(RPD4R));
    case GPIO_PIN('B',0):  return mode_output_group3(read_sfr(RPB0R));
    case GPIO_PIN('E',3):  return mode_output_group3(read_sfr(RPE3R));
    case GPIO_PIN('B',7):  return mode_output_group3(read_sfr(RPB7R));
    case GPIO_PIN('F',12): return mode_output_group3(read_sfr(RPF12R));
    case GPIO_PIN('D',12): return mode_output_group3(read_sfr(RPD12R));
    case GPIO_PIN('F',8):  return mode_output_group3(read_sfr(RPF8R));
    case GPIO_PIN('C',3):  return mode_output_group3(read_sfr(RPC3R));
    case GPIO_PIN('E',9):  return mode_output_group3(read_sfr(RPE9R));

    case GPIO_PIN('G',9):  return mode_output_group4(read_sfr(RPG9R));
    case GPIO_PIN('D',0):  return mode_output_group4(read_sfr(RPD0R));
    case GPIO_PIN('B',6):  return mode_output_group4(read_sfr(RPB6R));
    case GPIO_PIN('D',5):  return mode_output_group4(read_sfr(RPD5R));
    case GPIO_PIN('B',2):  return mode_output_group4(read_sfr(RPB2R));
    case GPIO_PIN('F',3):  return mode_output_group4(read_sfr(RPF3R));
    case GPIO_PIN('C',2):  return mode_output_group4(read_sfr(RPC2R));
    case GPIO_PIN('E',8):  return mode_output_group4(read_sfr(RPE8R));
    case GPIO_PIN('F',2):  return mode_output_group4(read_sfr(RPF2R));
    }
    return 0;
}

static gpio_mode_t mode_input_group1(int value)
{
    if (read_sfr(INT3R)     == value) return MODE_INT3;
    if (read_sfr(T2CKR)     == value) return MODE_T2CK;
    if (read_sfr(T6CKR)     == value) return MODE_T6CK;
    if (read_sfr(IC3R)      == value) return MODE_IC3;
    if (read_sfr(IC7R)      == value) return MODE_IC7;
    if (read_sfr(U1RXR)     == value) return MODE_U1RX;
    if (read_sfr(U2CTSR)    == value) return MODE_U2CTS;
    if (read_sfr(U5RXR)     == value) return MODE_U5RX;
    if (read_sfr(U6CTSR)    == value) return MODE_U6CTS;
    if (read_sfr(SDI1R)     == value) return MODE_SDI1;
    if (read_sfr(SDI3R)     == value) return MODE_SDI3;
    if (read_sfr(SDI5R)     == value) return MODE_SDI5;
    if (read_sfr(SS6R)      == value) return MODE_SS6I;
    if (read_sfr(REFCLKI1R) == value) return MODE_REFCLKI1;
    return 0;
}

static gpio_mode_t mode_input_group2(int value)
{
    if (read_sfr(INT4R)     == value) return MODE_INT4;
    if (read_sfr(T5CKR)     == value) return MODE_T5CK;
    if (read_sfr(T7CKR)     == value) return MODE_T7CK;
    if (read_sfr(IC4R)      == value) return MODE_IC4;
    if (read_sfr(IC8R)      == value) return MODE_IC8;
    if (read_sfr(U3RXR)     == value) return MODE_U3RX;
    if (read_sfr(U4CTSR)    == value) return MODE_U4CTS;
    if (read_sfr(SDI2R)     == value) return MODE_SDI2;
    if (read_sfr(SDI4R)     == value) return MODE_SDI4;
    if (read_sfr(C1RXR)     == value) return MODE_C1RX;
    if (read_sfr(REFCLKI4R) == value) return MODE_REFCLKI4;
    return 0;
}

static gpio_mode_t mode_input_group3(int value)
{
    if (read_sfr(INT2R)  == value) return MODE_INT2;
    if (read_sfr(T3CKR)  == value) return MODE_T3CK;
    if (read_sfr(T8CKR)  == value) return MODE_T8CK;
    if (read_sfr(IC2R)   == value) return MODE_IC2;
    if (read_sfr(IC5R)   == value) return MODE_IC5;
    if (read_sfr(IC9R)   == value) return MODE_IC9;
    if (read_sfr(U1CTSR) == value) return MODE_U1CTS;
    if (read_sfr(U2RXR)  == value) return MODE_U2RX;
    if (read_sfr(U5CTSR) == value) return MODE_U5CTS;
    if (read_sfr(SS1R)   == value) return MODE_SS1I;
    if (read_sfr(SS3R)   == value) return MODE_SS3I;
    if (read_sfr(SS4R)   == value) return MODE_SS4I;
    if (read_sfr(SS5R)   == value) return MODE_SS5I;
    if (read_sfr(C2RXR)  == value) return MODE_C2RX;
    return 0;
}

static gpio_mode_t mode_input_group4(int value)
{
    if (read_sfr(INT1R)     == value) return MODE_INT1;
    if (read_sfr(T4CKR)     == value) return MODE_T4CK;
    if (read_sfr(T9CKR)     == value) return MODE_T9CK;
    if (read_sfr(IC1R)      == value) return MODE_IC1;
    if (read_sfr(IC6R)      == value) return MODE_IC6;
    if (read_sfr(U3CTSR)    == value) return MODE_U3CTS;
    if (read_sfr(U4RXR)     == value) return MODE_U4RX;
    if (read_sfr(U6RXR)     == value) return MODE_U6RX;
    if (read_sfr(SS2R)      == value) return MODE_SS2I;
    if (read_sfr(SDI6R)     == value) return MODE_SDI6;
    if (read_sfr(OCFAR)     == value) return MODE_OCFA;
    if (read_sfr(REFCLKI3R) == value) return MODE_REFCLKI3;
    return 0;
}

gpio_mode_t gpio_get_input_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('A',14): return mode_input_group1(13);
    case GPIO_PIN('A',15): return mode_input_group2(13);
    case GPIO_PIN('B',0):  return mode_input_group3(5);
    case GPIO_PIN('B',1):  return mode_input_group2(5);
    case GPIO_PIN('B',2):  return mode_input_group4(7);
    case GPIO_PIN('B',3):  return mode_input_group2(8);
    case GPIO_PIN('B',5):  return mode_input_group1(8);
    case GPIO_PIN('B',6):  return mode_input_group4(5);
    case GPIO_PIN('B',7):  return mode_input_group3(7);
    case GPIO_PIN('B',8):  return mode_input_group3(2);
    case GPIO_PIN('B',9):  return mode_input_group1(5);
    case GPIO_PIN('B',10): return mode_input_group1(6);
    case GPIO_PIN('B',15): return mode_input_group3(3);
    case GPIO_PIN('C',1):  return mode_input_group1(10);
    case GPIO_PIN('C',2):  return mode_input_group4(12);
    case GPIO_PIN('C',3):  return mode_input_group3(12);
    case GPIO_PIN('C',4):  return mode_input_group2(10);
    case GPIO_PIN('C',13): return mode_input_group2(7);
    case GPIO_PIN('C',14): return mode_input_group1(7);
    case GPIO_PIN('D',0):  return mode_input_group4(3);
    case GPIO_PIN('D',2):  return mode_input_group1(0);
    case GPIO_PIN('D',3):  return mode_input_group2(0);
    case GPIO_PIN('D',4):  return mode_input_group3(4);
    case GPIO_PIN('D',5):  return mode_input_group4(6);
    case GPIO_PIN('D',6):  return mode_input_group1(14);
    case GPIO_PIN('D',7):  return mode_input_group2(14);
    case GPIO_PIN('D',9):  return mode_input_group3(0);
    case GPIO_PIN('D',11): return mode_input_group2(3);
    case GPIO_PIN('D',12): return mode_input_group3(10);
    case GPIO_PIN('D',14): return mode_input_group1(11);
    case GPIO_PIN('E',3):  return mode_input_group3(6);
    case GPIO_PIN('E',5):  return mode_input_group2(6);
    case GPIO_PIN('E',8):  return mode_input_group4(13);
    case GPIO_PIN('E',9):  return mode_input_group3(13);
    case GPIO_PIN('F',0):  return mode_input_group2(4);
    case GPIO_PIN('F',1):  return mode_input_group1(4);
    case GPIO_PIN('F',2):  return mode_input_group4(11);
    case GPIO_PIN('F',3):  return mode_input_group4(8);
    case GPIO_PIN('F',4):  return mode_input_group1(2);
    case GPIO_PIN('F',5):  return mode_input_group2(2);
    case GPIO_PIN('F',8):  return mode_input_group3(11);
    case GPIO_PIN('F',12): return mode_input_group3(9);
    case GPIO_PIN('G',0):  return mode_input_group2(12);
    case GPIO_PIN('G',1):  return mode_input_group1(12);
    case GPIO_PIN('G',7):  return mode_input_group2(1);
    case GPIO_PIN('G',8):  return mode_input_group1(1);
    case GPIO_PIN('G',9):  return mode_input_group4(1);
    }
    return 0;
}

static gpio_mode_t clear_input_group1(int value)
{
    if (read_sfr(INT3R)     == value) write_sfr(INT3R, 0);
    if (read_sfr(T2CKR)     == value) write_sfr(T2CKR, 0);
    if (read_sfr(T6CKR)     == value) write_sfr(T6CKR, 0);
    if (read_sfr(IC3R)      == value) write_sfr(IC3R, 0);
    if (read_sfr(IC7R)      == value) write_sfr(IC7R, 0);
    if (read_sfr(U1RXR)     == value) write_sfr(U1RXR, 0);
    if (read_sfr(U2CTSR)    == value) write_sfr(U2CTSR, 0);
    if (read_sfr(U5RXR)     == value) write_sfr(U5RXR, 0);
    if (read_sfr(U6CTSR)    == value) write_sfr(U6CTSR, 0);
    if (read_sfr(SDI1R)     == value) write_sfr(SDI1R, 0);
    if (read_sfr(SDI3R)     == value) write_sfr(SDI3R, 0);
    if (read_sfr(SDI5R)     == value) write_sfr(SDI5R, 0);
    if (read_sfr(SS6R)      == value) write_sfr(SS6R, 0);
    if (read_sfr(REFCLKI1R) == value) write_sfr(REFCLKI1R, 0);
    return 0;
}

static gpio_mode_t clear_input_group2(int value)
{
    if (read_sfr(INT4R)     == value) write_sfr(INT4R, 0);
    if (read_sfr(T5CKR)     == value) write_sfr(T5CKR, 0);
    if (read_sfr(T7CKR)     == value) write_sfr(T7CKR, 0);
    if (read_sfr(IC4R)      == value) write_sfr(IC4R, 0);
    if (read_sfr(IC8R)      == value) write_sfr(IC8R, 0);
    if (read_sfr(U3RXR)     == value) write_sfr(U3RXR, 0);
    if (read_sfr(U4CTSR)    == value) write_sfr(U4CTSR, 0);
    if (read_sfr(SDI2R)     == value) write_sfr(SDI2R, 0);
    if (read_sfr(SDI4R)     == value) write_sfr(SDI4R, 0);
    if (read_sfr(C1RXR)     == value) write_sfr(C1RXR, 0);
    if (read_sfr(REFCLKI4R) == value) write_sfr(REFCLKI4R, 0);
    return 0;
}

static gpio_mode_t clear_input_group3(int value)
{
    if (read_sfr(INT2R)  == value) write_sfr(INT2R, 0);
    if (read_sfr(T3CKR)  == value) write_sfr(T3CKR, 0);
    if (read_sfr(T8CKR)  == value) write_sfr(T8CKR, 0);
    if (read_sfr(IC2R)   == value) write_sfr(IC2R, 0);
    if (read_sfr(IC5R)   == value) write_sfr(IC5R, 0);
    if (read_sfr(IC9R)   == value) write_sfr(IC9R, 0);
    if (read_sfr(U1CTSR) == value) write_sfr(U1CTSR, 0);
    if (read_sfr(U2RXR)  == value) write_sfr(U2RXR, 0);
    if (read_sfr(U5CTSR) == value) write_sfr(U5CTSR, 0);
    if (read_sfr(SS1R)   == value) write_sfr(SS1R, 0);
    if (read_sfr(SS3R)   == value) write_sfr(SS3R, 0);
    if (read_sfr(SS4R)   == value) write_sfr(SS4R, 0);
    if (read_sfr(SS5R)   == value) write_sfr(SS5R, 0);
    if (read_sfr(C2RXR)  == value) write_sfr(C2RXR, 0);
    return 0;
}

static gpio_mode_t clear_input_group4(int value)
{
    if (read_sfr(INT1R)     == value) write_sfr(INT1R, 0);
    if (read_sfr(T4CKR)     == value) write_sfr(T4CKR, 0);
    if (read_sfr(T9CKR)     == value) write_sfr(T9CKR, 0);
    if (read_sfr(IC1R)      == value) write_sfr(IC1R, 0);
    if (read_sfr(IC6R)      == value) write_sfr(IC6R, 0);
    if (read_sfr(U3CTSR)    == value) write_sfr(U3CTSR, 0);
    if (read_sfr(U4RXR)     == value) write_sfr(U4RXR, 0);
    if (read_sfr(U6RXR)     == value) write_sfr(U6RXR, 0);
    if (read_sfr(SS2R)      == value) write_sfr(SS2R, 0);
    if (read_sfr(SDI6R)     == value) write_sfr(SDI6R, 0);
    if (read_sfr(OCFAR)     == value) write_sfr(OCFAR, 0);
    if (read_sfr(REFCLKI3R) == value) write_sfr(REFCLKI3R, 0);
    return 0;
}

void gpio_clear_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('A',14): clear_input_group1(13); write_sfr(RPA14R, 0); break;
    case GPIO_PIN('A',15): clear_input_group2(13); write_sfr(RPA15R, 0); break;
    case GPIO_PIN('B',0):  clear_input_group3(5);  write_sfr(RPB0R,  0); break;
    case GPIO_PIN('B',10): clear_input_group1(6);  write_sfr(RPB10R, 0); break;
    case GPIO_PIN('B',15): clear_input_group3(3);  write_sfr(RPB15R, 0); break;
    case GPIO_PIN('B',1):  clear_input_group2(5);  write_sfr(RPB1R,  0); break;
    case GPIO_PIN('B',2):  clear_input_group4(7);  write_sfr(RPB2R,  0); break;
    case GPIO_PIN('B',3):  clear_input_group2(8);  write_sfr(RPB3R,  0); break;
    case GPIO_PIN('B',5):  clear_input_group1(8);  write_sfr(RPB5R,  0); break;
    case GPIO_PIN('B',6):  clear_input_group4(5);  write_sfr(RPB6R,  0); break;
    case GPIO_PIN('B',7):  clear_input_group3(7);  write_sfr(RPB7R,  0); break;
    case GPIO_PIN('B',8):  clear_input_group3(2);  write_sfr(RPB8R,  0); break;
    case GPIO_PIN('B',9):  clear_input_group1(5);  write_sfr(RPB9R,  0); break;
    case GPIO_PIN('C',13): clear_input_group2(7);                        break;
    case GPIO_PIN('C',14): clear_input_group1(7);                        break;
    case GPIO_PIN('C',1):  clear_input_group1(10); write_sfr(RPC1R,  0); break;
    case GPIO_PIN('C',2):  clear_input_group4(12); write_sfr(RPC2R,  0); break;
    case GPIO_PIN('C',3):  clear_input_group3(12); write_sfr(RPC3R,  0); break;
    case GPIO_PIN('C',4):  clear_input_group2(10); write_sfr(RPC4R,  0); break;
    case GPIO_PIN('D',0):  clear_input_group4(3);  write_sfr(RPD0R,  0); break;
    case GPIO_PIN('D',11): clear_input_group2(3);  write_sfr(RPD11R, 0); break;
    case GPIO_PIN('D',12): clear_input_group3(10); write_sfr(RPD12R, 0); break;
    case GPIO_PIN('D',14): clear_input_group1(11); write_sfr(RPD14R, 0); break;
    case GPIO_PIN('D',2):  clear_input_group1(0);  write_sfr(RPD2R,  0); break;
    case GPIO_PIN('D',3):  clear_input_group2(0);  write_sfr(RPD3R,  0); break;
    case GPIO_PIN('D',4):  clear_input_group3(4);  write_sfr(RPD4R,  0); break;
    case GPIO_PIN('D',5):  clear_input_group4(6);  write_sfr(RPD5R,  0); break;
    case GPIO_PIN('D',6):  clear_input_group1(14); write_sfr(RPD6R,  0); break;
    case GPIO_PIN('D',7):  clear_input_group2(14); write_sfr(RPD7R,  0); break;
    case GPIO_PIN('D',9):  clear_input_group3(0);  write_sfr(RPD9R,  0); break;
    case GPIO_PIN('E',3):  clear_input_group3(6);  write_sfr(RPE3R,  0); break;
    case GPIO_PIN('E',5):  clear_input_group2(6);  write_sfr(RPE5R,  0); break;
    case GPIO_PIN('E',8):  clear_input_group4(13); write_sfr(RPE8R,  0); break;
    case GPIO_PIN('E',9):  clear_input_group3(13); write_sfr(RPE9R,  0); break;
    case GPIO_PIN('F',0):  clear_input_group2(4);  write_sfr(RPF0R,  0); break;
    case GPIO_PIN('F',12): clear_input_group3(9);  write_sfr(RPF12R, 0); break;
    case GPIO_PIN('F',1):  clear_input_group1(4);  write_sfr(RPF1R,  0); break;
    case GPIO_PIN('F',2):  clear_input_group4(11); write_sfr(RPF2R,  0); break;
    case GPIO_PIN('F',3):  clear_input_group4(8);  write_sfr(RPF3R,  0); break;
    case GPIO_PIN('F',4):  clear_input_group1(2);  write_sfr(RPF4R,  0); break;
    case GPIO_PIN('F',5):  clear_input_group2(2);  write_sfr(RPF5R,  0); break;
    case GPIO_PIN('F',8):  clear_input_group3(11); write_sfr(RPF8R,  0); break;
    case GPIO_PIN('G',0):  clear_input_group2(12); write_sfr(RPG0R,  0); break;
    case GPIO_PIN('G',1):  clear_input_group1(12); write_sfr(RPG1R,  0); break;
    case GPIO_PIN('G',7):  clear_input_group2(1);  write_sfr(RPG7R,  0); break;
    case GPIO_PIN('G',8):  clear_input_group1(1);  write_sfr(RPG8R,  0); break;
    case GPIO_PIN('G',9):  clear_input_group4(1);  write_sfr(RPG9R,  0); break;
    default:
        break;
    }
}
