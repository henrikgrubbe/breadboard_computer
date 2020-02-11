#ifndef __IDATA__
#define __IDATA__

class IData {
	public:
		virtual uint8_t getData(int address) = 0;
};

#endif