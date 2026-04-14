# AGENTS.md
Guidance for coding agents in `SubGHz_Phy_Per` (STM32WL55JC PER application).

## Scope
- Prioritize keeping STM32CubeIDE build green.
- Keep custom edits inside `/* USER CODE BEGIN */ ... /* USER CODE END */`.
- Avoid broad refactors; prefer minimal, local changes.
- Treat generated files as read-mostly unless task explicitly requires changes.

## Rule Sources Detected
- Repo instructions found: `CLAUDE.md`.
- No Cursor rules found (`.cursorrules` or `.cursor/rules/`).
- No Copilot rules found (`.github/copilot-instructions.md`).

## Project Map
- `Core/Src/main.c`: boot, clock, init, main loop.
- `Core/Src/usart_if.c`: UART trace + GPS RX interrupt buffering.
- `Core/Inc/sys_conf.h`: logging/debug/low-power config.
- `SubGHz_Phy/App/subghz_phy_app.c`: PER state machine and radio callbacks.
- `SubGHz_Phy/App/subghz_phy_app.h`: RF and modem compile-time settings.
- `SubGHz_Phy/App/app_subghz_phy.c`: app glue called from main.
- `STM32CubeIDE/Debug/makefile`: primary command-line build entry.
- `EWARM/SubGHz_Phy_Per.ewp`: IAR project.
- `MDK-ARM/SubGHz_Phy_Per.uvprojx`: Keil project.
- `SubGHz_Phy_Per.ioc`: CubeMX config; regeneration may overwrite non-USER code.

## Build Commands
Use repo root as working directory.

### Primary (STM32CubeIDE generated make)
```bash
make -C "STM32CubeIDE/Debug" all
```

### Clean
```bash
make -C "STM32CubeIDE/Debug" clean
```

### Rebuild
```bash
make -C "STM32CubeIDE/Debug" clean all
```

### Build Artifacts
- `STM32CubeIDE/Debug/SubGHz_Phy_Per.elf`
- `STM32CubeIDE/Debug/SubGHz_Phy_Per.map`
- `STM32CubeIDE/Debug/SubGHz_Phy_Per.list`

### Build One Target (closest equivalent to single-test run)
No unit-test framework exists in this repo. Use single-object build for fast checks:
```bash
make -C "STM32CubeIDE/Debug" Application/User/SubGHz_Phy/App/subghz_phy_app.o
```

Other common single-object checks:
```bash
make -C "STM32CubeIDE/Debug" Application/User/Core/usart_if.o
make -C "STM32CubeIDE/Debug" Application/User/Core/main.o
```

### Optional Alternative Toolchains
IAR (if installed):
```bash
IarBuild.exe "EWARM/SubGHz_Phy_Per.ewp" -build "SubGHz_Phy_Per"
```

Keil (if installed):
```bash
UV4.exe -b "MDK-ARM/SubGHz_Phy_Per.uvprojx" -t "SubGHz_Phy_Per"
```

## Lint / Test Reality
- No dedicated lint config found (`clang-format`, `clang-tidy`, `cppcheck`, etc.).
- No unit-test framework found (no CTest/Unity/Ceedling/pytest).
- Compiler warnings are the effective lint gate (`-Wall` in generated rules).

## "Single Test" Guidance
Since no automated tests exist, choose one:
1. Build a single object target for syntax/symbol verification.
2. Run one hardware scenario (Tx-only or Rx-only) and verify UART traces.

Hardware scenario steps:
- Set `TEST_MODE` in `SubGHz_Phy/App/subghz_phy_app.c`.
- Build and flash board(s).
- Verify expected serial logs at 115200 8N1.

## Code Style

### Formatting
- Language is C; build uses GNU11 settings.
- Follow local formatting in touched files; do not mass-reformat.
- Keep braces for all control blocks.
- Keep diffs small and readable.

### Includes and File Structure
- Preserve STM32 section order (`Includes`, `Private typedef`, etc.).
- Keep custom includes/logic in USER CODE sections when applicable.
- Prefer project headers before standard headers.

### Types and Macros
- Use fixed-width integers (`uint8_t`, `uint16_t`, `int32_t`) for protocol data.
- Use `volatile` for ISR-shared variables/flags.
- Keep compile-time knobs as macros (`RF_FREQUENCY`, `FSK_DATARATE`, etc.).
- Macro naming: `UPPER_SNAKE_CASE`.

### Naming
- Do not rename HAL/ST callback identifiers.
- Do not rename middleware API entry points.
- New local helper functions: prefer `lower_snake_case`.
- Shared state names should be explicit (`*_flag`, `*_len`, `*_ready`).

### Error Handling
- Check HAL/radio return values and handle failures explicitly.
- Use existing fatal-path conventions (`Error_Handler()` / while-loop traps).
- Keep IRQ-disabled windows (`__disable_irq`) short.
- Avoid blocking or heavy work inside callbacks/ISRs.

### Concurrency Pattern
- ISR/callback: capture minimal data, set flags, schedule task (`UTIL_SEQ_SetTask`).
- Main/task context (`Per_Process`): heavy work, logging, state transitions.
- Protect shared buffers between ISR and task context.

### Logging
- Use existing macros: `APP_LOG`, `APP_TPRINTF`, `APP_PPRINTF`, `APP_PRINTF`.
- Keep hot-path logs concise to avoid timing side effects.

### Memory
- Prefer static storage; avoid dynamic allocation.
- Respect fixed buffer sizes (`gps_rx_buffer[512]`, `MAX_APP_BUFFER_SIZE`).
- Clamp lengths before copy operations.

## CubeMX and Generated Code Safety
- Safe edits: USER CODE blocks.
- Risky edits: generated init code, generated make/project metadata.
- After `.ioc` regeneration, re-verify all custom USER CODE regions.

## Agent Completion Checklist
1. Build changed object(s) at minimum; full Debug build when feasible.
2. Confirm USER CODE markers remain intact.
3. Re-check volatile/shared buffer logic when touching ISR-related code.
4. Note any hardware validation not executed locally.
5. If RF parameters changed, list exact macros changed and expected UART behavior.

## Avoid Unless Requested
- Adding new frameworks (RTOS/test/lint) as part of unrelated tasks.
- Mass formatting or renaming generated/HAL APIs.
- Moving custom logic outside USER CODE sections in generated files.
