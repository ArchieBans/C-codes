// C++ program for Huffman Coding 
#include <iostream> 
#include <cstdlib> 
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <sstream>
#include <bitset>
#include <unordered_map>
#include <map>
#include <vector>
#include<cassert>
#include <algorithm>
#include "lzss.hpp"
#ifndef Huffman_HPP
#define Huffman_HPP


//Referneces : https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/

// This constant can be avoided by explicitly 
// calculating height of Huffman Tree 
using namespace std;
#define MAX_TREE_HT 15 
vector<int> ll_symbols;
vector<int> dist_symbols;
typedef pair<int,int> pairs;
vector<pairs> encodings_ll;
vector<pairs> encodings_dist;

class Huffman_Codes{
public:    
vector<pairs> encodings_CLTable;
//Block 2 Huffman Codes.
struct Huffman_data{
    int symbol_value;
    int encoding_value;
    int numbits;
};

vector<Huffman_data> huffman_table_ll_symbols;
vector<Huffman_data> huffman_table_dist_symbols;
vector<Huffman_data> huffman_cl_table;

struct MinHeapNode{
    int data; 
    unsigned freq; 
    struct MinHeapNode *left, *right; 
};

struct MinHeap { 
    unsigned size; 
    unsigned capacity; 
    struct MinHeapNode** array; 
}; 

struct MinHeapNode* newNode(int data, unsigned freq) 
{ 
    struct MinHeapNode* temp 
        = (struct MinHeapNode*)malloc
(sizeof(struct MinHeapNode)); 
  
    temp->left = temp->right = NULL; 
    temp->data = data; 
    temp->freq = freq; 
  
    return temp; 
} 
struct MinHeap* createMinHeap(unsigned capacity)   
{ 
  
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap)); 
    minHeap->size = 0; 
    minHeap->capacity = capacity; 
    minHeap->array 
        = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*)); 
    return minHeap; 
} 

void swapMinHeapNode(struct MinHeapNode** a,struct MinHeapNode** b)  
{ 
    struct MinHeapNode* t = *a; 
    *a = *b; 
    *b = t; 
} 
  
void minHeapify(struct MinHeap* minHeap, int idx)   
{  
    int smallest = idx; 
    unsigned int left = 2 * idx + 1; 
    unsigned int right = 2 * idx + 2; 
  
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) 
        smallest = left; 
  
    if (right < minHeap->size && minHeap->array[right]-> freq < minHeap->array[smallest]->freq) 
        smallest = right; 
  
    if (smallest != idx) { 
        swapMinHeapNode(&minHeap->array[smallest],&minHeap->array[idx]); 
        minHeapify(minHeap, smallest); 
    } 
} 

int isSizeOne(struct MinHeap* minHeap) 
{ 
    return (minHeap->size == 1); 
} 

struct MinHeapNode* extractMin(struct MinHeap* minHeap)   
{ 
  
    struct MinHeapNode* temp = minHeap->array[0]; 
    minHeap->array[0] = minHeap->array[minHeap->size - 1]; 
    --minHeap->size; 
    minHeapify(minHeap, 0); 
  
    return temp; 
} 

void insertMinHeap(struct MinHeap* minHeap,struct MinHeapNode* minHeapNode) 
  
{ 
    ++minHeap->size; 
    int i = minHeap->size - 1; 
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) { 
  
        minHeap->array[i] = minHeap->array[(i - 1) / 2]; 
        i = (i - 1) / 2; 
    } 
    minHeap->array[i] = minHeapNode; 
} 

void buildMinHeap(struct MinHeap* minHeap) 
  
{ 
    int n = minHeap->size - 1; 
    int i; 
    for (i = (n - 1) / 2; i >= 0; --i) 
        minHeapify(minHeap, i); 
} 

void printArr(int arr[], int n) 
{ 
    int i; 
    for (i = 0; i < n; ++i) 
        cout<< arr[i]; 
  
    cout<<"\n"; 
} 

int isLeaf(struct MinHeapNode* root)   
{ 
  
    return !(root->left) && !(root->right); 
} 
  
struct MinHeap* createAndBuildMinHeap(vector<int> data, vector<int> freq, int size)  
{ 
    struct MinHeap* minHeap = createMinHeap(size); 
    for (int i = 0; i < size; ++i) 
        minHeap->array[i] = newNode(data[i], freq[i]); 
  
