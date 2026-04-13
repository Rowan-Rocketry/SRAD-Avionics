# SubGHz_Phy_Per — CLAUDE.md

## Project Overview

**SubGHz_Phy Packet Error Rate (PER) Test Application** for the STM32WL55JC Nucleo board.

Tests RF link quality between two boards (Tx/Rx pair) using FSK or LoRa modulation over the STM32 SubGHz_Phy middleware. Includes custom GPS data transmission via UART.

- **MCU:** STM32WL55JC (Cortex-M4, 64 MHz max, dual-core with Cortex-M0+ for RF)
- **Board:** NUCLEO-WL55JC
- **App Version:** V1.5.0
- **SDK:** STM32Cube_FW_WL V1.4.0

---

## Architecture

```
main()
  ├─ HAL_Init / SystemClock_Config (MSI 32MHz + LSE)
  ├─ MX_GPIO_Init, MX_USART1_UART_Init
  └─ MX_SubGHz_Phy_Init()
      ├─ SystemApp_Init()  → timer, debug, trace, low-power manager
      └─ SubghzApp_Init()  → Radio.Init, Radio.SetChannel, FSK/LoRa config,
                             register Per_Process task, start Rx or Tx

Main loop: MX_SubGHz_Phy_Process() → UTIL_SEQ_Run() → Per_Process()
```

**Task flow:**
1. Radio IRQ fires → callback sets a flag + calls `UTIL_SEQ_SetTask(Per_Process)`
2. Main loop runs `Per_Process()` which handles the flag, updates LEDs/stats, restarts radio

---

## Key Files

| File | Purpose |
|---|---|
| `Core/Src/main.c` | Entry point, clock config, peripheral init |
| `Core/Inc/main.h` | GPIO pin definitions |
| `Core/Inc/sys_conf.h` | System-level config (verbosity, low-power, debugger) |
| `SubGHz_Phy/App/subghz_phy_app.c` | Core PER logic: radio config, Tx/Rx state machine, PER stats |
| `SubGHz_Phy/App/subghz_phy_app.h` | RF parameters (`RF_FREQUENCY`, `TX_OUTPUT_POWER`, etc.) |
| `SubGHz_Phy/App/app_subghz_phy.c` | Init/process wrappers called from main |
| `SubGHz_Phy/Target/radio_conf.h` | Radio driver config (SMPS, TCXO, DCDC, XTAL) |
| `SubGHz_Phy/Target/radio_board_if.c` | RF switch control (LP/HP path) |
| `SubGHz_Phy/Target/timer.h` | Timer macros (wraps UTIL_TIMER) |
| `Core/Src/sys_app.c` | SystemApp_Init: trace, LPM, debug probes |
| `Core/Src/usart_if.c` | UART trace output (polling + DMA) |

---

## Radio Configuration (current defaults)

All in `SubGHz_Phy/App/subghz_phy_app.h` / top of `subghz_phy_app.c`:

```c
#define USE_MODEM_LORA    0          // LoRa disabled
#define USE_MODEM_FSK     1          // FSK enabled
#define RF_FREQUENCY      869100000  // 868.1 MHz (EU sub-GHz band)
#define TX_OUTPUT_POWER   14         // dBm
#define PAYLOAD_LEN       64         // bytes
#define FSK_FDEV          25000      // Hz deviation
#define FSK_DATARATE      4800       // bps
#define FSK_BANDWIDTH     50000      // Hz
#define FSK_PREAMBLE_LENGTH 5        // bytes
#define TEST_MODE         RADIO_TX   // RADIO_TX or RADIO_RX
#define RX_TIMEOUT_VALUE  3000       // ms
#define TX_TIMEOUT_VALUE  3000       // ms
#define RX_CONTINUOUS_ON  1
```

**FSK sync word:** `{0xC1, 0x94, 0xC1}` (3 bytes)
**CRC:** 2-byte IBM CRC (poly=0x8005, seed=0xFFFF)
**Whitening:** IBM DC-free (seed=0x01FF)

---

## Switching Between TX and RX Mode

Edit `SubGHz_Phy/App/subghz_phy_app.c`:

```c
#define TEST_MODE  RADIO_TX   // transmit
#define TEST_MODE  RADIO_RX   // receive
```

Recompile and flash. Two boards required — one TX, one RX.

---

## Hardware Pin Assignments

| Signal | Pin | Notes |
|---|---|---|
| LED1 (Blue) | PB15 | ON during TX |
| LED2 (Green) | PB9 | ON on RX success |
| LED3 (Red) | PB11 | ON on RX error/timeout |
| BUT1 | PA0 (EXTI0) | TX: +16 bytes payload |
| BUT2 | PA1 (EXTI1) | TX: +1 byte payload |
| BUT3 | PC6 (EXTI9_5) | TX: toggle payload mode |
| USART1 TX | PA2 | Debug/trace output |
| USART1 RX | PA3 | GPS input |
| PROB1 | PB12 | Debug probe |
| PROB2 | PB13 | Debug probe |

---

## UART / Serial

