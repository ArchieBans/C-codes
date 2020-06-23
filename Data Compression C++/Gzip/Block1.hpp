#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <cassert>
#include "output_stream.hpp"
#include "huffman.hpp"
#include "lzss.hpp"

using namespace std;
class BlockType1{
public:
    
    
    
vector<bool> getBits(int n, int how_many)
{

      vector<int> bits(how_many,0);
      for(int k=0; k<how_many; k++){
        int mask =  1 << k;
        int masked_n = n & mask;
        int thebit = masked_n >> k;
        bits[k] = thebit;
      }
    
    vector<bool> new_set(how_many,0);
    int return_size =0;
    for(int i=how_many-1; i>=0;i--){
        new_set[return_size] = bits[i];
        return_size++;
      }
      
     //free(bits); 
    return new_set;
}    
    
    
vector<int> getHuffmanCodeBlock1(int type,unsigned int basevalue)
{
    Huffman_Codes huff;
    vector<int> dummy;
    vector<vector<int>> ll_codes = huff.huffmanCodesLLBlock_1();
    vector<vector<int>> dist_codes = huff.huffmanCodesDistBlock_1();
    if(type == 0)
    {
        for(unsigned int i = 0;i<ll_codes.size()/ll_codes[0].size();i++)
        {
            if(basevalue==i)
            {
                vector<int> codes_bits = {ll_codes[i][0],ll_codes[i][1]};
                return codes_bits;
            }    
        }
    }
    if(type==1)
    {
        for(unsigned int i = 0;i<dist_codes.size()/dist_codes[0].size();i++)
        {
            if(basevalue==i)
            {
            vector<int> codes_bits = {dist_codes[i][0],dist_codes[i][1]};
            return codes_bits;
            }    
        }        
    }
    return dummy;
}


    
void writeBlock(OutputBitStream& stream,vector<int> const &compressed, bool isLast)
{
    stream.push_bit(isLast?1:0);
    stream.push_bits(1, 2); 
    for(unsigned int i = 0; i < compressed.size();)
    {
            if(compressed[i]<256)
        {
            vector<int> huff_ll = getHuffmanCodeBlock1(0,compressed[i]);
            if(huff_ll.size()!=0)
            {
                vector<bool> bits = getBits(huff_ll[0],huff_ll[1]);
                for(unsigned int j = 0;j<bits.size();j++)
                {
                    stream.push_bit(bits[j]);
                }    
            }
         i++;
        }
        if(i<compressed.size())
        {
            if(compressed[i]>256)
            {
                int length_symbol = compressed[i];
                int length_offset = compressed[++i];
                int length_bits = compressed[++i];
                int distance_symbol = compressed[++i];
                int distance_offset = compressed[++i];
                int distance_bits = compressed[++i];
                i++;
                vector<bool> bits;
                 vector<int> huff_ll = getHuffmanCodeBlock1(0,length_symbol);
                if(huff_ll.size()!=0)
                {
                    bits = getBits(huff_ll[0],huff_ll[1]);
                    for(unsigned int j = 0;j<bits.size();j++)
                    {
                        stream.push_bit(bits[j]);
                    }
                    stream.push_bits(length_offset,length_bits);
                        
                }
                vector<int> huff_dist = getHuffmanCodeBlock1(1,distance_symbol);
                if(huff_dist.size()!=0)
                {
                    bits = getBits(huff_dist[0],huff_dist[1]);
                    for(unsigned int j = 0;j<bits.size();j++)
                    {
                        stream.push_bit(bits[j]);
                    }
                    stream.push_bits(distance_offset,distance_bits);
                       
                }
            }
        }
       
    }
    vector<int> huff_ll = getHuffmanCodeBlock1(0,256);
    vector<bool> bits = getBits(huff_ll[0],huff_ll[1]);
    for(unsigned int j = 0;j<bits.size();j++)
    {
        stream.push_bit(bits[j]);
    }
}

};
