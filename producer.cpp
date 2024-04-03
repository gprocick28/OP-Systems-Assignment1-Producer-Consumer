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

    int fd = shm_open("/sharedTable", O_CREAT | O_RDWR, 0666); // creates and opens new POSIX shared memory object
    ftruncate(fd, sizeof(struct sharedMemory));                // makes POSIX shared memory the size of sharedMemory
    // creates new mapping
    sharedMemory *table = static_cast<sharedMemory*>(mmap(NULL, sizeof(struct sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    // initialize semaphores
    sem_init(&table->full, 1, 0);
    sem_init(&table->empty, 1, memorySize);
    sem_init(&table->exclusion, 1, 1);

    auto producerLambda = [&]() {
        int number;
        while(true) {
            number = rand() % 1000 + 1; // creates random number between 1 and 1000 inclusive

            sem_wait(&table->empty);        // waits if table is full
            sem_wait(&table->exclusion);

            // places number in table, breaks after so as not to place it multiple times
            for(int i = 0; i < memorySize; ++i) {
                if(table->table[i] == 0) {
                    table->table[i] = number;
                    break;
                }
            }

            std::cout << "Producer produced number: " << number << '\n';

            // unlock semaphore
            sem_post(&table->exclusion);
            sem_post(&table->full);

            sleep(rand() % 5);  // slows down output, otherwise it goes very quickly forever

        }
    };

    std::thread producer(producerLambda);
    producer.join();

    return 0;
}

/* tried a regular functon before a lambda, couldn't get it to work
auto producer(sharedMemory *table) {
    int number;
    while(1) {
        number = rand() % 1000; // creates random number between 0 and 999 inclusive

        sem_wait(&table->empty);        // waits if table is full
        sem_wait(&table->exclusion);

        // places number in table, breaks after so as not to place it multiple times
        for(int i = 0; i < memorySize; ++i) {
            if(table->table[i] == 0) {
                table->table[i] = number;
                break;
            }
        }

        std::cout << "Producer produced number: " << number << '\n';
        return 0;
    }
}
*/