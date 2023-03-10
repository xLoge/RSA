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

#ifndef _NODISCARD
    #define _NODISCARD [[nodiscard]]
#endif

namespace RSA
{
    static constexpr uint32_t Auto = static_cast<uint32_t>(-1);

    template<class, bool>
    class basic_rsa;

    // char, no throw
    using RSA = basic_rsa<char, false>;
    // char, throw
    using tRSA = basic_rsa<char, true>;

    // wchar_t, no throw
    using wRSA = basic_rsa<wchar_t, false>;
    // wchar_t, throw
    using wtRSA = basic_rsa<wchar_t, true>;
    
#ifdef __cpp_char8_t
    // char8_t, no throw
    using u8RSA = basic_rsa<char8_t, false>;
    // char8_t, throw
    using u8tRSA = basic_rsa<char8_t, true>;
#endif

    // char16_t, no throw
    using u16RSA = basic_rsa<char16_t, false>;
    // char16_t, throw
    using u16tRSA = basic_rsa<char16_t, true>;

    // char32_t, no throw
    using u32RSA = basic_rsa<char32_t, false>;
    // char32_t, throw
    using u32tRSA = basic_rsa<char32_t, true>;

    typedef boost::multiprecision::cpp_int number_t;

    namespace detail
    {
        // * Decimal to number
        template <class Ty = int64_t>
        _NODISCARD constexpr Ty dton(const char* str) {
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
        _NODISCARD constexpr Ty hton(const char* str)
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
    }
    
    template<class _char, bool _throw>
    void print(basic_rsa<_char, _throw> _rsa) {
        std::cout << _rsa;
    }

    template <class char_type = char, bool throw_errors = false> 
    class basic_rsa
    {
    private:
        
        constexpr static inline auto one_in_block_size = std::numeric_limits<char_type>::digits10 + 2;
        constexpr static inline auto char_size = sizeof(char_type);
        const static inline auto thread_count = (std::thread::hardware_concurrency() == 0) ? 1 : std::thread::hardware_concurrency();

        typedef std::tuple<number_t, number_t, number_t, number_t, uint32_t, uint32_t> export_t;
        using string = std::basic_string<char_type>;
        using string_view = std::basic_string_view<char_type>;

    protected:
        constexpr static inline uint32_t DEFAULT_BITS = static_cast<uint32_t>(3072);
        constexpr static inline uint32_t DEFAULT_TRYS = static_cast<uint32_t>(-1);
        constexpr static inline uint32_t MIN_BLOCKSIZE = static_cast<uint32_t>(one_in_block_size);

    private:
        bool m_setupdone = false;
        uint32_t m_bits = DEFAULT_BITS;
        uint32_t m_trys = DEFAULT_TRYS;

    public:
        uint32_t e = 0;
        number_t p, q, n, d;

        std::tuple<uint32_t&, number_t&> public_key { e, n };
        std::tuple<number_t&, number_t&> private_key{ d, n };

    public:
        constexpr basic_rsa() :m_bits(3072), m_trys(17) {

        }

        constexpr basic_rsa(const uint32_t _bits, const uint32_t _trys = DEFAULT_TRYS) {
            set(_bits, _trys);
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
        _NODISCARD constexpr operator bool() const noexcept
        {
            return m_setupdone;
        }

        // Check if setup() is already done
        _NODISCARD constexpr bool setupdone() const noexcept
        {
            return m_setupdone;
        }

        // The bits of the generated primes
        _NODISCARD constexpr uint32_t keysize() const noexcept
        {
            return m_bits;
        }

        // The current amouth of Miller Rabin tests
        _NODISCARD constexpr uint32_t trys() const noexcept
        {
            return m_trys;
        }

        // How likely the numbers generated are prime in percent
        _NODISCARD constexpr double_t precision() const noexcept
        {
            return 100.0 - (std::pow(2, static_cast<int64_t>(m_trys) * -1) * 100.0);
        }
        
        // Set new keysize, blocksize and trys
        constexpr void set(uint32_t _bits = DEFAULT_BITS, uint32_t _trys = DEFAULT_TRYS) noexcept(!throw_errors)
        {
            if (_bits == 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("key size can`t be 0");
                }

                _bits = DEFAULT_BITS;
            }

            if (_trys == 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("can`t have 0 trys");
                }

                _trys = DEFAULT_TRYS;
            }