    minHeap->size = size; 
    buildMinHeap(minHeap); 
    return minHeap; 
} 


struct MinHeapNode* buildHuffmanTree(vector<int> data, vector<int> freq, int size)  
{ 
    struct MinHeapNode *left, *right, *top; 
  
    // Step 1: Create a min heap of capacity 
    // equal to size. Initially, there are 
    // modes equal to size. 
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size); 
  
    // Iterate while size of heap doesn't become 1 
    while (!isSizeOne(minHeap)) { 
  
        // Step 2: Extract the two minimum 
        // freq items from min heap 
        left = extractMin(minHeap); 
        right = extractMin(minHeap); 
  
        // Step 3: Create a new internal 
        // node with frequency equal to the 
        // sum of the two nodes frequencies. 
        // Make the two extracted node as 
        // left and right children of this new node. 
        // Add this node to the min heap 
        // '$' is a special value for internal nodes, not used 
        top = newNode('$', left->freq + right->freq); 
  
        top->left = left; 
        top->right = right; 
  
        insertMinHeap(minHeap, top); 
    } 
  
    // Step 4: The remaining node is the 
    // root node and the tree is complete. 
    return extractMin(minHeap); 
} 
  

void printCodes(struct MinHeapNode* root, int arr[], int top,int val)  
{ 
  
    // Assign 0 to left edge and recur 
    if (root->left) { 
  
        arr[top] = 0; 
        printCodes(root->left, arr, top + 1,val); 
    } 
  
    // Assign 1 to right edge and recur 
    if (root->right) { 
  
        arr[top] = 1; 
        printCodes(root->right, arr, top + 1,val); 
    } 
  
    // If this is a leaf node, then 
    // it contains one of the input 
    // characters, print the character 
    // and its code from arr[] 
    if (isLeaf(root)) { 
  
        //cout<< root->data <<": "; 
        pairs new_pair;
        new_pair.first = root->data;
        new_pair.second = top;
        Huffman_data new_data;
        new_data.symbol_value = root->data;
        new_data.encoding_value = convertHuffCodeToIntValue(arr,top);
        new_data.numbits = top;
        if(val==1)
        {
            encodings_ll.push_back(new_pair);
            huffman_table_ll_symbols.push_back(new_data);
        }
        if(val==0)
        {
            encodings_dist.push_back(new_pair);
            huffman_table_dist_symbols.push_back(new_data);
        }
        if(val==2)
        {
            encodings_CLTable.push_back(new_pair);
            huffman_cl_table.push_back(new_data);
        }
        //printArr(arr, top); 
    } 
} 
  
void extractLengthDistanceSymbols(vector<int> compressed)
{
    for(unsigned int i = 0;i<compressed.size();)
    {
            if(compressed[i]<=256)
        {
           ll_symbols.push_back(compressed[i]);
           i++;
        }
        if(i<compressed.size())
        {
            if(compressed[i]>256)
            {
                int length_symbol = compressed[i];
                ++i;//int length_offset = compressed[++i];
                ++i;//int length_bits = compressed[++i];
                int distance_symbol = compressed[++i];
                ++i;//int distance_offset = compressed[++i];
                ++i;//int distance_bits = compressed[++i];
                i++;
                ll_symbols.push_back(length_symbol);
                dist_symbols.push_back(distance_symbol);
            }    
        }
        
    }
}

vector<pairs> generateSymbolFrequencies(int val)
{
    unordered_map<int,int> huffman_frequencies;
    for(int i = 0;i<=285;i++)
    {
        huffman_frequencies[i] = 0;
    }
    vector<int> values;
    if(val==1)
    {
        values = ll_symbols;
    }
    if(val == 0)
    {
        values = dist_symbols;
    }
    for(unsigned int i = 0;i<values.size();i++)
    {
        auto val = huffman_frequencies.find(values.at(i));
        if (val == huffman_frequencies.end())
        {
            cout<<"Not found";
            }
        else
        {
            val->second = val->second+1;
        }
    }
    
    
    unordered_map<int,int> filter_frequencies;
    for(int i = 0;i<=285;i++)
    {
        auto val = huffman_frequencies.find(i);
        if (val == huffman_frequencies.end())
        {
            exit(0);
        }
        else if (val->second > 0)
        {
           filter_frequencies[i] = val->second;      
        }
    }
    
    vector<pairs> vec;
    copy(filter_frequencies.begin(),filter_frequencies.end(),back_inserter<vector<pairs>>(vec));
    sort(vec.begin(),vec.end(),[](const pairs& l,const pairs&r){
        if(l.second != r.second)
            return l.second < r.second;
        return l.first < r.first;
    });
    
    return vec;
}

