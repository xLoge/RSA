# RSA-Encryption

Note that it needs the boost library ( https://www.boost.org/ ).

* char - char16_t support
* automatic error handeling
* multithreaded

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

* Tested on I5 12400F (6 Cores 12 Threads)

```
128   BIT Key AVG of 100000 runs:   0.417ms
256   BIT Key AVG of 100000 runs:   0.998ms
512   BIT Key AVG of 100000 runs:   3.096ms
1024  BIT Key AVG of 10000  runs:   14.06ms
2048  BIT Key AVG of 1000   runs:   104.8ms
3072  BIT Key AVG of 1000   runs:   358.1ms
4096  BIT Key AVG of 1000   runs:   1004.ms
8192  BIT Key AVG of 10     runs:   14445ms 
16384 BIT Key AVG of 10     runs:  109905ms
```

