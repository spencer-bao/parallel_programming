#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct Task{
    int inserted;
    int extractor;
};

pthread_mutex_t task_queue_lock;
int task_available;
struct Task task_queue[2];
int N = 2;
int task_counter = 0;



void create_task(struct Task *task){

    task->inserted = 1;
    task_counter++;

}

int done(){

    if (task_counter < N){
        return 0;
    }
    else{
        return 1;
    }
}

void insert_into_queue(struct Task task){
    for (int i = 0; i < 2; i++){
        if (task_queue[i].inserted != 1){
            task_queue[i].inserted = 1;
            printf("Producer inserted task.");
            break;
        }
    }
}

void *producer(void *producer_thread_data){ //argument pointer to an array
        struct Task my_task;
        int inserted;
        while (!done()){
            inserted = 0;
            create_task(&my_task);
            while (inserted == 0){
                pthread_mutex_lock(&task_queue_lock);
                // critical section
                if (task_available == 0 || task_available == 1){
                    insert_into_queue(my_task);
                    task_available++;
                    inserted++;
                }
                pthread_mutex_unlock(&task_queue_lock);
            }
        }
        
}



void extract_from_queue(struct Task *task){
    for (int i = 0; i < 2; i++){
        if (task_queue[i].inserted != 0){
            task_queue[i].inserted = 0;
            break;
        }
    }
}

void process_task(struct Task task){
    printf("Consumer ");
    printf(task.extractor);
    printf(" extracted task.");
}

void *consumer(void *consumer_thread_data){
    int extracted;
    struct Task my_task;
    my_task.extractor = (int *) consumer_thread_data;
    while (!done()){
        extracted = 0;
        while (extracted == 0){
            pthread_mutex_lock(&task_queue_lock);
            if (task_available == 1 || task_available == 2){
                extract_from_queue(&my_task);
                task_available--;
                extracted++;
            }
            pthread_mutex_unlock(&task_queue_lock);
        }
        process_task(my_task);
    }
    pthread_exit(NULL);
}

int main(){
    //tasks = argv[1]
    

    pthread_t producer;
    pthread_t consumer1;
    pthread_t consumer2;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    task_available = 0;
    pthread_mutex_init(&task_queue_lock, NULL);

    pthread_create(&producer, &attr, producer, NULL);
    pthread_create(&consumer1, &attr, consumer, 1);
    pthread_create(&consumer2, &attr, consumer, 2);

    pthread_join(producer, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);

    return 0;
}