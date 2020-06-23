#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <sstream>
#include <bitset>
#include <unordered_map>
#include<cassert>
#include "output_stream.hpp"
#ifndef LZSS_HPP
#define LZSS_HPP

 
using namespace std;
class LZSS_Compression{
public:
    
vector<u8> previous_global = vector<u8>(2048);
vector<u8> readWindow(vector<u8>& data, int from, int to)
{
    vector<u8> lookAheadBuffer;
    for(int i = from;i<to;i++)
    {
        lookAheadBuffer.push_back(data.at(i));
    }
    return lookAheadBuffer;
}
void printVectorChar(vector<char> val)
{
    for(unsigned int i = 0; i < val.size(); i++)
    {
        cout<<val.at(i);
    }
    cout<<"\n";
}
void printVectorInt(vector<int> val)
{
    for(unsigned int i = 0; i < val.size(); i++)
    {
        cout<<val.at(i);
    }
    cout<<"\n";
}

vector<u8> shiftPrevious(vector<u8>& data,vector<u8> shift_buffer,int windowsize)
{
    vector<u8> new_previous(windowsize,0);
    unsigned int counter = 0;
    for(int i = shift_buffer.size()-1;i>=0;i--)
    {
        new_previous.at(counter) = shift_buffer.at(i);
        counter++;
    }
    for(int i = 0 ; i < windowsize;i++)
    {
        if(data.size()>counter && new_previous.size()>counter)
        {
        new_previous.at(counter) = data.at(i);
        counter++;
        }
        else
        {
            break;
        }
    }
        
    return new_previous;
}
            

int findCurrInPrevious(vector<u8>& data, u8 c)
{
    for(unsigned int i = 0;i<data.size();i++)
    {
        if((unsigned int)c!=0)
        {
            if(data[i]==c)
            {
                return i+1;
            }    
        }
    }
    return -1;
}

int checkForCharReverse(vector<u8>& data, u8 c)
{
    for(int i = data.size()-1;i>=0;i--)
    {
        if((unsigned int)c!=0)
        {
         if(data[i]==c)
            {
                return i;
            }   
        }
        
    }    
    return -1;
}

int checkForCloserValue(vector<int>& data,char c,vector<u8>& previous)
{
    int value = findCurrInPrevious(previous,c); 
    if(value < data[0])
    {
        return value;
    }   
    return data[0];    
}
vector<int> findPattern(vector<u8>& current_window, vector<u8>& previous_window)
{
    vector<int> storePositions;

    u8 c = current_window[0];
    int value = checkForCharReverse(previous_window,c);
    if(value!=-1)
    {
        unsigned int position_curr = 1;
        int counter = 1;
        storePositions.push_back(value+1);
        while(value!=-1 && position_curr < current_window.size())
        {
            c = current_window[position_curr];
            if(previous_window[value-counter]==c)
            {
                position_curr++;
                counter++;
            }
            else
            {
                value = -1;
            }
        }
        storePositions.push_back(counter);
        if(counter==1)
        {
            storePositions[0] = checkForCloserValue(storePositions,current_window[0],previous_window);
        }
        return storePositions;
    }
    else
    {
        storePositions.push_back(-1);
        return storePositions;
    }
   
    
    return storePositions;
}
vector<int> getSymbolByBaseLL(int baselength)
{
    int base_lengths[7][7] = {{3,10,0,257},{11,18,1,265},{19,34,2,269},{35,66,3,273},{67,130,4,277},{131,257,5,281},{258,258,0,285}};
    vector<int> encode_data;
    for(unsigned int i = 0;i<sizeof(base_lengths)/sizeof(base_lengths[0]);i++)
    {
        int start_Base = base_lengths[i][0];
        int end_Base = base_lengths[i][1];
        int num_bits = base_lengths[i][2];
        int symbol_value = base_lengths[i][3];
        int change_symbol = 1<<num_bits;
        // cout<<"Start Base - "<<start_Base<<"\n";
        // cout<<"End Base - "<<end_Base<<"\n";
        // cout<<"Num bits - "<<num_bits<<"\n";
        // cout<<"Symbol Value - "<<symbol_value<<"\n";
        // cout<<"Change symbol"<<change_symbol<<"\n";
        int start_Base_pseudo = start_Base;
        for(int j = start_Base,count_start = 0; j<=end_Base;j++,count_start++)
        {
            if(count_start==change_symbol)
            {
                symbol_value++;
                count_start = 0;
                start_Base_pseudo = start_Base_pseudo+change_symbol;
            }
            if(baselength == j )
            {
                encode_data.push_back(symbol_value);
                encode_data.push_back(num_bits);
                encode_data.push_back(start_Base_pseudo);
                return encode_data;
            }
            
        }
    }
    encode_data.push_back(-1);
    return encode_data;
}


vector<int> getEncodingDataLL(int baseLength)
{
    vector<int> getEncodedData = getSymbolByBaseLL(baseLength);
    vector<int> encode_data;       
    
    if(getEncodedData[0]!=-1)
    {
       int offsetValue = baseLength - getEncodedData[2];
        encode_data.push_back(getEncodedData[0]);
        encode_data.push_back(getEncodedData[1]);
        encode_data.push_back(offsetValue);    
    }
    
    return encode_data;
}


vector<int> getSymbolByBaseDist(int baselength)
{
    int base_lengths[14][14] = {{1,4,0,0},{5,8,1,4}
                               ,{9,16,2,6},{17,32,3,8},
                               {33,64,4,10},{65,128,5,12},
                               {129,256,6,14},{257,512,7,16},{513,1024,8,18},{1025,2048,9,20},{2049,4096,10,22},{4097,8192,11,24},{8193,16384,12,26},{16385,32768,13,28}};
    
    vector<int> encode_data;
    for(unsigned int i = 0;i<sizeof(base_lengths)/sizeof(base_lengths[0]);i++)
    {
        int start_Base = base_lengths[i][0];
        int end_Base = base_lengths[i][1];
        int num_bits = base_lengths[i][2];
        int symbol_value = base_lengths[i][3];
        int change_symbol = 1<<num_bits;
        int start_Base_pseudo = start_Base;
        for(int j = start_Base,count_start = 0; j<=end_Base;j++,count_start++)
        {
            if(count_start==change_symbol)
            {
                symbol_value++;
                count_start = 0;
                start_Base_pseudo = start_Base_pseudo+change_symbol;
            }
            if(baselength == j )
            {
                encode_data.push_back(symbol_value);
                encode_data.push_back(num_bits);
                encode_data.push_back(start_Base_pseudo);
                return encode_data;
            }
            
        }
        
    }
    encode_data.push_back(-1);
    return encode_data;
}
vector<int> getEncodingDataDist(int baseLength)
{
    vector<int> getEncodedData = getSymbolByBaseDist(baseLength);
    vector<int> encode_data;       
    
    if(getEncodedData[0]!=-1)
    {
       int offsetValue = baseLength - getEncodedData[2];
        encode_data.push_back(getEncodedData[0]);
        encode_data.push_back(getEncodedData[1]);
        encode_data.push_back(offsetValue);    
    }
    
    return encode_data;
}


vector<int> compressLZSS(vector<u8>& data)
{
    vector<u8> previous = previous_global;
    vector<int> encoding;
    int window_size = 258;
    
    
    
    
    for(unsigned int i = 0;i<data.size();)
    {
        unsigned int to = i+window_size;
        if(to>=data.size())
        {
            to = data.size();
        }
        vector<u8> lookAheadBuffer = readWindow(data,i,to);
        u8 current = lookAheadBuffer[0];
        vector<int> prev_val = findPattern(lookAheadBuffer,previous);
        //vector<int> prev_val(1,-1);
        if((unsigned int)current == 0)
        {
            encoding.push_back(0);
            i++;
        }
        else
        {
            if(prev_val[0]!=-1)
        {
            
            if(prev_val[1]>2)
            {
                //printVectorInt(prev_val);
                // unsigned int baselength_length = prev_val[1];
                // int baseLength_dist = prev_val[0];
                vector<int> ll_code = getEncodingDataLL(prev_val[1]);
                vector<int> dist_code = getEncodingDataDist(prev_val[0]);
                int symbol_val_ll = ll_code[0];
                int num_bits_ll = ll_code[1];
                int offset_ll = ll_code[2];
                int symbol_val_dist = dist_code[0];
                int num_bits_dist = dist_code[1];
                int offset_dist = dist_code[2];
                //cout<<symbol_val_ll<<":"<<offset_ll<<":"<<symbol_val_dist<<":"<<offset_dist<<"\n";
                encoding.push_back(symbol_val_ll);
                encoding.push_back(offset_ll);
                encoding.push_back(num_bits_ll);
                encoding.push_back(symbol_val_dist);
                encoding.push_back(offset_dist);
                encoding.push_back(num_bits_dist);
                vector<u8> shift_buffer = readWindow(data,i,i+prev_val[1]);
                i = i + prev_val[1];
                previous = shiftPrevious(previous,shift_buffer,2048);    
            }
            else
            {
                //cout<<current<<"\n";
                encoding.push_back((unsigned int)current);
                i++;
                vector<u8> shift_buffer;
                shift_buffer.push_back(current);
                previous = shiftPrevious(previous,shift_buffer,2048);
            }
        }
        else
        {
            //cout<<"CURRENT - "<<((int)current)<<"\n";
            encoding.push_back((unsigned int)current);
            // previous.push_back(current);
            vector<u8> shift_buffer;
            shift_buffer.push_back(current);
            previous = shiftPrevious(previous,shift_buffer,2048);
            i++;
        }
            
        }
    }
    previous_global = previous;
    return  encoding;
}
};

#endif