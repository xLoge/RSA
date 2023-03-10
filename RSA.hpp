#pragma once

#ifndef _RSA_
#define _RSA_

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <mutex>

namespace RSA
{
    template<class, bool>
    class basic_rsa;

    using RSA = basic_rsa<char, false>;
    using tRSA = basic_rsa<char, true>;

    using wRSA = basic_rsa<wchar_t, false>;
    using wtRSA = basic_rsa<wchar_t, true>;
    
#ifdef __cpp_char8_t
    using u8RSA = basic_rsa<char8_t, false>;
    using u8tRSA = basic_rsa<char8_t, true>;
#endif

    using u16RSA = basic_rsa<char16_t, false>;
    using u16tRSA = basic_rsa<char16_t, true>;

    using u32RSA = basic_rsa<char32_t, false>;
    using u32tRSA = basic_rsa<char32_t, true>;

    typedef boost::multiprecision::cpp_int number_t;

    namespace detail
    {
        // * Decimal to number
        template <class Ty = int64_t>
        constexpr Ty dton(const char* str) {
            constexpr Ty _max = std::numeric_limits<Ty>::max();

            bool _negative = false;

            if (*str == '-') {
                _negative = true;
                ++str;
            }

            Ty result = 0;
            while (*str)
            {
                if (*str < '0' || *str > '9')
                    throw std::invalid_argument("only number strings");

                const Ty temp = result * 10 + (*str++ - '0');
                
                if (temp < result)
                    return _max;

                result = temp;
            }
            
            if (_negative)
                result *= -1;

            return result;
        }

        // * Hex to number
        template <class Ty = int64_t>
        constexpr Ty hton(const char* str)
        {
            constexpr Ty _max = std::numeric_limits<Ty>::max();

            bool _negative = false;

            if (*str == '-') {
                _negative = true;
                ++str;
            }

            Ty result = 0;
            while (*str)
            {
                Ty temp = result * 16;

                if (*str >= '0' && *str <= '9') {
                    temp += *str++ - '0';
                }
                else if (*str >= 'a' && *str <= 'f') {
                    temp += 10 + *str++ - 'a';
                }
                else if (*str >= 'A' && *str <= 'F') {
                    temp += 10 + *str++ - 'A';
                }
                else {
                    throw std::invalid_argument("only hex number strings");
                }

                if (temp < result) {
                    return _max;
                }

                result = temp;
            }

            if (_negative)
                result *= -1;

            return result;
        }

