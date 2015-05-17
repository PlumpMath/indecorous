#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <cstddef>
#include <random>
#include <type_traits>

class random_t {
public:
    virtual ~random_t();

    virtual void fill(char *buffer, size_t length) = 0;

    template <typename T>
    T generate() {
        static_assert(std::is_integral<T>::value,
                      "Cannot generate a random number for a non-integral type");
        T res;
        fill(reinterpret_cast<char *>(&res), sizeof(res));
        return res;
    }

protected:
    template <typename gen_t>
    void fill_internal(char *buffer, size_t length, gen_t *generator);
};

class true_random_t : public random_t {
public:
    true_random_t();
    void fill(char *buffer, size_t length);
private:
    friend class random_t;
    typedef std::random_device::result_type val_t;
    val_t next();

    std::random_device dev;
};

// TODO: make this a one-per-thread singleton?
class pseudo_random_t : public random_t {
public:
    pseudo_random_t();
    void fill(char *buffer, size_t length);
private:
    friend class random_t;
    typedef std::mt19937::result_type val_t;
    val_t next();

    std::mt19937 dev;
};

class uuid_t {
public:
    static uuid_t generate(random_t *r);
    static uuid_t nil();
    bool operator ==(const uuid_t &other) const;
    bool operator <(const uuid_t &other) const;
private:
    static int cmp(const uuid_t &a, const uuid_t &b);
    uuid_t();
    char buffer[16];
};

#endif