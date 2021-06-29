#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<pthread.h>

#define MAX_PRODUCTION2_NUM 128
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 800

#define THREAD_NUM 31

typedef struct SubTree {
    /* map 数组实现 */
    unsigned vn[MAX_VN_NUM];
    /* set 数组实现 */
    int keys[MAX_VN_NUM];
    int mark[MAX_VN_NUM];
    int key_cnt;
    int initialized;
} SubTree;
/* Vn的数目 */
int vnNum;
/* 
    <Vn>::=Vt
    map 数组实现 key为vt value为vn 可能存在多个vn 所以需要cnt
    所以 production1[key][cnt] = value
        cnt = production1_cnt[key]
*/
int production1[128][MAX_VT_NUM];
int production1_cnt[128];
/*
    dp[i][j]记录着两个东西：
	    1、Map<int,int> vn：代表str[i][j]可能存在多少种Vn(Vn->str[i][j])，以及每种Vn对应的数目。key为Vn，value为数目。
	    2、Set<int> keys：存储所有的Vn
*/
SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];
/*
    对于<vn-l>:=<vn-r1><vn-r2>
    vnIndex[vn-r1][vn-r2][cnt] = vn-l   因为可能存在相同的vn-r1和vn-r2对应不同的vn-l 所以需要cnt
    cnt = vnIndex_num[vn-r1][vn-r2]
*/
int vnIndex[MAX_VN_NUM][MAX_VN_NUM][MAX_PRODUCTION2_NUM];
int vnIndex_num[MAX_VN_NUM][MAX_VN_NUM];
/* 输入字符串 */
char solve[MAX_STRING_LENGTH];
/* 字符串长度 */
int slen;

int thread_len;
void *task(void *args);
void input() {
    int production1_num, production2_num;
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vnNum);
    scanf("%d\n", &production2_num);
    for (int i = 0; i < production2_num; i++) {
        int key, value1, value2;
        scanf("<%d>::=<%d><%d>\n", &key, &value1, &value2);
        vnIndex[value1][value2][vnIndex_num[value1][value2]++] = key;
    }
    scanf("%d\n", &production1_num);
    for (int i = 0; i < production1_num; i++) {
        char key;
        int value;
        scanf("<%d>::=%c\n", &value, &key);
        production1[key][production1_cnt[key]++] = value;
    }
    scanf("%d\n", &slen);
    scanf("%s\n", solve);
    SubTree* char_map[128];
    memset(char_map, NULL, sizeof(SubTree*) * 128);
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        if (char_map[c] && char_map[c]->initialized == -1) {
            dp[i][i] = *char_map[c];
        } else {
            for (int index = 0; index < production1_cnt[c]; ++index) {
                int integer = production1[c][index];
                dp[i][i].vn[integer] = 1;
                if (!dp[i][i].mark[integer]) {
                    dp[i][i].keys[dp[i][i].key_cnt++] = integer;
                    dp[i][i].mark[integer] = 1;
                }
            }
            char_map[c] = &dp[i][i];
            char_map[c]->initialized = -1;
        }
    }
}

int main() {
    input();
    pthread_t handler[THREAD_NUM];
    for (int len = 2; len <= slen; len++) {
        thread_len = len;
        for (int i = 0; i < THREAD_NUM; i++) {
            pthread_create(&handler[i], NULL, task, (void *)i);
        }
        for (int i = 0; i < THREAD_NUM; i++) {
            pthread_join(handler[i], NULL);
        }
    }
    if (dp[0][slen - 1].vn[0] != 0) {
        printf("%u\n", dp[0][slen - 1].vn[0]);
    } else {
        printf("%u\n", 0);
    }
}

void *task(void *args) {
    int tid = (long long) args;
    int start = tid;
    int len = thread_len;
    for (int left = start; left <= slen - len; left += THREAD_NUM) {
        for (int right = left + 1; right < left + len; right++) {
            for (int leftIndex = 0; leftIndex < dp[left][right - 1].key_cnt; leftIndex++) {
                int leftVn = dp[left][right - 1].keys[leftIndex];
                int leftNum = dp[left][right - 1].vn[leftVn];
                for (int rightIndex = 0; rightIndex < dp[right][left + len - 1].key_cnt; rightIndex++) {
                    int rightVn = dp[right][left + len - 1].keys[rightIndex];
                    int rightNum = dp[right][left + len - 1].vn[rightVn];
                    if (vnIndex_num[leftVn][rightVn] <= 0) continue;
                    int tmpNum = vnIndex_num[leftVn][rightVn];
                    int total = leftNum * rightNum;
                    for (int index = 0; index < tmpNum; index++) {
                        int i = vnIndex[leftVn][rightVn][index];
                        if (!dp[left][left + len - 1].mark[i]) {
                            dp[left][left + len - 1].keys[dp[left][left + len - 1].key_cnt++] = i;
                            dp[left][left + len - 1].mark[i] = 1;
                        }
                        dp[left][left + len - 1].vn[i] += total;
                    }
                }
            }
        }
    }
    return NULL;
}

