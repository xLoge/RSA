#pragma once

#ifndef _RSA_
#define _RSA_

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <random>
#include <string>
#include <bitset>
#include <vector>
#include <tuple>
#include <future>

namespace mp = boost::multiprecision;

template<class Ty, class ostream>
constexpr ostream& operator<<(ostream& out, const std::vector<Ty>& vec)
{
    for (size_t i = 0, end = vec.size() - 1; i < end; ++i) {
        out << vec[i] << ',';
    }
    return out << vec.back();
}

namespace RSA
{
    typedef boost::multiprecision::cpp_int number_t;

    // * ascii to unsigned value
    template <class what>
    constexpr what atoux(const char* str)
    {
        static_assert(std::unsigned_integral<what>, "only unsigned types");
        constexpr auto _max = std::numeric_limits<what>::max();

        what val = 0;
        while (*str) {
            what tmp = val * 10 + (*str++ - '0');
            if (tmp < val) {
                return _max;
            }
            val = tmp;
        }
        return val;
    }

    template<class _char>
    constexpr uint32_t blocksize_for(const _char* const str) noexcept {
        auto _val = static_cast<uint32_t>(std::char_traits<_char>::length(str) * 8);
        while (_val % (16 * sizeof(_char))) { _val += 8; };
        return _val;
    }

    constexpr uint32_t blocksize_for(const std::string_view& str) noexcept {
        auto _val = static_cast<uint32_t>(str.size() * 8);
        while (_val % 16) { _val += 8; };
        return _val;
    }

    template <class char_type = char, bool throw_errors = false>
    class RSA
    {
    protected:
        constexpr static inline uint32_t DEFAULT_BITS = 4096;
        constexpr static inline uint32_t DEFAULT_TRYS = 4;
        constexpr static inline uint32_t DEFAULT_BLOCKSIZE = -1;

    private:
        const static inline uint32_t thread_count = std::thread::hardware_concurrency();

        static_assert(sizeof(char_type) <= 2, "max. is char16_t");

        using _char = char_type;
        using string = std::basic_string<_char>;
        using string_view = std::basic_string_view<_char>;

    private:
        bool m_setupdone = false;
        uint32_t m_blocksize = DEFAULT_BLOCKSIZE;
        uint32_t m_bits = DEFAULT_BITS;

    public:
        uint32_t e = 65537;
        number_t p, q, n, d;

        std::tuple<uint32_t&, number_t&> public_key { e, n };
        std::tuple<number_t&, number_t&> private_key{ d, n };

    public:
        constexpr RSA() {
            set();
        }

        constexpr RSA(const uint32_t _bits, const uint32_t _blocksize = DEFAULT_BLOCKSIZE)
        {
            set(_bits, _blocksize);
        }

        constexpr operator bool() const noexcept
        {
            return m_setupdone;
        }

        constexpr bool setupdone() const noexcept
        {
            return m_setupdone;
        }

        constexpr uint32_t blocksize() const noexcept
        {
            return m_blocksize;
        }

        constexpr uint32_t keysize() const noexcept
        {
            return m_bits;
        }

        constexpr void set(uint32_t _bits = DEFAULT_BITS, uint32_t _blocksize = DEFAULT_BLOCKSIZE)
        {
            constexpr uint32_t _size = 8 * sizeof(_char);
            constexpr uint32_t _mod = 8 * sizeof(_char);
            constexpr uint32_t _min_blocksize = 16 * sizeof(_char);

            if (_blocksize == 0) {
                throw std::invalid_argument("block size can`t be 0");
            }

            if (_bits == 0) {
                throw std::invalid_argument("key size can`t be 0");
            }

            if (_blocksize == DEFAULT_BLOCKSIZE) {
                constexpr uint32_t _max_blocksize = _min_blocksize * 3;

                _blocksize = _min_blocksize;

                while (_blocksize * 8 < _bits) {
                    _blocksize += _size;
                    if (_blocksize >= _max_blocksize) {
                        break;
                    }
                }
            }
            else if (_blocksize < _min_blocksize) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("block size should`t be 8 because each character will represent one number and thats equal to plain text (NOT SECURE!)");
                }

                _blocksize = _min_blocksize;
            }

            if (_bits % _mod != 0 || _blocksize % _mod != 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("both key and block size have to be dividable by 8");
                }

