`timescale 1ns / 1ps



parameter sizeof_sa = 3, WL=8, nrow_A=sizeof_sa, ncol_A = 6, nrow_B=ncol_A, ncol_B=sizeof_sa;



module systolic_array(clk, rst, A, B, out, cycle);



    input clk, rst;

    input [WL-1:0] A [0:nrow_A-1][0:ncol_A-1]; 

    input [WL-1:0] B [0:nrow_B-1][0:ncol_B-1]; 

    

    output reg [2*WL-1:0] out [0:sizeof_sa-1][0:sizeof_sa-1] = '{default:'d0};

    output integer cycle = '{default:'d0};

    

    reg [WL-1:0] store_A [0:sizeof_sa-1][0:ncol_A+sizeof_sa-2] = '{default:'d0}; 

    reg [WL-1:0] store_B [0:nrow_B+sizeof_sa-2][0:sizeof_sa-1] = '{default:'d0}; 

    

    reg [WL-1:0] buf_A [0:sizeof_sa-1][0:sizeof_sa-1]= '{default:'d0}; 

    reg [WL-1:0] buf_B [0:sizeof_sa-1][0:sizeof_sa-1]= '{default:'d0}; 

    

    integer i,j,x=0;

    

    always @(posedge clk) begin

        if(rst==0) begin

            // STORING LEFT INPUT 

            for(i=0;i<nrow_A; i=i+1) begin

                for(j=0;j<ncol_A; j=j+1) begin

                    store_A[i][j-i+sizeof_sa-1] = A[i][j]; 

                end    

            end

            

            // STORING TOP INPUT

            for(i=0;i<nrow_B; i=i+1) begin

                for(j=0;j<ncol_B; j=j+1) begin

                    store_B[i-j+sizeof_sa-1][j] = B[i][j]; 

                end    

            end

        end

    end

    

    always @(posedge clk) begin

        if(rst==0) begin

                if ((cycle<(ncol_A+nrow_A-1)+(sizeof_sa-1))) begin

                

                    cycle=cycle+1;

                

                    //SHIFTING THE LEFT INPUT RIGHT

                    for(i=sizeof_sa-2; i>=0; i=i-1) begin

                        for(j=0; j<sizeof_sa; j=j+1) begin

                            buf_A[j][i+1] = buf_A[j][i];

                            buf_B[i+1][j] = buf_B[i][j];

                        end  

                    end

                    

                    // STORING ELEMENTS IN FIRST ROW AND COLUMN

                    if(x<(ncol_A+sizeof_sa-1)) begin

                        for(i=0;i<sizeof_sa;i=i+1) begin

                                buf_B[0][i] = store_B[(ncol_A+sizeof_sa-2)-x][i];

                                buf_A[i][0] = store_A[i][(ncol_A+sizeof_sa-2)-x];

                            end 

                            x = x+1; 

                    end

                    

                    for(i=0;i<sizeof_sa;i=i+1) begin

                        for(j=0;j<sizeof_sa;j=j+1) begin

                            out[i][j]= out[i][j] + (buf_A[i][j]*buf_B[i][j]);

                        end

                    end

                    

                end // IF CYCLE LOOP

        end // IF RST LOOP

    end // ALWAYS BLOCK

    

endmodule