#include <iostream>
#include <cassert>
#include <set>
#include <random>
#include <chrono>
#include <unordered_map>
#include <algorithm>

using namespace std;

typedef long long ll;

ll builtin_clz(ll x) {
	for (int i = 0; i < 64; i++) if (x & (1ll << i)) return i;
	return 64;
}

template<int S_SZ>
class vEBTree {
public:
	vEBTree(int m) : m_(m), M_(1ll << m_), min_(M_), max_(-1), tree_(0) {
		if (m_ > S_SZ) aux_ = new vEBTree<S_SZ>((m_ + 1) / 2);
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
			// TO DO make better
			for (int i = 0; i < M_; i++) if (tree_ & (1ll << i)) {
				if (i < min_) min_ = i;
				if (i > max_) max_ = i;
			}
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
		// TO DO make better
		if (x == M_ - 1) return M_;
		ll aux = ((~0ll) << (x + 1)) & tree_;
		return aux == 0 ? M_ : builtin_clz(aux);
	}
	bool small_empty() {
		return tree_ == 0;
	}

	pair<ll, ll> partition(ll x) {
		ll up = x >> (m_ / 2), lo = x - (up << (m_ / 2));
		return {lo, up};
	}
	vEBTree<S_SZ>* ch(ll x) {
		if (not ch_.count(x)) ch_[x] = new vEBTree<S_SZ>(m_ / 2);
		return ch_[x];
	}
	void fix(ll x) {
		if (ch_[x]->empty()) ch_.erase(x);
	}

	void large_insert(ll x) {
		if (empty()) min_ = max_ = x;
		else {
			if (x < min_) swap(x, min_);
			max_ = max(max_, x);

			ll lo, up;
			tie(lo, up) = partition(x);

			ch(up)->insert(lo);
			if (ch(up)->min_ == ch(up)->max_) aux_->insert(up);
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

			ch(up)->in_erase(lo);
			if (ch(up)->empty()) aux_->in_erase(up);
			fix(up);
			if (x == max_) {
				if (aux_->empty()) max_ = min_;
				else {
					ll max_up = aux_->max_;
					max_ = (max_up << (m_ / 2)) + ch(max_up)->max_;
				}
			}
		}
	}
	ll large_find_next(ll x) {
		if (x < min_) return min_;
		if (x >= max_) return M_;

		ll lo, up;
		tie(lo, up) = partition(x);

		if (lo < ch(up)->max_) {
			ll next = ch(up)->find_next(lo);
			return next == ch(up)->M_ ? M_ : (up << (m_ / 2)) + next;
		}
		else {
			ll next_up = aux_->find_next(up);
			return (next_up << (m_ / 2)) + ch(next_up)->min_;
		}
	}
	bool large_empty() { return min_ > max_; }

	int m_;
	ll M_, min_, max_, tree_;
	vEBTree<S_SZ>* aux_;
	unordered_map<ll, vEBTree<S_SZ>*> ch_;
};

int main() {

	srand(0);

	int t; cin >> t;

	int m = 24;
	vEBTree<6> vEB(m);

	set<int> st;
	for (int i = 0; i < t; i++) {
		ll op = rand() % 4, key = rand() % (1ll << m);

		if (op == 0) {
			cout << "INSERT " << key << endl;
			st.insert(key), vEB.insert(key);
		}
		else if (op == 1) {
			cout << "ERASE " << key << endl;
			st.erase(key), vEB.erase(key);
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

			ll ct_st = st.count(key), ct_vEB = vEB.count(key);
			cout << "set = " << ct_st << endl;
			cout << "vEB = " << ct_vEB << endl;

			assert(ct_st == ct_vEB);
		}
		cout << endl;
		cout.flush();
	}

	return 0;
}
