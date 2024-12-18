#include "Data.hh"

#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <math.h>

using namespace std;

Data::Data(const string& filename) {
  ifstream file(filename);

  // Check if file opened correctly
  if (!file.is_open()) {
    throw runtime_error("Error could not open file with name: " +
                        filename);
  }

  int size = -1;
  file >> size;

  // Read in data from file: m_siz+1 bin edges
  for (int i = 0; i < size + 1; ++i) {
    double edge;
    file >> edge;
    m_bins.push_back(edge);
  }
  // Read in data from file: m_siz bin contents
  for (int i = 0; i < size; ++i) {
    double entries;
    file >> entries;
    m_data.push_back(entries);
  }
  // Read in data from file: m_siz bin errors
  for (int i = 0; i < size; ++i) {
    double errors;
    file >> errors;
    m_err.push_back(errors);
  }

  // Done! Close the file.
  file.close();

  assertSizes();
};

void Data::assertSizes() { assert(m_data.size() + 1 == m_bins.size()); }

int Data::checkCompatibility(const Data& in, int n) {
  int diff = 0;

  // Count nr. of data points which exceed compatibility
  for (int i = 0; i < m_data.size(); ++i) {
    if (fabs(m_data[i] - in.measurement(i)) > n*sqrt(pow(m_err[i], 2.) + pow(in.error(i), 2.))) {
      diff++;
    }
  }

  return diff;
}

Data Data::operator+(const Data& rhs) const {
  Data dat = *this;

  // Calculate weighted average and error
  for (unsigned int i = 0; i < dat.size(); ++i) {
    double avg;
    avg = ((1/pow(dat.error(i), 2.))*dat.measurement(i) + (1/pow(rhs.error(i), 2.))*rhs.measurement(i))/(1/pow(dat.error(i), 2.) + 1/pow(rhs.error(i), 2.));

    double err;
    err = sqrt(1/(1/pow(dat.error(i), 2.) + 1/pow(rhs.error(i), 2.)));

    dat.m_data[i] = avg;
    dat.m_err[i] = err;
  }

  return dat;
}
