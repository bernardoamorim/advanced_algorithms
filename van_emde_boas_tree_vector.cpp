#include <bits/stdc++.h>
 
using namespace std;
 
typedef long long ll;
 
template<int S_SZ>
class vEBTree {
public:
	vEBTree(int m) : m_(m), M_(1ll << m_), min_(M_), max_(-1), tree_(0) {
		if (m_ > S_SZ) {
			aux_ = new vEBTree<S_SZ>((m_ + 1) / 2);
			ch_ = vector<vEBTree<S_SZ>*>(1 << ((m_ + 1) / 2));
			for (int i = 0; i < ch_.size(); i++) 
				ch_[i] = new vEBTree(m_ / 2);
		}
	}
	int count(ll x) { 
		return x == 0 ? min_ == 0 : find_next(x - 1) == x; 
	}
	void insert(ll x) {
		if (not count(x)) m_ <= S_SZ ? small_insert(x) : large_insert(x);
	}
	void erase(ll x) {
		if (count(x)) in_erase(x);
	}
	ll find_next(ll x) { 
		return m_ <= S_SZ ? small_find_next(x) : large_find_next(x); 
	}
	bool empty() { 
		return m_ <= S_SZ ? small_empty() : large_empty(); 
	}
 
private:
	void in_erase(ll x) {
		return m_ <= S_SZ ? small_erase(x) : large_erase(x);
	}
	void small_recalc() {
		min_ = M_, max_ = -1;
		if (tree_ != 0) {
			min_ = __builtin_ctzll(tree_);
			max_ = 63 - __builtin_clzll(tree_);
		}
	}
 
	void small_insert(ll x) {
		tree_ |= (1ll << x);
		small_recalc();
	}
	void small_erase(ll x) {
		tree_ &= ~(1ll << x);
		small_recalc();
	}
	ll small_find_next(ll x) {
		if (x == M_ - 1) return M_;
		ll aux = ((~0ll) << (x + 1)) & tree_;
		return aux == 0 ? M_ : __builtin_ctzll(aux);
	}
	bool small_empty() {
		return tree_ == 0;
	}
 
	pair<ll, ll> partition(ll x) {
		ll up = x >> (m_ / 2), lo = x - (up << (m_ / 2));
		return {lo, up};
	}
 
	void large_insert(ll x) {
		if (empty()) min_ = max_ = x;
		else {
			if (x < min_) swap(x, min_);
			max_ = max(max_, x);
 
			ll lo, up;
			tie(lo, up) = partition(x);
 
			ch_[up]->insert(lo);
			if (ch_[up]->min_ == ch_[up]->max_) aux_->insert(up);
		}
	}
	void large_erase(ll x) {
		if (min_ == max_ and max_ == x) min_ = M_, max_ = -1;
		else {
			if (x == min_) {
				ll up = aux_->min_;
				min_ = x = (up << (m_ / 2)) + ch_[up]->min_;
			}
			ll lo, up;
			tie(lo, up) = partition(x);
 
			ch_[up]->in_erase(lo);
			if (ch_[up]->empty()) aux_->in_erase(up);
			if (x == max_) {
				if (aux_->empty()) max_ = min_;
				else {
					ll max_up = aux_->max_;
					max_ = (max_up << (m_ / 2)) + ch_[max_up]->max_;
				}
			}
		}
	}
	ll large_find_next(ll x) {
		if (x < min_) return min_;
		if (x >= max_) return M_;
 
		ll lo, up;
		tie(lo, up) = partition(x);
 
		if (lo < ch_[up]->max_) {
			ll next = ch_[up]->find_next(lo);
			return next == ch_[up]->M_ ? M_ : (up << (m_ / 2)) + next;
		}
		else {
			ll next_up = aux_->find_next(up);
			return (next_up << (m_ / 2)) + ch_[next_up]->min_;
		}
	}
	bool large_empty() { return min_ > max_; }
 
	int m_;
	ll M_, min_, max_, tree_;
	vEBTree<S_SZ>* aux_;
	vector<vEBTree<S_SZ>*> ch_;
};
 
int main() {
 
	srand(0);
 
	int t; cin >> t;
 
	int m = 20;
	vEBTree<2> vEB(m);
 
	set<int> st;
	for (int i = 0; i < t; i++) {
		ll op = rand() % 4, key = rand() % (1ll << m);
 
		if (op == 0) {
			cout << "INSERT " << key << endl;
			st.insert(key);
			vEB.insert(key);
		}
		else if (op == 1) {
			cout << "ERASE " << key << endl;
			st.erase(key); 
			vEB.erase(key);
		}
		else if (op == 2) {
			cout << "FIND NEXT " << key << endl;
 
			auto it = st.upper_bound(key);
			int st_next = (it == st.end() ? (1 << m) : *it);
			int vEB_next = vEB.find_next(key);
 
			cout << "set = " << st_next << endl;
			cout << "vEB = " << vEB_next << endl;
 
			assert(st_next == vEB_next);
		}
		else if (op == 3) {
			cout << "COUNT " << key << endl;
 
			ll ct_st = st.count(key);
			ll ct_vEB = vEB.count(key);
			cout << "set = " << ct_st << endl;
			cout << "vEB = " << ct_vEB << endl;
 
			assert(ct_st == ct_vEB);
		}
	}
 
	return 0;
}