            if (_bits % 8 != 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("keysize has to be dividable by 8");
                }

                _bits = (8 - _bits % 8);
            }

            m_bits = _bits;

            if (_trys == DEFAULT_TRYS) {
                m_trys = get_rabin_trys(_bits);
            }
            else {
                m_trys = _trys;
            }
        }

        // Set the size of the key
        constexpr void set_keysize(const uint32_t _bits)
        {
            set(_bits, m_trys);
        }

        // Set how many Miller Rabin tests to perform
        constexpr void set_trys(const uint32_t _trys)
        {
            set(m_bits, _trys);
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
                p.assign(_line);

            if (std::getline(_file, _line))
                q.assign(_line);

            if (std::getline(_file, _line))
                n.assign(_line);

            if (std::getline(_file, _line))
                d.assign(_line);

            if (std::getline(_file, _line))
                e = detail::dton<uint32_t>(_line.c_str());

            if (std::getline(_file, _line))
                m_bits = detail::dton<uint32_t>(_line.c_str());

            _file.close();
            
            set(m_bits);

            m_setupdone = true;

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

            set(m_bits);

            m_setupdone = true;
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

            _file << p.str() << '\n';
            _file << q.str() << '\n';
            _file << n.str() << '\n';
            _file << d.str() << '\n';
            _file << std::to_string(e) << '\n';
            _file << std::to_string(m_bits) << '\n';

            _file << "-----END EXPORT-----\n";

            _file.close();

            return true;
        }

        // Export current setup
        _NODISCARD const export_t export_key()
        {
            return { p, q, n, d, e, m_bits };
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

                if (boost::multiprecision::gcd(e, phi) != 1) {
                    e = 2;
                    while (boost::multiprecision::gcd(e, phi) != 1) {
                        ++e;
                    }
                }
                
                d = inverse_mod(e, phi);

                if ((e * d) % phi == 1) {
                    break;
                }
            }

