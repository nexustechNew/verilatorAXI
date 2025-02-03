module axi_slave_regs #(
  parameter ADDR_WIDTH = 4,   // Enough for addresses up to 0xC
  parameter DATA_WIDTH = 32
)(
  input  wire                     aclk,
  input  wire                     aresetn,

  // AXI-Lite Write Address
  input  wire [ADDR_WIDTH-1:0]   s_axi_awaddr,
  input  wire                     s_axi_awvalid,
  output reg                      s_axi_awready,

  // AXI-Lite Write Data
  input  wire [DATA_WIDTH-1:0]   s_axi_wdata,
  input  wire [(DATA_WIDTH/8)-1:0] s_axi_wstrb,
  input  wire                     s_axi_wvalid,
  output reg                      s_axi_wready,

  // AXI-Lite Write Response
  output reg  [1:0]              s_axi_bresp,
  output reg                      s_axi_bvalid,
  input  wire                     s_axi_bready,

  // AXI-Lite Read Address
  input  wire [ADDR_WIDTH-1:0]   s_axi_araddr,
  input  wire                     s_axi_arvalid,
  output reg                      s_axi_arready,

  // AXI-Lite Read Data
  output reg [DATA_WIDTH-1:0]    s_axi_rdata,
  output reg [1:0]               s_axi_rresp,
  output reg                      s_axi_rvalid,
  input  wire                     s_axi_rready
);

  localparam OKAY = 2'b00;

  // 4 internal registers (32 bits each)
  reg [DATA_WIDTH-1:0] regfile[0:3];

  // Write logic
  always @(posedge aclk or negedge aresetn) begin
    if (!aresetn) begin
      s_axi_awready <= 1'b0;
      s_axi_wready  <= 1'b0;
      s_axi_bvalid  <= 1'b0;
      s_axi_bresp   <= OKAY;
    end else begin
      // Defaults
      s_axi_awready <= 1'b0;
      s_axi_wready  <= 1'b0;

      // Accept address
      if (s_axi_awvalid && !s_axi_awready && !s_axi_bvalid) begin
        s_axi_awready <= 1'b1;
      end

      // Accept data
      if (s_axi_wvalid && !s_axi_wready && !s_axi_bvalid) begin
        s_axi_wready <= 1'b1;

        // Write to register (using address bits [3:2] for 4 registers)
        case (s_axi_awaddr[3:2])
          2'b00: regfile[0] <= apply_wstrb(regfile[0], s_axi_wdata, s_axi_wstrb);
          2'b01: regfile[1] <= apply_wstrb(regfile[1], s_axi_wdata, s_axi_wstrb);
          2'b10: regfile[2] <= apply_wstrb(regfile[2], s_axi_wdata, s_axi_wstrb);
          2'b11: regfile[3] <= apply_wstrb(regfile[3], s_axi_wdata, s_axi_wstrb);
          default: /* ignoring out of range */
             ;
        endcase

        // Generate response
        s_axi_bvalid <= 1'b1;
        s_axi_bresp  <= OKAY;
      end

      // Once master sees bvalid, it will assert bready
      if (s_axi_bvalid && s_axi_bready) begin
        s_axi_bvalid <= 1'b0; // clear response
      end
    end
  end

  // Read logic
  always @(posedge aclk or negedge aresetn) begin
    if (!aresetn) begin
      s_axi_arready <= 1'b0;
      s_axi_rvalid  <= 1'b0;
      s_axi_rresp   <= OKAY;
      s_axi_rdata   <= {DATA_WIDTH{1'b0}};
    end else begin
      s_axi_arready <= 1'b0;

      if (s_axi_arvalid && !s_axi_arready && !s_axi_rvalid) begin
        s_axi_arready <= 1'b1;
        // Provide read data
        case (s_axi_araddr[3:2])
          2'b00: s_axi_rdata <= regfile[0];
          2'b01: s_axi_rdata <= regfile[1];
          2'b10: s_axi_rdata <= regfile[2];
          2'b11: s_axi_rdata <= regfile[3];
          default: s_axi_rdata <= 32'hDEAD_BEEF;
        endcase
        s_axi_rresp  <= OKAY;
        s_axi_rvalid <= 1'b1;
      end

      // Wait for RREADY
      if (s_axi_rvalid && s_axi_rready) begin
        s_axi_rvalid <= 1'b0;
      end
    end
  end

  // Apply WSTRB on a per-byte basis
  function [31:0] apply_wstrb(
    input [31:0] old_data,
    input [31:0] new_data,
    input [3:0]  wstrb
  );
    integer i;
    begin
      apply_wstrb = old_data;
      for (i = 0; i < 4; i=i+1) begin
        if (wstrb[i]) begin
          apply_wstrb[8*i +: 8] = new_data[8*i +: 8];
        end
      end
    end
  endfunction

endmodule
