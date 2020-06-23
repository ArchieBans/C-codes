#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <cassert>
#include "output_stream.hpp"
#include "huffman.hpp"
#include "lzss.hpp"


#ifndef Block_2_HPP
#define Block_2_HPP

using namespace std;

const int BLOCK_MAX = 100000;

using Block = array<u8, BLOCK_MAX>;

class BlockType2{
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
vector<u32> construct_canonical_code(vector<int> const & lengths)
{
    unsigned int size = lengths.size();
    vector<unsigned int > length_counts(16,0);
    int max_length = 0;
    for (auto i:lengths){
        assert(i<=15);
        length_counts.at(i)++;
        max_length = std::max(i,max_length);
    }
    length_counts[0] = 0;
    
    vector<u32> result_codes(size,0);
    vector<unsigned int> next_code(size,0);
    {
        unsigned int code = 0;
        for( int  i = 1;i<=max_length;i++)
        {
            code = (code+length_counts.at(i-1))<<1;
            next_code.at(i) = code;
        }
    }
    {
        for(unsigned int symbol = 0;symbol < size;symbol++)
        {
            unsigned int length = lengths.at(symbol);
            if(length>0){
                result_codes.at(symbol) = next_code.at(length)++;
            }
        }
    }
    return result_codes;
}
void write_cl_data(OutputBitStream& stream, vector<int> const& ll_code_lengths, vector<int> const& dist_code_lengths ){
    //This function should be easy to port into your own code to start you off
    //The construct_canonical_code function below might also be useful.
    //The rest of the code in this file is probably useless except as a point of reference.

    //This is a very basic implementation of the encoding logic for the block 2 header. There are plenty of ways
    //the scheme used here can be improved to use fewer bits.
    //In particular, this implementation does not use the clever CL coding (where a prefix code is generated
    //for the code length tables) at all.

    //Variables are named as in RFC 1951
    assert(ll_code_lengths.size() >= 257); //There needs to be at least one use of symbol 256, so the ll_code_lengths table must have at least 257 elements
    unsigned int HLIT = ll_code_lengths.size() - 257;

    unsigned int HDIST = 0;
    if (dist_code_lengths.size() == 0){
        //Even if no distance codes are used, we are required to encode at least one.
    }else{
        HDIST = dist_code_lengths.size() - 1;
    }

    //This is where we would compute a proper CL prefix code.

    //We will use a fixed CL code that uses 4 bits for values 0 - 13 and 5 bits for everything else
    //(including the RLE symbols, which we do not use).
    unsigned int HCLEN = 15; // = 19 - 4 (since we will provide 19 CL codes, whether or not they get used)

    //Push HLIT, HDIST and HCLEN. These are all numbers so Rule #1 applies
    stream.push_bits(HLIT, 5);
    stream.push_bits(HDIST,5);
    stream.push_bits(HCLEN,4);

    std::vector<u32> cl_code_lengths {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5};

    //The lengths are written in a strange order, dictated by RFC 1951
    //(This seems like a sadistic twist of the knife, but there is some amount of weird logic behind the ordering)
    std::vector<u32> cl_permutation {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    //Now push each CL code length in 3 bits (the lengths are numbers, so Rule #1 applies)
    assert(HCLEN+4 == 19);
    for (unsigned int i = 0; i < HCLEN+4; i++)
        stream.push_bits(cl_code_lengths.at(cl_permutation.at(i)),3);
    
    //Now push the LL code lengths, using the CL symbols
    //Notice that we just push each length as a literal CL value, without even using the 
    //RLE features of the CL encoding.
    for (auto len: ll_code_lengths){
        assert(len <= 15); //Lengths must be at most 15 bits
        unsigned int cl_symbol = len; 
        //Push the CL symbol as a 5 bit value, MSB first
        //(If we had computed a real CL prefix code, we would use it here instead)
        for(int i = 3; i >= 0; i--)
            stream.push_bit((cl_symbol>>(unsigned int)i)&1);
    }

    //Same for distance code lengths
    if (dist_code_lengths.size() == 0){
        //If no distance codes were used, just push a length of zero as the only code length
        stream.push_bits(0,5);
    }else{
        for (auto len: dist_code_lengths){
            assert(len <= 15); //Lengths must be at most 15 bits
            unsigned int cl_symbol = len; 
            for(int i = 3; i >= 0; i--)
                stream.push_bit((cl_symbol>>(unsigned int)i)&1);
        }
    }

}


void CLDataNewCustomDynamicCodes(OutputBitStream& stream,vector<int> ll_code_lengths,vector<int> dist_code_lengths)
{
    //Settings for HLIT and HDIST
    Huffman_Codes huff;
    ll_code_lengths = huff.trimCodeLength(ll_code_lengths);
    dist_code_lengths = huff.trimCodeLength(dist_code_lengths);
    assert(ll_code_lengths.size() >= 257);
    
     int HLIT = ll_code_lengths.size() - 257;
     unsigned int HDIST = 0;
     if (dist_code_lengths.size() == 0){
        //Even if no distance codes are used, we are required to encode at least one.
      }
     else{
        HDIST = dist_code_lengths.size() - 1;
      }
      
    //Extract CLEncoding By Computing RLEForLengthTables
    auto rle_table_ll = huff.RLEForLengthTables(ll_code_lengths);
    auto rle_table_dist = huff.RLEForLengthTables(dist_code_lengths);
    
    auto symbol_encoding_ll = huff.generateCLSymbolEncodingForTable(rle_table_ll);
    auto symbol_encoding_dist = huff.generateCLSymbolEncodingForTable(rle_table_dist);
      
    //Merge the symbol encodings.
    vector<int> merged_encodings;
    for(unsigned int i = 0;i<symbol_encoding_ll.size();i++)
        merged_encodings.push_back(symbol_encoding_ll.at(i));
    for(unsigned int i = 0;i<symbol_encoding_dist.size();i++)
        merged_encodings.push_back(symbol_encoding_dist.at(i));
    

    auto filtered_encodings = huff.extractOnlySymbolsCL(merged_encodings);
    auto clTableFrequencies = huff.generateSymbolFrequenciesCLTable(filtered_encodings);
    huff.HuffmanCodes(clTableFrequencies,2);
    
    //Compute HCLEN
    vector<int> cl_permutation {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    vector<pairs> lengths_for_codes;
    for(unsigned int i = 0;i<cl_permutation.size();i++)
    {
        pairs new_pair;
        new_pair.first = cl_permutation.at(i);
        for(auto x : huff.encodings_CLTable)
        {
            if(x.first == cl_permutation.at(i))
                new_pair.second = x.second;
        }
        lengths_for_codes.push_back(new_pair);
    }
    int last_value_index=0;
    int counter = 1;
    for(auto x : lengths_for_codes)
    {
        if(x.second>0)
            last_value_index = counter;
        counter++;
    }
    int HCLEN = last_value_index - 4;
    
    stream.push_bits(HLIT, 5);
    stream.push_bits(HDIST,5);
    stream.push_bits(HCLEN,4);
    counter = 0;
    
    
    //Write CL Code lengths
    for(auto x: lengths_for_codes)
    {
        if(counter==last_value_index)
            break;
        else
        stream.push_bits(x.second,3);
        counter++;
    }
    
    
    //Prefix Codes creation and storage.
    vector<int> length_count_cltable(19,0);
    for(auto x : lengths_for_codes)
        length_count_cltable.at(x.first) = x.second;
    
    
    auto new_values = construct_canonical_code(length_count_cltable);
    unordered_map<int,int> prefix_Codes;
    for(unsigned int i = 0;i<new_values.size();i++)
    {
        if(new_values.at(i)>0)
            prefix_Codes[i] = new_values.at(i);
    }
    
    //CL bitstream
        for(unsigned int i =0;i<merged_encodings.size();i++)
    {
        if(merged_encodings.at(i) >=16 && merged_encodings.at(i) <=18)
        {
           int symbol_value = merged_encodings.at(i);
           vector<bool> bits = getBits(prefix_Codes[symbol_value],length_count_cltable.at(symbol_value));
           for(unsigned int j =0;j<bits.size();j++)
               stream.push_bit(bits[j]);
           i++; 
           int off_set = merged_encodings.at(i);
           int numbits;
        if(symbol_value==16){numbits = 2;}
        if(symbol_value==17){numbits = 3;}
        if(symbol_value==18){numbits = 7;}
        stream.push_bits(off_set,numbits);
        }
        else
        {
           int symbol_value = merged_encodings.at(i);
           vector<bool> bits = getBits(prefix_Codes[symbol_value],length_count_cltable.at(symbol_value));
           for(unsigned int j =0;j<bits.size();j++)
               stream.push_bit(bits[j]);
        }
    }
    
}



void writeBlock(OutputBitStream& stream,vector<int> compressed, bool isLast)
{
    Huffman_Codes huff;
    stream.push_bit(isLast?1:0);
    stream.push_bits(2,2);
    compressed.push_back(256);
    huff.extractLengthDistanceSymbols(compressed);
    
    auto frequencies_ll = huff.generateSymbolFrequencies(1);
    auto frequencies_dist = huff.generateSymbolFrequencies(0);
    
    
    //Generate Huffman Codes
    if(frequencies_ll.size()>0)
    {
      huff.HuffmanCodes(frequencies_ll,1);      
    }
    
    if(frequencies_dist.size()>0)
    {
      huff.HuffmanCodes(frequencies_dist,0);      
    }
    
    
    //Extract Code Lengths from the Huffman Encodings
    auto ll_code_lengths = huff.getLengthsFromEncoding(1);
    auto dist_code_lengths = huff.getLengthsFromEncoding(0);
    
    CLDataNewCustomDynamicCodes(stream,ll_code_lengths,dist_code_lengths);
    
    //Prefix Codes Creation and storage
    auto ll_code = construct_canonical_code(ll_code_lengths);
    auto dist_code = construct_canonical_code(dist_code_lengths);
    
    unordered_map<int,int> prefix_Codes_ll;
    unordered_map<int,int> prefix_Codes_dist;
    
    for(unsigned int i = 0;i<ll_code.size();i++)
    {
        if(ll_code.at(i)>0)
            prefix_Codes_ll[i] = ll_code.at(i);
    }
    for(unsigned int i = 0;i<dist_code.size();i++)
    {
        if(dist_code.at(i)>0)
            prefix_Codes_dist[i] = dist_code.at(i);
    }
    
    for(unsigned int i = 0; i < compressed.size();)
    {
            if(compressed[i]<=256)
        {
            //vector<int> huff_ll = huff.findElementInHuffmanTable(compressed[i],1);
            //vector<bool> bits = getBits(huff_ll[0],ll_code_lengths.at(compressed[i]));
            vector<bool>bits = getBits(prefix_Codes_ll[compressed[i]],ll_code_lengths.at(compressed[i]));
            for(unsigned int j = 0;j<bits.size();j++)
            {
                stream.push_bit(bits[j]);
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
                //vector<bool> bits;
                //vector<int> huff_ll = huff.findElementInHuffmanTable(length_symbol,1);
                 vector<bool>bits = getBits(prefix_Codes_ll[length_symbol],ll_code_lengths.at(length_symbol));
                for(unsigned int j = 0;j<bits.size();j++)
                {
                    stream.push_bit(bits[j]);
                }
                stream.push_bits(length_offset,length_bits);
                
                
                //vector<int> huff_dist = huff.findElementInHuffmanTable(distance_symbol,0);
                bits = getBits(prefix_Codes_dist[distance_symbol],dist_code_lengths.at(distance_symbol));
                for(unsigned int j = 0;j<bits.size();j++)
                {
                    stream.push_bit(bits[j]);
                }
                stream.push_bits(distance_offset,distance_bits);
               
            }
        }    
    }
}


};


#endif