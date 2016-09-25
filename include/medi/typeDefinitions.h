#pragma once

namespace medi {

  struct HandleBase;
  typedef void (*ReverseFunction)(HandleBase* h);
  typedef void (*ContinueFunction)(HandleBase* h);
  typedef void (*PreAdjointOperation)(void* adjoints, void* primals, int count);
  typedef void (*PostAdjointOperation)(void* adjoints, void* primals, void* rootPrimals, int count);

  struct HandleBase {
    ReverseFunction func;

    virtual ~HandleBase() {}
  };

  // structures for the passive types

  template <typename T>
  struct PairWithInt {
      T a;
      int b;
  };

  typedef PairWithInt<float> FloatIntPair;
  typedef PairWithInt<double> DoubleIntPair;
  typedef PairWithInt<long> LongIntPair;
  typedef PairWithInt<int> IntIntPair;
  typedef PairWithInt<short> ShortIntPair;
  typedef PairWithInt<long double> LongDoubleIntPair;

}
