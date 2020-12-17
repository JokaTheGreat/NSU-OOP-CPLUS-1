#include "Tritset.h"

Tritset::TritsetProxy::TritsetProxy(Tritset& reference, unsigned index) {
	set = &reference;
	id = index;
}

void Tritset::TritsetProxy::operator=(trit value) {
	set->setTrit(id, value);
}

bool Tritset::TritsetProxy::operator==(trit value) {
	if (id >= set->tritLimit) {
		if (value == Unknown) return true;
		return false;
	}
	return set->getTrit(id) == value;
}

Tritset::Tritset() {
	controlHalfTritData = nullptr;
	valueHalfTritData = nullptr;
	lastTritIdPlusOne = 0;
	unsignedNumber = 0;
	tritLimit = 0;
}

Tritset::Tritset(unsigned number) {
	if (number < 0) return;

	lastTritIdPlusOne = 0;
	unsignedNumber = static_cast<unsigned>(ceil(static_cast<double>(number) / bitsInUnsigned));
	tritLimit = unsignedNumber * bitsInUnsigned;

	if (number > 0) {
		controlHalfTritData = new unsigned[unsignedNumber]();
		valueHalfTritData = new unsigned[unsignedNumber]();//"выдел€ю пам€ть м м м"
	}
	else {
		controlHalfTritData = nullptr;
		valueHalfTritData = nullptr;
	}
}

Tritset::~Tritset() {
	delete[] controlHalfTritData;
	delete[] valueHalfTritData;
}

unsigned Tritset::capacity() {
	return 2 * unsignedNumber;
}

Tritset::TritsetProxy Tritset::operator[](unsigned index) {
	return TritsetProxy(*this, index);
}

trit Tritset::getTrit(unsigned index) {
	if (index >= tritLimit) return Unknown;

	unsigned unsignedIndex = static_cast<unsigned>(ceil(static_cast<double>(index + 1) / bitsInUnsigned));
	unsigned tritIndex = index % bitsInUnsigned;

	unsigned mask = 0;
	mask |= controlHalfTritData[unsignedIndex];
	mask >>= tritIndex;
	if (mask % 2 == 0) return Unknown;

	mask = 0;
	mask |= valueHalfTritData[unsignedIndex];
	mask >>= tritIndex;
	if (mask % 2 == 0) return False;
	return True;
}

void Tritset::setTrit(unsigned index, trit value) {
	if (index >= tritLimit) {
		if (value == Unknown) {
			return;
		}
		//выделить новую пам€ть
		unsigned tempUnsignedNumber = static_cast<unsigned>(ceil(static_cast<double>(index + 1) / bitsInUnsigned));
		unsigned* temp1 = new unsigned[tempUnsignedNumber](), *temp2 = new unsigned[tempUnsignedNumber]();
		memcpy(temp1, controlHalfTritData, sizeof(unsigned*) * unsignedNumber);//"выдел€ю пам€ть м м м"
		memcpy(temp2, valueHalfTritData, sizeof(unsigned*) * unsignedNumber);
		delete[] controlHalfTritData;
		delete[] valueHalfTritData;
		controlHalfTritData = temp1;
		valueHalfTritData = temp2;

		unsignedNumber = tempUnsignedNumber;
		tritLimit = unsignedNumber * bitsInUnsigned;
		lastTritIdPlusOne = index + 1;
	}

	unsigned unsignedIndex = static_cast<unsigned>(floor(static_cast<double>(index) / bitsInUnsigned));
	unsigned tritIndex = index % bitsInUnsigned;

	unsigned oneMask = 1; //&= zeroMask устанавливает ноль, |= oneMask - единичку
	oneMask <<= tritIndex;
	unsigned zeroMask = UINT_MAX, tempMask = 1;
	tempMask <<= tritIndex;
	zeroMask ^= tempMask;

	if (value != Unknown) {
		controlHalfTritData[unsignedIndex] |= oneMask;
		if (value == True) valueHalfTritData[unsignedIndex] |= oneMask;
		else valueHalfTritData[unsignedIndex] &= zeroMask;

		if (index >= lastTritIdPlusOne) lastTritIdPlusOne = index + 1;
	}
	else {
		controlHalfTritData[unsignedIndex] &= zeroMask;
		valueHalfTritData[unsignedIndex] &= zeroMask;

		if (index == lastTritIdPlusOne - 1) {
			unsigned i = 0;

			for (i; (i <= unsignedIndex) && (controlHalfTritData[unsignedIndex - i] == 0); i++) {
			
			}
			if (i <= unsignedIndex) {
				unsigned temp = controlHalfTritData[unsignedIndex - i];
				lastTritIdPlusOne = (unsignedIndex - i) * bitsInUnsigned;

				for (i = 0; temp % 2 == 0; i++) temp >>= 1;
				lastTritIdPlusOne += i + 1;
			}
			else lastTritIdPlusOne = 0;
		}
	}
}

