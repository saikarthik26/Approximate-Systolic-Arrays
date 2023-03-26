#include <ap_int.h>

#include <iostream>

#include <stdio.h>



#define nrow_A 5

#define ncol_A 5

#define sizeof_sa 3

#define kernel_size 3



using namespace std;



void convolution_using_sa(

		ap_uint<8> A[nrow_A][ncol_A],

		ap_uint<8> B[kernel_size][kernel_size],

		ap_uint<17> C[(nrow_A-kernel_size+1)][(ncol_A-kernel_size+1)]);



int main()

{

	ap_uint<8> A[nrow_A][ncol_A];

	ap_uint<8> B[kernel_size][kernel_size];

	/*= {{1,3,6,3,1},

			{3,15,25,15,3},

			{6, 25, 41, 25, 6},

			{3,15,25,15,3},

			{1,3,6,3,1}};*/

	ap_uint<17> C[(nrow_A-kernel_size+1)][(ncol_A-kernel_size+1)];

	short i,j, a=1;

	for(i=0;i<nrow_A;i++)

		{

			for(j=0; j<ncol_A;j++)

			{

				A[i][j] = a;

				a=a+1;

			}

		}



	for(i=0;i<kernel_size;i++)

	{

		for(j=0; j<kernel_size;j++)

		{

			B[i][j] = 1;

		}

	}

	

	convolution_using_sa(A, B, C);

	for(i=0;i<((nrow_A-kernel_size+1));i++)

	{

		for(j=0; j<((ncol_A-kernel_size+1));j++)

		{

			cout<<C[i][j]<<" ";

		}

		cout<<"\n";

	}

	return 0;

}