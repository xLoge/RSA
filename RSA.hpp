#pragma once

#ifndef _RSA_
#define _RSA_

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <string>
#include <random>
#include <bitset>
#include <vector>
#include <tuple>
#include <future>
#include <fstream>

template<class Ty, class ostream>
constexpr ostream& operator<<(ostream& out, const std::vector<Ty>& vec)
{
    for (size_t i = 0, end = vec.size() - 1; i < end; ++i) {
        out << vec[i] << ", ";
    }
    return out << vec.back();
}

#ifndef _BASE64_
#define _BASE64_

namespace Base64
{
    static std::string Encode(const std::string data) {
        static constexpr char sEncodingTable[] = {
          'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
          'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
          'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
          'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
          'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
          'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
          'w', 'x', 'y', 'z', '0', '1', '2', '3',
          '4', '5', '6', '7', '8', '9', '+', '/'
        };

        size_t in_len = data.size();
        size_t out_len = 4 * ((in_len + 2) / 3);
        std::string ret(out_len, '\0');
        size_t i;
        char* p = const_cast<char*>(ret.c_str());

        for (i = 0; i < in_len - 2; i += 3) {
            *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
            *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
            *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int)(data[i + 2] & 0xC0) >> 6)];
            *p++ = sEncodingTable[data[i + 2] & 0x3F];
        }
        if (i < in_len) {
            *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
            if (i == (in_len - 1)) {
                *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
                *p++ = '=';
            }
            else {
                *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
                *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
            }
            *p++ = '=';
        }

        return ret;
    }

    static std::string Decode(const std::string& input) {
        static constexpr unsigned char kDecodingTable[] = {
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
          52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
          64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
          15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
          64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
          41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
        };

        std::string out;

        size_t in_len = input.size();
        if (in_len % 4 != 0) return "Input data size is not a multiple of 4";

        size_t out_len = in_len / 4 * 3;
        if (input[in_len - 1] == '=') out_len--;
        if (input[in_len - 2] == '=') out_len--;

        out.resize(out_len);

        for (size_t i = 0, j = 0; i < in_len;) {
            uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
            uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
            uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
            uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

            uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

            if (j < out_len) out[j++] = (triple >> 2 * 8) & 0xFF;
            if (j < out_len) out[j++] = (triple >> 1 * 8) & 0xFF;
            if (j < out_len) out[j++] = (triple >> 0 * 8) & 0xFF;
        }

        return out;
    }
}

#endif

namespace RSA
{
    namespace mp = boost::multiprecision;
    typedef boost::multiprecision::cpp_int number_t;

    template<class Ty1, class Ty2>
    constexpr Ty1 pow(Ty1 base, Ty2 exp)
    {
        Ty2 res = 1;
        while (exp > 0) {
            if (exp & 1) {
                res = (res * base);
            }
            exp >>= 1;
            base *= base;
        }
        return res;
    }

