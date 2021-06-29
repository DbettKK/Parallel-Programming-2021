#include<stdio.h>
#include<time.h>
#include<omp.h>

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

typedef struct SubTree {
    unsigned trees[MAX_VN_NUM];
} SubTree;

typedef struct VnProduction {
    int parent, child1, child2; // vn
} VnProduction;

typedef struct VtProduction {
    int parent; // vn
    char child; // vt
} VtProduction;

SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

int vnProductions_cnt, vtProductions_cnt;
VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];

int slen;
char solve[MAX_STRING_LENGTH];

int vnNum;

void input() {
    freopen("input2.txt", "r", stdin);
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
    clock_t start, end;
    start = clock();
    input();
    //#pragma omp parallel for
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int vt_index = 0; vt_index < vtProductions_cnt; vt_index++) {
            VtProduction tmp = vtProductions[vt_index];
            if (c == tmp.child)  dp[i][i].trees[tmp.parent]++;
        }
    }

    for (int len = 2; len <= slen; len++) {
        #pragma omp parallel for
        for (int left = 0; left <= slen - len; left++) {
            for (int right = left + 1; right < left + len; right++) {
                for (int vn_index = 0; vn_index < vnProductions_cnt; vn_index++) {
                    VnProduction tmp = vnProductions[vn_index];
                    dp[left][left + len - 1].trees[tmp.parent] += 
                        dp[left][right - 1].trees[tmp.child1] * dp[right][left + len - 1].trees[tmp.child2];
                }
            }
        }
    }
    printf("%u\n", dp[0][slen - 1].trees[0]);
    end = clock();
    printf("%lfms\n", (double)(end - start));
    return 0;
}