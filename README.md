# RSA-Encryption

Note that it needs the boost library ( https://www.boost.org/ ).

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
RSA::RSA<true>      Errors Enabled
RSA::RSA<false>     Errors Disabled (Default)
```

#Speed

