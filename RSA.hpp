#pragma once

#ifndef _RSA_
#define _RSA_

#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <bitset>
#include <random>
#include <vector>
#include <future>

#define DEFAULT_BITS 4096
#define DEFAULT_BLOCK 256
#define DEFAULT_STRENGHT 50

using namespace boost::multiprecision;

typedef cpp_int number_t;

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
    out << vec[vec.size() - 1];
    return out;
}

#endif // !_VECPP_

namespace RSA
{
    inline bool Miller_Rabin(number_t d, const number_t& n)
    {
        std::random_device rd;
        std::uniform_int_distribution<size_t> dist(2, size_t(-1));
        
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

    inline bool is_prime(const number_t& n, const uint32_t& strenght) noexcept
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

    inline number_t __fastcall random_number(uint32_t bits)
    {
        constexpr std::string_view nums = "0123456789";

        bits = bits >> 3; // divided by 8

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, nums.size() - 1);

        std::string str; str.reserve(bits);

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

    inline number_t random_possible_prime(const uint32_t bits) noexcept
    {
        while (true)
        {
            const number_t num(random_number(bits));

            if (num <= 1 || num == 2 || num == 3)
                return num;
            if (num % 2 == 0 || num % 3 == 0)
                continue;
            
            bool passed = true;
            for (short i = 4; i < 89; ++i)
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

    inline number_t random_prime(const uint32_t bits = DEFAULT_BITS, const uint32_t strenght = DEFAULT_STRENGHT)
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

    class RSA
    {
    private:
        uint32_t block  = DEFAULT_BLOCK;
        uint32_t bits   = DEFAULT_BITS;
        bool bGood = false;
    public:
        number_t p, q, n, phi, d;
        size_t e;

        std::array<number_t, 2> public_key;
        std::array<number_t, 2> private_key;

        RSA()
        {

        }

        RSA(const uint32_t key_bits, const uint32_t block_size = DEFAULT_BLOCK)
            :block(block_size)
        {
            if (!block_size || !key_bits || key_bits < block_size * 8 || key_bits % 8 != 0)
            {
                throw std::runtime_error("");
            }
            else
            {
                bits = key_bits;
            }
        }

        ~RSA()
        {
            bGood       = false;
            bits        = DEFAULT_BITS;
            block       = DEFAULT_BLOCK;
            p           = 0;
            q           = 0;
            n           = 0;
            phi         = 0;
            e           = 0;
            d           = 0;
            public_key  = { 0, 0 };
            private_key = { 0, 0 };
        }

        constexpr bool good()
        {
            return bGood;
        }

        constexpr uint32_t blocksize()
        {
            return block;
        }

        constexpr uint32_t keysize()
        {
            return bits;
        }
        
        void set_and_init(const uint32_t key_bits = DEFAULT_BITS, const uint32_t block_size = DEFAULT_BLOCK, const uint32_t strenght = DEFAULT_STRENGHT)
        {
            set(key_bits, block_size);
            init(strenght);
            return;
        }

        void set(const uint32_t key_bits = DEFAULT_BITS, const uint32_t block_size = DEFAULT_BLOCK)
        {
            if (!block_size || !key_bits || key_bits < block_size * 8 || key_bits % 8 != 0)
            {
                throw std::runtime_error("");
            }
            else
            {
                //bits    = key_bits  ;
                block   = block_size;
            }
            return;
        }

        void init(const uint32_t strenght = DEFAULT_STRENGHT)
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
                const std::future<number_t> pf(std::async(random_prime, bits + rand1, strenght));
                const std::future<number_t> qf(std::async(random_prime, bits + rand2, strenght));

                p = pf._Get_value();
                q = qf._Get_value();

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
                d = inverse_mod();

            } while ((e * d) % phi != 1);

            public_key  = { e, n };
            private_key = { d, n };
            bGood = true;
        }

        std::vector<number_t> encrypt(const std::string str, const std::array<number_t, 2>& public_key)
        {
            const auto power = [](const number_t& num, const number_t& e, const number_t& n)
            {
                return pow(num, e, n);
            };

            if (str.empty())
                return { 0 };

            std::vector<number_t> result(create_block_vector(str));
            std::vector<std::future<number_t>> threads; threads.reserve(result.size());

            for (const auto& i : result)
            {
                threads.emplace_back(std::async(power, i, public_key[0], public_key[1]));
            }

            result.clear();
            result.shrink_to_fit();

            for (const auto& thread : threads)
            {
                result.emplace_back(thread._Get_value());
            }

            return result;
        }

        std::string decrypt(const std::vector<number_t>& vec)
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

                for (size_t i = 0; i < blocksize; ++i)
                {
                    const long long num(std::atoll(block.substr(0, 8).c_str()));
                    block = block.substr(8);

                    if (num == 11111111)
                        continue;

                    result.push_back(to_char(num));
                }
                
                return result;
            };

            const size_t    blocks      = vec.size();
            const uint32_t  blocksize   = block / 8;

            std::vector<std::future<std::string>> threads; threads.reserve(blocks);

            for (const auto& i : vec)
            {
                threads.emplace_back(std::async(dec, i, d, n, blocksize));
            }

            std::string decrypted;

            for (const auto& thread : threads)
            {
                decrypted += thread._Get_value();
            }

            return decrypted;
        }

        std::vector<number_t> create_block_vector(const std::string str)
        {
            const size_t blocksize = block / 8;

            std::string msg;

            for (const auto& i : str)
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

            std::vector<number_t> results; results.reserve(blocks);

            for (size_t i = 0; i < blocks; ++i)
            {
                results.emplace_back(number_t(msg.substr(0, get_size)));
                msg = msg.substr(get_size);
            }

            return results;
        }

    private:

        number_t egcd(const number_t& a, const number_t& b, number_t* x, number_t* y)
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

        number_t inverse_mod()
        {
            number_t x, y;
            if (egcd(e, phi, &x, &y) == 1)
            {
                return (x % phi + phi) % phi;
            }
            else
            {
                return 0;
            }
        }
    };
}

#undef DEFAULT_BITS
#undef DEFAULT_BLOCK
#undef DEFAULT_STRENGHT

#endif