                while (_bits % _mod != 0 || _blocksize % _mod != 0) {
                    if (_bits % _mod != 0) {
                        ++_bits;
                    }
                    if (_blocksize % _mod != 0) {
                        ++_blocksize;
                    }
                }
            }

            if (_bits < _blocksize * 8) {
                if constexpr (throw_errors) {
                    throw std::runtime_error("key size can`t be smaller than the block size * 8 (It would cause undefined behaviour)");
                }

                while (_bits < _blocksize * 8) {
                    if (_blocksize > _min_blocksize) {
                        _blocksize -= _size;
                    }
                    else {
                        _bits += _size;
                    }
                }
            }

            m_bits = _bits;
            m_blocksize = _blocksize;
        }

        void set_and_setup(const uint32_t _bits = DEFAULT_BITS, const uint32_t _blocksize = DEFAULT_BLOCKSIZE, const uint32_t _trys = DEFAULT_TRYS)
        {
            set(_bits, _blocksize);
            setup(_trys);
        }

        void setup(const uint32_t _trys = DEFAULT_TRYS) noexcept
        {
            thread_local std::random_device rd;
            thread_local std::mt19937_64 gen(rd());
            std::uniform_int_distribution<uint32_t> dist(0, m_bits / 8);

            m_setupdone = false;
            e = 65537;
            p = 0;
            q = 0;
            n = 0;
            number_t _phi;
            d = 0;

            do
            {
                const uint32_t _mod = 8 * sizeof(_char);
                uint32_t _rand1 = dist(gen), _rand2 = dist(gen);

                while (_rand1 % _mod != 0 || _rand2 % _mod != 0) {
                    if (_rand1 % _mod != 0) {
                        ++_rand1;
                    }
                    if (_rand2 % _mod != 0) {
                        ++_rand2;
                    }
                }

                // generate p and q
                if (thread_count >= 2) {
                    std::future<number_t> pf = std::async(&RSA::random_prime, this, m_bits + _rand1, _trys);
                    std::future<number_t> qf = std::async(&RSA::random_prime, this, m_bits + _rand2, _trys);
                    p = pf.get();
                    q = qf.get();
                }
                else {
                    p = random_prime(m_bits, _trys);
                    q = random_prime(m_bits, _trys);
                }

                // calculate N
                n = p * q;

                // calculate phi
                _phi = (p - 1) * (q - 1);

                // calculate e
                if (mp::gcd(e, _phi) != 1) {
                    uint32_t i = 2;
                    while (mp::gcd(i, _phi) != 1) {
                        ++i;
                    }
                    e = i;
                }

                //calculate d
                d = inverse_mod(e, _phi);

            } while ((e * d) % _phi != 1);

            m_setupdone = true;
        }

        std::vector<number_t> encrypt(const string_view& str, const std::tuple<uint32_t&, number_t&>& public_key)
        {
            auto async_pow = [&public_key](const number_t& num) noexcept -> number_t
            {
                return mp::powm(num, std::get<0>(public_key), std::get<1>(public_key));
            };

            if (str.empty()) {
                return { 0 };
            }

            std::vector<number_t> block_vector = create_block_vector(str);
            std::vector<std::future<number_t>> threads(block_vector.size());

            for (size_t i = 0; i != block_vector.size(); ++i) {
                threads[i] = std::async(std::launch::async, async_pow, block_vector[i]);
            }

            std::vector<number_t> result(threads.size());

            for (size_t i = 0; i != threads.size(); ++i) {
                result[i] = threads[i]._Get_value();
            }

            return result;
        }

        string decrypt(const std::vector<number_t>& message)
        {
            const uint32_t blocksize = m_blocksize / 8;
            const size_t blocks = message.size();

            auto async_decrypt = [this, blocksize](const number_t& _num) noexcept -> string
            {
                const std::string block = mp::powm(_num, d, n).convert_to<std::string>();
                string result;
                result.reserve(blocksize);

                constexpr uint32_t _bits = 8 * sizeof(_char);

                for (uint32_t i = 0; i != blocksize; ++i) {
                    const size_t num = atoux<size_t>(block.substr(i * _bits, _bits).c_str());

                    if constexpr (sizeof(_char) == 1) {
                        if (num == 11111111ULL) {
                            break;
                        }
                    }
                    else {
                        if (num == 1111111111111111ULL) {
                            break;
                        }
                    }
                    result.push_back(bits_to_char(num));
                }

                return result;
            };

            std::vector<std::future<string>> threads(blocks);

            for (size_t i = 0; i != message.size(); ++i) {
                threads[i] = std::async(std::launch::async, async_decrypt, message[i]);
            }

            string decrypted;
            decrypted.reserve(message.size() * blocksize);

            for (const auto& thread : threads) {
                decrypted.append(thread._Get_value());
            }

            return decrypted;
        }

    private:
        std::vector<number_t> create_block_vector(const string_view& str) const noexcept
        {
            constexpr uint32_t _bits = 8 * sizeof(_char);
            const size_t blocksize = m_blocksize / (8 / sizeof(_char));

            std::string msg;
            msg.reserve(str.size() * _bits + blocksize);

            for (const _char& ch : str) {
                const std::bitset<_bits> bits(ch);
                msg.append(bits.to_string());
            }

            for (char& chr : msg) {
                if (chr == '0') {
                    chr = '1';
                }
                else {
                    chr = '2';
                }
            }

            size_t msg_size = msg.size() / 8;

            while (msg_size % blocksize != 0) {
                ++msg_size;
                if constexpr (std::is_same_v<_char, char>) {
                    msg.append("11111111", 8);
                }
                else {
                    msg.append("1111111111111111", 16);
                }
            }

            const size_t blocks = msg_size / blocksize;
            const size_t get_size = msg.size() / blocks;

            std::vector<number_t> results;
            results.reserve(blocks * sizeof(number_t));

            for (size_t i = 0; i != blocks; ++i) {
                results.emplace_back(msg.substr(i * get_size, get_size));
            }

            return results;
        }

        template <class Ty = size_t>
        char_type bits_to_char(Ty bits) const noexcept
        {
            std::string bit_str = std::to_string(bits);

            for (auto& bit : bit_str) {
                if (bit == '1') {
                    bit = '0';
                }
                else {
                    bit = '1';
                }
            }

            bits = atoux<size_t>(bit_str.c_str());

            char_type result = 0, i = 0, rem = 0;

            while (bits) {
                rem = bits % 10;
                bits /= 10;
                result += rem * static_cast<char_type>(std::pow(2, i));
                ++i;
            }

            return result;
        }

        number_t egcd(const number_t& a, const number_t& b, number_t& x, number_t& y) const noexcept
        {
            if (a == 0) {
                x = 0;
                y = 1;
                return b;
            }

            number_t x1, y1;
            const number_t gcd = egcd(b % a, a, x1, y1);

            x = y1 - (b / a) * x1;
            y = x1;

            return gcd;
        };

        number_t inverse_mod(const number_t& e, const number_t& phi) const noexcept
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, x, y) == 1) {
                return (x % phi + phi) % phi;
            }
            return 0;
        }

        number_t random_number(const uint32_t _bits) const noexcept
        {
            thread_local std::random_device rd;
            thread_local std::mt19937_64 gen(rd());
            std::uniform_int_distribution<int> dist('0', '9');

            const uint32_t bytes = _bits / 8;

            std::string str(bytes, 0);

            do {
                str[0] = static_cast<char>(dist(gen));
            } while (str[0] == '0');

            for (uint32_t i = 1; i != bytes; ++i) {
                str[i] = static_cast<char>(dist(gen));
            }

            return number_t(str);
        }

        number_t random_possible_prime(const uint32_t _bits) const noexcept
        {
            auto small_test = [_bits](const number_t& num) noexcept -> bool
            {
                if (_bits == 8) {
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

            number_t num = random_number(_bits);

            while (!small_test(num)) {
                num = random_number(_bits);
            }

            return num;
        }

        number_t random_prime(const uint32_t _bits, const uint32_t _trys) const noexcept
        {
            if (thread_count > 4) {
                static const uint32_t _thread_count = (thread_count - 2) / 2;

                number_t result;

                auto search_thread = [this, _bits, _trys, &result]() noexcept -> void
                {
                    number_t num(random_possible_prime(_bits));

                    do
                    {
                        if (is_prime(num, _trys) && result.is_zero()) {
                            result = num;
                            return;
                        }
                        else {
                            num = random_possible_prime(_bits);
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
                number_t num(random_possible_prime(_bits));
                while (!is_prime(num, _trys)) {
                    num = random_possible_prime(_bits);
                }
                return num;
            }
        }

        bool miller_rabin(number_t d, const number_t& n) const noexcept
        {
            thread_local std::random_device rd;
            thread_local std::mt19937_64 mt(rd());
            boost::random::uniform_int_distribution<number_t> dist(2, n - 4);

            const number_t good = n - 1;
            const number_t a = dist(mt);
            number_t x = mp::powm(a, d, n);

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

#endif