    template <class what>
    constexpr what atoux(const char* str) {
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

    constexpr int ctoi(const char ch) {
        return static_cast<int>(ch - '0');
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
        constexpr static inline size_t char_size = sizeof(char_type);
        const static inline uint32_t thread_count = std::thread::hardware_concurrency();

        using string = std::basic_string<char_type>;
        using string_view = std::basic_string_view<char_type>;
        typedef std::tuple<number_t&, number_t&, number_t&, number_t&, uint32_t&, uint32_t&, uint32_t&> _export;

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
            m_bits = DEFAULT_BITS;
            m_blocksize = 256;
        }

        constexpr RSA(const uint32_t _bits, const uint32_t _blocksize = DEFAULT_BLOCKSIZE) {
            set(_bits, _blocksize);
        }

        constexpr RSA(const std::string_view& _file) {
            import_file(_file);
        }

        constexpr RSA(std::ifstream& _file) {
            import_file(_file);
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
            constexpr uint32_t _size = 8 * char_size;
            constexpr uint32_t _mod  = 8 * char_size;
            constexpr uint32_t _min_blocksize = 16;
            const uint32_t _max_blocksize = _blocksize * 4;

            if (_blocksize == 0) {
                throw std::invalid_argument("block size can`t be 0");
            }
            
            if (_bits == 0) {
                throw std::invalid_argument("key size can`t be 0");
            }

            if (_blocksize == DEFAULT_BLOCKSIZE) {
                _blocksize = _min_blocksize;
                
                while (_blocksize * 4 < _bits) {
                    _blocksize += _size;
                    if (_blocksize >= 256) {
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
            else if (_blocksize > _max_blocksize) {
                _blocksize = _max_blocksize;
            }
            
            while (_bits % _mod != 0 || _blocksize % _mod != 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("both key and block size have to be dividable by 8");
                }
                if (_bits % _mod != 0) {
                    ++_bits;
                }
                if (_blocksize % _mod != 0) {
                    ++_blocksize;
                }
            }
            
            while (_bits < _blocksize * 4) {
                if constexpr (throw_errors) {
                    throw std::runtime_error("key size can`t be smaller than the block size (It would cause undefined behaviour)");
                }
                else if (_blocksize > _min_blocksize) {
                    _blocksize -= _size;
                }
                else {
                    _bits += _size;
                }
            }

            m_bits = _bits;
            m_blocksize = _blocksize;
        }

        bool import_file(const std::string_view& _filename)
        {
            std::ifstream _file(_filename.data());
            return import_file(_file);
        }

        bool import_file(std::ifstream& _file)
        {
            if (_file.good() == false) {
                return false;
            }

            std::string _line;

            while (std::getline(_file, _line)) {
                if (_line == std::string_view("-----BEGIN EXPORT-----")) {
                    std::getline(_file, _line);
                    std::string _data = Base64::Decode(_line);

                    size_t _find = _data.find(',');
                    p = number_t(_data.substr(0, _find));
                    _data = _data.substr(_find + 1);

                    _find = _data.find(',');
                    q = number_t(_data.substr(0, _find));
                    _data = _data.substr(_find + 1);

                    _find = _data.find(',');
                    n = number_t(_data.substr(0, _find));
                    _data = _data.substr(_find + 1);

                    _find = _data.find(',');
                    d = number_t(_data.substr(0, _find));
                    _data = _data.substr(_find + 1);

                    _find = _data.find(',');
                    e = atoux<uint32_t>(_data.substr(0, _find).data());
                    _data = _data.substr(_find + 1);

                    _find = _data.find(',');
                    m_bits = atoux<uint32_t>(_data.substr(0, _find).data());
                    _data = _data.substr(_find + 1);

                    m_blocksize = atoux<uint32_t>(_data.data());
                    break;
                }
            }

            _file.close();
            m_setupdone = true;

            return true;
        }

        bool export_file(const std::string_view& _filename)
        {
            std::ofstream _file(_filename.data());
            return export_file(_file);
        }

        bool export_file(std::ofstream& _file)
        {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you try to export a key.");
            }

            if (_file.good() == false) {
                return false;
            }

            std::string _out;

            _out.append(p.str());
            _out.push_back(',');
            _out.append(q.str());
            _out.push_back(',');
            _out.append(n.str());
            _out.push_back(',');
            _out.append(d.str());
            _out.push_back(',');
            _out.append(std::to_string(e));
            _out.push_back(',');
            _out.append(std::to_string(m_bits));
            _out.push_back(',');
            _out.append(std::to_string(m_blocksize));
            _out.assign(Base64::Encode(_out));

            _file << std::string_view("-----BEGIN EXPORT-----\n");
            _file << _out;
            _file << std::string_view("\n-----ENDOF EXPORT-----\n");
            _file.close();

            return true;
        }

        void import_key(const _export& _key)
        {
            p = std::get<0>(_key);
            q = std::get<1>(_key);
            n = std::get<2>(_key);
            d = std::get<3>(_key);
            e = std::get<4>(_key);
            m_bits = std::get<5>(_key);
            m_blocksize = std::get<6>(_key);
        }

        _export export_key()
        {
            return { p, q, n, d, e, m_bits, m_blocksize };
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
            boost::random::uniform_int_distribution<uint32_t> dist(0, m_bits / 8);

            m_setupdone = false;
            e = 65537;
            p = 0;
            q = 0;
            n = 0;
            d = 0;
            number_t _phi;

            do
            {
                const uint32_t _mod = 8;
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
                if (gcd(e, _phi) != 1) {
                    uint32_t i = 2;
                    while (gcd(i, _phi) != 1) {
                        ++i;
                    }
                    e = i;
                }

                //calculate d
                d = inverse_mod(e, _phi);

            } while ((e * d) % _phi != 1);

            m_setupdone = true;
        }

        std::vector<number_t> encrypt(const string_view& str, const std::tuple<uint32_t&, number_t&>& public_key) const
        {
            check_setup();
            
            auto async_pow = [public_key](const number_t& num) noexcept -> number_t {
                return mp::powm(num, std::get<0>(public_key), std::get<1>(public_key));
            };

            if (str.empty()) {
                return { };
            }

            std::vector<number_t> block_vector(create_block_vector(str));
            std::vector<number_t> result      (block_vector.size());
            std::vector<std::future<number_t>> threads(block_vector.size());

            for (size_t i = 0; i != block_vector.size(); ++i) {
                threads[i] = std::async(std::launch::async, async_pow, block_vector[i]);
            }

            for (size_t i = 0; i != threads.size(); ++i) {
                result[i] = threads[i]._Get_value();
            }
            return result;
        }

        string decrypt(const std::vector<number_t>& message) const
        {
            check_setup();

            const size_t blocks = message.size();

            auto async_decrypt = [this](const number_t& _encrypted) noexcept -> string
            {
                const std::string decrypted = mp::powm(_encrypted, d, n).convert_to<std::string>();
                
                string result;
                result.reserve(m_blocksize);

                for (size_t i = 0; decrypted[i] != '0' || i > m_blocksize;) {
                    size_t _len = decrypted[i++] - '0';
                    if (decrypted[i] == '0' && _len > 1) {
                        result.push_back(-static_cast<char_type>(std::stoll(decrypted.substr(++i, --_len))));
                    }
                    else {
                        result.push_back(static_cast<char_type>(atoux<size_t>(decrypted.substr(i, _len).c_str())));
                    }
                    i += _len;
                }

                return result;
            };

            std::vector<std::future<string>> threads(blocks);

            for (size_t i = 0; i != message.size(); ++i) {
                threads[i] = std::async(std::launch::async, async_decrypt, message[i]);
            }

            string decrypted;
            decrypted.reserve(message.size() * m_blocksize);

            for (const auto& thread : threads) {
                decrypted.append(thread._Get_value());
            }

            return decrypted;
        }

    //private:
        void check_setup() const {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you use the class");
            }
        }

        std::vector<number_t> create_block_vector(const string_view& str) const noexcept
        {
            std::string msg;
            msg.reserve(m_blocksize * char_size);

            std::vector<std::string> blocks;

            for (size_t i = 0;;) {
                const std::string _num = std::to_string(str[i]);
                size_t _nextsize = msg.size() + _num.size() + 1;

                if (_num.front() == '-') { ++_nextsize; }

                if (_nextsize > m_blocksize) {
                    msg.push_back('0');
                    blocks.push_back(msg);
                    msg.clear();
                }
                
                msg.reserve(_nextsize);
                msg.append(std::to_string(_num.size()));

                if (_num.front() == '-') {
                    msg.push_back('0');
                    msg.append(_num.substr(1));
                }
                else {
                    msg.append(_num);
                }


                if (++i == str.size()) {
                    msg.push_back('0');
                    blocks.push_back(msg);
                    break;
                }
            }

            std::vector<number_t> results;
            results.reserve(blocks.size() * sizeof(number_t));

            for (const std::string& str : blocks) {
                results.emplace_back(str);
            }

            return results;
        }

        static number_t egcd(const number_t& a, const number_t& b, number_t& x, number_t& y) noexcept
        {
            if (a == 0) {
                x = 0;
                y = 1;
                return b;
            }
            
            number_t x1, y1;
            number_t gcd = egcd(b % a, a, x1, y1);

            x = y1 - (b / a) * x1;
            y = x1;

            return gcd;
        };

        static number_t inverse_mod(const number_t& e, const number_t& phi) noexcept
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, x, y) == 1) {
                return ((x % phi) + phi) % phi;
            }
            return 0;
        }

        number_t random_number(const uint32_t _bits) const noexcept
        {
            thread_local std::random_device rd;
            thread_local std::mt19937_64 gen(rd());
            boost::random::uniform_int_distribution<char> dist('0', '9');

            const uint32_t bytes = _bits / 8;

            std::string str(bytes, 0);

            do {
                str[0] = dist(gen);
            } while (str[0] == '0');

            for (uint32_t i = 1; i != bytes; ++i) {
                str[i] = dist(gen);
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
            
            while (small_test(num) == false) {
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

            const number_t d_base(n - 1);
            const number_t rand(dist(mt));
            number_t x(mp::powm(rand, d, n));

            if (x == 1 || x == d_base) {
                return true;
            }

            while (d < d_base) {
                x = (x * x) % n;

                if (x == 1) {
                    return false;
                }
                if (x == d_base) {
                    return true;
                }

                d *= 2;
            }

            return false;
        }

        bool is_prime(const number_t& n, const uint32_t _trys) const noexcept
        {
            number_t d = n - 1;

            while (d % 2 == 0) {
                d /= 2;
            }

            for (uint32_t i = 0; i != _trys; ++i) {
                if (miller_rabin(d, n) == false) {
                    return false;
                }
            }

            return true;
        }
    };
}

#endif
