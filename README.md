# RSA-Encryption
RSA Class for C++

Note that it needs the boost lib (https://www.boost.org/).

Example:
```
int main()
{
    RSA::RSA rsa;
    
    rsa.gen();
    
    if (rsa)
    {
        const auto enc = rsa.encrypt("Hello World!", rsa.public_key);
        std::cout << rsa.decrypt(enc);
    }
}
```

Also you can Enable / Disable errors
```
RSA::RSA<true> // Enable Errors
RSA::RSA<false> // Disable Errors (Default)
```
