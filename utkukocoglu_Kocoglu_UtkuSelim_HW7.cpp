#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <random>
#include <time.h>
#include <mutex>
#include "HW7DynIntQueue.h"
using namespace std;

//Utku Selim Kocoglu
//Date: 26 May 2021

HW7DynIntQueue fqueue, pqueue;
mutex myMutex, coutMutex;
int counter1 = 0;
int counter2 = 0;

int random_range(const int & min, const int & max) {
	static mt19937 generator(time(0));
	uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

void producer(int total_box_count){
	
	for (int i=1; i<=total_box_count; i++){
		
		myMutex.lock();
		int s = fqueue.getCurrentSize();
		if (s <total_box_count){
			fqueue.enqueue(i);
			myMutex.unlock();

			coutMutex.lock();
			time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			struct tm *ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout << "Producer has enqueued a new box " << i << " to filling queue (filling queue size is " << fqueue.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
			coutMutex.unlock();

		}
		else {
			myMutex.unlock();
			i--;
		}
	}
}
void filling (int ID, int min, int max, int total_box_count){
	
	int item;
	while (counter1 < total_box_count){
		
		myMutex.lock();
		if (!fqueue.isEmpty()){
			
			fqueue.dequeue(item);
			counter1++;
			myMutex.unlock();

			coutMutex.lock();
			time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			struct tm *ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout << "Filler " << ID << " started filling the box " << item << " (filling queue size is " << fqueue.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
			coutMutex.unlock();

			int r = random_range(min, max);
			this_thread::sleep_for(chrono::seconds(r));

			coutMutex.lock();
			tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout << "Filler " << ID << " finished filling the box " << item << ": " <<  put_time(ptm,"%X") << endl;
			coutMutex.unlock();

			myMutex.lock();
			int p = pqueue.getCurrentSize();
			if (p <total_box_count){
				pqueue.enqueue(item);
				myMutex.unlock();

				coutMutex.lock();
				time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
				struct tm *ptm = new struct tm; 
				localtime_s(ptm, &tt);
				cout << "Filler " << ID << " put box " << item << " into packaging queue (packaging queue size is " << pqueue.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
				coutMutex.unlock();
			}
		}
		else {
			myMutex.unlock();
		}
	}
}

void packaging (int ID, int min, int max, int total_box_count){
	
	int item;
	while (counter2 < total_box_count){
		
		myMutex.lock();
		if (!pqueue.isEmpty()){
			
			pqueue.dequeue(item);
			counter2++;
			myMutex.unlock();

			coutMutex.lock();
			time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			struct tm *ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout << "Packager " << ID << " started packaging the box " << item << " (packaging queue size is " << pqueue.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
			coutMutex.unlock();
			
			int r = random_range(min, max);
			this_thread::sleep_for(chrono::seconds(r));

			coutMutex.lock();
			tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout << "Packager " << ID << " finished packaging the box " << item << ": " <<  put_time(ptm,"%X") << endl;
			coutMutex.unlock();
		}
		else{
			myMutex.unlock();
		}
	}
}

int main(){
	
	int total_box_count;
	int minpro, maxpro;
	int minfill, maxfill;
	int minpack, maxpack;
	cout << "Please enter the total number of items: ";
	cin >> total_box_count;
	cout << "Please enter the min-max waiting time range of producer:\n";
	cout << "Min: ";
	cin >> minpro;
	cout << "Max: ";
	cin >> maxpro;
	cout << "Please enter the min-max waiting time range of filler workers:\n";
	cout << "Min: ";
	cin >> minfill;
	cout << "Max: ";
	cin >> maxfill;
	cout << "Please enter the min-max waiting time range of packager workers:\n";
	cout << "Min: ";
	cin >> minpack;
	cout << "Max: ";
	cin >> maxpack;
	time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
	struct tm *ptm = new struct tm; 
	localtime_s(ptm, &tt); 
	cout << "Simulation starts " << put_time(ptm,"%X") << endl;
	thread prdc(&producer, total_box_count);
	thread fill (&filling, 1, minfill, maxfill, total_box_count);
	thread fill2 (&filling, 2, minfill, maxfill, total_box_count);
	thread pack (&packaging, 1, minpack, maxpack, total_box_count);
	thread pack2 (&packaging, 2, minpack, maxpack, total_box_count);

	prdc.join();
	fill.join();
	fill2.join();
	pack.join();
	pack2.join();
	
	
	return 0;
}