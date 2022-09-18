#include <bits/stdc++.h>

// Order Statistics Skip List
//
// All operations cost expected O(log(n)).
// The size of the strucure is expected O(n).
//
// Based on the paper Skip Lists: A Probabilistic Alternative to Balanced Trees
// with the addition of order statistics.
// 
// The following code was entirelly written by Bernardo Amorim and used no
// code/pseudo-code as inspiration.

using namespace std;

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;
template <class T>
	using ord_set = tree<T, null_type, less<T>, rb_tree_tag,
	tree_order_statistics_node_update>;

template <typename T>
class skip_list {
private:

    struct node {
    	struct link {
			shared_ptr<node> ptr;
			unsigned int jump;

			link(shared_ptr<node> ptr_, unsigned int jump_) : ptr(ptr_), jump(jump_) {}

			shared_ptr<node>& operator->() {
				return ptr;
			}

			bool operator==(const shared_ptr<node>& r) {
				return ptr == r;
			}

			bool operator!=(const shared_ptr<node>& r) {
				return ptr != r;
			}
		};

		T key;
        vector<link> links;
        bool is_start;
        
        node() : is_start(true) {}

        node(const T& key_) : key(key_), links(1, {nullptr, 0}), is_start(false) {
            while (rand()%2) {
                links.emplace_back(nullptr, 0);
            }
        }

        unsigned int height() {
            return links.size();
        }
    };

    shared_ptr<node> start;
    unsigned int sz;

public:
    skip_list() : start(new node()), sz(0) {}
    
    shared_ptr<node> previous(const T& key) {
        shared_ptr<node> cur = start;
        int h = int(cur->height()) - 1;
        while (h >= 0 && (cur->is_start || cur->key != key)) {
            if (h < cur->height() && cur->links[h] != nullptr && cur->links[h]->key <= key) {
                cur = cur->links[h].ptr;
            } else {
                h--;
            }
        }
        return cur->is_start ? nullptr : cur;
    }

    unsigned int count(const T& key) {
        shared_ptr<node> prev = previous(key);
        return prev != nullptr and prev->key == key;
    }

	unsigned int order_of_key(const T& key) {
		shared_ptr<node> cur = start;
		int h = int(cur->height()) - 1;
		unsigned int ord = 0;

		while (h >= 0) {
			if (cur->links[h] != nullptr && cur->links[h]->key < key) {
				ord += cur->links[h].jump;
				cur = cur->links[h].ptr;
			} else {
				h--;
			}
		}

		return ord;
	} 

    void insert(const T& key) {
        if (count(key)) {
            return;
        }

        shared_ptr<node> new_node(new node(key)), cur = start;
        
		int h = int(cur->height()) - 1;
		unsigned int ord = order_of_key(key), cur_ord = 0;

        while (h >= 0) {
            if (cur->links[h] != nullptr && cur->links[h]->key < key) {
                cur_ord += cur->links[h].jump;
				cur = cur->links[h].ptr;
            } else {
                if (h < new_node->height() && 
					(cur->links[h] == nullptr || key < cur->links[h]->key)) {
			
					unsigned int l_jump = ord - cur_ord + 1;
					unsigned int r_jump = cur->links[h] == nullptr ? 0 : cur->links[h].jump - (ord - cur_ord);
					
					new_node->links[h] = {cur->links[h].ptr, r_jump};
                    cur->links[h] = {new_node, l_jump};
                } else if (cur->links[h] != nullptr && key < cur->links[h]->key) {
					cur->links[h].jump++;
				}
                h--;
            }
        }

		while (start->height() < new_node->height()) {
            start->links.emplace_back(new_node, ord + 1);
        }

        sz++;
    }

