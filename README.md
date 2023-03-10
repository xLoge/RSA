# RSA-Encryption

* Support for char, wchar_t, char8_t, char16_t and char32_t
* Automatic error handeling
* Multithreaded

Example:
```
int main()
{
    RSA::RSA rsa;
    
    rsa.setup();
    
    if (rsa)
    {
        const auto enc = rsa.encrypt("Hello World!", rsa.public_key);
        std::cout << rsa.decrypt(enc);
    }
}
```

# Speed

* MPZ Version is as fast as Openssl
