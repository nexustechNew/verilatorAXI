# Directory structure:
#  - We are in sim/, the files to compile are ../rtl/top_wrapper.sv and ../rtl/axi_slave_regs.sv
#  - We link them with testbench.cpp to produce Vtop_wrapper

TOP        = top_wrapper
VERILATOR  = verilator
SRCS       = ../rtl/$(TOP).sv ../rtl/axi_slave_regs.sv
CXX_SRCS   = test_bench.cpp

# If you want wave traces, build with: make TRACE=1
TRACEFLAGS =
CXXDEFS    =
ifeq ($(TRACE),1)
  TRACEFLAGS = --trace
  CXXDEFS    = -DTRACE
endif

all: sim

sim:
	$(VERILATOR) -Wall -Wno-fatal $(TRACEFLAGS) --cc $(SRCS) \
	             --top-module $(TOP) --exe $(CXX_SRCS) \
				 --trace \
	             --trace-structs -O3 $(CXXDEFS)
	@cd obj_dir && make -f V$(TOP).mk
	@echo "Build complete. To run: ./obj_dir/V$(TOP)"

run: sim
	./obj_dir/V$(TOP)

clean:
	rm -rf obj_dir *.vcd
