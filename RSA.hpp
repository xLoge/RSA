#pragma once

#ifndef _RSA_
#define _RSA_

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random.hpp>
#include <string>
#include <bitset>
#include <vector>
#include <future>

typedef boost::multiprecision::cpp_int number_t;

namespace RSA
{
    template<class Ty>
    constexpr Ty pow(Ty base, Ty exp)
    {
        Ty res = 1;
        while (exp > 0) {
            if (exp & 1) {
                res = (res * base);
            }
            exp >>= 1;
            base *= base;
        }
        return res;
    }

    template<class Ty>
    constexpr Ty pow(Ty base, Ty exp, const Ty& mod)
    {
        Ty res = 1;
        base %= mod;
        while (exp > 0) {
            if (exp & 1) {
                res = (res * base) % mod;
            }
            exp >>= 1;
            base = (base * base) % mod;
        }
        return res;
    }

    template <bool ERROR = false> 
    class RSA
    {
    protected:
        constexpr static inline uint32_t DEFAULT_BITS = 4096;
        constexpr static inline uint32_t DEFAULT_BLOCK = -1;
        constexpr static inline uint32_t DEFAULT_TEST = 4;
        const static inline uint32_t thread_count = std::thread::hardware_concurrency();

    private:
        bool bGood = false;
        uint32_t block = DEFAULT_BLOCK;
        uint32_t bits = DEFAULT_BITS;

    public:
        uint32_t e = 65537;
        number_t p, q, n, d;

        std::array<number_t, 2> public_key; //{ e, n };
        std::array<number_t, 2> private_key; // { d, n };

    public:
        RSA() = default;

        RSA(const uint32_t key_bits, const uint32_t block_size = DEFAULT_BLOCK)
        {
            set(key_bits, block_size);
        }

        constexpr operator bool() const noexcept
        {
            return bGood;
        }

        constexpr uint32_t blocksize() const noexcept
        {
            return block;
        }

        constexpr uint32_t keysize() const noexcept
        {
            return bits;
        }

        void set_and_gen(const uint32_t key_bits = DEFAULT_BITS, const uint32_t block_size = DEFAULT_BLOCK, const uint32_t trys = DEFAULT_TEST)
        {
            set(key_bits, block_size);
            gen(trys);
        }

        void set(uint32_t key_bits = DEFAULT_BITS, uint32_t block_size = DEFAULT_BLOCK)
        {
            if (block_size == 0) {
                throw std::invalid_argument("block size can`t be 0");
            }
            if (key_bits == 0) {
                throw std::invalid_argument("key size can`t be 0");
            }

            if (block_size < 16) {
                if constexpr (ERROR) {
                    throw std::invalid_argument("block size should`t be 8 because each character will represent one number and thats equal to plain text (NOT SECURE!)");
                }

                block_size = 16;
            }
            if (key_bits % 8 != 0 || block_size % 8 != 0) {
                if constexpr (ERROR) {
                    throw std::invalid_argument("both key and block size have to be dividable by 8");
                }

                while (key_bits % 8 != 0 || block_size % 8 != 0) {
                    if (key_bits % 8 != 0) {
                        ++key_bits;
                    }
                    if (block_size % 8 != 0) {
                        ++block_size;
                    }
                }
            }
            if (key_bits < block_size * 8) {
                if constexpr (ERROR) {
                    throw std::runtime_error("key size can`t be smaller than the block size * 8 (It would cause undefined behaviour)");
                }

                while (key_bits < block_size * 8) {
                    if (block_size > 16) {
                        block_size -= 8;
                    }
                    else {
                        key_bits += 8;
                    }
                }
            }

            block = block_size;
            bits = key_bits;
        }

