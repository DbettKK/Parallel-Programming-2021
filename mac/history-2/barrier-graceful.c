#include<stdio.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>

#define _POSIX_BARRIERS 1
#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

#define THREAD_NUM 31

typedef int pthread_barrierattr_t;
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

pthread_barrier_t barrier;

typedef struct VnProduction {
    int parent, child1, child2;
} VnProduction;

typedef struct VtProduction {
    int parent;
    char child;
} VtProduction;

int vnProductions_cnt, vtProductions_cnt;
unsigned dp[MAX_STRING_LENGTH * MAX_STRING_LENGTH * MAX_VN_NUM];
VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];


int slen;
char solve[MAX_STRING_LENGTH];

void *task(void *args);
void input() {
    int vnNum;
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vnNum);
    scanf("%d\n", &vnProductions_cnt);
    for (int i = 0; i < vnProductions_cnt; i++)
        scanf("<%d>::=<%d><%d>\n", &vnProductions[i].parent, &vnProductions[i].child1, &vnProductions[i].child2);
    scanf("%d\n", &vtProductions_cnt);
    for (int i = 0; i < vtProductions_cnt; i++)
        scanf("<%d>::=%c\n", &vtProductions[i].parent, &vtProductions[i].child);
    scanf("%d\n", &slen);
    scanf("%s\n", solve);
}

int main() {
    //clock_t start, end;
    //start = clock();
    input();
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int vt_index = 0; vt_index < vtProductions_cnt; vt_index++) {
            VtProduction tmp = vtProductions[vt_index];
            if (c == tmp.child)  dp[i * MAX_STRING_LENGTH * vnProductions_cnt + i * vnProductions_cnt + tmp.parent]++;
        }
    }

    pthread_barrier_init(&barrier, NULL, THREAD_NUM);
    pthread_t handlers[THREAD_NUM];
    for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) {
        pthread_create(&handlers[thread_index], NULL, task, (void *)thread_index);
    }
    for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) {
        pthread_join(handlers[thread_index], NULL);
    }
    printf("%u\n", dp[(slen - 1) * vnProductions_cnt]);
    //end = clock();
    //printf("%lfms\n", (double)(end - start));
    return 0;
}

void *task(void *args) {
    int tid = (long long) args;
    for (int len = 2; len <= slen; len++) {
        for (int left = tid; left <= slen - len; left += THREAD_NUM) {
            for (int right = left + 1; right < left + len; right++) {
                int all = left * MAX_STRING_LENGTH * vnProductions_cnt + (left + len - 1) * vnProductions_cnt;
                int leftPart = left * MAX_STRING_LENGTH * vnProductions_cnt + (right - 1) * vnProductions_cnt;
                int rightPart = right * MAX_STRING_LENGTH * vnProductions_cnt + (left + len - 1) * vnProductions_cnt;
                for (int vn_index = 0; vn_index < vnProductions_cnt; vn_index++) {
                    VnProduction tmp = vnProductions[vn_index];
                    dp[all + tmp.parent] += dp[leftPart + tmp.child1] * dp[rightPart + tmp.child2];
                }
            }
        }
        pthread_barrier_wait(&barrier);
    }
    return NULL;
}