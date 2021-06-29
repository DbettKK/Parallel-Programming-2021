#include<stdio.h>

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024


typedef struct VnProduction {
    int parent, child1, child2;
} VnProduction;

typedef struct VtProduction {
    int parent;
    char child;
} VtProduction;

int vnProductions_cnt, vtProductions_cnt;

VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];
unsigned dp[(1 + MAX_STRING_LENGTH) * MAX_STRING_LENGTH / 2 * MAX_VN_NUM];

int slen;

char solve[MAX_STRING_LENGTH];

int vnNum;
void input() {
    freopen("input3.txt", "r", stdin);
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
            if (c == tmp.child)  {
                int row = (slen + slen - (i - 1)) * i / 2;
                dp[(row + i) * vnNum + tmp.parent]++;
            }
        }
    }
    
    for (int len = 2; len <= slen; len++) {
        int left_end = slen - len;
        for (int left = 0; left <= left_end; left++) {
            int right_end = left + len;
            for (int right = left + 1; right < right_end; right++) {
                int all = ((slen + slen - (left - 1)) * left / 2 + (left + len - 1)) * vnNum;
                int leftPart = ((slen + slen - (left - 1)) * left / 2 + (right - 1)) * vnNum;
                int rightPart = ((slen + slen - (right - 1)) * right / 2 + (left + len - 1)) * vnNum;
                for (int vn_index = 0; vn_index < vnProductions_cnt; vn_index++) {
                    VnProduction tmp = vnProductions[vn_index];
                    dp[all + tmp.parent] += dp[leftPart + tmp.child1] * dp[rightPart + tmp.child2];
                }
            }
        }
    }
    printf("%u\n", dp[(slen - 1) * vnNum]);
    return 0;
}