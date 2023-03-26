#include <iostream>

#include <stdio.h>

#include <ap_int.h>



#define nrow_A 5

#define ncol_A 5

#define sizeof_sa 3

#define kernel_size 3



// FOR MATRIX MULT MODULE

#define padding_factor_col_B ((sizeof_sa-(((nrow_A-kernel_size+1)*(ncol_A-kernel_size+1))%sizeof_sa))%sizeof_sa)

#define final_col_B (nrow_A-kernel_size+1)*(ncol_A-kernel_size+1) + padding_factor_col_B



// FOR SYSTOLIC ARRAY

#define ncol_A_sa (kernel_size*kernel_size)



using namespace std;



#include <ap_int.h>

#include <iostream>

#include <stdio.h>



using namespace std;



void generate_partial_products_8(ap_uint<8> x,ap_uint<8> y, ap_uint<1> p[8][8])

{

	int i,j;



	for (i=0;i<8;i++)

	{

		for(j=0;j<8;j++)

		{

			p[i][j] = x[j]&y[i];

		}

	}

}



ap_uint<2> compressor3_2(ap_uint<3>p)

{

	ap_uint<3> w;

	ap_uint<1> a1;

	a1 = p[0]&p[1];

	w[2] = a1|p[2];

	w[1] = p[0]|p[1];

	return w[2],w[1];

}



ap_uint<2> compressor4_2(ap_uint<4>p)

{

	ap_uint<3> w;

	ap_uint<1> a1, a2;

	a1 = p[0]&p[1];

	a2 = p[2]&p[3];

	w[2] = a1|p[2]|p[3];

	w[1] = a2|p[0]|p[1];

	return w[2],w[1];

}



ap_uint<3> compressor5_3(ap_uint<5>p)

{

	ap_uint<4> w;

	ap_uint<1> a1, a2;

	a1 = p[0]&p[1];

	a2 = p[2]&p[3];

	w[3] = p[0]|p[1];

	w[2] = a2|p[4];

	w[1] = a1|p[2]|p[3];

	return w[3], w[2],w[1];

}



ap_uint<3> compressor6_3(ap_uint<6>p)

{

	ap_uint<4> w;

	ap_uint<1> a1, a2,a3;

	a1 = p[0]&p[1];

	a2 = p[2]&p[3];

	a3 = p[4]&p[5];

	w[3] = a2|p[4]|p[5];

	w[2] = a3|p[0]|p[1];

	w[1] = a1|p[2]|p[3];

	return w[3], w[2],w[1];

}



ap_uint<4> compressor7_4(ap_uint<7>p)

{

	ap_uint<5> w;

	w[4], w[3] = compressor4_2((p[6], p[5], p[4], p[3]));

	w[2], w[1] = compressor3_2((p[2], p[1], p[0]));

	return w[4],w[3],w[2],w[1];

}



ap_uint<4> compressor8_4(ap_uint<8>p)

{

	ap_uint<5> w;

	w[4], w[3] = compressor4_2((p[7], p[6], p[5], p[4]));

	w[2], w[1] = compressor4_2((p[3], p[2], p[1], p[0]));

	return w[4],w[3],w[2],w[1];

}



ap_uint<2> FA(ap_uint<3>p)

{

	ap_uint<3> w;

	w[1] = p[2]^p[1]^p[0];

	w[2] = (p[2]&p[1]) | (p[2]&p[0]) | (p[1]&p[0]);

	return w[2],w[1];

}



ap_uint<2> HA(ap_uint<2>p)

{

	ap_uint<3> w;

	w[1] = p[1]^p[0];

	w[2] = p[1]&p[0];

	return w[2],w[1];

}



ap_uint<30> processing_block_8_2_step(	ap_uint<1> p[8][8])

