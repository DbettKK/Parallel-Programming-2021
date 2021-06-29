#include<stdio.h>
#include<pthread.h>

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

#define THREAD_NUM 31

typedef struct SubTree {
    unsigned trees[MAX_VN_NUM];
} SubTree;

typedef struct VnProduction {
    int parent, child1, child2;
} VnProduction;

typedef struct VtProduction {
    int parent;
    char child;
} VtProduction;

SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

int vnProductions_cnt, vtProductions_cnt;
VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];

int slen;
char solve[MAX_STRING_LENGTH];

int vnNum;

int thread_len, interval;

void *task(void *args);
void input() {
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
    input();
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int vt_index = 0; vt_index < vtProductions_cnt; vt_index++) {
            VtProduction tmp = vtProductions[vt_index];
            if (c == tmp.child)  dp[i][i].trees[tmp.parent]++;
        }
    }
    pthread_t handlers[THREAD_NUM];
    for (int len = 2; len <= slen; len++) {
        thread_len = len;
        for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) {
            pthread_create(&handlers[thread_index], NULL, task, (void *)thread_index);
        }
        for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) {
            pthread_join(handlers[thread_index], NULL);
        }
    }
    printf("%u\n", dp[0][slen - 1].trees[0]);
    return 0;
}

void *task(void *args) {
    int tid = (long long) args;
    int start = tid;
    int len = thread_len;
    for (int left = start; left <= slen - len; left += THREAD_NUM) {
        for (int right = left + 1; right < left + len; right++) {
            //SubTree *all =  &dp[left][left + len - 1];
            //SubTree *leftPart =  &dp[left][right - 1];
            //SubTree *rightPart =  &dp[right][left + len - 1];
            for (int vn_index = 0; vn_index < vnProductions_cnt; vn_index++) {
                VnProduction tmp = vnProductions[vn_index];
                //all->trees[tmp.parent] += 
                    //leftPart->trees[tmp.child1] * rightPart->trees[tmp.child2];
                dp[left][left + len - 1].trees[tmp.parent] += 
                    dp[left][right - 1].trees[tmp.child1] * dp[right][left + len - 1].trees[tmp.child2];
            }
        }
    }
    return NULL;
}