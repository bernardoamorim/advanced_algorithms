#include <bits/stdc++.h>

using namespace std;

int op(int l, int r) {
    return l + r;
}

template <typename T>
class DisjointSparseTable {
private:
    vector<vector<T>> st;
    inline int log_ceil(int x) { return __builtin_clz(1) - __builtin_clz(x); }
public:
    DisjointSparseTable(const vector<T>& v) {
        int n = v.size(), lg = 0;
        while (1 << lg < n) lg++;
        n = 1 << lg;
        st = vector<vector<T>>(lg, vector<T>(n));
        for (int i = 0; i < v.size(); i++) st[0][i] = v[i];
        
        for (int i = 1; i < lg; i++) {
            for (int j = 1 << i; j < n;) { // Calcula prefixos
                st[i][j] = j&((1 << i) - 1) ? op(st[i][j - 1], st[0][j]) : st[0][j];
                j++;
                if (~(j/(1 << i))&1) j += 1 << i;
            }
            for (int j = n - 1 - (1 << i); j >= 0;) { // Calcula sufixos
                st[i][j] = (j&((1 << i) - 1)) != ((1 << i) - 1) ? op(st[0][j], st[i][j + 1]) : st[0][j];
                j--;
                if ((j/(1 << i))&1) j -= 1 << i;
            }        
        }
    }
    T operator()(int l, int r) {
        if (l == r) return st[0][l];
        return op(st[log_ceil(l ^ r)][l], st[log_ceil(l ^ r)][r]);
    }
};

int main(int argc, char** argv) {

	//srand(atoi(argv[1]));
    
    int n = 10;
    cin >> n;
    vector<int> v(n);
    for (int i = 0; i < n; i++) v[i] = i + 1;
    
    DisjointSparseTable st(v);
    
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            int acc = 0;
            for (int k = i; k <= j; k++) acc = op(acc, v[k]);
            assert(st(i, j) == acc);
        }
    }
    
    return 0;    
}
