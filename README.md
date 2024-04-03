# Operating Systems Producer-Consumer Problem
This program consists of two C++ files, one containing code for the consumer, and the other the producer. They use shared memory and semaphores to allow the two processses to interact with a table located in the shared memory. The producer produces a random number between 1 and 1000 and places it in array(table) in shared memory. The consumer consumes the numbers that it finds in the table, and replaces them with a 0 signifying that the slot is empty.
# Compilation Instructions
*producer.cpp can be compiled using the following command in a terminal: g++ producer.cpp -pthread -lrt -o producer
*consumer.cpp can be compiled using the following command in a terminal: g++ consumer.cpp -pthread -lrt -o consumer
# Execution Intstructions
The program can be executed using the following command in a terminal: ./producer & ./consumer &
# Author
Gavin Procick