{

	ap_uint<15> out1;

	ap_uint<15> out2;

	ap_uint<30> out3;

	ap_uint<1> out[15][2];

	ap_uint<1> r[15][4];



	for(int i=0;i<15;i++)

	{

		for(int j=0;j<4; j++)

		{

			if(j<2)

			{

				out[i][j] = 0;

			}

			r[i][j] = 0;

			out1[i] = 0;

			out2[i] = 0;

		}

	}



	r[0][0] = p[0][0];

	r[1][0] = p[1][0];

	r[1][1] = p[0][1];



	(r[2][0],r[2][1]) = compressor3_2((p[2][0],p[1][1],p[0][2]));

	(r[3][0],r[3][1]) = compressor4_2((p[3][0],p[2][1],p[1][2],p[0][3]));

	(r[4][0],r[4][1],r[4][2]) = compressor5_3((p[4][0],p[3][1],p[2][2],p[1][3],p[0][4]));

	(r[5][0],r[5][1],r[5][2]) = compressor6_3((p[5][0],p[4][1],p[3][2],p[2][3],p[1][4],p[0][5]));

	(r[6][0],r[6][1],r[6][2],r[6][3]) = compressor7_4((p[6][0],p[5][1],p[4][2],p[3][3],p[2][4],p[1][5],p[0][6]));

	(r[7][0],r[7][1],r[7][2],r[7][3]) = compressor8_4((p[7][0],p[6][1],p[5][2],p[4][3],p[3][4],p[2][5],p[1][6],p[0][7]));



	(r[9][0], r[8][0]) = FA((p[7][1],p[6][2],p[5][3]));

	(r[9][1], r[8][1]) = HA((p[4][4],p[3][5]));

	r[8][2] = p[2][6];

	r[8][3] = p[1][7];



	(r[10][0], r[9][2]) = FA((p[7][2],p[6][3],p[5][4]));

	(r[10][1], r[9][3]) = FA((p[4][5],p[3][6],p[2][7]));



	(r[11][0], r[10][2]) = FA((p[7][3],p[6][4],p[5][5]));

	(r[11][1], r[10][3]) = HA((p[4][6],p[3][7]));



	(r[12][0], r[11][2]) = FA((p[7][4],p[6][5],p[5][6]));

	r[11][3] = p[4][7];



    r[12][1] = p[7][5]; //column 12

    r[12][2] = p[6][6];

    r[12][3] = p[5][7];



    r[13][0] = p[7][6]; //column 13

    r[13][1] = p[6][7];



    r[14][0] = p[7][7]; //column 14



    ////// PP REDUCTION STEP TWO   //////



    out[0][0] = r[0][0]; //column 0



    out[1][0] = r[1][0]; //column 1

    out[1][1] = r[1][1];



    out[2][0] = r[2][0]; //column 2

    out[2][1] = r[2][1];



    out[3][0] = r[3][0]; //column 3

    out[3][1] = r[3][1];



    (out[4][0],out[4][1]) = compressor3_2((r[4][0] ,r[4][1] ,r[4][2]));

    (out[5][0],out[5][1]) = compressor3_2((r[5][0] ,r[5][1] ,r[5][2]));

    (out[6][0],out[6][1]) = compressor4_2((r[6][0] ,r[6][1] ,r[6][2] ,r[6][3]));

    (out[7][0],out[7][1]) = compressor4_2((r[7][0] ,r[7][1] ,r[7][2] ,r[7][3]));

    (out[8][0],out[8][1]) = compressor4_2((r[8][0] ,r[8][1] ,r[8][2] ,r[8][3]));

    (out[9][0],out[9][1]) = compressor4_2((r[9][0] ,r[9][1] ,r[9][2] ,r[9][3]));

    (out[10][0],out[10][1]) = compressor4_2((r[10][0],r[10][1],r[10][2],r[10][3]));

    (out[11][0],out[11][1]) = compressor4_2((r[11][0],r[11][1],r[11][2],r[11][3]));



    (out[13][0],out[12][0]) = FA((r[12][0],r[12][1],r[12][2]));

    out[12][1] = r[12][3];



    (out[14][0],out[13][1]) = HA((r[13][0],r[13][1]));

    out[14][1] = r[14][0];



    out[0][1] = 0;



    //////////////////////////////////////////// Outputs of processing block   ////////////////////////



    	out1[0] = out[0][0];

        out1[1] = out[1][0];

        out1[2] = out[2][0];

        out1[3] = out[3][0];

        out1[4] = out[4][0];

        out1[5] = out[5][0];

        out1[6] = out[6][0];

        out1[7] = out[7][0];

        out1[8] = out[8][0];

        out1[9] = out[9][0];

        out1[10] = out[10][0];

        out1[11] = out[11][0];

        out1[12] = out[12][0];

        out1[13] = out[13][0];

        out1[14] = out[14][0];



        out2[0] = out[0][1];

        out2[1] = out[1][1];

        out2[2] = out[2][1];

        out2[3] = out[3][1];

        out2[4] = out[4][1];

        out2[5] = out[5][1];

        out2[6] = out[6][1];

        out2[7] = out[7][1];

        out2[8] = out[8][1];

        out2[9] = out[9][1];

        out2[10] = out[10][1];

        out2[11] = out[11][1];

        out2[12] = out[12][1];

        out2[13] = out[13][1];

        out2[14] = out[14][1];



        out3 = (out2, out1);

        return out3;

}



