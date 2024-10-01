#ifndef GSL_PLACEHOLDER_HPP
#define GSL_PLACEHOLDER_HPP
#include <stdint.h>

template <typename Dtype>
class FakeSpan {
public:

	FakeSpan (const Dtype * data,int nSamples, int vectorSize):data_(data),nSamples(nSamples),vectorSize(vectorSize)
	{}

	int getnSamples() const{
		return nSamples;
	}
	int getVectorSize() const{
		return vectorSize;
	}

	const Dtype * getData() const{
		return data_;
	}

	const Dtype * data() const{
		return data();
	}

	const Dtype * begin() const{
		return data_;
	}


	const Dtype * end() const{
			return data_ + size();
	}

	uint64_t size() const{
		return nSamples*((uint64_t) vectorSize);
	}

private:
	const Dtype * data_;
	int nSamples;
	int vectorSize;
};


#endif
