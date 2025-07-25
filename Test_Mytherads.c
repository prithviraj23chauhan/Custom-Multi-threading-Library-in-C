void threadFunctionNoArg(){
    int i;
    for (i = 0; i < 10; ++i) {
        
        lock();              // *** try removing lock()

        printf("tip");
        sleep(1);
        printf(" toe\n");
        
        release();
    }
    // JOIN(1);
    printf("tip toe done\n");
}

void threadFunctionNoArg2(){
   int i;
    for (i = 0; i < 5; ++i) {
       
        
        lock();             // *** try removing lock()
        printf("kit");
        sleep(1);
        printf(" kat\n");
        release();
    }
    join(0);                // *** try removing join()
    printf("kit kat done\n");
}



void* add(void* arg){
    int i;
    int * temp = (int*)arg;
    for (i = 0; i < 10; ++i) {
        
        lock();             
        
        (*temp)++;
        printf("\nadd : %d\n",(*temp));
        usleep(1000000);
        
        release();
    }
    // JOIN(1);
    printf("addition done\n");
    return temp;
}

void* multiply(void* arg) {
    int i;
    int * temp = (int*)GetThreadResult(0);
    // int* temp = (int*)arg;
    for (i = 0; i < 5; ++i) {

        lock();             

        *temp *= 2 ;
        printf("mul : %d\n",(*temp));
        usleep(1000000);

        release();
        
    }
    printf("multiplication done done\n");
    return temp;
}

void test1(void){
    // testing lock and join feature
    printf(" ** STARTING TEST 1  **\n\n");
    int thread1 = create(threadFunctionNoArg);
    int thread2 = create(threadFunctionNoArg2);
    start();
    printf(" ** TEST 1 DONE  **\n\n");
}

void test2(void){
    printf(" ** STARTING TEST 2  **\n\n");
    int arg = 1;
    int thread1 = createWithArgs(add, &arg);
    int thread2 = createWithArgs(multiply, &arg);
    start();
    printf(" ** TEST 2 DONE  **\n\n");
}

void main() {
    int c;
    printf("TEST 1 OR TEST 2 :");
    scanf("%d",&c);
    if(c==1)
        test1();
    else if(c==2)
        test2();
    else    
        printf("wrong choice");
    printf(" ** ALL TESTS DONE  **\n\n\n");    
}