ap_uint<17> CLA16(ap_uint<16>a, ap_uint<16>b )

{

	ap_uint<17> sum_out ;

	ap_uint<17> w_c;

	ap_uint<16> w_g, w_p, w_sum;

	int i;



	w_c[0] = 0;



	for(i=0; i<16; i++)

	{

		w_g[i] = a[i] & b[i];

		w_p[i] = a[i] | b[i];

		w_c[i+1] = w_g[i] | (w_p[i] & w_c[i]);

	}



	for(i=0; i<16; i++)

	{

		//sum_out[i] = full_adder(a[i], b[i], w_c[i]);

		sum_out[i] = a[i] ^ b[i] ^ w_c[i];

	}



	sum_out[16] = w_c[16];

	return sum_out;



}



ap_uint<17> top8_2step_CLA16(	ap_uint<8> x, ap_uint<8> y)

{

	ap_uint<1> p[8][8];

	ap_uint<15> PRE1;

	ap_uint<15> PRE2=0;

	ap_uint<30> PRE3;

	ap_uint<17> prod;

	int a=0,i;



	generate_partial_products_8(x, y, p);

	PRE3 = processing_block_8_2_step(p);

	for(i=0; i<15; i++)

	{

		PRE1[i] = PRE3[i];

	}

	for(i=15; i<30; i++)

	{

		PRE2[a] = PRE3[i];

		a = a+1;

	}

	prod = CLA16(PRE1, PRE2);

	return prod;



}



void systolic_array(	ap_uint<8> A[sizeof_sa][ncol_A_sa],

		ap_uint<8> B[ncol_A_sa][sizeof_sa],

		ap_uint<17> C[sizeof_sa][sizeof_sa])

{

	short x = 0, i, j, cycles, m, n;

	short a,b;



	ap_uint<8> store_A[sizeof_sa][ncol_A_sa+(sizeof_sa-1)];

	ap_uint<8> store_B[ncol_A_sa+(sizeof_sa-1)][sizeof_sa];



	ap_uint<8> buf_A[sizeof_sa][sizeof_sa];

	ap_uint<8> buf_B[sizeof_sa][sizeof_sa];



	ap_uint<17> apprx_mult_out;



	for(i=0;i<sizeof_sa;i++)

		{

			for(j=0; j<(ncol_A_sa+(sizeof_sa-1));j++)

			{

				store_A[i][j] = 0;

			}

		}



	for(i=0;i<(ncol_A_sa+(sizeof_sa-1));i++)

			{

				for(j=0; j<sizeof_sa;j++)

				{

					store_B[i][j] = 0;

				}

			}



	for(i=0;i<sizeof_sa;i++)

	{

		// #pragma HLS PIPELINE

		for(j=0; j<sizeof_sa;j++)

		{

			buf_A[i][j] = 0;

			buf_B[i][j]=0;

			C[i][j] = 0;

		}

	}



	// FOR STORING IN APPROPORIATE FORMAT

	for(i=0; i<sizeof_sa; i++)

	{

		for(j=0; j<ncol_A_sa; j++)

		{

			store_A[i][j-i+sizeof_sa-1] = A[i][j];

		}

	}



	for(i=0; i<ncol_A_sa; i++)

	{

		for(j=0; j<sizeof_sa; j++)

		{

			store_B[i-j+sizeof_sa-1][j] = B[i][j];

		}

	}



	for(cycles=0; cycles<((sizeof_sa+ncol_A_sa-1)+(sizeof_sa-1)); cycles++)

	{

		// SHIFTING INPUTS

		for(i=sizeof_sa-2; i>=0; i--)

		{

			for(j=0; j<sizeof_sa; j++)

			{

				buf_A[j][i+1] = buf_A[j][i];

				buf_B[i+1][j] = buf_B[i][j];

			}

		}



		// STORING IN 1st ROW AND COLUMN

		if(x<(ncol_A_sa+sizeof_sa-1))

		{

			for(i=0; i<sizeof_sa; i++)

			{

				buf_A[i][0] = store_A[i][(ncol_A_sa+sizeof_sa-2)-x];

				buf_B[0][i] = store_B[(ncol_A_sa+sizeof_sa-2)-x][i];

			}

			x = x+1;

		}



		// COMPUTING OUTPUT MATRIX

		for(i=0;i<sizeof_sa;i++)

		{

			for(j=0; j<sizeof_sa;j++)

			{

				// C[i][j] = C[i][j] + (buf_A[i][j] * buf_B[i][j]);

				apprx_mult_out = top8_2step_CLA16(buf_A[i][j], buf_B[i][j]);

				C[i][j] = C[i][j] + apprx_mult_out;

			}

		}



	} // END OF FOR CYCLES LOOP



}



