CSC-485B - Assignment 2.
Archit Kumar - V00875990.

Files provided.

uvgz.cpp, Block1.hpp, Block2.hpp, huffman.hpp ,lzss.hpp , input.txt, output_stream.hpp, CRC.hpp.

The program defaults to using block type 1. If you want to use block type 2 please use this command.

./uvgz 2 < input.txt > compressed.gz

Features implemented.
    According to the rubric in section 6, a readme file is being provided and the initial code was submitted on June 15th.
    
    Requirement 1.
    The code uses a class called "lzss.hpp" to generate the lzss compressed stream with a window size of 2048. There are 
    evidence that it uses backreference on trivial inputs. I have provided a file called input.txt. Which has the original size
    of 178 bytes, for further explanations I am only talking about this file for now. 
    When you run the program with this file and using block type 1 it reduces the file size to 74 bytes. You can check the backreference
    generation using gzstat tool that was provided with the assignment.
    When you run this program with the file and using block type 2 it reduces the file size to 74 bytes as well. You can check backreference
    generation using gzstat tool that was provided with the assignment. Please note that upon further testing it was found that the program does not 
    work correctly on all input and there are some bugs here and there. And that's it, it does not work on big inputs and fail somewhere in between.
    But what I am trying to say here is that the correct logic is implemented and can proved on smaller inputs.
    
    Requirement 2.
    When the block type 2 is used to encode data, please refer to "Block2.hpp" for the details of the implementation. As specified in the requirement
    it uses the frequency of the actual data to generate the huffman codes for LL and distance codes. In the essence of time I was unable to fix the 
    requirement for maximum length 15. The code correctly generates the huffman codes, but on big inputs you will find that the assert statement for maximum
    length 15 breaks the code.
    
    Advanced feature 1.
    The block type 2 header was correctly optimized using proper computation of CL codes for the encoding of LL and distance codes produced in requirement 2.
    This can be verified by running the output of the file(mentioned above 74 bytes)through gzstat and you will find that this requirement is satisfied. RLE 
    enciding was used to reduce the long runs and the techniques provided in the lecture were implemented.
    
    Advanced feature 2.
    Trivial backreference were omitted from the lzss stream. Please consult "lzss.hpp" for details for this implementation, trivial lengths such as 1 and 2 are
    omitted from being backreferenced. Further more null characters are encoded as null in the stream. Saving more space. When lengths were 1 and 2 literal encoding
    is being used in compressed stream. Furthermore there are also measures to find closer references.
    
Refernces:

1. Only outside code was used for reference was the huffman encoding but has been modified heavily. Consult "huffman.hpp" for more details.  
Here is the link : https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/  