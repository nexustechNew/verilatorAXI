#include <verilated.h>
#include <verilated_vcd_c.h>
#include <iostream>
#include <iomanip>
#include "Vtop_wrapper.h"

// Current simulation time (64-bit)
static vluint64_t main_time = 0;

// Required by Verilator for accurate time tracking
double sc_time_stamp() { return main_time; }

// Toggle clock helper
void tick_clock(Vtop_wrapper* tb, VerilatedVcdC* trace = nullptr) {
    // Falling edge
    tb->clk = 0;
    tb->eval();
    if (trace) trace->dump(main_time);
    main_time++;
    
    // Rising edge
    tb->clk = 1;
    tb->eval();
    if (trace) trace->dump(main_time);
    main_time++;
}

// A simple AXI-Lite write transaction in C++
void axi_lite_write(Vtop_wrapper* tb, uint32_t address, uint32_t data, VerilatedVcdC* trace=nullptr) {
    // Setup AW channel
    tb->s_axi_awaddr  = address & 0xF; // 4-bit address
    tb->s_axi_awvalid = 1;
    
    // Setup W channel
    tb->s_axi_wdata   = data;
    tb->s_axi_wstrb   = 0xF;  // Write all bytes
    tb->s_axi_wvalid  = 1;

    // Prepare for B response
    tb->s_axi_bready  = 1;

    // Wait for AWREADY & WREADY
    while ((tb->s_axi_awready == 0) || (tb->s_axi_wready == 0)) {
        tick_clock(tb, trace);
    }

    // Deassert AWVALID & WVALID
    tb->s_axi_awvalid = 0;
    tb->s_axi_wvalid  = 0;

    // Wait for BVALID
    while (tb->s_axi_bvalid == 0) {
        tick_clock(tb, trace);
    }

    // Optionally, check bresp
    // std::cout << "BRESP = " << tb->s_axi_bresp << "\n";

    // Deassert BREADY
    tick_clock(tb, trace);
    tb->s_axi_bready = 0;
}

// A simple AXI-Lite read transaction in C++
uint32_t axi_lite_read(Vtop_wrapper* tb, uint32_t address, VerilatedVcdC* trace=nullptr) {
    // Setup AR channel
    tb->s_axi_araddr  = address & 0xF;  // 4-bit address
    tb->s_axi_arvalid = 1;

    // Prepare for R data
    tb->s_axi_rready  = 1;

    // Wait for ARREADY
    while (tb->s_axi_arready == 0) {
        tick_clock(tb, trace);
    }

    // Deassert ARVALID
    tb->s_axi_arvalid = 0;

    // Wait for RVALID
    while (tb->s_axi_rvalid == 0) {
        tick_clock(tb, trace);
    }

    // Capture read data
    uint32_t rdata = tb->s_axi_rdata;

    // Deassert RREADY
    tick_clock(tb, trace);
    tb->s_axi_rready = 0;

    return rdata;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // Instantiate our top_wrapper
    Vtop_wrapper* tb = new Vtop_wrapper;

    // Optionally enable VCD tracing
    VerilatedVcdC* trace = nullptr;
#ifdef TRACE
    Verilated::traceEverOn(true);
    trace = new VerilatedVcdC;
    tb->trace(trace, 99);
    trace->open("dump.vcd");
#endif

    // Init signals
    tb->clk = 0;
    tb->rst_n = 0;

    tb->s_axi_awaddr  = 0;
    tb->s_axi_awvalid = 0;
    tb->s_axi_wdata   = 0;
    tb->s_axi_wstrb   = 0;
    tb->s_axi_wvalid  = 0;
    tb->s_axi_bready  = 0;
    tb->s_axi_araddr  = 0;
    tb->s_axi_arvalid = 0;
    tb->s_axi_rready  = 0;

    // Reset for a few cycles
    for (int i = 0; i < 5; i++) {
        tick_clock(tb, trace);
    }
    tb->rst_n = 1; // De-assert reset
    for (int i = 0; i < 5; i++) {
        tick_clock(tb, trace);
    }

    // 1) Write some registers
    std::cout << "Write to addr=0x0, data=0xDEADBEEF\n";
    axi_lite_write(tb, 0x0, 0xDEADBEEF, trace);

    std::cout << "Write to addr=0x4, data=0x12345678\n";
    axi_lite_write(tb, 0x4, 0x12345678, trace);

    // 2) Read them back and check
    uint32_t rd0 = axi_lite_read(tb, 0x0, trace);
    std::cout << "Read from addr=0x0 => 0x" 
              << std::hex << rd0 << "\n";
    if (rd0 != 0xDEADBEEF) {
        std::cerr << "ERROR: Expected 0xDEADBEEF\n";
    } else {
        std::cout << "PASS: Read matches!\n";
    }

    uint32_t rd4 = axi_lite_read(tb, 0x4, trace);
    std::cout << "Read from addr=0x4 => 0x"
              << std::hex << rd4 << "\n";
    if (rd4 != 0x12345678) {
        std::cerr << "ERROR: Expected 0x12345678\n";
    } else {
        std::cout << "PASS: Read matches!\n";
    }

    // Run a few more cycles
    for (int i = 0; i < 10; i++) {
        tick_clock(tb, trace);
    }

#ifdef TRACE
    trace->close();
    delete trace;
#endif
    delete tb;
    std::cout << "Simulation complete.\n";
    return 0;
}