        void gen(const uint32_t trys = DEFAULT_TEST) noexcept
        {
            thread_local boost::random::random_device rd;
            thread_local boost::random::mt19937_64 gen(rd());
            boost::random::uniform_int_distribution<uint32_t> dist(0, bits / 8);

            bGood = false;
            e = 65537;
            p = 0;
            q = 0;
            n = 0;
            d = 0;

            number_t phi;

            do
            {
                uint32_t rand1 = dist(gen), rand2 = dist(gen);

                while (rand1 % 8 != 0 || rand2 % 8 != 0) {
                    if (rand1 % 8 != 0) {
                        ++rand1;
                    }
                    if (rand2 % 8 != 0) {
                        ++rand2;
                    }
                }

                // generate p and q
                if (thread_count > 1) {
                    std::future<number_t> pf = std::async(&RSA::random_prime, this, bits + rand1, trys);
                    std::future<number_t> qf = std::async(&RSA::random_prime, this, bits + rand2, trys);
                    p = pf.get();
                    q = qf.get();
                }
                else {
                    p = random_prime(bits, trys);
                    q = random_prime(bits, trys);
                }

                // calculate N
                n = p * q;

                // calculate phi
                phi = (p - 1) * (q - 1);

                // calculate e
                if (gcd(e, phi) != 1) {
                    uint32_t i = 2;
                    while (gcd(i, phi) != 1) {
                        ++i;
                    }
                    e = i;
                }

                //calculate d
                d = inverse_mod(e, phi);

            } while ((e * d) % phi != 1);

            public_key = { e, n };
            private_key = { d, n };
            bGood = true;
        }

        std::vector<number_t> encrypt(const std::string_view& str, const std::array<number_t, 2>& public_key)
        {
            const auto _pow = [public_key](const number_t& num) {
                return pow(num, public_key[0], public_key[1]);
            };

            if (str.empty()) {
                return std::vector<number_t>{ };
            }

            std::vector<number_t> result = create_block_vector(str, block);
            std::vector<std::future<number_t>> threads;
            threads.reserve(result.size());

            for (const number_t& num : result) {
                threads.push_back(std::async(_pow, num));
            }

            result.clear();

            for (std::future<number_t>& thread : threads) {
                result.push_back(thread.get());
            }

            return result;
        }

        std::string decrypt(const std::vector<number_t>& vec)
        {
            auto _decrypt = [&](const number_t& num, const uint32_t blocksize) -> std::string
            {
                auto bits_to_char = [](long long _bits) -> char
                {
                    std::string bits = std::to_string(_bits);

                    for (auto& bit : bits) {
                        if (bit == '1') {
                            bit = '0';
                        }
                        else {
                            bit = '1';
                        }
                    }

                    _bits = std::atoll(bits.c_str());

                    char result = 0;
                    uint32_t i = 0, rem = 0;

                    while (_bits != 0) {
                        rem = _bits % 10;
                        _bits /= 10;
                        result += static_cast<char>(rem * pow(static_cast<uint32_t>(2), i));
                        ++i;
                    }

                    return result;
                };

                std::string block = pow(num, d, n).str();
                std::string result;
                result.reserve(blocksize);

                std::string_view view = block;

                for (uint32_t i = 0; i != blocksize; ++i) {
                    const long long num = std::atoll(block.substr(0, 8).c_str());
                    block = view.substr(8);
                    view = block;

                    if (num == 11111111) {
                        continue;
                    }

                    result.push_back(bits_to_char(num));
                }

                return result;
            };

            const size_t blocks = vec.size();
            const uint32_t blocksize = block / 8;

            std::vector<std::future<std::string>> threads;
            threads.reserve(blocks);

            for (const number_t& i : vec) {
                threads.push_back(std::async(_decrypt, i, blocksize));
            }

            std::string decrypted;

            for (std::future<std::string>& thread : threads) {
                decrypted.append(thread.get());
            }

            return decrypted;
        }

    private:
        std::vector<number_t> create_block_vector(const std::string_view& str, const uint32_t block) const noexcept
        {
            const size_t blocksize = block / 8;

            std::string msg;

            for (const char& ch : str) {
                const std::bitset<8> bits(ch);
                msg.append(bits.to_string());
            }

            for (auto& i : msg) {
                if (i == '0') {
                    i = '1';
                }
                else {
                    i = '2';
                }
            }

            size_t msg_size = msg.size() / 8;

            while (msg_size % blocksize != 0) {
                ++msg_size;
                msg.append("11111111", 8);
            }

            const size_t blocks = msg_size / blocksize;
            const size_t get_size = msg.size() / blocks;

            std::vector<number_t> results;
            results.reserve(blocks);

            std::string_view view = msg;

            for (size_t i = 0; i != blocks; ++i) {
                results.emplace_back(view.substr(0, get_size));
                msg = view.substr(get_size);
            }

            return results;
        }

