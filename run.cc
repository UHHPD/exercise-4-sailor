#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <fstream>
#include <memory>

#include "Data.hh"

// Generic function comparing two values of some type T used later for int and
// double
template <class T>
bool testEqual(const std::string& name, T expected, T real) {
  if (expected != real) {
    std::cout << "(" << name << ": <" << expected << ">"
              << ", " << real << ") ";
    return false;
  }
  return true;
}

bool testReadingSize() {
  std::cout << "testReadingSize...";
  Data datA("testA");
  return testEqual<int>("size", 1, datA.size());
}

bool testReadingMeasurement() {
  std::cout << "testReadingMeasurement...";
  Data datA("testA");
  return testEqual("measurement", 10., datA.measurement(0));
}

bool testReadingBinEdges() {
  std::cout << "testReadingBinEdges...";
  Data datA("testA");
  return testEqual("bin low", 0., datA.binLow(0)) &&
         testEqual("bin high", 1., datA.binHigh(0));
}

bool testReadingErrors() {
  std::cout << "testReadingErrors...";
  Data datA("testA");
  return testEqual("error", 2., datA.error(0));
}

bool testCopyConstructor() {
  std::cout << "testCopyConstructor...";
  Data a("testA");
  Data b(a);
  Data c = a;  // equivalent to Data c(a)
  return testEqual("measurement", 10., b.measurement(0)) &&
         testEqual("measurement", 10., c.measurement(0));
}

bool testCompatibilityFunction() {
  std::cout << "testCompatibilityFunction...";
  Data datA("testA");
  Data datB("testB");
  return testEqual("compatibility", 1, datA.checkCompatibility(datB, 1)) &&
         testEqual("compatibility", 1, datB.checkCompatibility(datA, 1));
}

void runTests() {
  std::cout << "Running tests...\n";
  std::vector<std::function<bool()> > tests(
      {testReadingSize, testReadingMeasurement, testReadingBinEdges,
	  testReadingErrors, testCopyConstructor, testCompatibilityFunction});
  for (auto test : tests)
    std::cout << (test() ? " ok" : " FAILED!") << std::endl;
}

int main() {
  using namespace std;

  cout << "******************************************************" << endl;
  runTests();
  cout << "******************************************************" << endl;
  // Create object vector which holds data of experiments A, B, C and D
  vector<Data> dat;
  dat.push_back(Data("exp_A"));
  dat.push_back(Data("exp_B"));
  dat.push_back(Data("exp_C"));
  dat.push_back(Data("exp_D"));

  vector<string> letter = {"A", "B", "C", "D"};
  vector<Data> w_sum;
  vector<unique_ptr<ofstream>> fout;

  cout << "Bin 27: from " << dat[0].binLow(27) << " to " << dat[0].binHigh(27)
       << endl;
  // Loop over to read the data from the experiments
  for (int i = 0; i < 4; ++i) {
    cout << "Measurement of experiment " << letter[i] << " in bin 27: " << dat[i].measurement(27)
         << endl;
    for (int j = i + 1; j < 4; ++j) {
      for (int n = 1; n < 4; ++n) {
	cout << "Nr. of incompatible data between experiments " << letter[i] << " and " << letter[j] << " (" << n << " standard deviation(s)): " << dat[i].checkCompatibility(dat[j], n)
	     << endl;
      }

      // Create output files
      Data sum = dat[i] + dat[j];
      w_sum.push_back(sum);
      string filename = "avg_" + letter[i] + letter[j];
      unique_ptr<ofstream> out(new ofstream(filename));
      fout.push_back(move(out));
    } 
  }

  // Write to files
  for (int i = 0; i < 6; ++i) {
    *fout[i] << w_sum[i].size() << endl;

    for (int j = 0; j < w_sum[i].size() + 1; ++j) {
      *fout[i] << w_sum[i].binLow(j) << "  ";
    }
    *fout[i] << endl;

    for (int k = 0; k < w_sum[i].size(); ++k) {
      double avg = w_sum[i].measurement(k);
      *fout[i] << avg << "  ";
    }
    *fout[i] << endl;

    for (int l = 0; l < w_sum[i].size(); ++l) {
      double err = w_sum[i].error(l);
      *fout[i] << err << "  ";
    }
    *fout[i] << endl;
  }

  return 0;
}
