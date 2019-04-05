// Semin Jeon(146453162)
// Workshop 9 - Multi-Threading
// SecureData.cpp
// Chris Szalwinski after Cornel Barna
// 2019/03/19

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include "SecureData.h"

using namespace std;

namespace sict {

	void converter(char* t, char key, int n, const Cryptor& c) {
		for (int i = 0; i < n; i++)
			t[i] = c(t[i], key);
	}

	SecureData::SecureData(const char* file, char key, ostream* pOfs) {
		ofs = pOfs;

		// open text file
		fstream input(file, std::ios::in);
		if (!input)
			throw string("\n***Failed to open file ") +
			string(file) + string(" ***\n");

		// copy from file into memory
		input.seekg(0, std::ios::end);
		nbytes = (int)input.tellg() + 1;

		text = new char[nbytes];

		input.seekg(ios::beg);
		int i = 0;
		input >> noskipws;
		while (input.good())
			input >> text[i++];
		text[nbytes - 1] = '\0';
		*ofs << "\n" << nbytes - 1 << " bytes copied from file "
			<< file << " into memory (null byte added)\n";
		encoded = false;

		// encode using key
		code(key);
		*ofs << "Data encrypted in memory\n";
	}

	SecureData::~SecureData() {
		delete[] text;
	}

	void SecureData::display(std::ostream& os) const {
		if (text && !encoded)
			os << text << std::endl;
		else if (encoded)
			throw std::string("\n***Data is encoded***\n");
		else
			throw std::string("\n***No data stored***\n");
	}
  
  // Added multi-threading
	void SecureData::code(char key) {
		
    const int threadNo = 3;
    int split = nbytes / threadNo;
    std::thread t[threadNo];
    Cryptor cryptors[threadNo];
    for (int i = 0; i < threadNo; i++) {
      t[i] = std::thread(std::bind(converter, text + i * split, key, i + 1 < threadNo ? split : (nbytes - i * split), cryptors[i]));
    }
    for (int i = 0; i < threadNo; i++) t[i].join();
    encoded = !encoded;

	}

  // Initialize a binary file stream and writes data from program into the file
	void SecureData::backup(const char* file) {
		if (!text)
			throw std::string("\n***No data stored***\n");
		else if (!encoded)
			throw std::string("\n***Data is not encoded***\n");
		else
		{
			// TODO: open a binary file for writing
      std::fstream m_file(file, std::ios::out | std::ios::binary);


			// TODO: write data into the binary file
			//         and close the file
      for (int i = 0; i < nbytes; ++i) {
        m_file << text[i];
      }
      m_file.close();
		}
	}

  // Read data from a binary file and allocate memory for the data
	void SecureData::restore(const char* file, char key) {
		// TODO: open binary file for reading
    std::fstream m_file(file, std::ios::in | std::ios::binary);


		// TODO: - allocate memory here for the file content
    m_file >> noskipws;
    while (!m_file) {
      m_file.get();
      ++nbytes;
    }
    text = new char[nbytes + 1];

		// TODO: - read the content of the binary file
    m_file.clear();
    m_file.seekg(0, std::ios::beg);
    int i = 0;
    while (!m_file.eof()) m_file >> text[i++];
    text[nbytes] = '\0';

		*ofs << "\n" << nbytes << " bytes copied from binary file "
			<< file << " into memory.\n";

		encoded = true;

		// decode using key
		code(key);

		*ofs << "Data decrypted in memory\n\n";
	}

	std::ostream& operator<<(std::ostream& os, const SecureData& sd) {
		sd.display(os);
		return os;
	}
}