void matrix_mult( 	ap_uint<8> A[1][kernel_size*kernel_size],

		ap_uint<8> B[kernel_size*kernel_size][(nrow_A-kernel_size+1)*(ncol_A-kernel_size+1)],

		ap_uint<17> C[1][(nrow_A-kernel_size+1)*(ncol_A-kernel_size+1)])

{



	ap_uint<8> padded_A[sizeof_sa][kernel_size*kernel_size];

	ap_uint<8> padded_B[kernel_size*kernel_size][final_col_B];

	ap_uint<8> send_B[kernel_size*kernel_size][sizeof_sa];



	ap_uint<17> rx_C[sizeof_sa][sizeof_sa] = {0};

	short i,j,k,a,b=0;

	short x,y;



	for(i=0; i<sizeof_sa; i++)

	{

		for(j=0; j<kernel_size*kernel_size; j++)

		{

			padded_A[i][j] = 0;

		}

	}



	for(i=0; i<kernel_size*kernel_size; i++)

	{

		for(j=0; j<final_col_B; j++)

		{

			padded_B[i][j] = 0;

		}

	}



	// MAKE FIRST MATRIX ROWS EQUAL TO THAT OF SYSTOLIC ARRAY

	for(i=0; i<kernel_size*kernel_size; i++)

	{

		padded_A[0][i] = A[0][i];

	}





	// MAKE SECOND MATRIX COLUMNS EQUAL TO THAT OF MOD SYSTOLIC ARRAY

	for(i=0; i<(kernel_size*kernel_size); i++)

	{

		for(j=0; j<((nrow_A-kernel_size+1)*(ncol_A-kernel_size+1)); j++)

		{

			padded_B[i][j] = B[i][j];

		}

	}



	for(j=0; j<final_col_B; j=j+sizeof_sa)

	{

		for(i=0; i<(kernel_size*kernel_size); i++)

		{

			for(k=0; k<sizeof_sa; k++)

			{

				send_B[i][k] = padded_B[i][k+j];

			}

		}



		systolic_array(padded_A, send_B, rx_C);



		for(a=0;a<sizeof_sa;a++)

		{

			if(b<(nrow_A-kernel_size+1)*(ncol_A-kernel_size+1))

			{

			C[0][b] = rx_C[0][a];

			b=b+1;

			}

		}

	}



}



void convolution_using_sa(

		ap_uint<8> A[nrow_A][ncol_A],

		ap_uint<8> B[kernel_size][kernel_size],

		ap_uint<17> C[(nrow_A-kernel_size+1)][(ncol_A-kernel_size+1)])

{

	short i,j,m,n,a,b=0,c=0, d=0;



	ap_uint<8> temp_A[kernel_size*kernel_size][(nrow_A-kernel_size+1)*(ncol_A-kernel_size+1)];

	ap_uint<8> temp_B[1][kernel_size*kernel_size];

	ap_uint<17> temp_C[1][(nrow_A-kernel_size+1)*(ncol_A-kernel_size+1)];



	// CREATING FIRST INPUT TO SYSTOLIC ARRAY 1x(kxk)

	for(i=0;i<kernel_size; i++)

	{

		for(j=0;j<kernel_size;j++)

		{

			temp_B[0][c] = B[i][j];

			c=c+1;

		}

	}



	// CREATING SECOND INPUT MATRIX TO SYSTOLIC ARRAY (kxk) x ((n-k+1)x(n-k+1))

	for(i=0; i<(nrow_A-kernel_size+1); i=i+1)

	{

		for(j=0; j<(ncol_A-kernel_size+1); j=j+1)

		{

			a=0;

			for(m=0; m<kernel_size; m=m+1)

			{

				for(n=0; n<kernel_size; n=n+1)

				{

					temp_A[a][b] = A[m+i][n+j];

					a=a+1;

				}

			}

			b=b+1;

		} // END OF j LOOP

	} // END OF i LOOP



	// WE NOW HAVE 1x(kxk) and (kxk)x((n-k+1)x(n-k+1))

	matrix_mult(temp_B, temp_A, temp_C);



	// CREATING 2D MATRIX OUT OF 1D temp_C

	for(i=0;i<(nrow_A-kernel_size+1); i++)

	{

		for(j=0;j<(ncol_A-kernel_size+1);j++)

		{

			C[i][j] = temp_C[0][d];

			d=d+1;

		}

	}



}

