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
			std::cout << m_Name <<  " took: " << ms << "ms\n";
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

int main()
{
	constexpr std::string_view message = "Hello World!";
	
	RSA::RSA rsa;

	Benchmark(
		"Generation",
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
}