void Tritset::shrink() {
	if (lastTritIdPlusOne == 0) {
		unsignedNumber = 0;
		tritLimit = 0;
		delete[] controlHalfTritData;
		delete[] valueHalfTritData;
		controlHalfTritData = nullptr;
		valueHalfTritData = nullptr;
		return;
	}

	unsigned limitId = static_cast<unsigned>(ceil(static_cast<double>(lastTritIdPlusOne) / bitsInUnsigned));
	unsigned* temp1 = new unsigned[limitId](), * temp2 = new unsigned[limitId]();//"выдел€ю пам€ть м м м"
	memcpy(temp1, controlHalfTritData, sizeof(unsigned*) * limitId);
	memcpy(temp2, valueHalfTritData, sizeof(unsigned*) * limitId);
	delete[] controlHalfTritData;
	delete[] valueHalfTritData;
	controlHalfTritData = temp1;
	valueHalfTritData = temp2;
	
	unsignedNumber = limitId;
	tritLimit = unsignedNumber * bitsInUnsigned;
}

Tritset Tritset::operator&(Tritset& anotherTritset) {
	unsigned tempUnsignedNumber, tempLastTritIdPlusOne = 0;
	tempUnsignedNumber = unsignedNumber > anotherTritset.unsignedNumber ? unsignedNumber : anotherTritset.unsignedNumber;
	Tritset tempTritSet(tempUnsignedNumber * bitsInUnsigned);

	unsigned bothSettedTrits = lastTritIdPlusOne < anotherTritset.lastTritIdPlusOne ? lastTritIdPlusOne : anotherTritset.lastTritIdPlusOne,
		i = 0;
	trit tempTrit;

	for (i; i < bothSettedTrits; i++) {
		tempTrit = this->getTrit(i) & anotherTritset.getTrit(i);
		tempTritSet.setTrit(i, tempTrit);

		if (tempTrit != Unknown && (i + 1) > tempLastTritIdPlusOne) tempLastTritIdPlusOne = i + 1;
	}
	if (lastTritIdPlusOne > anotherTritset.lastTritIdPlusOne) {
		for (i; i < lastTritIdPlusOne; i++) {
			tempTrit = this->getTrit(i) & Unknown;
			tempTritSet.setTrit(i, tempTrit);

			if (tempTrit != Unknown && (i + 1) > tempLastTritIdPlusOne) tempLastTritIdPlusOne = i + 1;
		}
	}
	else if (lastTritIdPlusOne < anotherTritset.lastTritIdPlusOne) {
		for (i; i < lastTritIdPlusOne; i++) {
			tempTrit = Unknown & anotherTritset.getTrit(i);
			tempTritSet.setTrit(i, tempTrit);

			if (tempTrit != Unknown && (i + 1) > tempLastTritIdPlusOne) tempLastTritIdPlusOne = i + 1;
		}
	}

	tempTritSet.lastTritIdPlusOne = tempLastTritIdPlusOne;
	return tempTritSet;
}

trit operator&(trit first, trit second) {
	if ((first == False) || (second == False)) return False;
	if ((first == Unknown) || (second == Unknown)) return Unknown;
	return True;
}

Tritset Tritset::operator|(Tritset& anotherTritset) {
	unsigned tempUnsignedNumber, tempLastTritIdPlusOne = 0;
	tempUnsignedNumber = unsignedNumber > anotherTritset.unsignedNumber ? unsignedNumber : anotherTritset.unsignedNumber;
	Tritset tempTritSet(tempUnsignedNumber * bitsInUnsigned);

	unsigned bothSettedTrits = lastTritIdPlusOne < anotherTritset.lastTritIdPlusOne ? lastTritIdPlusOne : anotherTritset.lastTritIdPlusOne,
		i = 0;
	trit tempTrit;

	for (i; i < bothSettedTrits; i++) {
		tempTrit = this->getTrit(i) | anotherTritset.getTrit(i);
		tempTritSet.setTrit(i, tempTrit);

		if (tempTrit != Unknown && (i + 1) > tempLastTritIdPlusOne) tempLastTritIdPlusOne = i + 1;
	}
	if (lastTritIdPlusOne > anotherTritset.lastTritIdPlusOne) {
		for (i; i < lastTritIdPlusOne; i++) {
			tempTrit = this->getTrit(i) | Unknown;
			tempTritSet.setTrit(i, tempTrit);

			if (tempTrit != Unknown && (i + 1) > tempLastTritIdPlusOne) tempLastTritIdPlusOne = i + 1;
		}
	}
	else if (lastTritIdPlusOne < anotherTritset.lastTritIdPlusOne) {
		for (i; i < lastTritIdPlusOne; i++) {
			tempTrit = Unknown | anotherTritset.getTrit(i);
			tempTritSet.setTrit(i, tempTrit);

			if (tempTrit != Unknown && (i + 1) > tempLastTritIdPlusOne) tempLastTritIdPlusOne = i + 1;
		}
	}

	tempTritSet.lastTritIdPlusOne = tempLastTritIdPlusOne;
	return tempTritSet;
}