vector<pairs> generateSymbolFrequenciesCLTable(vector<int> values)
{
    unordered_map<int,int> huffman_frequencies;
    for(int i = 0;i<=18;i++)
    {
        huffman_frequencies[i] = 0;
    }
    
    for(unsigned int i = 0;i<values.size();i++)
    {
        
            auto val = huffman_frequencies.find(values.at(i));
            if (val == huffman_frequencies.end())
            {
                //cout<<"Not found";
            }
            else
            {
                val->second = val->second+1;
            }
        
    }
    
    
    unordered_map<int,int> filter_frequencies;
    for(int i = 0;i<=18;i++)
    {
        auto val = huffman_frequencies.find(i);
        if (val == huffman_frequencies.end())
        {
            exit(0);
        }
        else if (val->second > 0)
        {
           filter_frequencies[i] = val->second;      
        }
    }
    
    vector<pairs> vec;
    copy(filter_frequencies.begin(),filter_frequencies.end(),back_inserter<vector<pairs>>(vec));
    sort(vec.begin(),vec.end(),[](const pairs& l,const pairs&r){
        if(l.second != r.second)
            return l.second < r.second;
        return l.first < r.first;
    });
    
    return vec;
}

vector<int> extractOnlySymbolsCL(vector<int> values)
{
    vector<int> new_values;
    for(unsigned int i =0;i<values.size();i++)
    {
        if(values.at(i) >=16 && values.at(i) <=18)
        {
           new_values.push_back(values.at(i));
           i++; 
        }
        else
        {
            new_values.push_back(values.at(i));
        }
    }
    return new_values;
}


vector<int> getLengthsFromEncoding(int val)
{
    vector<int> dummy(0,0);
    if(val==1)
    {
       vector<int> ll_lengths(286,0);
        for (auto const &pairs: encodings_ll) {
            ll_lengths.at(pairs.first) = pairs.second;
        }
        return ll_lengths;
    }
    if(val==0)
    {
        vector<int> dist_lengths(32,0);
        for (auto const &pairs: encodings_dist) {
            dist_lengths.at(pairs.first) = pairs.second;
        }
        return dist_lengths;
    }
    return dummy;
}


vector<int> findElementInHuffmanTable(int symbol_value,int mode)
{
    vector<int> dummy;
    if(mode==1)
    {
        for(unsigned int i = 0;i<huffman_table_ll_symbols.size();i++)
        {
            if(huffman_table_ll_symbols[i].symbol_value==symbol_value)
            {
                vector<int> encoding;
                encoding.push_back(huffman_table_ll_symbols[i].encoding_value);
                encoding.push_back(huffman_table_ll_symbols[i].numbits);
                return encoding;
            }
        }
    }
    if(mode==0)
    {
        for(unsigned int i = 0;i<huffman_table_dist_symbols.size();i++)
        {
            if(huffman_table_dist_symbols[i].symbol_value==symbol_value)
            {
                vector<int> encoding;
                encoding.push_back(huffman_table_dist_symbols[i].encoding_value);
                encoding.push_back(huffman_table_dist_symbols[i].numbits);
                return encoding;
            }
        }
    }
        if(mode == 2)
    {
        for(unsigned int i = 0;i<huffman_cl_table.size();i++)
        {
            if(huffman_cl_table[i].symbol_value==symbol_value)
            {
                vector<int> encoding;
                encoding.push_back(huffman_cl_table.at(i).encoding_value);
                encoding.push_back(huffman_cl_table.at(i).numbits);
                return encoding;
            }
        }
    }
    return dummy;    
}

int convertHuffCodeToIntValue(int arr[],int n)
{
    int size = n-1;
    int newval = 0;
    for(int i = 0;i<n;++i)
    {
        int power = pow(2,size);
        if(arr[i]==1)
        {
            newval = newval+power;
        }
        size--;
    }
    return newval;
}


