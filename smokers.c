#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

// Data about items and participants at the table
typedef struct {
    // Keep track of agent availability
    sem_t agentSem;
    // Keep track of items on table
    sem_t tobacco, paper, match;
    // Pusher mutex
    sem_t pusher_mutex;
    // Item trackers for pusher logic
    bool isTobacco, isPaper, isMatch;
    // Signals for smokers
    sem_t tobacco_sem, paper_sem, match_sem;
} table_t;

// Agent code, function names indicate the speicic items they generate
void* agent_tp(void* table_arg);
void* agent_tm(void* table_arg);
void* agent_pm(void* table_arg);
// Pusher code, function names indicate the specific item they wait for
void* pusher_tobacco(void* table_arg);
void* pusher_paper(void* table_arg);
void* pusher_match(void* table_arg);
// Smoker code, function names indicate the specific item they already have
void* smoker_tobacco(void* table_arg);
void* smoker_paper(void* table_arg);
void* smoker_match(void* table_arg);

int main() {
    // Seed for rand()
    srand(time(NULL));

    // Initialize the table
    table_t table;
    sem_init(&table.agentSem, 0, 1);
    sem_init(&table.tobacco, 0, 0);
    sem_init(&table.paper, 0, 0);
    sem_init(&table.match, 0, 0);
    sem_init(&table.pusher_mutex, 0, 1);
    table.isTobacco = false;
    table.isPaper = false;
    table.isMatch = false;
    sem_init(&table.tobacco_sem, 0, 0);
    sem_init(&table.paper_sem, 0, 0);
    sem_init(&table.match_sem, 0, 0);

    // Create agent threads
    pthread_t agent_thread_tp, agent_thread_tm, agent_thread_pm;
    pthread_create(&agent_thread_tp, NULL, agent_tp, (void*)&table);
    pthread_create(&agent_thread_tm, NULL, agent_tm, (void*)&table);
    pthread_create(&agent_thread_pm, NULL, agent_pm, (void*)&table);

    // Create pusher threads
    pthread_t pusher_thread_tobacco, pusher_thread_paper, pusher_thread_match;
    pthread_create(&pusher_thread_tobacco, NULL, pusher_tobacco, (void*)&table);
    pthread_create(&pusher_thread_paper, NULL, pusher_paper, (void*)&table);
    pthread_create(&pusher_thread_match, NULL, pusher_match, (void*)&table);

    // Create smoker threads
    pthread_t smoker_thread_tobacco1, smoker_thread_paper1, smoker_thread_match1,
              smoker_thread_tobacco2, smoker_thread_paper2, smoker_thread_match2;
    pthread_create(&smoker_thread_tobacco1, NULL, smoker_tobacco, (void*)&table);
    pthread_create(&smoker_thread_paper1, NULL, smoker_paper, (void*)&table);
    pthread_create(&smoker_thread_match1, NULL, smoker_match, (void*)&table);
    pthread_create(&smoker_thread_tobacco2, NULL, smoker_tobacco, (void*)&table);
    pthread_create(&smoker_thread_paper2, NULL, smoker_paper, (void*)&table);
    pthread_create(&smoker_thread_match2, NULL, smoker_match, (void*)&table);

    // Wait for all threads to finish
    pthread_join(agent_thread_tp, NULL);
    pthread_join(agent_thread_tm, NULL);
    pthread_join(agent_thread_pm, NULL);
    pthread_join(pusher_thread_tobacco, NULL);
    pthread_join(pusher_thread_paper, NULL);
    pthread_join(pusher_thread_match, NULL);
    pthread_join(smoker_thread_tobacco1, NULL);
    pthread_join(smoker_thread_paper1, NULL);
    pthread_join(smoker_thread_match1, NULL);
    pthread_join(smoker_thread_tobacco2, NULL);
    pthread_join(smoker_thread_paper2, NULL);
    pthread_join(smoker_thread_match2, NULL);
}

// Agent code for providing tobacco and paper
void* agent_tp(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    // Generate items
    for (int i = 0; i < 6; ++i) {
        // Wait for agent to be available
        sem_wait(&table->agentSem);

        // Put tobacco and paper on the table
        sem_post(&table->tobacco);
        sem_post(&table->paper);
        printf("Agent: Generate tobacco and paper\n");

        // Finish job and sleep for up to 200ms
        usleep((rand() % 200) * 1000);
    }

    return NULL;
}