        number_t egcd(const number_t& a, const number_t& b, number_t* x, number_t* y) const noexcept
        {
            if (a == 0) {
                *x = 0;
                *y = 1;
                return b;
            }

            number_t x1, y1;
            const number_t gcd = egcd(b % a, a, &x1, &y1);

            *x = y1 - (b / a) * x1;
            *y = x1;

            return gcd;
        };

        number_t inverse_mod(const number_t& e, const number_t& phi) const noexcept
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, &x, &y) == 1) {
                return (x % phi + phi) % phi;
            }
            return 0;
        }

        number_t random_number(const uint32_t bits) const noexcept
        {
            thread_local boost::random::random_device rd;
            thread_local boost::random::mt19937_64 gen(rd());
            boost::random::uniform_int_distribution<int> dist('0', '9');

            const uint32_t bytes = bits / 8;

            std::string str(bytes, 0);

            do {
                str[0] = static_cast<char>(dist(gen));
            } while (str[0] == '0');

            for (uint32_t i = 1; i != bytes; ++i) {
                str[i] = static_cast<char>(dist(gen));
            }

            return number_t(str);
        }

        number_t random_possible_prime(const uint32_t bits) const noexcept
        {
            auto small_test = [bits](const number_t& num) noexcept -> bool
            {
                if (bits == 8) {
                    if (num <= 1 || num == 2 || num == 3) {
                        return true;
                    }
                }

                for (uint32_t i = 2; i != 10000; ++i) {
                    if (num % i == 0) {
                        if (num != i) {
                            return false;
                        }
                    }
                }

                return true;
            };

            number_t num = random_number(bits);
            
            while (!small_test(num)) {
                num = random_number(bits);
            }

            return num;
        }

        number_t random_prime(const uint32_t bits, const uint32_t trys) const noexcept
        {
            if (thread_count > 4) {
                static const uint32_t _thread_count = (thread_count - 2) / 2;

                number_t result;

                auto search_thread = [this, bits, trys, &result]() noexcept -> void
                {
                    number_t num(random_possible_prime(bits));

                    do 
                    {
                        if (is_prime(num, trys) && result.is_zero()) {
                            result = num;
                            return;
                        }
                        else {
                            num = random_possible_prime(bits);
                        }
                    } while (result.is_zero());
                };

                std::vector<std::future<void>> threads(_thread_count);
                for (auto& thrd : threads) {
                    thrd = std::async(std::launch::async, search_thread);
                }
                return result;
            }
            else {
                number_t num(random_possible_prime(bits));
                while (!is_prime(num, trys)) {
                    num = random_possible_prime(bits);
                }
                return num;
            }
        }

        bool miller_rabin(number_t d, const number_t& n) const noexcept
        {
            thread_local boost::random::random_device rd;
            thread_local boost::random::mt19937_64 mt(rd());
            boost::random::uniform_int_distribution<number_t> dist(2, n - 4);

            const number_t good = n - 1;
            const number_t a = dist(mt);
            number_t x = pow(a, d, n);

            if (x == 1 || x == n - 1) {
                return true;
            }

            while (d != good) {
                x = (x * x) % n;

                if (x == 1) {
                    return false;
                }
                if (x == good) {
                    return true;
                }
                if (d < good) {
                    d = d * 2;
                }
            }

            return false;
        }

        bool is_prime(const number_t& n, const uint32_t trys) const noexcept
        {
            if (n == 2 || n == 3) {
                return true;
            }
            if (n <= 1 || n % 2 == 0 || n % 3 == 0) {
                return false;
            }

            number_t d = n - 1;

            while (d % 2 == 0) {
                d /= 2;
            }

            for (uint32_t i = 0; i != trys; ++i) {
                if (miller_rabin(d, n) == false) {
                    return false;
                }
            }

            return true;
        }
    };
}

template<class Ty>
constexpr std::ostream& operator<<(std::ostream& out, const std::vector<Ty>& vec)
{
    for (size_t i = 0, end = vec.size() - 1; i < end; ++i) {
        out << vec.at(i) << ", ";
    }
    return out << vec.back();
}

#endif
