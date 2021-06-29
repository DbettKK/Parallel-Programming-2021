#include<stdio.h>
#include<time.h>
#include<pthread.h>

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

#define THREAD_NUM 36
typedef long long ll;

pthread_mutex_t public_mutex;
pthread_cond_t public_cond;
int end_thread = 0;

typedef struct VnProduction {
    int parent, child1, child2;
} VnProduction;

typedef struct VtProduction {
    int parent;
    char child;
} VtProduction;

int vnProductions_cnt, vtProductions_cnt;
unsigned dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH][MAX_VN_NUM + 1];

VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];

int slen;
char solve[MAX_STRING_LENGTH];

int vnNum;

int taskNum = 0;
pthread_mutex_t while_mutex;

void *task(void *args);

void input() {
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vnNum);
    scanf("%d\n", &vnProductions_cnt);
    for (int i = 0; i < vnProductions_cnt; i++) {
        scanf("<%d>::=<%d><%d>\n", &vnProductions[i].parent, &vnProductions[i].child1, &vnProductions[i].child2);
    }    
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
            if (c == tmp.child)  {
                dp[i][i][tmp.parent]++;
                dp[i][i][MAX_VN_NUM] = 1;
            }
        }
    }
    
    pthread_mutex_init(&public_mutex, NULL);
    pthread_cond_init(&public_cond, NULL);
    pthread_mutex_init(&while_mutex, NULL);

    pthread_t handlers[THREAD_NUM];
    for (ll thread_index = 0; thread_index < THREAD_NUM; thread_index++) {
        pthread_create(&handlers[thread_index], NULL, task, (void *)thread_index);
    }
    for (ll thread_index = 0; thread_index < THREAD_NUM; thread_index++) {
        pthread_join(handlers[thread_index], NULL);
    }
    printf("%u\n", dp[0][slen - 1][0]);
    //end = clock();
    //printf("%lfms\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

void *task(void *args) {
    int tid = (long long) args;
    for (int len = 2; len <= slen; len++) {
        while (1) {
            pthread_mutex_lock(&while_mutex);
            int left = taskNum++;
            if (left > slen - len) {
                pthread_mutex_unlock(&while_mutex);
                break;
            }
            pthread_mutex_unlock(&while_mutex);
            for (int right = left + 1; right < left + len; right++) {
                if (dp[left][right-1][MAX_VN_NUM] != 1 || dp[right][left+len-1][MAX_VN_NUM] != 1) {
                    continue;           
                }
                int flag = 0;
                for (int vn_index = 0; vn_index < vnProductions_cnt; vn_index+=4) {
                    VnProduction tmp[4] = {
                        vnProductions[vn_index],
                        vnProductions[vn_index + 1],
                        vnProductions[vn_index + 2],
                        vnProductions[vn_index + 3]
                    };
                    dp[left][left+len-1][tmp[0].parent] += dp[left][right-1][tmp[0].child1] * dp[right][left+len-1][tmp[0].child2];
                    dp[left][left+len-1][tmp[1].parent] += dp[left][right-1][tmp[1].child1] * dp[right][left+len-1][tmp[1].child2];
                    dp[left][left+len-1][tmp[2].parent] += dp[left][right-1][tmp[2].child1] * dp[right][left+len-1][tmp[2].child2];
                    dp[left][left+len-1][tmp[3].parent] += dp[left][right-1][tmp[3].child1] * dp[right][left+len-1][tmp[3].child2];
                    if (!flag && (dp[left][left+len-1][tmp[0].parent] != 0 || 
                        dp[left][left+len-1][tmp[1].parent] != 0 ||
                            dp[left][left+len-1][tmp[2].parent] != 0 ||
                            dp[left][left+len-1][tmp[3].parent] != 0)) {
                        flag = 1;
                    }
                }
                if (flag) dp[left][left+len-1][MAX_VN_NUM] = 1;
            }
        }
        pthread_mutex_lock(&public_mutex);
        end_thread++;
        if (end_thread != THREAD_NUM) {
            pthread_cond_wait(&public_cond, &public_mutex);
        } else {
            end_thread = 0;
            taskNum = 0;
            pthread_cond_broadcast(&public_cond);
        }
        pthread_mutex_unlock(&public_mutex);
    }
    return NULL;
}
