#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <list>

#include "task.hpp"

using namespace std;

std::list<pthread_t *> thread_list;

void create_thread()
{
  pthread_t *thread = new pthread_t;

  cout << "Creating task" << endl;

  pthread_create(thread, NULL, task_main, NULL);

  thread_list.push_back(thread);
}

int main()
{
  cout << "Spawner started" << endl;

  create_thread();

  while (!thread_list.empty()) {
    pthread_t *thread = thread_list.front();
    thread_list.pop_front();
    pthread_join(*thread, NULL);
  }

  cout << "Spawner ended" << endl;
  return 0;
}
