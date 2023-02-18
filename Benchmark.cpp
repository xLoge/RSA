#include <iostream>
#include <custom/rsa.hpp>

class Timer
{
private:
	bool m_Out = true;
	const char* m_Name = "Timer";

public:

	std::chrono::time_point<std::chrono::steady_clock>
		start = std::chrono::high_resolution_clock::now(),
		end;
	std::chrono::duration<float> duration{};

	Timer() = default;

	Timer(const char* _Name) : m_Name(_Name) {}

	~Timer()
	{
		if (m_Out) {
			end = std::chrono::high_resolution_clock::now();
			duration = end - start;

			float ms = duration.count() * 1000.F;
			std::cout << m_Name << " took: " << ms << "ms\n";
		}
	}

	void toggle_output()
	{
		m_Out = !m_Out;
	}
};

template <class Time = std::chrono::milliseconds>
class Benchmark
{
public:

	template <class Fn, class... Args>
	explicit Benchmark(Fn&& _Func, Args&&... _Args) {
		Timer T;
		try
		{
			_Func(_Args...);
		}
		catch (const std::exception& ex)
		{
			std::cerr << "Benchmark throw: " << ex.what() << "\n";
			T.toggle_output();
		}
	}

	template <class Fn, class... Args>
	explicit Benchmark(const char* _Name, Fn&& _Func, Args&&... _Args) {
		Timer T(_Name);
		try
		{
			_Func(_Args...);
		}
		catch (const std::exception& ex)
		{
			std::cerr << _Name << " throw: " << ex.what() << "\n";
			T.toggle_output();
		}
	}
};

void test1()
{
	constexpr std::string_view message = "Hello World!";

	RSA::RSA rsa(256, 16);

	Benchmark(
		"Generation (256 BIT)",
		[&rsa]()
		{
			rsa.gen();
		}
	);

	std::cout << '\n';

	if (rsa)
	{
		std::vector<number_t> enc;

		Benchmark(
			"Encryption",
			[&rsa, &enc, message]()
			{
				enc = rsa.encrypt(message, rsa.public_key);

		std::cout << "Encypted: " << enc << '\n';
			}
		);

		std::cout << '\n';

		Benchmark(
			"Decryption",
			[&rsa, &enc]()
			{
				std::cout << "Decypted: " << rsa.decrypt(enc) << '\n';
			}
		);
	}

	std::cout << '\n';
}

void test2()
{
	Benchmark(
		"Generation of Key (128  BIT)",
		[]()
		{
			RSA::RSA rsa(128, 16);
	rsa.gen();
		}
	);


	Benchmark(
		"Generation of Key (256  BIT)",
		[]()
		{
			RSA::RSA rsa(256, 16);
	rsa.gen();
		}
	);

	Benchmark(
		"Generation of Key (512  BIT)",
		[]()
		{
			RSA::RSA rsa(512, 16);
	rsa.gen();
		}
	);

	Benchmark(
		"Generation of Key (1024 BIT)",
		[]()
		{
			RSA::RSA rsa(1024, 16);
	rsa.gen();
		}
	);

	Benchmark(
		"Generation of Key (2048 BIT)",
		[]()
		{
			RSA::RSA rsa(2048, 16);
	rsa.gen();
		}
	);

	Benchmark(
		"Generation of Key (3072 BIT)",
		[]()
		{
			RSA::RSA rsa(3072, 16);
			rsa.gen();
		}
	);

	Benchmark(
		"Generation of Key (4096 BIT)",
		[]()
		{
			RSA::RSA rsa(4096, 16);
	rsa.gen();
		}
	);

	Benchmark(
		"Generation of Key (8192 BIT)",
		[]()
		{
			RSA::RSA rsa(8192, 16);
	rsa.gen();
		}
	);
}

int main()
{
	test1();
	test2();

	std::cout << "\nNote that these results are RANDOM in therms of time (8192 Bits can take 4 - 20 Seconds)\n";

	std::cin.get();
}
