#ifndef TEST_UNITIGGING_TEST_
#define TEST_UNITIGGING_TEST_

#include <overlap/read.h>
#include <layout/unitigging.h>

#include <vector>

namespace test {

class UnitiggingTest {
 public:
  UnitiggingTest();
  virtual ~UnitiggingTest();
  virtual bool run()=0;
  overlap::Read* makeRead(const char* data);
};

class UnitiggingIsTransitiveTest : public UnitiggingTest {
 public:
  UnitiggingIsTransitiveTest();
  virtual ~UnitiggingIsTransitiveTest();
  virtual bool run();
};

class UnitiggingContainmentTest : public UnitiggingTest {
 public:
  UnitiggingContainmentTest();
  virtual ~UnitiggingContainmentTest();
  virtual bool run();
};

class UnitiggingTransitiveTest : public UnitiggingTest {
 public:
  UnitiggingTransitiveTest();
  virtual ~UnitiggingTransitiveTest();
  virtual bool run();
};

class UnitiggingContigTest : public UnitiggingTest {
 public:
  UnitiggingContigTest();
  virtual ~UnitiggingContigTest();
  virtual bool run();
};

class UnitiggingTestRunner {
 public:
  UnitiggingTestRunner();
  virtual ~UnitiggingTestRunner();

  void run();
  void addTest(UnitiggingTest* test);

 private:
  std::vector< UnitiggingTest* > tests_;
};

};  // namespace test

#endif  // TEST_UNITIGGING_TEST_
