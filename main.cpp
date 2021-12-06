/******************************************************************************

                              Online C++ Debugger.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Debug" button to debug it.

*******************************************************************************/

// PROBLEM:
//  - Producer will produce and consumer will consume with synchronisation
//     of common buffer.
//  - Will continue intil producer thread does not produce any data for the consumer
//  - Threads will use a condition variable to notify each other
//  - Mutex needs to be used because the CV waits on the Mutex


#include <iostream>
#include <chrono>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

using namespace std;

const unsigned int NUM_REPUB = 4;
const unsigned int NUM_DEMOC = 7;
const unsigned int NUM_UBERDRVR = 1;


class Semaphore
{
public:
Semaphore (int count_ = 0):count (count_)
  {
  };

  inline void notify (int tid)
  {
    std::unique_lock < std::mutex > lock (mtx);
    --count;
    // cout << "thread " << tid << " notify" << endl;
    cv.notify_one ();
  };
  inline void wait (int tid)
  {
    std::unique_lock < std::mutex > lock (mtx);
    while (count == 0)
      {
// 	cout << "thread " << tid << " wait" << endl;
	cv.wait (lock);
// 	cout << "thread " << tid << " run" << endl;
      }
    ++count;
  };
  int waiting()
  {
      return count;
  };
private:
  std::mutex mtx;
  std::condition_variable cv;
  atomic<int> count;
};


class Uber
{
    public:
        Uber(Semaphore* repub_sem, Semaphore* democ_sem, int id_): repubsem(repub_sem), democsem(democ_sem), id(id), t(&Uber::checkready, this)
        {
            cout << "Uber driver ID " << id << " was born" << endl;
        };
        void checkready()
        {
            unique_lock<mutex> lck(m);
            cv.wait(lck, [this](){return (repubsem->waiting() + democsem->waiting()) >= 4;
            });
            if(repubsem->waiting() + democsem->waiting() >= 4)
               {
                   if(repubsem->waiting() >= 4)
                   {
                       for(int i=0; i<4; i++)
                       {
                           repubsem->notify(i);
                       }
                   }
                   else if(democsem->waiting() >= 4)
                   {
                       for (int i=0; i<4; i++)
                       {
                           democsem->notify(i);
                       }
                   }
                   else if(democsem->waiting() >= 2 && repubsem->waiting() >= 4)
                   {
                       for (int i=0; i<2; i++)
                       {
                           democsem->notify(i);
                           repubsem->notify(i);
                       }
                   }
                   
               }
        };
        ~Uber()
        {
            t.join();  
        };
    private:
        thread t;
        int id;
        Semaphore* repubsem;
        Semaphore* democsem;
        condition_variable cv;
        mutex m;
};

class Repub
{
    public:
        Repub(Semaphore* repub_sem, int id): repubsem(repub_sem), id(id), t(&Repub::getRide, this)
        {
            
        };
        void getRide()
        {
            cout << "Republican " << id << " is waiting for Uber" << endl;
            repubsem->wait(id);
        };
        ~Repub()
        {
            t.join();
        }
    private:
        thread t;
        int id;
        Semaphore* repubsem;
};

class Democ
{
    public:
        Democ(Semaphore* democ_sem, int id): democsem(democ_sem), id(id), t(&Democ::getRide, this)
        {
            
        };
        void getRide()
        {
            cout << "Democrat " << id << " is waiting for Uber" << endl;
            democsem->wait(id);
        };
        ~Democ()
        {
            t.join();
        }
    private:
        thread t;
        int id;
        Semaphore* democsem;
};

int main ()
{
    Semaphore repub_sem;
    Semaphore democ_sem;
    vector<Repub*> repubvec;
    vector<Democ*> democvec;
    vector<Uber*> ubervec;
    repubvec.reserve(NUM_REPUB);
    democvec.reserve(NUM_DEMOC);
    ubervec.reserve(NUM_UBERDRVR);
    
    for(int i=0; i<NUM_UBERDRVR; ++i)
    {
        ubervec.push_back(new Uber(&repub_sem, &democ_sem, i+1));
    }
    
    for(int i=0; i<NUM_REPUB; ++i)
    {
        repubvec.push_back(new Repub(&repub_sem, i+1));
    }
    for(int i=0; i<NUM_DEMOC; ++i)
    {
        democvec.push_back(new Democ(&democ_sem, i+1));
    }
    // int jj = 10;
    // returnint(&jj);
    // cout << jj << endl;
    
    for(auto s : repubvec)
    {
        delete s;
    }
    for(auto s : democvec)
    {
        delete s;
    }
    for(auto s : ubervec)
    {
        delete s;
    }
    
    // Fork t;

    

  

  cout << "Program has ended." << endl;
  return 0;
}
