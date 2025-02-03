# AXI BFMs and Verification Examples

This repository provides a comprehensive verification framework for AXI-based designs using Verilator. It includes:

- **BFMs for AXI Full**: A set of Bus Functional Models that support full AXI read and write transactions.
- **Implementation Examples**:
  - **AXI_RAM**: An example of an AXI-compatible RAM implementation.
  - **AXI_Reg**: An example of an AXI register block implementation.
- **Pure C++ Testbench**: All test stimulus and checking are performed in C++ using Verilator’s generated model.
- **Waveform Tracing (Optional)**: VCD waveform generation is supported for debugging purposes.

These examples are intended to serve as a reference for integrating AXI BFMs into your verification environment and can be extended for more advanced verification scenarios.

---

## Directory Structure

```
.
├── rtl
│   ├── axi_ram.sv         # AXI-compatible RAM implementation example
│   ├── axi_reg.sv         # AXI register block (AXI_Reg) example
│   └── bfms
│       ├── axi_full_bfm.v # BFMs for full AXI transactions (read/write)
│       └── ...            # Additional AXI BFM modules as needed
├── sim
│   ├── testbench.cpp      # Pure C++ testbench controlling BFMs and transactions
│   └── Makefile           # Makefile for building with Verilator
├── docs                   # (Optional) Additional documentation or examples
└── README.md              # This file
```

---

## Features

- **AXI Full BFMs**:  
  Transaction-level interfaces to drive and verify full AXI protocol behavior are provided. They support read and write transactions without manually toggling individual signals in your C++ testbench.

- **AXI_RAM and AXI_Reg Examples**:  
  Sample RTL modules demonstrate how to implement common AXI peripherals. These examples can be used as starting points for your own designs.

- **Pure C++ Testbench**:  
  The testbench in `sim/testbench.cpp` handles:
  - Clock and reset generation.
  - AXI transactions (read/write) driven through the BFMs.
  - Response checking and error reporting.

- **Optional VCD Tracing**:  
  When enabled, the simulation produces a waveform dump (`dump.vcd`) that can be viewed with waveform viewers such as GTKWave.

---

## Requirements

- **Verilator**: Version 4.100 or later is recommended.  
  [Verilator Website](https://www.veripool.org/verilator/)
- **C++ Compiler**: (e.g., `g++`)
- **Make**: To use the provided Makefile.

---

## Build and Simulation Instructions

### 1. Clone the Repository

Clone this repository to your local machine:
```bash
git clone https://github.com/nexustechNew/verilatorAXI.git
cd verilatorAXI
```

### 2. Build the Simulation

Navigate to the `sim` directory and run:
```bash
cd sim
make
```
This command runs Verilator on the RTL files, compiles the generated model along with the C++ testbench, and creates an executable in the `obj_dir` folder.

> **Note:**  
> If you want to enable VCD waveform tracing, run:
> ```bash
> make clean
> make TRACE=1
> ```
> This sets the `TRACE` flag and enables waveform dumping.

### 3. Run the Simulation

After a successful build, run the executable:
```bash
./obj_dir/Vtop_wrapper
```
(Replace `Vtop_wrapper` with the appropriate top-level module name if different.)

### 4. View Waveforms (Optional)

If you built with tracing enabled, a file named `dump.vcd` will be generated in the `sim` directory. You can view this file using a waveform viewer such as [GTKWave](http://gtkwave.sourceforge.net/).

---

## Project Usage

- **BFM Integration**:  
  The BFMs located in `rtl/bfms` provide transaction-level control for AXI signals. You can invoke these either directly in SystemVerilog or via DPI calls from your C++ testbench.

- **AXI_RAM and AXI_Reg Examples**:  
  These RTL modules demonstrate the implementation of common AXI peripherals:
  - **AXI_RAM**: Acts as a memory model with an AXI interface.
  - **AXI_Reg**: Implements a register block accessible via AXI.
  
  They serve as practical examples of how to integrate BFMs with real hardware blocks.

- **C++ Testbench**:  
  The `sim/testbench.cpp` file is a self-contained testbench that:
  - Generates clock and reset signals.
  - Drives AXI transactions (writes and reads) using custom C++ functions.
  - Validates responses against expected values and reports pass/fail results.
  
  You can extend this testbench to add randomized transactions, scoreboard comparisons, or other verification features.

---

## Contributing

Contributions, improvements, and bug fixes are welcome! If you have suggestions or patches, please open an issue or submit a pull request. For major changes, please discuss them via an issue first.

---

## License

This project is licensed under the GPL-3.0.

---

## Contact

For questions or feedback, please open an issue in this repository or contact the maintainers.

emre.goncu@nexus-tech.space

## Additional Notes

- **Lint Warnings**:  
  You may see lint warnings regarding unused signal bits in the RTL code (e.g., lower bits of address signals). These are intentional if your design uses only the upper bits to address 32-bit boundaries and can be safely ignored or suppressed using Verilator lint pragmas.

- **Verilator Tracing**:  
  The simulation is set up to optionally generate VCD traces if the `TRACE` flag is defined at compile time. Ensure that both your Verilator command (via `--trace`) and your C++ build (with `-DTRACE`) are in sync if you require waveform outputs.

Enjoy exploring and extending this AXI verification framework!
```