trit operator|(trit first, trit second) {
	if ((first == True) || (second == True)) return True;
	if ((first == Unknown) || (second == Unknown)) return Unknown;
	return False;
}

Tritset Tritset::operator!() {
	Tritset tempTritSet(unsignedNumber * bitsInUnsigned);
	tempTritSet.lastTritIdPlusOne = lastTritIdPlusOne;
	for (unsigned i = 0; i < lastTritIdPlusOne; i++) tempTritSet.setTrit(i, !this->getTrit(i));

	return tempTritSet;
}

trit operator!(trit first) {
	if (first == True) return False;
	if (first == False) return True;
	return Unknown;
}

void Tritset::operator=(Tritset& anotherTritset) {
	if (this == &anotherTritset) return;

	unsignedNumber = anotherTritset.unsignedNumber;
	tritLimit = anotherTritset.tritLimit;
	lastTritIdPlusOne = anotherTritset.lastTritIdPlusOne;
	delete[] controlHalfTritData;
	delete[] valueHalfTritData;
	controlHalfTritData = new unsigned[unsignedNumber](), valueHalfTritData = new unsigned[unsignedNumber]();//"выдел€ю пам€ть м м м"
	memcpy(controlHalfTritData, anotherTritset.controlHalfTritData, sizeof(unsigned*) * unsignedNumber);
	memcpy(valueHalfTritData, anotherTritset.valueHalfTritData, sizeof(unsigned*) * unsignedNumber);
}

unsigned Tritset::cardinality(trit value) {
	unsigned counter = 0;

	for (unsigned i = 0; i < tritLimit; i++) {
		if (value == this->getTrit(i)) counter++;
	}
	return counter;
}

std::unordered_map<trit, unsigned> Tritset::cardinality() {
	std::unordered_map<trit, unsigned> map;
	unsigned counter[4] = { 0 };

	for (unsigned i = 0; i < tritLimit; i++) counter[static_cast<unsigned>(this->getTrit(i))]++;

	map.insert({Unknown, counter[0]});
	map.insert({False, counter[2]});
	map.insert({True, counter[3]});
	return map;
}

void Tritset::trim(unsigned lastIndex) {
	unsignedNumber = static_cast<unsigned>(ceil(static_cast<double>(lastIndex + 1) / bitsInUnsigned));
	tritLimit = unsignedNumber * bitsInUnsigned;

	unsigned* temp1 = new unsigned[unsignedNumber](), * temp2 = new unsigned[unsignedNumber](),//"выдел€ю пам€ть м м м"
		tritsInLastUnsigned = lastIndex % bitsInUnsigned + 1,
		i = 0, lastUnsignedIndex = unsignedNumber - 1;
	memcpy(temp1, controlHalfTritData, sizeof(unsigned*) * unsignedNumber);
	memcpy(temp2, valueHalfTritData, sizeof(unsigned*) * unsignedNumber);
	delete[] controlHalfTritData;
	delete[] valueHalfTritData;
	controlHalfTritData = temp1;
	valueHalfTritData = temp2;

	controlHalfTritData[lastUnsignedIndex] <<= (bitsInUnsigned - tritsInLastUnsigned);
	controlHalfTritData[lastUnsignedIndex] >>= (bitsInUnsigned - tritsInLastUnsigned);
	valueHalfTritData[lastUnsignedIndex] <<= (bitsInUnsigned - tritsInLastUnsigned);
	valueHalfTritData[lastUnsignedIndex] >>= (bitsInUnsigned - tritsInLastUnsigned);

	for (i = 0; i <= lastUnsignedIndex && controlHalfTritData[lastUnsignedIndex - i] == 0; i++);
	if (i <= lastUnsignedIndex) {
		unsigned temp = controlHalfTritData[lastUnsignedIndex - i];
		lastTritIdPlusOne = (lastUnsignedIndex - i) * bitsInUnsigned;

		for (i = 0; temp % 2 == 0; i++) temp >>= 1;
		lastTritIdPlusOne += i + 1;
	}
	else lastTritIdPlusOne = 0;
}

unsigned Tritset::length() {
	return lastTritIdPlusOne;
}

bool operator==(trit first, trit second) {
	return (static_cast<unsigned>(first) == static_cast<unsigned>(second));
}