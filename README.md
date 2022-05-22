# Bloom Filters: Identify Badspeak/Oldspeak Words in Text Streams

This program determines whether a word is in "badspeak.txt" or "newspeak.txt". If a word is in "badspeak.txt", it will be added to a bloom filter that is constructed from a bit vector. If a word is in "newspeak.txt" it will be added to a hashtable that is constructed from a binary tree consisting of nodes. The program will then parse the user input to determine if any words used are in the bloom filter or hash table. If it is, the word will be added to a linked list. If the linked list is not empty, it's contents will be printed to the screen.

To build the program:

"make clean" will get rid of the object/executable files
"make format" will format all the files
"make" compiles the program

To run the program:

"./banhammer -t size -f size [text file]" will print a  message followed by the prohibited words the user wrote
"./banhammer -t size -f size -s [text file]" will print statistics of the binary tree

Command line options:

"-h" will print a helper message
"-t" will specify the hash table size
"-f" will specify the bloom filter size
"-s" will calculate and print statistics