        namespace base64
        {
            std::string encode(const std::string data) {
                constexpr char sEncodingTable[] = {
                  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                  'w', 'x', 'y', 'z', '0', '1', '2', '3',
                  '4', '5', '6', '7', '8', '9', '+', '/'
                };

                const uint64_t in_len = data.size();
                const uint64_t out_len = 4 * ((in_len + 2) / 3);
                std::string ret(out_len, '\0');
                uint64_t i;
                char* p = const_cast<char*>(ret.c_str());

                for (i = 0; i < in_len - 2; i += 3) {
                    *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
                    *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int32_t)(data[i + 1] & 0xF0) >> 4)];
                    *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int32_t)(data[i + 2] & 0xC0) >> 6)];
                    *p++ = sEncodingTable[data[i + 2] & 0x3F];
                }
                if (i < in_len) {
                    *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
                    if (i == (in_len - 1)) {
                        *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
                        *p++ = '=';
                    }
                    else {
                        *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int32_t)(data[i + 1] & 0xF0) >> 4)];
                        *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
                    }
                    *p++ = '=';
                }

                return ret;
            }

            std::string decode(const std::string& input) {
                constexpr uint8_t kDecodingTable[] = {
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

                uint64_t in_len = input.size();
                if (in_len % 4 != 0) return "Input data size is not a multiple of 4";

                uint64_t out_len = in_len / 4 * 3;
                if (input.at(in_len - 1) == '=') out_len--;
                if (input.at(in_len - 2) == '=') out_len--;

                out.resize(out_len);

                for (uint64_t i = 0, j = 0; i < in_len;) {
                    uint32_t a = input.at(i) == '=' ? 0 & i++ : kDecodingTable[static_cast<int32_t>(input.at(i++))];
                    uint32_t b = input.at(i) == '=' ? 0 & i++ : kDecodingTable[static_cast<int32_t>(input.at(i++))];
                    uint32_t c = input.at(i) == '=' ? 0 & i++ : kDecodingTable[static_cast<int32_t>(input.at(i++))];
                    uint32_t d = input.at(i) == '=' ? 0 & i++ : kDecodingTable[static_cast<int32_t>(input.at(i++))];

                    uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

                    if (j < out_len) out[j++] = (triple >> 2 * 8) & 0xFF;
                    if (j < out_len) out[j++] = (triple >> 1 * 8) & 0xFF;
                    if (j < out_len) out[j++] = (triple >> 0 * 8) & 0xFF;
                }

                return out;
            }
        }
    }

    template<class _char>
    constexpr uint32_t blocksize_for(const _char* const str, const uint64_t count) noexcept {
        auto _val = count * 8;
        while (_val % (16 * sizeof(_char))) { _val += 8; };
        return _val;
    }

    template<class _char>
    constexpr uint32_t blocksize_for(const _char* const str) noexcept {
        return blocksize_for<_char>(str, std::char_traits<_char>::length(str));
    }

    template<class _char>
    constexpr uint32_t blocksize_for(const std::basic_string<_char>& str) noexcept {
        return blocksize_for(str.data(), str.size());
    }

    template<class _char>
    constexpr uint32_t blocksize_for(const std::basic_string_view<_char>& str) noexcept {
        return blocksize_for(str.data(), str.size());
    }
    
    template<class _char, bool _throw>
    void print(basic_rsa<_char, _throw> _rsa) {
        std::cout << _rsa;
    }

    template <class char_type = char, bool throw_errors = false> 
    class basic_rsa
    {
    protected:
        constexpr static inline uint32_t DEFAULT_BITS = static_cast<uint32_t>(3072);
        constexpr static inline uint32_t DEFAULT_TRYS = static_cast<uint32_t>(-1);
        constexpr static inline uint32_t DEFAULT_BLOCKSIZE = static_cast<uint32_t>(-1);
    
    private:
        constexpr static inline auto char_size = sizeof(char_type);
        const static inline auto thread_count = (std::thread::hardware_concurrency() == 0) ? 1 : std::thread::hardware_concurrency();

        typedef std::tuple<number_t&, number_t&, number_t&, number_t&, uint32_t&, uint32_t&, uint32_t&> export_t;
        using string = std::basic_string<char_type>;
        using string_view = std::basic_string_view<char_type>;

    private:
        bool m_setupdone = false;
        uint32_t m_blocksize = DEFAULT_BLOCKSIZE;
        uint32_t m_bits = DEFAULT_BITS;
        uint32_t m_trys = DEFAULT_TRYS;

    public:
        uint32_t e = 0;
        number_t p, q, n, d;

        std::tuple<uint32_t&, number_t&> public_key { e, n };
        std::tuple<number_t&, number_t&> private_key{ d, n };

    public:
        constexpr basic_rsa() :m_bits(3072), m_blocksize(256), m_trys(20) {

        }

        constexpr basic_rsa(const uint32_t _bits, const uint32_t _blocksize = DEFAULT_BLOCKSIZE, const uint32_t _trys = DEFAULT_TRYS) {
            set(_bits, _blocksize, _trys);
        }

        // Import values from exported RSA file
        constexpr basic_rsa(const std::string_view& _file) {
            import_file(_file);
        }

        // Import values from exported RSA file
        constexpr basic_rsa(std::ifstream& _file) {
            import_file(_file);
        }

        // Import values from other RSA class
        constexpr basic_rsa(const export_t& _key) {
            import_key(_key);
        }

        // Check if setup() is already done
        constexpr operator bool() const noexcept
        {
            return m_setupdone;
        }

        // Check if setup() is already done
        constexpr bool setupdone() const noexcept
        {
            return m_setupdone;
        }

        // The bits of the generated primes
        constexpr uint32_t blocksize() const noexcept
        {
            return m_blocksize;
        }

        // The bits of the generated primes
        constexpr uint32_t keysize() const noexcept
        {
            return m_bits;
        }

        // The current amouth of Miller Rabin tests
        constexpr uint32_t trys() const noexcept
        {
            return m_trys;
        }

        // How likely the numbers generated are prime in percent
        constexpr double_t precision() const noexcept
        {
            return 100.0 - (std::pow(2, static_cast<int64_t>(m_trys) * -1) * 100.0);
        }
        
        // Set new keysize, blocksize and trys
        constexpr void set(uint32_t _bits = DEFAULT_BITS, uint32_t _blocksize = DEFAULT_BLOCKSIZE, uint32_t _trys = DEFAULT_TRYS)
        {
            constexpr uint32_t _size = 8 * char_size;
            constexpr uint32_t _mod  = 8 * char_size;
            constexpr uint32_t _min_blocksize = 16 * char_size;
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
                    throw std::runtime_error("key size can`t be smaller than the block size * 4 (It would cause undefined behaviour)");
                }
                else if (_blocksize > _min_blocksize) {
                    _blocksize -= _size;
                }
                else {
                    _bits += _size;
                }
            }

            if (_trys == DEFAULT_TRYS) {
                m_trys = get_rabin_trys(m_bits);
            }
            else {
                m_trys = _trys;
            }

            m_bits = _bits;
            m_blocksize = _blocksize;
        }

        // Set how many Miller Rabin tests to perform
        constexpr void set_trys(const uint32_t _trys)
        {
            m_trys = _trys;
        }

        // Import file from a filepath
        bool import_file(const std::string_view& _filename)
        {
            std::ifstream _file(_filename.data());
            return import_file(_file);
        }

        // Import file from an ifstream
        bool import_file(std::ifstream& _file)
        {
            if (_file.good() == false) {
                return false;
            }

            std::string _line;

            do
            {
                if (!std::getline(_file, _line)) {
                    _file.close();
                    return false;
                }
            } while (_line != "-----BEGIN EXPORT-----");

            if (std::getline(_file, _line))
                p.assign(detail::base64::decode(_line));

            if (std::getline(_file, _line))
                q.assign(detail::base64::decode(_line));

            if (std::getline(_file, _line))
                n.assign(detail::base64::decode(_line));

            if (std::getline(_file, _line))
                d.assign(detail::base64::decode(_line));

            if (std::getline(_file, _line))
                e = detail::dton<uint32_t>(detail::base64::decode(_line).c_str());

            if (std::getline(_file, _line))
                m_bits = detail::dton<uint32_t>(detail::base64::decode(_line).c_str());

            if (std::getline(_file, _line))
                m_blocksize = detail::dton<uint32_t>(detail::base64::decode(_line).c_str());

            _file.close();
            
            // Check
            set(m_bits, m_blocksize);

            m_setupdone = true;

            return true;
        }

        // Export current setup to a filepath
        bool export_file(const std::string_view& _filename)
        {
            std::ofstream _file(_filename.data());
            return export_file(_file);
        }

        // Export current setup to an ostream
        bool export_file(std::ofstream& _file)
        {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you try to export a key.");
            }

            if (_file.good() == false) {
                return false;
            }

            _file << "-----BEGIN EXPORT-----\n";

            _file << detail::base64::encode(p.str()) << '\n';
            _file << detail::base64::encode(q.str()) << '\n';
            _file << detail::base64::encode(n.str()) << '\n';
            _file << detail::base64::encode(d.str()) << '\n';
            _file << detail::base64::encode(std::to_string(e)) << '\n';
            _file << detail::base64::encode(std::to_string(m_bits)) << '\n';
            _file << detail::base64::encode(std::to_string(m_blocksize)) << '\n';

            _file << "-----END EXPORT-----\n";

            _file.close();

            return true;
        }

        // Import setup from other RSA class export
        void import_key(const export_t& _key)
        {
            p = std::get<0>(_key);
            q = std::get<1>(_key);
            n = std::get<2>(_key);
            d = std::get<3>(_key);
            e = std::get<4>(_key);
            m_bits = std::get<5>(_key);
            m_blocksize = std::get<6>(_key);

            set(m_bits, m_blocksize, m_trys);
        }

        // Export current setup
        const export_t export_key()
        {
            return { p, q, n, d, e, m_bits, m_blocksize };
        }

        // Generate the primes
        void setup() noexcept
        {
            m_setupdone = false;

            while (true)
            {
                const auto pair = generate_prime_pair(m_bits, m_trys);

                p = pair.first;
                q = pair.second;

                n = p * q;

                const number_t phi = (p - 1) * (q - 1);

                e = 65537;

                while (boost::multiprecision::gcd(e, phi) != 1) {
                    ++e;
                }

                d = inverse_mod(e, phi);

                if ((e * d) % phi == 1) {
                    break;
                }
            }
            
            m_setupdone = true;
        }

        // Encrypts a string
        std::vector<number_t> encrypt(const string_view& _str, const std::tuple<uint32_t&, number_t&>& _public_key) const
        {
            check_setup();

            auto pow_block = [_public_key](const number_t& _block) noexcept -> number_t {
                return boost::multiprecision::powm(_block, std::get<0>(_public_key), std::get<1>(_public_key));
            };

            if (_str.empty()) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("Can`t Encrypt empty string");
                }
                return { };
            }

            const auto blocks = create_blocks(_str);

            std::vector<std::future<number_t>> threads;

            for (uint64_t i = 0; i < blocks.size(); ++i) {
                threads.push_back(std::async(std::launch::async, pow_block, blocks.at(i)));
            }

            std::vector<number_t> result;

            for (auto& thread : threads) {
                result.push_back(thread.get());
            }

            return result;
        }

        // Decrypts the encrypted blocks
        string decrypt(const std::vector<number_t>& _encrypted) const
        {
            check_setup();

            auto decrypt_block = [this, d, n](const number_t& _block) noexcept -> string
            {
                const std::string decrypted = boost::multiprecision::powm(_block, d, n).convert_to<std::string>();

                string result; result.reserve(m_blocksize);

                for (uint32_t i = 0; decrypted.at(i) != '0' || i > m_blocksize;)
                {
                    uint16_t _len = decrypted.at(i++) - '0';

                    if (decrypted.at(i) == '0' && _len > 1) {
                        const auto _chr = decrypted.substr(++i, --_len);
                        const auto _dec = detail::dton<char_type>(_chr.data()) * (-1);
                        result.push_back(_dec);
                    }
                    else {
                        const auto _chr = decrypted.substr(i, _len);
                        const auto _dec = detail::dton<char_type>(_chr.data());
                        result.push_back(_dec);
                    }

                    i += _len;
                }

                return result;
            };
            
            std::vector<std::future<string>> threads;

            for (uint64_t i = 0; i < _encrypted.size(); ++i) {
                threads.push_back(std::async(std::launch::async, decrypt_block, _encrypted.at(i)));
            }

            string decrypted; decrypted.reserve(_encrypted.size() * m_blocksize * char_size);

            for (auto& thread : threads) {
                decrypted.append(thread.get());
            }

            return decrypted;
        }

        // Creates the blocks for encryption
        std::vector<number_t> create_blocks(const string_view& _str) const noexcept
        {
            std::string msg; msg.reserve(m_blocksize * char_size);

            std::vector<std::string> blocks;

            for (uint32_t i = 0;;)
            {
                const std::string    num = std::to_string(_str.at(i));
                const bool      negative = (num.front() == '-');
                const uint32_t  nextsize = msg.size() + num.size() + static_cast<int32_t>(negative) + 1;

                if (nextsize > m_blocksize) {
                    msg.push_back('0');
                    blocks.push_back(msg);
                    msg.clear();
                }

                msg.append( std::to_string(num.size()) );

                if (negative) {
                    msg.push_back('0');
                    msg.append(num.substr(1));
                }
                else {
                    msg.append(num);
                }

                if (++i == _str.size()) {
                    msg.push_back('0');
                    blocks.push_back(msg);
                    break;
                }
            }

            std::vector<number_t> results; results.reserve(blocks.size() * sizeof(number_t));

            for (const auto& str : blocks) {
                results.emplace_back(str);
            }

            return results;
        }

    private:
        // Check if setup() has been called
        void __forceinline check_setup() const
        {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you use the class");
            }
        }

        // Calculate the extended Euclidean algorithm
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

        // Calculate modular multiplicative inverse of e and phi
        static number_t inverse_mod(const number_t& e, const number_t& phi) noexcept
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, x, y) == 1) {
                return ((x % phi) + phi) % phi;
            }
            return 0;
        }

        // Generate random primelike string with X bits
        static std::string random_primish_number(const uint32_t _bits) noexcept
        {
            std::mt19937_64 gen(std::random_device{ }());
            std::uniform_int_distribution<int32_t> dist('0', '9');

            const uint32_t bytes = _bits / 8;

            std::string str(bytes, 0);

            for (char& ch : str) {
                ch = static_cast<char>(dist(gen));
            }

            while (str.front() == '0') {
                str.front() = static_cast<char>(dist(gen));
            }

            while (str.back() != '1' && str.back() != '3' && str.back() != '7' && str.back() != '9') {
                str.back() = static_cast<char>(dist(gen));
            }

            return str;
        }

        // Generate 2 primes p and q
        static std::pair<number_t, number_t> generate_prime_pair(const uint32_t _bits, const uint32_t _trys) noexcept
        {
            auto search_thread = [_trys](const uint32_t bits, number_t& X, std::mutex& lock) noexcept -> void
            {
                number_t possible_prime;

                while (X.is_zero())
                {
                    possible_prime.assign(random_primish_number(bits));

                    if (is_prime(possible_prime, _trys))
                    {
                        lock.lock();
                        {
                            if (X.is_zero())
                                X = possible_prime;
                        }
                        lock.unlock();

                        return;
                    }
                }
            };

            static const uint32_t threads_per = thread_count / 2;

            number_t p = 0, q = 0;

            std::mt19937_64 mt(std::random_device{ }());
            std::uniform_int_distribution<int32_t> dist((_bits / 64) * -1, _bits / 64);

            const int32_t _SP = _bits + dist(mt), _SQ = _bits + dist(mt);

            std::mutex p_lock;
            std::mutex q_lock;

            std::vector<std::future<void>> threads;

            for (uint32_t i = 0; i < threads_per; ++i) {
                threads.push_back(std::async(std::launch::async, search_thread, _SP, std::ref(p), std::ref(p_lock)));
                threads.push_back(std::async(std::launch::async, search_thread, _SQ, std::ref(q), std::ref(q_lock)));
            }

            for (const auto& thread : threads) {
                thread.wait();
            }

            return std::make_pair(std::move(p), std::move(q));
        }

        // Check if X number is prime with Y Miller Rabin trys
        static bool is_prime(const number_t& _num, const uint32_t _trys) noexcept
        {
            for (uint32_t i = 2; i < 10000; ++i)
            {
                if (_num % i == 0)
                {
                    if (_num != i) {
                        return false;
                    }
                    else {
                        break;
                    }
                }
            }

            const number_t d_base = _num - 1;
            number_t d = d_base;

            uint32_t _divides = 0;
            while (d % 2 == 0)
            {
                d /= 2;
                ++_divides;
            }

            std::mt19937_64 mt(std::random_device{ }());
            boost::random::uniform_int_distribution<number_t> dist(2, _num - 2);

            for (uint32_t i = 0; i < _trys; ++i)
            {
                const number_t rand = dist(mt);
                number_t test = boost::multiprecision::powm(dist(mt), d, _num);

                if (test == 1 || test == d_base)
                    continue;

                bool _continue = true;

                for (uint32_t j = 1; j < _divides; ++j)
                {
                    test = (test * test) % _num;

                    if (test == d_base) {
                        _continue = false;
                        break;
                    }
                }

                if (_continue)
                    return false;
            }

            return true;
        }

        // Get a good amouth of Miller Rabin test for X bits
        static uint32_t get_rabin_trys(const uint64_t _bits) noexcept
        {
            const auto bits = _bits * _bits * _bits;
            return static_cast<uint32_t>((std::log10(_bits * _bits * _bits)) * 1.5);
        }

        // Output for RSA class
        template<class _char, bool _throw, class ostream = std::ostream>
        friend ostream& operator<<(ostream& out, const basic_rsa<_char, _throw>& rsa)
        {
            out << "Charset  : " << typeid(_char).name() << '\n';
            out << "Keysize  : " << rsa.keysize() << '\n';
            out << "Blocksize: " << rsa.blocksize() << " or " << rsa.blocksize() / (8 * sizeof(_char)) << ' ' << typeid(_char).name() << "`s\n";
            out << "Trys     : " << rsa.trys() << '\n';
            out << "Precision: " << rsa.precision() << '%' << '\n';
            out << "Setupdone: " << (rsa.setupdone() == true ? "true" : "false") << "\n\n";
            out << "P: " << rsa.p << "\n\n";
            out << "Q: " << rsa.q << "\n\n";
            out << "N: " << rsa.n << "\n\n";
            out << "E: " << rsa.e << "\n\n";
            out << "D: " << rsa.d << "\n\n";
            return out;
        }
    };
}

#endif