	void erase(const T& key) {
		if (count(key) == 0) {
			return;
		}

		shared_ptr<node> cur = start;
		int h = int(cur->height()) - 1;

		while (h >= 0) {
			if (cur->links[h] != nullptr && cur->links[h]->key < key) {
				cur = cur->links[h].ptr;
			} else {
				if (cur->links[h] != nullptr) {
					if (cur->links[h]->key == key) {
						unsigned int full_jump = cur->links[h].jump + cur->links[h]->links[h].jump;
						cur->links[h] = {cur->links[h]->links[h].ptr, full_jump};
					}
					if (cur->links[h] != nullptr) {
						cur->links[h].jump--;
					}
				} 
				h--;
			}
		}

		sz--;

		while (start->height() > 0 and start->links.back() == nullptr) {
			start->links.pop_back();
		}
	}

	shared_ptr<node> find_by_order(unsigned int ord) {
		if (ord >= sz) {
			return nullptr;
		}
		
		ord++;

		shared_ptr<node> cur = start;
		int h = int(cur->height()) - 1;

		while (h >= 0) {
			if (cur->links[h] != nullptr and ord >= cur->links[h].jump) {
				ord -= cur->links[h].jump;
				cur = cur->links[h].ptr;
			} else {
				h--;
			}
		}

		return cur;
	}

    unsigned int size() {
        return sz;
    }
};

int main(int argc, char** argv) {

	srand(atoi(argv[1]));

    ord_set<int> st;
    skip_list<int> sl;

	// Tests comparing the results to the results from an 
	// Order Statistics Tree from GNU Compiler Library

    for (int i = 0; i < 1000000; i++) {
        int op = rand()%7, key = rand()%10000 + 1;

        if (op == 0) {
            cout << "INSERT " << key << endl;
            st.insert(key), sl.insert(key);
        } else if (op == 1) {
            cout << "SIZE set = " << st.size() << " ";
            cout << "skip_list = " << sl.size() << endl;
            assert(st.size() == sl.size());
        } else if (op == 2) {
            cout << "COUNT " << key << " ";
			cout << "set " << (st.find(key) != st.end()) << " ";
            cout << "skip_list " << sl.count(key) << endl;
            assert((st.find(key) != st.end()) == sl.count(key));
        } else if (op == 3) {
            auto st_prev = st.upper_bound(key);
            if (st_prev != st.begin()) {
				st_prev = prev(st_prev);
			} else {
				st_prev = st.end();
			}
			
			cout << "PREVIOUS " << key << " ";
            cout << "set ";
            if (st_prev != st.end()) {
                cout << *st_prev << " ";
            } else {
                cout << "*" << " ";
            }

            auto sl_prev = sl.previous(key);
            cout << "skip_list ";
            if (sl_prev != nullptr) {
               cout << sl_prev->key << endl;
            } else {
                cout << "*" << endl;
            }
            
            assert((st_prev == st.end() && sl_prev == nullptr) || (st_prev != st.end() && sl_prev != nullptr && *st_prev == sl_prev->key));
        } else if (op == 4) {
			cout << "DELETE " << key << endl;
			st.erase(key), sl.erase(key);
		} else if (op == 5) {
			cout << "ORDER OF KEY " << key << " ";
			cout << "set " << st.order_of_key(key) << " ";
			cout << "skip_list " << sl.order_of_key(key) << endl;

			assert(st.order_of_key(key) == sl.order_of_key(key));
		} else if (op == 6) {
			cout << "FIND BY ORDER " << key << " ";
			cout << "set ";
			auto st_fbo = st.find_by_order(key);
			if (st_fbo == st.end()) {
				cout << "* ";
			} else {
				cout << *st_fbo << " ";
			}

			cout << "skip_list ";
			auto sl_fbo = sl.find_by_order(key);
			cout.flush();
			if (sl_fbo == nullptr) {
				cout << "*" << endl;
			} else {
				cout << sl_fbo->key << endl;
			}
	
			assert((st_fbo == st.end() && sl_fbo == nullptr) || (st_fbo != st.end() && sl_fbo != nullptr && *st_fbo == sl_fbo->key));
		}
    }
    
	return 0;    
}
