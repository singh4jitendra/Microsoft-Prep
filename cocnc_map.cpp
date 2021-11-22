#define SIZE 10

#include <iostream>
#include <algorithm>
#include <list>
#include <string> // missing include
#include <utility>
#include <functional>
#include <pthread.h>
#include <unordered_map>
#include <unistd.h>

using namespace std;

struct  node {
    int key; int val;
    struct node *next;
};

struct node *ht[SIZE];
pthread_mutex_t mt[SIZE+1];
unordered_map <int, struct node*> mp;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct node * newnode(int key, int val)
{
    struct node *tmp = new node;
    tmp->key = key;
    tmp->val = val;
    tmp->next = NULL;
    return tmp;
}

void add_at_front(int key, int val, struct node *root)
{
    struct node *tmp = new node;
    tmp->key = key;
    tmp->val = val;
    tmp->next = NULL;

    if(root == NULL)
        root = tmp;
    else
    {
        struct node *q = root;
        while(q->next != NULL)
            q = q->next;

        q->next = tmp;
    }
}

int search_in(struct node *root, int key, int hash)
{
    struct node *q = root;
   
    while(q != NULL)
    {
        if(q->key == key)
        {
            int tmp = q->val;
            return tmp;
        }
        q = q->next;
    }

    return -1;
}

int get_hash(int key)
{
    return (key % SIZE);
}

void put(int key, int val)
{
    pthread_mutex_lock(&(mt[SIZE+1]));
    int hash = get_hash(key);

    pthread_mutex_lock(&(mt[hash]));
    if(mp.find(hash) != mp.end())
    {
        add_at_front(key, val, mp[hash]);
    }
    else
    {
        mp[hash] = newnode(key,val);
    }
    pthread_cond_signal( &cond ); 
    pthread_mutex_unlock(&(mt[hash]));
}

int get(int key)
{
    pthread_mutex_lock(&(mt[SIZE+1]));
    while(mp.size()==0)
        pthread_cond_wait( &cond, &(mt[SIZE+1])); 
    int hash = get_hash(key);
    if(mp.find(hash) != mp.end())
    {
        pthread_mutex_lock(&(mt[hash]));
        return search_in(mp[hash], key, hash);
        pthread_mutex_unlock(&(mt[hash]));
        pthread_mutex_unlock(&(mt[SIZE+1]));
    }
    else
    {
        cout << "NOT FOUND" << endl;
        pthread_mutex_unlock(&(mt[SIZE+1]));
        return -1;
    }
}

void init_ht()
{
    int i;
    for(i=0; i<SIZE; i++)
    {
        ht[i] = NULL;
    }
}

void print()
{
    for (auto a: mp)
    {
        cout << a.first <<" -> ";
        struct node *q  =  a.second;
        while(q !=NULL)
        {
            cout << q->key <<":"<<q->val << ", ";
            q = q->next;
        }
        cout << endl;
    }
}

void *func1(void *varg)
{
    put(2,102);
    put(2,402);
    put(902,302);
    return NULL;
}
void *func2(void *varg)
{
    cout << get(802) << endl;
    cout << get(902) << endl;
    return NULL;
}


int main()
{
    pthread_t tid1, tid2;
    init_ht();
    pthread_mutex_init(mt,0);

    pthread_create(&tid1, NULL, &func1, NULL);    
    pthread_create(&tid2, NULL, &func2, NULL);   
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    

    //cout << get(902) << endl;
    print();
    pthread_exit(NULL);
    

    return 0;
}