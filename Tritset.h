#include <cmath>
#include <iostream>
#include <unordered_map>

const unsigned bitsInUnsigned = sizeof(unsigned) * 8;


enum trit { Unknown = 0, False = 2, True = 3}; //00 - unknown, 10 - false, 11 - true

class Tritset {
private:
	unsigned* controlHalfTritData;
	unsigned* valueHalfTritData;//??
	unsigned lastTritIdPlusOne;
	unsigned unsignedNumber;
	unsigned tritLimit;
public:
	class TritsetProxy {
	private:
		Tritset* set;
		unsigned id;
	public:
		TritsetProxy(Tritset& reference, unsigned index);
		void operator=(trit value);
		bool operator==(trit value);
	};
	Tritset();
	Tritset(unsigned number);
	~Tritset();
	unsigned capacity();
	Tritset::TritsetProxy operator[](unsigned index);
	trit getTrit(unsigned index);
	void setTrit(unsigned index, trit value);
	void shrink();
	Tritset operator&(Tritset& anotherTritset);
	Tritset operator|(Tritset& anotherTritset);
	Tritset operator!();
	void operator=(Tritset& anotherTritset);
	unsigned cardinality(trit value);
	std::unordered_map<trit, unsigned> cardinality();
	void trim(unsigned lastIndex);
	unsigned length();

	friend class TritsetProxy;
};

trit operator&(trit first, trit second);
trit operator|(trit first, trit second);
trit operator!(trit first);
bool operator==(trit first, trit second);