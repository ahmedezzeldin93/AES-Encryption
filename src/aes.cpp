//============================================================================
// Name        : aes.cpp
// Author      : Ahmed Mohamed Ezz Eldin
// Version     : 1
// Copyright   : Your copyright notice
// Description : Implementation of AES Encryption 128 bit as Designed by Federal
// 				 Information Processing Standards Publication 197
//				 http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
//============================================================================

#include <iostream>
#include <sstream>
using namespace std;

//Define byte(8 bits) as unsigned short
#define byte unsigned short
//Number of words(4 bytes) in plain text or Number of columns in state
#define Nb 4
//Number of words(4 bytes) in cipher key
#define Nk 4
//Number of cipher rounds
#define Nr 10

byte in[Nb*4], out[Nb*4], state[4][Nb];
byte key[Nk*4];
byte roundKeys[176];

static const byte sBox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const byte Rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

byte gf(byte a, byte b);
void print();

void keyExpand(){
	int i;
	byte temp_word[4];
	byte temp_byte;

	for(i=0 ;i<Nk;i++){
		for(int j=0;j<4;j++){
			roundKeys[i*4+j]=key[i*4+j];
		}
	}

	while (i < (Nb * (Nr+1))){
		for(int j=0;j<4;j++){
			temp_word[j]=roundKeys[(i-1) * 4 + j];
		}

		if (i % Nk == 0){
			// function RotWord()
			{
				temp_byte = temp_word[0];
				temp_word[0] = temp_word[1];
				temp_word[1] = temp_word[2];
				temp_word[2] = temp_word[3];
				temp_word[3] = temp_byte;
			}
			// subword()
			{
				temp_word[0]=sBox[temp_word[0]];
				temp_word[1]=sBox[temp_word[1]];
				temp_word[2]=sBox[temp_word[2]];
				temp_word[3]=sBox[temp_word[3]];
			}

			temp_word[0] =  temp_word[0] ^ (Rcon[int(i/Nk)-1]);
		}
		for(int k=0;k<4;k++){
			roundKeys[i*4+k] = roundKeys[(i-Nk)*4+k] ^ temp_word[k];
		}
		i++;
	}
}

void subBytes(){
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			state[i][j] = sBox[state[i][j]];
}

void shiftRows(){
	byte temp;
	//Cyclic 1-shift row#1 columns to left
	temp=state[1][0];
	state[1][0]=state[1][1];
	state[1][1]=state[1][2];
	state[1][2]=state[1][3];
	state[1][3]=temp;

	//Cyclic 2-shifts row#2 columns to left
	temp=state[2][0];
	state[2][0]=state[2][2];
	state[2][2]=temp;
	temp=state[2][1];
	state[2][1]=state[2][3];
	state[2][3]=temp;

	//Cyclic 3-shifts row#3 columns to left
	temp=state[3][0];
	state[3][0]=state[3][3];
	state[3][3]=state[3][2];
	state[3][2]=state[3][1];
	state[3][1]=temp;
}

void mixColumns(){
	for(int i=0;i<Nb;i++){
		byte oldstate_0 = state[0][i];
		byte oldstate_1 = state[1][i];
		byte oldstate_2 = state[2][i];
		byte oldstate_3 = state[3][i];

		state[0][i] = gf(0x02,oldstate_0)^gf(0x03,oldstate_1)^oldstate_2^oldstate_3;
		state[1][i] = oldstate_0^gf(0x02,oldstate_1)^gf(0x03,oldstate_2)^oldstate_3;
		state[2][i] = oldstate_0^oldstate_1^gf(0x02,oldstate_2)^gf(0x03,oldstate_3);
		state[3][i] = gf(0x03,oldstate_0)^oldstate_1^oldstate_2^gf(0x02,oldstate_3);
	}
}

byte gf(byte a, byte b){
	short gfout=0;
	for (int i=0;i<8;i++) {
		if(b&1) gfout^=a;
		if(a&0x80) a=(a<<1)^0x11b;
		else a=a<<1;
		b>>=1;
	}
	return gfout;
}

void addRoundKey(int round){
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			state[j][i] ^= roundKeys[round * Nb * 4 + i * Nb + j];
}

void cipher(){
	int round=0;

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			state[j][i] = in[i*4 + j];

	addRoundKey(0);

	for(round=1;round<Nr;round++){
		subBytes();
		shiftRows();
		mixColumns();
		addRoundKey(round);
		//print();
	}

	subBytes();
	shiftRows();
	addRoundKey(Nr);

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			out[i*4+j]=state[j][i];
			in[i*4+j]=state[j][i];
		}
	}
}

void printRoundkeys(){
	for(int i=0;i<11;i++){
		for(int j=0;j<16;j++){
			if(roundKeys[i*11+j]<16){
				cout<< hex << 0;
			}
			cout << hex<<roundKeys[i*11+j];
		}
		cout<<endl;
	}
}

void print(){
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			if(state[j][i]<16)
				cout<< hex << 0;
			cout<<hex<<state[j][i];
		}
	}
	cout<<endl;
}

int main() {

	int testCases,aesNumber;
	string plaintext,inputKey;
    stringstream ss;
    cin>>testCases;
    while(testCases--) {
    	//cout<<"Plain Text: ";
    	cin>>plaintext;
    	for(int i=0,c=0;i<4;i++)
    		for(int j=0;j<4;j++, c+=2){
    			ss << hex << plaintext.substr(c,2);
    			ss >> in[4*i+j];
    			ss.clear();
    		}

    	//cout<<"Key: ";
    	cin>>inputKey;
    	for(int i=0,c=0;i<4;i++){
    		for(int j=0;j<4;j++, c+=2){
    			ss << hex <<inputKey.substr(c,2);
    			ss >> key[4*i+j];
    			ss.clear();
    		}
    	}
    	cin>>aesNumber;

    	keyExpand();
    	//printRoundkeys();

    	while(aesNumber--){
    		cipher();
    	}
        //cout<<"Cipher Text: ";
        for(int i=0;i<Nk*4;i++){
        	if(out[i]<16)
        		cout<<hex<<0;
        	cout<<hex<<out[i];
        }
        cout<<endl;
    }
    return 0;
}
