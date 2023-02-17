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
