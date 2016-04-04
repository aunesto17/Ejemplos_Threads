//
//  main.cpp
//  Threads_Examples
//
//  Created by Alexander Arturo Baylon Ibanez on 12/12/15.
//  Copyright © 2015 aunesto17. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

using namespace std;

//declaramos un mutex global
static mutex barrier;


//funcion simple
void call_from_thread(){
    cout << "Hola Amiguito " << endl;
}


//funcion con parametros
void call_from_thread_p(int tid)
{
    cout << "Creado por el Thread: " << tid << endl;
}

//funcion que suma los elementos de dos vectores
//esta funcion tiene problemas, ya que los threads pueden accesar a la variable result
//al mismo tiempo, pudiendo haber errores en la suma final
void dot_product(const vector<int> & v1, const vector<int> & v2, int & result, int L, int R){
    for (int i=L; i<R; i++) {
        result += v1[i] * v2[i];
    }
}

//usando mutex
void dot_product_mutex(const vector<int> & v1, const vector<int> & v2, int & result, int L, int R){
    int partial_sum = 0;//declaramos una suma parcial que nos ayuda a controlar el resultado final junto
                        //con el mutex
    for (int i=L; i<R; i++) {
        partial_sum += v1[i] * v2[i];
    }
    //a partir de aqui los threads esperaran a que otros threads terminen de usar la variable 'result'
    lock_guard<mutex> block_threads_until_finish_this_job(barrier);
    result += partial_sum;
}

//usando variable atomic
void dot_product_atomic(const vector<int> & v1, const vector<int> & v2, atomic<int> & result, int L, int R){
    int partial_sum = 0;//declaramos una suma parcial que nos ayuda a controlar el resultado final junto
    //con el mutex
    for (int i=L; i<R; i++) {
        partial_sum += v1[i] * v2[i];
    }
    //la variable atomic automaticamente bloquea entradas simultaneas de diferentes threads
    result += partial_sum;
}




vector<int> bounds(int parts, int mem) {
    vector<int>bnd;
    int delta = mem / parts;
    int reminder = mem % parts;
    int N1 = 0, N2 = 0;
    bnd.push_back(N1);
    for (int i = 0; i < parts; ++i) {
        N2 = N1 + delta;
        if (i == parts - 1)
            N2 += reminder;
        bnd.push_back(N2);
        N1 = N2;
    }
    return bnd;
}

int main(int argc, const char * argv[]) {
    
    //Un Thread
    /*
    //lanzar thread
    thread t1(call_from_thread);
    //Une el thread con el thread principal(main)
    t1.join();
    */
    
    
    //Varios Threads
    /*
    //De hecho 10 mas el thread principal(main) serian 11 threads
    thread t[10];
    //Lanzar el grupo de threads
    for (int i=0; i<10; i++) {
        t[i] = thread(call_from_thread);
    }
    
    cout << "Lanzado desde el main." << endl;
    
    //Unir los Threads con el thread principal
    for (int j=0; j<10; j++) {
        t[j].join();
    }
    */
    
    //Varios Threads con una variable
    /*
    //thread t[10];
    vector<thread> th;//tambien podemos utilizar vector con threads
    for (int i=0; i<10; i++) {
        //t[i] = thread(call_from_thread_p,i);
        th.push_back(thread(call_from_thread_p,i));
    }
    
    //for (int j=0; j<10; j++) {
    //    t[j].join();
    //}
    
    for (auto &t : th){
        t.join();
    }
    */
    
    int nr_elements = 100000;
    int nr_threads  = 2;
    int result      = 0;
    vector<thread> threads;
    
    vector<int> v1 (nr_elements,1);//lleno de unos
    vector<int> v2 (nr_elements,2);//lleno de dos
    
    vector<int> limits = bounds(nr_threads, nr_elements);
    
    //for (int i=0; i<nr_threads; i++) {
    //    threads.push_back(thread(dot_product,ref(v1),ref(v2),ref(result),limits[i],limits[i+1]));
    //}
    
    //for (int i=0; i<nr_threads; i++) {
    //    threads.push_back(thread(dot_product_mutex,ref(v1),ref(v2),ref(result),limits[i],limits[i+1]));
    //}
    
    //declaramos una variable atomic de tipo int
    atomic<int> result_a(0);
    
    for (int i=0; i<nr_threads; i++) {
        threads.push_back(thread(dot_product_atomic,ref(v1),ref(v2),ref(result_a),limits[i],limits[i+1]));
    }
    
    for (auto &t : threads){
        t.join();
    }
    
    //cout << "producto: " << result << endl;
    cout << "producto: " << result_a << endl;
    
    return 0;
}