- **UART1:** 115200 8N1, PA2(TX)/PA3(RX)
- Connect a serial terminal at 115200 baud to see trace output
- **GPS integration:** UART1 RX collects GPS NMEA bytes into `gps_rx_buffer[512]` via interrupt; `tx_payload_generator()` can send this buffer as the TX payload

---

## GPS Integration (Custom)

GPS data arrives on UART1 RX (interrupt-driven):

```c
extern volatile uint8_t  gps_byte;            // latest received byte
extern volatile uint16_t gps_rx_len;          // bytes accumulated
extern volatile uint16_t gps_rx_len_ready;    // set when message complete
extern uint8_t           gps_rx_buffer[512];  // full message buffer
```

`tx_payload_generator()` in `subghz_phy_app.c` reads this buffer and builds the radio packet. Currently returns a fixed `'Y'` byte — GPS logic is partially implemented and commented out. PRBS9 pattern generation is also available but commented.

---

## System Configuration (`sys_conf.h`)

```c
#define VERBOSE_LEVEL    VLEVEL_M   // M=functional traces (change to VLEVEL_H for all)
#define APP_LOG_ENABLED  1
#define DEBUGGER_ENABLED 0          // Set to 1 when debugging with probe
#define LOW_POWER_DISABLE 0         // Set to 1 to disable STOP2 (easier debugging)
```

**Verbosity levels:** `VLEVEL_OFF(0)` < `VLEVEL_L(1)` < `VLEVEL_M(2)` < `VLEVEL_H(3)`

---

## Radio Driver Config (`radio_conf.h`)

```c
#define XTAL_FREQ              32000000UL   // 32 MHz
#define XTAL_DEFAULT_CAP_VALUE 0x20UL       // Crystal cap tuning
#define TCXO_CTRL_VOLTAGE      TCXO_CTRL_1_7V
#define RF_WAKEUP_TIME         1UL          // ms
#define DCDC_ENABLE            1UL
#define SMPS_DRIVE_SETTING_DEFAULT SMPS_DRV_40
#define SMPS_DRIVE_SETTING_MAX     SMPS_DRV_60
```

RF output: `RBI_CONF_RFO_LP_HP` (dual LP/HP path on NUCLEO-WL55JC)

---

## Build / IDE

Three IDE projects are provided:

| IDE | Project files |
|---|---|
| **STM32CubeIDE** (primary) | `STM32CubeIDE/.project`, `.cproject` |
| IAR Embedded Workbench | `EWARM/SubGHz_Phy_Per.ewp` |
| Keil uVision | `MDK-ARM/SubGHz_Phy_Per.uvprojx` |

**CubeMX:** `SubGHz_Phy_Per.ioc` (v6.7.0+) — regenerating will overwrite `USER CODE` sections if not inside `/* USER CODE BEGIN/END */` guards.

**Startup/linker:**
- STM32CubeIDE: `STM32CubeIDE/Application/User/Startup/startup_stm32wl55jcix.s`
- IAR: `EWARM/stm32wl55xx_flash_cm4.icf`
- Keil: `MDK-ARM/stm32wl55xx_flash_cm4.sct`

---

## Expected Serial Output

**TX board:**
```
APPLICATION_VERSION: V1.5.0
MW_RADIO_VERSION: V1.x.x
FSK_MODULATION
FSK_BW=50000 Hz
FSK_DR=4800 bits/s
Tx Mode
Tx 1, 64 bytes
Tx 2, 64 bytes
```

**RX board:**
```
Rx Mode
OnRxDone
RssiValue=-XX dBm, cfo=0 kHz
payloadLen=64 bytes
data= XX XX XX ...
Rx 1>>> PER= 0 %
```

PER formula: `PER = (RxKo * 100) / (RxKo + RxOk)`

---

## Common Tasks

**Change RF frequency:**
```c
// subghz_phy_app.h
#define RF_FREQUENCY  915000000  // e.g., 915 MHz for US
```

**Change TX power:**
```c
#define TX_OUTPUT_POWER  22  // max for HP path; 14 dBm default for LP
```

**Enable LoRa instead of FSK:**
```c
#define USE_MODEM_LORA  1
#define USE_MODEM_FSK   0
// Then set SF, BW, CR parameters in subghz_phy_app.h
```

**Enable all debug traces:**
```c
// sys_conf.h
#define VERBOSE_LEVEL    VLEVEL_H
#define LOW_POWER_DISABLE 1
#define DEBUGGER_ENABLED  1
```

**Enable GPS TX payload:**
Implement `tx_payload_generator()` in `subghz_phy_app.c` to read from `gps_rx_buffer` and copy to `data_buffer`.

---

## Middleware Dependencies (from STM32Cube_FW_WL)

- `Middlewares/Third_Party/SubGHz_Phy/` — radio driver (SX126x abstraction)
- `Utilities/sequencer/` — `UTIL_SEQ_*` cooperative task sequencer
- `Utilities/timer/` — `UTIL_TIMER_*` RTC-based software timers
- `Utilities/trace/adv_trace/` — `UTIL_ADV_TRACE_*` for UART logging
- `Utilities/lpm/` — `UTIL_LPM_*` low-power manager (STOP2 mode)
