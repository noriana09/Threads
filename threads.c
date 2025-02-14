/////////////////////////////////////
//
// @author : NORIANA XHAXHAJ CSD5016
// date    : 06/11/2024
//
/////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 12

int number_of_students;
int seat_Departments;

//students
typedef struct{
    int id;
    int department;
    int sg;
    pthread_cond_t cond;
    pthread_mutex_t lock;
}Student;

//queue type
typedef struct Node{
    Student *student;
    struct Node *next;
} Node;

//Queue structure
typedef struct{
    Node *front;
    Node *rear;
    pthread_mutex_t lock;
} Queue;

Queue student_queue;  //queue at stop A
Queue queueB;         //queue at stop B
Queue students_finished;   //queue for students who are home
Student *students;  //table of students

int seats[4];
int can_study = 0;
int students_home=0;
pthread_mutex_t can_study_lock = PTHREAD_MUTEX_INITIALIZER;//mutex can_study
pthread_mutex_t home_lock = PTHREAD_MUTEX_INITIALIZER;//mutex students_home
pthread_barrier_t barrier;

char* dep_name(int id){
    if(id % 4 == 0){
        return "Math";
    }else if(id % 4 == 1){
        return "Physics";
    }else if(id % 4 == 2){
        return "Chemistry";
    }else if (id % 4 == 3){
        return "CSD";
    }else{
        return "Unknown";
    }
}

//Init queue
void initialize_queue(Queue *queue){
    queue->front=queue->rear = NULL;
    pthread_mutex_init(&queue->lock, NULL);
}

void enqueue(Queue *queue, Student *student){
    Node *new_node=(Node *)malloc(sizeof(Node));
    new_node->student=student;
    new_node->next=NULL;
    pthread_mutex_lock(&queue->lock);
    if(queue->rear == NULL){
        queue->front=queue->rear=new_node;
    }else{
        queue->rear->next=new_node;
        queue->rear=new_node;
    }
    pthread_mutex_unlock(&queue->lock);
}


void *bus_f(void *arg){
    while (1) {
        printf("\nBus is at Stop A: \n\n");
        //students are home
        pthread_mutex_lock(&students_finished.lock);
        Node *current2 = students_finished.front;
        while (current2 != NULL) {
            printf("Student %d (%s) has gone home.\n", current2->student->id, dep_name(current2->student->department));
            Node *temp=current2;
            current2=current2->next;
            free(temp);
        }
        students_finished.front=students_finished.rear=NULL;//reset queue
        pthread_mutex_unlock(&students_finished.lock);
        //check if all students are home
        pthread_mutex_lock(&home_lock);
        if (students_home==number_of_students){
            pthread_mutex_unlock(&home_lock);
            break;  //finish students threads
        }
        pthread_mutex_unlock(&home_lock);
        sleep(3);//wait at stop A
        //reset seats for departments
        for(int i = 0; i <4; i++){
            seats[i]=0;
        }
        pthread_mutex_lock(&student_queue.lock);

        //treverse queue 
        Node *current=student_queue.front;
        while (current != NULL){
            int dep=current->student->department;
            //elegxos opoiow einai sthn stash A kai an xwraei 
            if (current->student->sg == 0 && seats[dep] < seat_Departments){
                pthread_mutex_lock(&current->student->lock);
                current->student->sg=1;//signal that student boarded to the bus
                pthread_cond_signal(&current->student->cond);
                pthread_mutex_unlock(&current->student->lock);
                printf("Student %d (%s) boarded to the bus.\n", current->student->id, dep_name(current->student->department));
                seats[dep]++;
            }else{
                if(current->student->sg==1){
    
                }else{
                    printf("Student %d (%s) can not boarded to the bus.\n", current->student->id, dep_name(current->student->department));
                }           
            }
            current = current->next;
        }
        pthread_mutex_unlock(&student_queue.lock);
        printf("\nBus is on the way to University...!\n\n");
        sleep(10);
        printf("\nBus arrived at university!\n\n");
        //students can study , bus arived an Uni
        pthread_mutex_lock(&can_study_lock);
        can_study = 1;
        pthread_mutex_unlock(&can_study_lock);
        sleep(3);// wait an stop B
        pthread_mutex_lock(&can_study_lock);
        can_study = 0;
        pthread_mutex_unlock(&can_study_lock);
        printf("\nBus is heading back to Stop A.\n\n");
        //move students from queueB (uni) to home and print each student going home
        pthread_mutex_lock(&queueB.lock);
        current = queueB.front;
        while(current != NULL){
            Node *temp=current;
            current=current->next;
            enqueue(&students_finished, temp->student);
            pthread_mutex_lock(&home_lock);
            students_home++;
            pthread_mutex_unlock(&home_lock);
            free(temp);
        }
        queueB.front=queueB.rear=NULL;
        pthread_mutex_unlock(&queueB.lock);
        sleep(10);// back to stop A
    }
    printf("\nAll students from stop A went to University and came back!.\n\n");
    return NULL;
}

// Student function
void *students_f(void *arg){
    //barrier to syncronize students
    pthread_barrier_wait(&barrier);
    Student *student=(Student *)arg;
    enqueue(&student_queue, student);
    //wait for signal 1, students board to the bus
    pthread_mutex_lock(&student->lock);
    while(student->sg == 0){
        pthread_cond_wait(&student->cond, &student->lock);
    }
    pthread_mutex_unlock(&student->lock);
    //weit to study students
    pthread_mutex_lock(&can_study_lock);
    while (can_study == 0) {
        pthread_mutex_unlock(&can_study_lock);
        pthread_mutex_lock(&can_study_lock);
    }
    pthread_mutex_unlock(&can_study_lock);
    //study time 5-15 sec
    int study_time = 5 + rand() % 11;
    printf("Student %d (%s) went to University!\n",student->id, dep_name(student->department));
    sleep(study_time);
    printf("Student %d studied for %d seconds, and now is heading to Stop B.\n", student->id, study_time);
    enqueue(&queueB, student);
    return NULL;
}

int main() {
    int cap;
    printf("Enter the number of studens: \n");
    scanf("%d", &number_of_students);
    printf("Enter the Bus Capasity: \n");
    scanf("%d", &cap);
    seat_Departments=cap/4; 
    students=(Student *)malloc(number_of_students * sizeof(Student));
    pthread_t student_threads[number_of_students];
    pthread_t bus_thread;
    // init queues
    initialize_queue(&student_queue);
    initialize_queue(&queueB);
    initialize_queue(&students_finished);
    pthread_barrier_init(&barrier,NULL,number_of_students);
    for(int i = 0; i < number_of_students; i++){
        students[i].id=i;
        students[i].department = i % 4;
        students[i].sg=0;
        pthread_cond_init(&students[i].cond, NULL);
        pthread_mutex_init(&students[i].lock, NULL);
        pthread_create(&student_threads[i], NULL, students_f, (void *)&students[i]);
        printf("Student %d (%s) created.\n", students[i].id, dep_name(students[i].department));
    }
    pthread_create(&bus_thread, NULL, bus_f, NULL);
    for(int i = 0; i < number_of_students; i++){
        pthread_join(student_threads[i], NULL);
        pthread_cond_destroy(&students[i].cond);
        pthread_mutex_destroy(&students[i].lock);
    }
    pthread_join(bus_thread, NULL);
    return 0;
}
