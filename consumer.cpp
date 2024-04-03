#include <iostream>
#include <semaphore.h>
#include <thread>
#include <ctime>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

const int memorySize = 2;

struct sharedMemory {
    int table[memorySize];
    sem_t empty; 
    sem_t full;
    sem_t exclusion;
};

int main() {
    srand(time(nullptr));       // seeds rand for number generation

    int fd = shm_open("/sharedTable", O_RDWR, 0666); // opens POSIX shared memory object
    // maps shared memory
    sharedMemory *table = static_cast<sharedMemory*>(mmap(NULL, sizeof(struct sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    auto consumerLambda = [&]() {
        int number;
        while(true) {
            sem_wait(&table->full); // wait if table is empty
            sem_wait(&table->exclusion);

            // checks table for item to consume
            for(int i = 0; i < memorySize; ++i) {
                if (table->table[i] != 0) {
                    number = table->table[i];
                    table->table[i] = 0;
                    break;
                }
            }

            std::cout << "Consumer consumed number: " << number << '\n';

             // unlock semaphore
             sem_post(&table->exclusion);
             sem_post(&table->empty);

             sleep(rand() % 5); // slows down output, otherwise it goes very quickly forever
        }
    };

    std::thread consumer(consumerLambda);
    consumer.join();

    return 0;
}