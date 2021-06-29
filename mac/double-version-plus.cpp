#include<iostream>
#include<cstring>
#include<unordered_map>
using namespace std;

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

typedef struct SubTree {
    unsigned vn[MAX_VN_NUM];
    int keys[MAX_VN_NUM];
    int mark[MAX_VN_NUM];
    int key_cnt;
} SubTree;

unordered_map<unsigned, unsigned> dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

typedef struct VnProduction {
    int parent, child1, child2;
} VnProduction;

VnProduction vn_production[MAX_PRODUCTION2_NUM];
int vnNum;
int production1[128][MAX_VT_NUM];
int production1_cnt[128];
//SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];
int vnIndex[MAX_VN_NUM][MAX_VN_NUM][MAX_PRODUCTION2_NUM];
int vnIndex_num[MAX_VN_NUM][MAX_VN_NUM];
char solve[MAX_STRING_LENGTH];
int slen;
int production1_num, production2_num;
void input() {
    freopen("input1.txt", "r", stdin);
    scanf("%d\n", &vnNum);
    scanf("%d\n", &production2_num);
    for (int i = 0; i < production2_num; i++) {
        int key, value1, value2;
        scanf("<%d>::=<%d><%d>\n", &key, &value1, &value2);
        vnIndex[value1][value2][vnIndex_num[value1][value2]++] = key;
        vn_production[i].parent = key;
        vn_production[i].child1 = value1;
        vn_production[i].child2 = value2;
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
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int index = 0; index < production1_cnt[c]; ++index) {
            int tmp = production1[c][index];
            dp[i][i].insert(pair<unsigned, unsigned>(tmp, 1));
            /*dp[i][i].vn[integer] = 1;
            if (!dp[i][i].mark[integer]) {
                dp[i][i].keys[dp[i][i].key_cnt++] = integer;
                dp[i][i].mark[integer] = 1;
            }*/
        }
    }
}

int main() {
    input();
    clock_t start, end;
    start = clock();
    int count = 0, tcount = 0;
    for (int len = 2; len <= slen; len++) {
        printf("%d\n", len);
        for (int left = 0; left <= slen - len; left++) {
            for (int right = left + 1; right < left + len; right++) {
                int left_cnt = dp[left][right - 1].size();
                int right_cnt = dp[right][left + len - 1].size();
                tcount++;
                if (left_cnt * right_cnt < 64) {
                    count++;
                    for (auto& l_kv : dp[left][right - 1]) {
                        for (auto& r_kv : dp[right][left + len - 1]) {
                            int leftVn = l_kv.first;
                            int rightVn = r_kv.first;
                            if (vnIndex_num[leftVn][rightVn] <= 0) continue;
                            int tmpNum = vnIndex_num[leftVn][rightVn];
                            int total = l_kv.second * r_kv.second;
                            for (int index = 0; index < tmpNum; index++) {
                                int i = vnIndex[leftVn][rightVn][index];
                                if (dp[left][left + len - 1].find(i) == dp[left][left + len - 1].end()) {
                                    dp[left][left + len - 1].insert(pair<unsigned, unsigned>(i, total));
                                }
                                else dp[left][left + len - 1].find(i)->second += total;
                            }
                        }
                    }
                } else {
                    for (int vn_index = 0; vn_index < production2_num; vn_index++) {
                        VnProduction tmp = vn_production[vn_index];
                        if (dp[left][right - 1].find(tmp.child1) != dp[left][right - 1].end() &&
                            dp[right][left + len - 1].find(tmp.child2) != dp[right][left + len - 1].end()) {
                                if (dp[left][left + len - 1].find(tmp.parent) == dp[left][left + len - 1].end()) {
                                    dp[left][left + len - 1].insert(pair<unsigned, unsigned>(tmp.parent, 
                                        dp[left][right - 1].find(tmp.child1)->second * dp[right][left + len - 1].find(tmp.child2)->second));
                                } else {
                                    dp[left][left + len - 1].find(tmp.parent)->second += 
                                        dp[left][right - 1].find(tmp.child1)->second * dp[right][left + len - 1].find(tmp.child2)->second;
                                }
                        }
                        
                    }
                }
            }
        }
    }
    if (dp[0][slen - 1].find(0)->second != 0) {
        printf("%u\n", dp[0][slen - 1].find(0)->second);
    } else {
        printf("%u\n", 0);
    }
    end = clock();
    printf("%lfms\n", (double)(end - start) / CLOCKS_PER_SEC);
    //printf("%d - %d\n", count, tcount);
    //printf("%lf%%\n", (double) count / tcount * 100);
}