vector<int> trimCodeLength(vector<int> values)
{
    int last_value_index = 0;
    for(unsigned int i = 0;i<values.size();i++)
    {
        if(values.at(i)>0)
        {
            last_value_index = i;
        }
    }

    vector<int> new_lengths;
    for(int i = 0;i<last_value_index+1;i++)
    {
        new_lengths.push_back(values.at(i));
    }
    return new_lengths;
}


vector<int> RLEForLengthTables(vector<int> code_lengths)
{
        vector<int> rle_table;
        for (unsigned int i = 0; i < code_lengths.size(); i++) { 
        int count = 1; 
        while (i < code_lengths.size() - 1 && code_lengths.at(i) == code_lengths.at(i+1)) { 
            count++; 
            i++; 
        } 
  
        // Print character and its count 
        // cout << code_lengths.at(i) << count<<"\n";
        rle_table.push_back(code_lengths.at(i));
        rle_table.push_back(count);
        
    }
    return rle_table;    
}
void HuffmanCodes(vector<pairs> filter_frequencies,int val)   
{ 
    
    vector<int> data;
    vector<int> freq;
    
  for (auto& x: filter_frequencies) {
    data.push_back(x.first);
    freq.push_back(x.second);
  }
    
    // Construct Huffman Tree 
    struct MinHeapNode* root = buildHuffmanTree(data, freq, filter_frequencies.size()); 
  
    // Print Huffman codes using 
    // the Huffman tree built above 
    int arr[MAX_TREE_HT], top = 0;  
    printCodes(root, arr, top,val); 
}  
  

vector<int> generateCLSymbolEncodingForTable(vector<int> values)
{
    vector<int> symbol_table;
    for(unsigned int i = 0;i<values.size();)
    {
        int encoding_of = values.at(i);
        
        int length = values.at(++i);
        if(length==1)
        {
            symbol_table.push_back(encoding_of);
        }
        if(length==2)
        {
            symbol_table.push_back(encoding_of);
            symbol_table.push_back(encoding_of);
        }
        if(length ==3)
        {
            symbol_table.push_back(encoding_of);
            symbol_table.push_back(encoding_of);
            symbol_table.push_back(encoding_of);
        }
        if(length >3 && encoding_of > 0)
        {
            symbol_table.push_back(encoding_of);
            symbol_table.push_back(16);
            symbol_table.push_back(length - 3);
        }
        if(length >=3 && length<=10 && encoding_of == 0)
        {
            symbol_table.push_back(17);
            symbol_table.push_back(length - 3);
        }
        if(length >10 && encoding_of == 0)
        {
            symbol_table.push_back(18);
            int off_set = length - 11;
            if(off_set>=127)
            {
                symbol_table.push_back(127);
                length = off_set - 127;
                symbol_table.push_back(18);
                symbol_table.push_back(length - 11);    
            }
            else
            {
                symbol_table.push_back(off_set);
            }
            
        }
        //cout<<encoding_of<<","<<length;
        i++;
    }
    return symbol_table;
}  





//Block 1 Huffman Codes
 vector<vector<int>> huffmanCodesLLBlock_1()
 {
     vector<vector<int>> codes;
      codes.resize(288);
      for(int i = 0;i<144;i++)
      {
          codes.push_back(vector<int>());
          codes[i].push_back(48+i);
          codes[i].push_back(8);
      }
      for(int i = 144;i<256;i++)
      {
          codes.push_back(vector<int>());
          codes[i].push_back(i-144);
          codes[i].push_back(9);
      }
      for(int i = 256;i<280;i++)
      {
          codes.push_back(vector<int>());
          codes[i].push_back(i-256);
          codes[i].push_back(7);
      }
      for(int i = 280;i<288;i++)
      {
          codes.push_back(vector<int>());
          codes[i].push_back(i-280);
          codes[i].push_back(8);
      }
      return codes;
      
 }
 vector<vector<int>> huffmanCodesDistBlock_1()
 {
     vector<vector<int>> codes;
      codes.resize(32);
      for(int i = 0;i<32;i++)
      {
          codes.push_back(vector<int>());
          codes[i].push_back(i);
          codes[i].push_back(5);
      }
      return codes;
 }
};

#endif 