#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <bits/stdc++.h>
#include <fstream>
using namespace std;


std::mutex mtx;
std::mutex map_mutex;

int total_primes = 100000000;
int num_threads = 8;

//starts at 2
int current_N = 2;


int fetch_next_prime(){
    mtx.lock();
    int n = current_N;
    current_N += 1;
    mtx.unlock();
    return n;
}

void multi_sieveOfEratosthenes(std::vector<bool>& prime, int endN, int thread_no){
    std::vector<bool> local_prime(endN, true);

    while (true){
        int p = fetch_next_prime();
        if ( p*p > endN) break;

        // If prime[p] is not changed, then it is a prime
        if (local_prime[p])
        {
            // Update all multiples of p
            for (unsigned long int i=p*2; i<=endN; i += p){
                local_prime[i] = false;
            }
        }
    }

    map_mutex.lock();//so they don't overwrite values
    for(int i = 0; i < endN; i++){
        prime[i] = prime[i] && local_prime[i];
    }
    map_mutex.unlock();
}







int main()
{


    std::vector<std::thread> threads;
    std::vector<bool> prime(total_primes + 1, true);


    auto start = std::chrono::high_resolution_clock::now();

    //==== Spawning threads
    cout<<"Number of threads:"<<num_threads<<endl;
    for(int i = 0; i < num_threads; i++){
        threads.push_back( std::thread(multi_sieveOfEratosthenes, std::ref(prime), total_primes, i) );
    }

    //==== Joining threads
    for(int i = 0; i < num_threads; i++){
        threads[i].join();
    }

    auto finish = std::chrono::high_resolution_clock::now();

    //=== Counting all primes and sum
    unsigned long long sum = 0;
    long prime_count = 0;
    for (int i = 2; i < total_primes; i++){
        if(prime[i]==true){
            prime_count+=1;
            sum+=i;
        }
    }

    //=== Getting the last 10 primes
    int printed_primes = 0;
    int last_ten_primes[9];
    for(int i = total_primes-1; printed_primes<10; i--){
        if(prime[i]==true){
            last_ten_primes[printed_primes] = i;
            printed_primes+=1;
        }
    }



    double time = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();

    //=== Output into console and file
    ofstream myfile;
    myfile.open ("primes.txt");
    cout<<time/1000<<" "<<prime_count<<" "<<sum<<endl;
    myfile<<time/1000<<" "<<prime_count<<" "<<sum<<endl;
    for(int i = 9; i>-1; i--){
        cout<<last_ten_primes[i]<<endl;
        myfile<<last_ten_primes[i]<<endl;
    }

}
