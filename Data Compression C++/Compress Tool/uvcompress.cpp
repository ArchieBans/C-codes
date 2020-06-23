/* uvcompress.cpp
  Archit Kumar
  V00875990.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <sstream>
#include <bitset>
#include <unordered_map>
#include<cassert>
 
using namespace std;


vector<bool> reverseBit(vector<bool> val)
{
    //cout<<"Size:\n"<<val.size()<<"\n";
    vector<bool> temp(val.size(),0);
    int reverse_val  = val.size()-1;
    for(unsigned int i = 0 ;i < val.size() ; i++)
    {
        temp.at(reverse_val) = val.at(i);
        reverse_val--;
    }
    return temp;
}

vector<bool> cleanVector(vector<bool> val)
{
    vector<bool> temp(val.size(),0);
    return temp;    
}

vector<bool> getBits(int n, int how_many)
{
      // int *bits = (int *)malloc(sizeof(int) * how_many);
      vector<bool> bits(how_many,0);
      for(unsigned int k=0; k<how_many; k++){
        int mask =  1 << k;
        int masked_n = n & mask;
        int thebit = masked_n >> k;
        bits.at(k) = thebit;
      }
    
    return bits;
}

void printVector(vector<bool> val)
{
    for(unsigned int i = 0; i < val.size(); i++)
    {
        cout<<val.at(i);
    }
    cout<<"\n";
}

void writeBits(vector<bool> val)
{
    assert(val.size()==8);
    bitset<8> write_set;
    int save_index = 7;
    for(unsigned int index = 0; index<val.size();index++)
    {
        write_set[save_index] = val.at(index);
        save_index--;
    }
    unsigned long value = write_set.to_ulong(); 
    unsigned char c = static_cast<unsigned char>(value); // simplest -- no checks for 8 bit bitsets
    std::cout.put(c);
}



void write_encoded(vector<int> encoded)
{
    unsigned char a  = 0x1f;
    unsigned char b  = 0x9d;
    unsigned char mode = 144;
    std::cout.put(a);
    std::cout.put(b);
    std::cout.put(mode);
    vector<bool> write_char(8,0);
    int bits_written = 0;
    int master_position = 0;
    int num_bits = 9;
    for(unsigned int i = 0;i<encoded.size();i++)
    {
        if(encoded[i]>pow(2,num_bits) && num_bits < 17)
        {
            num_bits++;
        }
        vector<bool> output = getBits(encoded[i],num_bits);
        for(unsigned int j = 0;j <output.size() ;j++) //This one writes the new bits.
            {
                write_char.at(master_position) = output.at(j);
                bits_written++;
                master_position++;
                if(bits_written==8)
                {
                    write_char = reverseBit(write_char);
                    writeBits(write_char);
                    bits_written = 0;
                    master_position = 0;
                    vector<bool>temp(8,0);
                    write_char = temp;
                }
            }
        }
        
        if(bits_written>0)
        {
           write_char = reverseBit(write_char);
           writeBits(write_char);    
        }
}

vector<int> encoding()
{
    vector<int> encoded;
    unordered_map<string, int> symbol_table;
    
    //Initialize symbol table.
    for(int i = 0;i<=255;i++)
    {
        string c = "";
        c += char(i);
        symbol_table[c] = i;
    }
    
    
    string working = "";
    string read_char = "";
    char c;
    int new_index = 257;
    std::cin.get(c);
    working += c;
    
    while(std::cin.get(c))
    {
    
      read_char += c;
      if(symbol_table.find(working+read_char) != symbol_table.end())
      {
          working = working + read_char;
      }              
          else
          {
              encoded.push_back(symbol_table[working]);
              symbol_table[working+read_char] = new_index;
              new_index++;
              working = read_char;
          }
          read_char = "";
    }
    encoded.push_back(symbol_table[working]);
    return encoded;
}


int main(int argc, char** argv){

    vector<int> output_code = encoding();
    write_encoded(output_code);
}