            m_setupdone = true;
        }

        // Encrypts a string
        _NODISCARD std::vector<number_t> encrypt(const string_view& _str, const std::tuple<uint32_t&, number_t&>& _public_key)
        {
            check_setup();

            const uint32_t& e = std::get<0>(_public_key);
            const number_t& n = std::get<1>(_public_key);

            auto pow_block = [e, n](const number_t& _block) noexcept -> number_t {
                return boost::multiprecision::powm(_block, e, n);
            };

            if (_str.empty()) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("Can`t Encrypt empty string");
                }
                return { };
            }

            const auto blocks = create_blocks(_str, n, m_bits);

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
        _NODISCARD string decrypt(const std::vector<number_t>& _encrypted) const
        {
            check_setup();

            auto decrypt_block = [this](const number_t& _block) noexcept -> string
            {
                const std::string decrypted = boost::multiprecision::powm(_block, d, n).convert_to<std::string>();

                string result; result.reserve(decrypted.size());

                for (uint32_t i = 0; decrypted.at(i) != '0' || i > decrypted.size();)
                {
                    uint8_t _len = decrypted.at(i++) - '0';

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

            string decrypted; decrypted.reserve(_encrypted.size() * char_size);

            for (auto& thread : threads) {
                decrypted.append(thread.get());
            }

            return decrypted;
        }

    private:
        // Check if setup() has been called
        void __forceinline check_setup() const
        {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you use the class");
            }
        } 

        // Creates the blocks for encryption
        _NODISCARD static std::vector<number_t> create_blocks(const string_view& _str, const number_t& _n, const uint32_t _keysize) noexcept
        {
            uint32_t blocksize = std::max(_keysize, MIN_BLOCKSIZE);

            std::vector<number_t> blocks;
            std::string msg; msg.reserve(blocksize * char_size);
            uint32_t i = 0;

            auto check_for_n = [_n, &blocksize](const number_t& _check) noexcept -> bool
            {
                if (_n < _check) {
                    blocksize -= one_in_block_size;
                    return false;
                }
                else {
                    return true;
                }
            };

            auto push_block = [&msg, &blocks, &i, check_for_n]() noexcept -> bool
            {
                number_t block(msg);

                const bool valid = check_for_n(block);

                if (valid) {
                    blocks.push_back(block);
                }
                else {
                    i = 0;
                    blocks.clear();
                }

                msg.clear();

                return valid;
            };

            while (true)
            {
                const std::string    num = std::to_string(_str.at(i));
                const bool      negative = (num.front() == '-');
                const uint32_t  nextsize = msg.size() + num.size() + static_cast<int32_t>(negative) + 1;

                if (nextsize > blocksize) {
                    msg.push_back('0');
                    if (!push_block())
                        continue;
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
                    if (!push_block()) {
                        continue;
                    }
                    else {
                        break;
                    }
                }
            }

            return blocks;
        }

        // Calculate the extended Euclidean algorithm
        _NODISCARD static number_t egcd(const number_t& a, const number_t& b, number_t& x, number_t& y) noexcept
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
        _NODISCARD static number_t inverse_mod(const number_t& e, const number_t& phi) noexcept
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, x, y) == 1) {
                return ((x % phi) + phi) % phi;
            }
            return 0;
        }

        // Generate random primelike string with X bits
        _NODISCARD static std::string random_primish_number(const uint32_t _bits) noexcept
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
        _NODISCARD static std::pair<number_t, number_t> generate_prime_pair(const uint32_t _bits, const uint32_t _trys) noexcept
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
            std::uniform_int_distribution<int32_t> dist(((_bits / 64) * -1) - 8, (_bits / 64) + 8);

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

        // Miller Rabin test num X with Y tests
        _NODISCARD static bool miller_rabin(const number_t& _num, const uint32_t _trys) noexcept
        {
            const number_t d_base = _num - 1;
            number_t d = d_base;

            uint32_t _divides = 0;
            while (d % 2 == 0) {
                d /= 2;
                ++_divides;
            }

            std::mt19937_64 mt(std::random_device{ }());
            boost::random::uniform_int_distribution<number_t> dist(2, _num - 2);

            for (uint32_t i = 0; i < _trys; ++i)
            {
                const number_t rand = dist(mt);
                number_t x = boost::multiprecision::powm(rand, d, _num);

                if (x == 1 || x == d_base)
                    continue;

                bool not_prime = true;

                for (uint32_t j = 1; j < _divides; ++j)
                {
                    x = (x * x) % _num;

                    if (x == d_base) {
                        not_prime = false;
                        break;
                    }
                }

                if (not_prime)
                    return false;
            }

            return true;
        }

        // Check if X number is prime with Y Miller Rabin tests
        _NODISCARD static bool is_prime(const number_t& _num, const uint32_t _trys) noexcept
        {
            if (_num == 2 || _num == 3)
                return true;

            if (_num <= 1 || _num % 2 == 0 || _num % 3 == 0)
                return false;

            for (uint64_t i = 5; i < 1000; i += 6) {
                if (_num % i == 0 || _num % (i + 2) == 0)
                    return false;
            }

            return miller_rabin(_num, _trys);
        }

        // Get a good amouth of Miller Rabin test for X bits
        _NODISCARD static uint32_t get_rabin_trys(const uint64_t _bits) noexcept
        {
            return static_cast<uint32_t>(2 + std::log2(std::sqrt(_bits)) * 3.0);
        }

        // Output for RSA class
        template<class _char, bool _throw, class ostream = std::ostream>
        friend ostream& operator<<(ostream& out, const basic_rsa<_char, _throw>& rsa)
        {
            out << "Charset  : " << typeid(_char).name() << '\n';
            out << "Keysize  : " << rsa.keysize() << '\n';
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
