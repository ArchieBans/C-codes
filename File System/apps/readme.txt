Archit Kumar
V00875990.
Readme File LLFS.

Key Points of the file system.
1. Naming of the functions is not as same as said in the assignment, but FUNCTIONALITY is same.
2. I have used 31 blocks for my workings of the file system. When you initialize() the disk you see more details on that.
3. Currently with one block pointer for each group the max file size system can store is 10 blocks or 5120 bytes.
4. There are two methods write_files_alpha and write_files_beta that writes the files. I am using write_files_beta as it can write max 10 blocks to disk.
5. The programs are setup using the directory structure provided in the description.There should be 3 folder apps, disk and io. If they are not there then program will not work.
6. The tests provided do exactly what they have been asked to do and demonstrate that they work.
7. Some words about system robustness. Out of those 31 blocks 15 blocks are checkpoint functionality. Everytime there is write or anythihg disk perform
   system makes checkpoints and updateSuperblock that it is in sync mode. Upon successfull completion of the requested task superBlock is again updated 
   saying sync is over.
   If for some reason system crashes then superBlock knows that sync wasn't done properly and upon start it will check and restore the system from checkpoints.
   This gives a good scenario that the system is robust to crashes. 
8. You only need to run the MakeFile provided and then you can start executing the tests.
9. If you are looking to debug the system there are methods in it that will allow you to do that. Look for prints in file.c, if you know what you are looking there
   is everything provided for the debug. 
10. The use -w in diff is necessary. Because different framework use different file endings. The contents of the files are same.   