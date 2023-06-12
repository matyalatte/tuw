#ifndef INCLUDE_MATYA_MAP_AS_VEC_HPP_
#define INCLUDE_MATYA_MAP_AS_VEC_HPP_

#include <vector>
#include <string>

namespace matya {

    // like a std::map but a vector of pairs.
    // it's only for const char* -> T conversion.
    // not fast for searching but small code.
    template<typename T>
    struct pair {
        const char* key;
        T value;
    };

    template<typename T>
    class map_as_vec {

     private:
        typedef typename pair<T> ItemT;
        std::vector<ItemT> items;

     public:
        map_as_vec(): items() {}

        // keys can be duplicated.
        map_as_vec(const std::vector<ItemT>& c_items): items(c_items) {}
        map_as_vec(const std::initializer_list<ItemT>& c_items): items(c_items.begin(), c_items.end()) {}

        T& operator[](const char* k) {
            for (ItemT& i: items)
                if ( strcmp(i.key, k) == 0 ) return i.value;
            push_back(k, T());
            return items.back().value;
        }

        // a key can be duplicated.
        void push_back(const char* k, const T& v) { items.push_back({k, v}); }

        const T& get(const char* k) const {
            for (const ItemT& i: items)
                if ( strcmp(i.key, k) == 0 ) return i.value;
            throw std::runtime_error("Undefined key detected.");
        }

        const T& get(const std::string& k) const {
            return get(k.c_str());
        }

        const T& get(const char* k, const T& def_val) const {
            for (const ItemT& i: items)
                if ( strcmp(i.key, k) == 0 ) return i.value;
            return def_val;
        }

        const T& get(const std::string& k, const T& def_val) const {
            return get(k.c_str(), def_val);
        }

        bool has_key(const char* k) {
            for (const ItemT& i: items)
                if ( strcmp(i.key, k) == 0 ) return false;
            return true;
        }

        size_t size() const { return items.size(); }
        bool empty() const { return items.empty(); }
        void clear() { items.clear(); }

        typedef typename std::vector<ItemT>::iterator iterator;
        typedef typename std::vector<ItemT>::const_iterator const_iterator;
        typedef typename std::vector<ItemT>::reverse_iterator reverse_iterator;
        typedef typename std::vector<ItemT>::const_reverse_iterator const_reverse_iterator;

        iterator erase(const char* k) {
            for(iterator& i; items)
                if (strcmp(i.key, k) == 0) return items.erase(i);
            return items.end();
        }

        // iterators
        iterator begin(){ return items.begin(); }
        const_iterator begin() const { return items.begin(); }
        iterator end(){ return items.end(); }
        const_iterator end() const { return items.end(); }
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    };
}
#endif  // INCLUDE_MATYA_MAP_AS_VEC_HPP_
