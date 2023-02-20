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

* 128  BIT Key took: 0.3ms - 0.9ms
* 256  BIT Key took: 0.4ms - 1.2ms
* 512  BIT Key took: 1.2ms - 4.0ms
* 1024 BIT Key took: 6.0ms - 12.0ms
* 2048 BIT Key took: 60.0ms - 200.0ms
* 3072 BIT Key took: 100.0ms - 700.0ms
* 4096 BIT Key took: 300.0ms - 4000.0ms
* 8192 BIT Key took: 2500.0ms - 11000.0ms
* 16384 BIT Key took: 6000.0ms - 60000.0ms
