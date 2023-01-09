#pragma once

#ifndef _RSA_
#define _RSA_

#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <bitset>
#include <random>
#include <vector>
#include <execution>
#include <future>

typedef boost::multiprecision::cpp_int number_t;

#ifndef _FASTPOW_
#define _FASTPOW_

template<class T>
constexpr T pow(T base, T exp)
{
    T res = 1;

    while (exp > 0)
    {
        if (exp & 1)
            res = (res * base);
        exp = exp >> 1;
        base = (base * base);
    }
    return res;
}

template<class T>
constexpr T pow(T base, T exp, const T mod)
{
    T res = 1;
    base = base % mod;
    while (exp > 0)
    {
        if (exp & 1)
            res = (res * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return res;
}

#endif // !_FASTPOW_

#ifndef _VECPP_
#define _VECPP_

template<class T>
constexpr std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
    for (size_t i = 0; i < vec.size() - 1; ++i)
        out << vec[i] << ", ";
    return out << vec[vec.size() - 1];
}

#endif // !_VECPP_

namespace RSA
{
    class RSA
    {
    private:
        uint32_t block = DEFAULT_BLOCK;
        uint32_t bits = DEFAULT_BITS;
        bool bGood = false;

        constexpr static uint32_t DEFAULT_BITS = 4096;
        constexpr static uint32_t DEFAULT_BLOCK = 256;
        constexpr static uint32_t DEFAULT_STRENGHT = 50;

    public:
        number_t p, q, n, phi, d, e;
        std::array<number_t, 2> public_key;
        std::array<number_t, 2> private_key;

    public:
        constexpr RSA() = default;

        constexpr RSA(const uint32_t key_bits, const uint32_t block_size = DEFAULT_BLOCK)
            :block(block_size), bits(key_bits)
        {
            if (!block_size || !key_bits || key_bits < block_size * 8 || key_bits % 8 != 0) {
                throw std::runtime_error("");
            }
        }

        constexpr const bool& good() const
        {
            return bGood;
        }

        constexpr operator bool() const
        {
            return bGood;
        }

        constexpr const uint32_t& blocksize() const
        {
            return block;
        }

        constexpr const uint32_t& keysize() const
        {
            return bits;
        }

        inline void set_and_init(const uint32_t key_bits = DEFAULT_BITS, const uint32_t block_size = DEFAULT_BLOCK, const uint32_t strenght = DEFAULT_STRENGHT)
        {
            set(key_bits, block_size);
            init(strenght);
            return;
        }

        inline void set(const uint32_t key_bits = DEFAULT_BITS, const uint32_t block_size = DEFAULT_BLOCK)
        {
            if (!block_size || !key_bits || key_bits < block_size * 8 || key_bits % 8 != 0)
            {
                throw std::runtime_error("");
            }
            else
            {
                //bits    = key_bits  ;
                block = block_size;
            }
            return;
        }

        inline void init(const uint32_t strenght = DEFAULT_STRENGHT)
        {
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_int_distribution<> dist(0, bits >> 3);

            do
            {
                uint32_t rand1 = dist(gen), rand2 = dist(gen);

                while (rand1 % 8 != 0 || rand2 % 8 != 0)
                {
                    if (rand1 % 8 != 0)
                        ++rand1;
                    if (rand2 % 8 != 0)
                        ++rand2;
                }

                // generate p and q
                std::future<number_t> pf(std::async(random_prime, bits + rand1, strenght));
                std::future<number_t> qf(std::async(random_prime, bits + rand2, strenght));

                p = pf.get();
                q = qf.get();

                // calculate N
                n = p * q;

                // calculate phi
                phi = (p - 1) * (q - 1);

                // calculate e
                if (gcd(65537, phi) != 1)
                {
                    size_t i = 2;
                    while (gcd(i, phi) != 1)
                    {
                        ++i;
                    }
                    e = i;
                }
                else
                {
                    e = 65537;
                }

                //calculate d
                d = inverse_mod(e, phi);

            } while ((e * d) % phi != 1);

            public_key =  { e, n };
            private_key = { d, n };
            bGood = true;
        }

        inline std::vector<number_t> encrypt(const std::string str, const std::array<number_t, 2>& public_key)
        {
            const auto power = [](const number_t& num, const number_t& e, const number_t& n)
            {
                return pow(num, e, n);
            };

            if (str.empty()) {
                return std::vector<number_t>();
            }

            std::vector<number_t> result = create_block_vector(str, block);
            std::vector< std::future<number_t> > threads;
            threads.reserve(result.size());

            for (const auto& num : result)
            {
                threads.emplace_back(std::async(std::launch::async, power, num, public_key[0], public_key[1]));
            }

            result.clear();

            for (std::future<number_t>& thread : threads)
            {
                result.emplace_back(thread.get());
            }

            return result;
        }

        inline std::string decrypt(const std::vector<number_t>& vec)
        {
            const auto dec = [](const number_t& num, const number_t& d, const number_t& n, const uint32_t& blocksize)
            {
                const auto to_char = [](long long bits)
                {
                    std::string bit = std::to_string(bits);

                    for (auto& i : bit)
                    {
                        if (i == '1')
                            i = '0';
                        else
                            i = '1';
                    }

                    bits = atoll(bit.c_str());

                    char dec = 0;
                    int i = 0;
                    int rem = 0;

                    while (bits != 0)
                    {
                        rem = bits % 10;
                        bits /= 10;
                        dec += rem * pow(2, i);
                        ++i;
                    }

                    return dec;
                };

                std::string block(pow(num, d, n).str()), result;

                std::string_view view = block;

                for (uint32_t i = 0; i < blocksize; ++i)
                {
                    const long long num = std::atoll(block.substr(0, 8).data());
                    block = view.substr(8);
                    view = block;

                    if (num == 11111111)
                        continue;

                    result.push_back(to_char(num));
                }

                return result;
            };

            const size_t    blocks = vec.size();
            const uint32_t  blocksize = block / 8;

            std::vector<std::future<std::string>> threads; threads.reserve(blocks);

            for (const number_t& i : vec)
            {
                threads.emplace_back(std::async(dec, i, d, n, blocksize));
            }

            std::string decrypted;

            for (std::future<std::string>& thread : threads)
            {
                decrypted.append(thread.get());
            }

            return decrypted;
        }

        inline static std::vector<number_t> create_block_vector(const std::string& str, const uint32_t block)
        {
            const size_t blocksize = block / 8;

            std::string msg;

            for (const char& i : str)
            {
                const std::bitset<8> bits(i);
                msg += bits.to_string();
            }
            for (auto& i : msg)
            {
                if (i == '0')
                {
                    i = '1';
                }
                else
                {
                    i = '2';
                }
            }

            size_t msg_size = msg.size() / 8;

            while (msg_size % blocksize != 0)
            {
                ++msg_size;
                msg += "11111111";
            }

            const size_t blocks = msg_size / blocksize;
            const size_t get_size = msg.size() / blocks;

            std::vector<number_t> results;
            results.reserve(blocks);

            std::string_view view(msg);

            for (size_t i = 0; i < blocks; ++i)
            {
                results.emplace_back(number_t(view.substr(0, get_size)));
                msg = view.substr(get_size);
            }

            return results;
        }

        inline static number_t egcd(const number_t& a, const number_t& b, number_t* x, number_t* y)
        {
            if (a == 0) {
                *x = 0, * y = 1;
                return b;
            }

            number_t x1, y1;
            const number_t gcd = egcd(b % a, a, &x1, &y1);

            *x = y1 - (b / a) * x1;
            *y = x1;

            return gcd;
        };

        inline static number_t inverse_mod(const number_t& e, const number_t& phi)
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, &x, &y) == 1)
            {
                return (x % phi + phi) % phi;
            }
            else
            {
                return 0;
            }
        }

        inline static number_t random_number(uint32_t bits)
        {
            constexpr static const char* nums = "0123456789";

            bits >>= 3;

            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<size_t> dist(0, 9);

            std::string str;
            str.reserve(bits);

            do
            {
                str = nums[dist(gen)];
            } while (str[0] == '0');

            while (--bits)
            {
                str.push_back(nums[dist(gen)]);
            }

            return number_t(str);
        }

        inline static number_t random_possible_prime(const uint32_t bits) noexcept
        {
            while (true)
            {
                const number_t num(random_number(bits));

                if (num <= 1 || num == 2 || num == 3)
                    return num;
                if (num % 2 == 0 || num % 3 == 0)
                    continue;

                bool passed = true;

                for (int i = 4; i < 1000; ++i)
                {
                    if (num % i == 0 && num != i)
                    {
                        passed = false;
                        break;
                    }
                }

                if (!passed)
                    continue;
                else
                    return num;
            }
        }

        inline static number_t random_prime(const uint32_t bits, const uint32_t strenght)
        {
            while (true)
            {
                const number_t num(random_possible_prime(bits));
                if (is_prime(num, strenght))
                {
                    return num;
                }
            }
        }

        inline static bool Miller_Rabin(number_t d, const number_t& n)
        {
            static std::random_device rd;
            static std::uniform_int_distribution<size_t> dist(2, static_cast<size_t>(-1));

            const number_t a = dist(rd) % (n - 4);
            number_t x = pow(a, d, n);

            if (x == 1 || x == n - 1)
            {
                return true;
            }

            while (d != n - 1)
            {
                x = (x * x) % n;
                d *= 2;

                if (x == 1)
                {
                    return false;
                }
                else if (x == n - 1)
                {
                    return true;
                }
            }

            return false;
        }

        inline static bool is_prime(const number_t& n, const uint32_t& strenght) noexcept
        {
            if (n == 2 || n == 3)
                return true;
            if (n <= 1 || n % 2 == 0 || n % 3 == 0)
                return false;

            number_t d = n - 1;

            while (d % 2 == 0)
            {
                d /= 2;
            }

            for (uint32_t i = 0; i < strenght; ++i)
            {
                if (Miller_Rabin(d, n) == false)
                {
                    return false;
                }
            }

            return true;
        }
    };
}

#endif
