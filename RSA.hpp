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
        class InstructionSet
        {
        public:
            InstructionSet() {
                std::array<int, 4> cpui{};

                __cpuid(cpui.data(), 0);
                nIds_ = cpui[0];

                for (int i = 0; i <= nIds_; ++i)
                {
                    __cpuidex(cpui.data(), i, 0);
                    data_.push_back(cpui);
                }

                char vendor[32];
                memset(vendor, 0, sizeof(vendor));
                *reinterpret_cast<int*>(vendor) = data_[0][1];
                *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
                *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
                vendor_ = vendor;

                if (vendor_ == "GenuineIntel") {
                    isIntel_ = true;
                }
                else if (vendor_ == "AuthenticAMD") {
                    isAMD_ = true;
                }

                if (nIds_ >= 1) {
                    f_1_ECX_ = data_[1][2];
                    f_1_EDX_ = data_[1][3];
                }

                if (nIds_ >= 7) {
                    f_7_EBX_ = data_[7][1];
                    f_7_ECX_ = data_[7][2];
                }

                __cpuid(cpui.data(), 0x80000000);
                nExIds_ = cpui[0];

                char brand[0x40];
                memset(brand, 0, sizeof(brand));

                for (int i = 0x80000000; i <= nExIds_; ++i)
                {
                    __cpuidex(cpui.data(), i, 0);
                    extdata_.push_back(cpui);
                }

                // load bitset with flags for function 0x80000001
                if (nExIds_ >= 0x80000001)
                {
                    f_81_ECX_ = extdata_[1][2];
                    f_81_EDX_ = extdata_[1][3];
                }

                // Interpret CPU brand string if reported
                if (nExIds_ >= 0x80000004)
                {
                    memcpy(brand, extdata_[2].data(), sizeof(cpui));
                    memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
                    memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
                    brand_ = brand;
                }
            };

            int nIds_ = 0;
            int nExIds_ = 0;
            std::string vendor_;
            std::string brand_;
            bool isIntel_ = 0;
            bool isAMD_ = 0;
            std::bitset<32> f_1_ECX_;
            std::bitset<32> f_1_EDX_;
            std::bitset<32> f_7_EBX_;
            std::bitset<32> f_7_ECX_;
            std::bitset<32> f_81_ECX_;
            std::bitset<32> f_81_EDX_;
            std::vector<std::array<int, 4>> data_;
            std::vector<std::array<int, 4>> extdata_;
        };

        // * Decimal to number
        template <class Ty = long long>
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
                    throw std::invalid_argument("only numeric strings");

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
        template <class Ty = long long>
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
                    throw("");
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
            static constexpr std::string encode(const std::string data) {
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

            static constexpr std::string decode(const std::string& input) {
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

        namespace random
        {
            static const InstructionSet _cpu_instructions;
            inline const bool _has_RDRAND = _cpu_instructions.f_1_ECX_[30];
            inline const bool _has_RDSEED = _cpu_instructions.f_7_EBX_[18];

#ifdef _M_X64
            using engine = std::mt19937_64;
#else 
            using engine = std::mt19937;
#endif

#ifdef _M_X64
            static size_t _fast_rand64() {
                if (_has_RDRAND) {
                    size_t _out;
                    while (!_rdrand64_step(&_out));
                    return _out;
                }
                else {
                    std::mt19937_64 mt(std::random_device{}());
                    std::uniform_int_distribution<size_t> dist(0, -1);
                    return dist(mt);
                }
            }

            static size_t _rand64() {
                if (_has_RDSEED) {
                    size_t _out;
                    while (!_rdseed64_step(&_out));
                    return _out;
                }
                else {
                    return _fast_rand64();
                }
            }
#endif

            static uint32_t _fast_rand32() {
                if (_has_RDRAND) {
                    uint32_t _out;
                    while (!_rdrand32_step(&_out));
                    return _out;
                }
                else {
                    std::mt19937 mt(std::random_device{}());
                    std::uniform_int_distribution<uint32_t> dist(0, -1);
                    return dist(mt);
                }
            }

            static uint32_t _rand32() {
                if (_has_RDSEED) {
                    uint32_t _out;
                    while (!_rdseed32_step(&_out));
                    return _out;
                }
                else {
                    return _fast_rand32();
                }
            }

            static size_t rand() noexcept {
#ifdef _M_X64
                return _rand64();
#else 
                return _rand32();
#endif
            }

            static uint32_t rand32() noexcept {
                return _rand32();
            }

#ifdef _M_X64
            static size_t rand64() noexcept {
                return _rand64();
            }
#endif

            static size_t fast_rand() noexcept {
#ifdef _M_X64
                return _fast_rand64();
#else 
                return _fast_rand32();
#endif
            }

            static uint32_t fast_rand32() noexcept {
                return _fast_rand32();
            }

#ifdef _M_X64
            static size_t fast_rand64() noexcept {
                return _fast_rand64();
            }
#endif

            static size_t rand_in_range(size_t _MIN, size_t _MAX) noexcept {
                engine mt(_rand32());
                std::uniform_int_distribution<size_t> dist(_MIN, _MAX);
                return dist(mt);
            }

            static uint32_t rand_in_range32(uint32_t _MIN, uint32_t _MAX) noexcept {
                engine mt(_rand32());
                std::uniform_int_distribution<uint32_t> dist(_MIN, _MAX);
                return dist(mt);
            }

#ifdef _M_X64
            static size_t rand_in_range64(size_t _MIN, size_t _MAX) noexcept {
                engine mt(_rand32());
                std::uniform_int_distribution<size_t> dist(_MIN, _MAX);
                return dist(mt);
            }
#endif

            static size_t fast_rand_in_range(size_t _MIN, size_t _MAX) noexcept {
                engine mt(_fast_rand32());
                std::uniform_int_distribution<size_t> dist(_MIN, _MAX);
                return dist(mt);
            }

            static size_t fast_rand_in_range32(uint32_t _MIN, uint32_t _MAX) noexcept {
                engine mt(_fast_rand32());
                std::uniform_int_distribution<uint32_t> dist(_MIN, _MAX);
                return dist(mt);
            }

#ifdef _M_X64
            static size_t fast_rand_in_range64(size_t _MIN, size_t _MAX) noexcept {
                engine mt(_fast_rand32());
                std::uniform_int_distribution<size_t> dist(_MIN, _MAX);
                return dist(mt);
            }
#endif

            static double rand_double(double _MIN = 0., double _MAX = std::numeric_limits<double>::max())
            {
                return (static_cast<double>(rand_in_range(0, std::numeric_limits<uint32_t>::max())) / static_cast<double>(std::numeric_limits<uint32_t>::max())) * (_MAX - _MIN);
            }

            static double fast_rand_double(double _MIN = 0., double _MAX = std::numeric_limits<double>::max())
            {
                return (static_cast<double>(fast_rand_in_range(0, std::numeric_limits<uint32_t>::max())) / static_cast<double>(std::numeric_limits<uint32_t>::max())) * (_MAX - _MIN);
            }

            static float rand_float(float _MIN = 0.F, float _MAX = std::numeric_limits<float>::max())
            {
                return (static_cast<float>(rand_double(_MIN, _MAX)));
            }

            static float fast_rand_float(float _MIN = 0.F, float _MAX = std::numeric_limits<float>::max())
            {
                return (static_cast<float>(fast_rand_double(_MIN, _MAX)));
            }
        }
    }

    template<class _char>
    constexpr uint32_t blocksize_for(const _char* const str, const size_t count) noexcept {
        auto _val = static_cast<uint32_t>(count * 8);
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
    
    template <class char_type = char, bool throw_errors = false> 
    class basic_rsa
    {
    protected:
        constexpr static inline uint32_t DEFAULT_BITS = 3072;
        constexpr static inline uint32_t DEFAULT_TRYS = -1;
        constexpr static inline uint32_t DEFAULT_BLOCKSIZE = -1;
    
    private:
        constexpr static inline size_t char_size = sizeof(char_type);
        const static inline uint32_t thread_count = std::thread::hardware_concurrency();

        typedef std::tuple<number_t&, number_t&, number_t&, number_t&, uint32_t&, uint32_t&, uint32_t&> _export;
        using string = std::basic_string<char_type>;
        using string_view = std::basic_string_view<char_type>;

    private:
        bool m_setupdone = false;
        uint32_t m_blocksize = DEFAULT_BLOCKSIZE;
        uint32_t m_bits = DEFAULT_BITS;

    public:
        uint32_t e = 0;
        number_t p, q, n, d;

        std::tuple<uint32_t&, number_t&> public_key { e, n };
        std::tuple<number_t&, number_t&> private_key{ d, n };

    public:
        constexpr basic_rsa() noexcept {
            m_bits = DEFAULT_BITS;
            m_blocksize = 256;
        }

        constexpr basic_rsa(const uint32_t _bits, const uint32_t _blocksize = DEFAULT_BLOCKSIZE) {
            set(_bits, _blocksize);
        }

        constexpr basic_rsa(const std::string_view& _file) {
            import_file(_file);
        }

        constexpr basic_rsa(std::ifstream& _file) {
            import_file(_file);
        }

        constexpr basic_rsa(const _export& _key) {
            import_key(_key);
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
                    std::string _data = detail::base64::decode(_line);

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
                    e = detail::dton<uint32_t>(_data.substr(0, _find).data());
                    _data = _data.substr(_find + 1);

                    _find = _data.find(',');
                    m_bits = detail::dton<uint32_t>(_data.substr(0, _find).data());
                    _data = _data.substr(_find + 1);

                    m_blocksize = detail::dton<uint32_t>(_data.data());
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
            _out.assign(detail::base64::encode(_out));

            _file << "-----BEGIN EXPORT-----\n";
            _file << _out;
            _file << "\n-----ENDOF EXPORT-----\n";
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

        void setup(uint32_t _trys = DEFAULT_TRYS) noexcept
        {
            if (_trys == DEFAULT_TRYS) {
                _trys = get_rabin_trys();
            }

            std::mt19937_64 mt(detail::random::rand());
            std::uniform_int_distribution<uint32_t> dist(0, m_bits / 8);

            m_setupdone = false;
            e = 65537;
            p = 0;
            q = 0;
            n = 0;
            d = 0;

            while (true)
            {
                uint32_t _rand1 = dist(mt), _rand2 = dist(mt);

                while (_rand1 % 8 || _rand2 % 8) {
                    if (_rand1 % 8) { ++_rand1; }
                    if (_rand2 % 8) { ++_rand2; }
                }

                // generate p and q
                if (thread_count >= 2) {
                    std::future<number_t> pf = std::async(&basic_rsa::random_prime, this, m_bits + _rand1, _trys);
                    std::future<number_t> qf = std::async(&basic_rsa::random_prime, this, m_bits + _rand2, _trys);
                    p = pf.get();
                    q = qf.get();
                }
                else {
                    p = random_prime(m_bits, _trys);
                    q = random_prime(m_bits, _trys);
                }

                n = p * q;

                const number_t phi = (p - 1) * (q - 1);

                if (boost::multiprecision::gcd(e, phi) != 1) {
                    uint32_t _e = 2;
                    while (boost::multiprecision::gcd(_e, phi) != 1) { ++_e; }
                    e = _e;
                }

                d = inverse_mod(e, phi);

                if ((e * d) % phi == 1) {
                    break;
                }
            }
            
            m_setupdone = true;
        }

        std::vector<number_t> encrypt(const string_view& str, const std::tuple<uint32_t&, number_t&>& public_key) const
        {
            auto async_pow = [public_key](const number_t& num) noexcept -> number_t {
                return boost::multiprecision::powm(num, std::get<0>(public_key), std::get<1>(public_key));
            };
            
            check_setup();

            if (str.empty()) {
                return { };
            }

            std::vector<number_t> block_vector(create_block_vector(str));
            std::vector<number_t> result      (block_vector.size());
            std::vector<std::future<number_t>> threads(block_vector.size());

            for (size_t i = 0; i != block_vector.size(); ++i) {
                threads[i] = std::async(std::launch::async, async_pow, block_vector[i]);
            }

            for (size_t i = 0; i != block_vector.size(); ++i) {
                result[i] = threads[i].get();
            }

            return result;
        }

        string decrypt(const std::vector<number_t>& message) const
        {
            auto async_decrypt = [this](const number_t& _encrypted) noexcept -> string
            {
                const std::string decrypted = boost::multiprecision::powm(_encrypted, d, n).convert_to<std::string>();

                string result;
                result.reserve(m_blocksize * 2); 

                for (size_t i = 0; decrypted[i] != '0' || i > m_blocksize;) {
                    size_t _len = decrypted[i++] - '0';
                    if (decrypted[i] == '0' && _len > 1) {
                        result.push_back(-static_cast<char_type>(detail::dton<long long>(decrypted.substr(++i, --_len).c_str())));
                    }
                    else {
                        result.push_back(static_cast<char_type>(detail::dton<size_t>(decrypted.substr(i, _len).c_str())));
                    }
                    i += _len;
                }

                return result;
            };

            check_setup();
            
            std::vector<std::future<string>> threads(message.size());

            for (size_t i = 0; i != message.size(); ++i) {
                threads[i] = std::async(std::launch::async, async_decrypt, message[i]);
            }

            string decrypted;
            decrypted.reserve(message.size() * m_blocksize * char_size);

            for (std::future<string>& thread : threads) {
                decrypted.append(thread.get());
            }

            return decrypted;
        }

    private:
        void check_setup() const
        {
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
                const std::string _num(std::to_string(str[i]));
                const bool _negative = (_num.front() == '-');
                const size_t _nextsize = msg.size() + _num.size() + 1 + _negative;

                if (_nextsize > m_blocksize) {
                    msg.push_back('0');
                    blocks.push_back(msg);
                    msg.clear();
                }
                
                msg.append(std::to_string(_num.size()));

                if (_negative) {
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
            static std::mt19937_64 gen(detail::random::rand());
            std::uniform_int_distribution<int> dist('0', '9');

            const uint32_t bytes = _bits >> 3;

            std::string str(bytes, 0);

            for (char& c : str) {
                c = dist(gen);
            }

            while (str[0] == '0') {
                str[0] = dist(gen);
            }

            return number_t(str.data());
        }

        number_t random_possible_prime(const uint32_t _bits) const noexcept
        {
            auto small_test = [_bits](const number_t& num) noexcept -> bool
            {
                for (uint32_t i = 2; i != 10000; ++i) {
                    if (num % i == 0) {
                        if (num != i) {
                            return false;
                        }
                        else {
                            return true;
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
                
                for (std::future<void>& thread : threads) {
                    thread = std::async(std::launch::async, search_thread);
                }

                return result;
            }
            else {
                number_t num(random_possible_prime(_bits));
                
                while (is_prime(num, _trys) == false) {
                    num = random_possible_prime(_bits);
                }
                
                return num;
            }
        }

        uint32_t get_rabin_trys() const noexcept
        {
            return static_cast<uint32_t>(std::log(std::pow(m_bits, 3)));
        }

        bool is_prime(const number_t& n, const uint32_t _trys) const noexcept
        {
            const number_t d_base = n - 1;
            number_t d = d_base;

            size_t s = 0;
            while (d % 2 == 0) {
                d /= 2;
                ++s;
            }

            std::mt19937_64 mt(detail::random::rand());
            boost::random::uniform_int_distribution<number_t> dist(2, n - 2);

            for (size_t i = 0; i < _trys; ++i)
            {
                number_t x = boost::multiprecision::powm(dist(mt), d, n);
                
                if (x == 1 || x == d_base) {
                    continue;
                }
                
                bool _continue = true;
                
                for (size_t j = 1; j < s; ++j)
                {
                    x = (x * x) % n;

                    if (x == d_base) {
                        _continue = false;
                        break;
                    }
                }

                if (_continue) {
                    return false;
                }
            }
            
            return true;
        }
    };
}

template<class Ty, class ostream>
constexpr ostream& operator<<(ostream& out, const std::vector<Ty>& vec)
{
    for (size_t i = 0, end = vec.size() - 1; i < end; ++i) {
        out << vec[i] << ", ";
    }
    return out << vec.back();
}

template<class _char, bool _throw, class ostream>
constexpr ostream& operator<<(ostream& out, const RSA::basic_rsa<_char, _throw>& rsa)
{
    out << "Charset  : " << typeid(_char).name() << '\n';
    out << "Keysize  : " << rsa.keysize() << '\n';
    out << "Blocksize: " << rsa.blocksize() << '\n';
    out << "Setupdone: " << (rsa.setupdone() == true ? "true" : "false") << "\n\n";
    out << "P: " << rsa.p << "\n\n";
    out << "Q: " << rsa.q << "\n\n";
    out << "N: " << rsa.n << "\n\n";
    out << "E: " << rsa.e << "\n\n"; 
    out << "D: " << rsa.d << "\n\n";
    return out;
}

#endif