// Agent code for providing tobacco and matches
void* agent_tm(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    // Generate items
    for (int i = 0; i < 6; ++i) {
        // Wait for agent to be available
        sem_wait(&table->agentSem);

        // Put tobacco and match on the table
        sem_post(&table->tobacco);
        sem_post(&table->match);
        printf("Agent: Generate tobacco and match\n");

        // Finish job and sleep for up to 200ms
        usleep((rand() % 200) * 1000);
    }

    return NULL;
}

// Agent code for providing paper and matches
void* agent_pm(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    // Generate items
    for (int i = 0; i < 6; ++i) {
        // Wait for agent to be available
        sem_wait(&table->agentSem);

        // Put paper and match on the table
        sem_post(&table->paper);
        sem_post(&table->match);
        printf("Agent: Generate paper and match\n");

        // Finish job and sleep for up to 200ms
        usleep((rand() % 200) * 1000);
    }

    return NULL;
}

// Pusher code for pushing tobacco + X
void* pusher_tobacco(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    // Wait for items and push them to smokers
    for (int i = 0; i < 12; ++i) {
        sem_wait(&table->tobacco);
        sem_wait(&table->pusher_mutex);
        if (table->isPaper) {
            table->isPaper = false;
            sem_post(&table->match_sem);
            printf("Pusher: Push tobacco and paper to smoker\n");
        } else if (table->isMatch) {
            table->isMatch = false;
            sem_post(&table->paper_sem);
            printf("Pusher: Push tobacco and match to smoker\n");
        } else
            table->isTobacco = true;
        sem_post(&table->pusher_mutex);
    }

    return NULL;
}

// Pusher code for pushing paper + X
void* pusher_paper(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    // Wait for items and push them to smokers
    for (int i = 0; i < 12; ++i) {
        sem_wait(&table->paper);
        sem_wait(&table->pusher_mutex);
        if (table->isTobacco) {
            table->isTobacco = false;
            sem_post(&table->match_sem);
            printf("Pusher: Push tobacco and paper to smoker\n");
        } else if (table->isMatch) {
            table->isMatch = false;
            sem_post(&table->tobacco_sem);
            printf("Pusher: Push paper and match to smoker\n");
        } else
            table->isPaper = true;
        sem_post(&table->pusher_mutex);
    }

    return NULL;
}

// Pusher code for pushing match + X
void* pusher_match(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    // Wait for items and push them to smokers
    for (int i = 0; i < 12; ++i) {
        sem_wait(&table->match);
        sem_wait(&table->pusher_mutex);
        if (table->isTobacco) {
            table->isTobacco = false;
            sem_post(&table->paper_sem);
            printf("Pusher: Push tobacco and match to smoker\n");
        } else if (table->isPaper) {
            table->isPaper = false;
            sem_post(&table->tobacco_sem);
            printf("Pusher: Push paper and match to smoker\n");
        } else
            table->isMatch = true;
        sem_post(&table->pusher_mutex);
    }

    return NULL;
}
// Smoker with infinite tobacco
void* smoker_tobacco(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    for (int i = 0; i < 3; ++i) {
        sem_wait(&table->tobacco_sem);
        // Make the cigarette
        usleep((rand() % 50) * 1000);
        sem_post(&table->agentSem);
        printf("Smoker (Tobacco): Smoke a cigarette\n");
        usleep((rand() % 50) * 1000);
    }

    printf("Smoker (Tobacco): Done\n");
    
    return NULL;
}

// Smoker with infinite paper
void* smoker_paper(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    for (int i = 0; i < 3; ++i) {
        sem_wait(&table->paper_sem);
        // Make the cigarette
        usleep((rand() % 50) * 1000);
        sem_post(&table->agentSem);
        printf("Smoker (Paper): Smoke a cigarette\n");
        usleep((rand() % 50) * 1000);
    }

    printf("Smoker (Paper): Done\n");
    
    return NULL;
}

// Smoker with infinite matches
void* smoker_match(void* table_arg) {
    // Convert table to usable format
    table_t *table = (table_t*) table_arg;

    for (int i = 0; i < 3; ++i) {
        sem_wait(&table->match_sem);
        // Make the cigarette
        usleep((rand() % 50) * 1000);
        sem_post(&table->agentSem);
        printf("Smoker (Match): Smoke a cigarette\n");
        usleep((rand() % 50) * 1000);
    }

    printf("Smoker (Match): Done\n");
    
    return NULL;
}

